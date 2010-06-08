/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_runtime_management.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 */


#include "silc_runtime_management.h"

#include <SILC_Timing.h>
#include <SILC_Error.h>
#include <SILC_Omp.h>
#include <SILC_Memory.h>
#include "silc_status.h"
#include "silc_mpi.h"
#include "silc_thread.h"
#include "silc_definition_structs.h"

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>


/* *INDENT-OFF* */
extern bool silc_create_experiment_dir(char* dirName, int dirNameSize, void (*createDir) (const char*) );
static void silc_create_directory(const char* dirname);
static void silc_create_experiment_dir_name();
static void silc_set_event_writer_location_id(OTF2_EvtWriter* writer);
static bool silc_dir_name_is_created();
static void silc_set_otf2_event_writer_location_id();
static void silc_update_location_definition_id( SILC_Thread_LocationData* location );
/* *INDENT-ON* */


#define dir_name_size  30

char silc_experiment_dir_name[ dir_name_size ];

OTF2_ArchiveData* silc_otf2_archive = 0;


typedef struct silc_deferred_location silc_deferred_location;
struct silc_deferred_location
{
    SILC_Thread_LocationData* location;
    silc_deferred_location*   next;
};

static silc_deferred_location silc_deferred_locations_head_dummy = { 0, 0 };


char*
SILC_GetExperimentDirName()
{
    return silc_experiment_dir_name;
}


void
SILC_CreateExperimentDir()
{
    if ( SILC_IsExperimentDirCreated() )
    {
        return;
    }
    silc_create_experiment_dir_name();

    if ( silc_create_experiment_dir( silc_experiment_dir_name,
                                     dir_name_size,
                                     silc_create_directory ) )
    {
        SILC_OnExperimentDirCreation();
    }
}


void
silc_create_experiment_dir_name()
{
    if ( silc_dir_name_is_created() )
    {
        return;
    }
    snprintf( silc_experiment_dir_name, 21, "%s", "silc-measurement-tmp" );
}


bool
silc_dir_name_is_created()
{
    return strlen( silc_experiment_dir_name );
}


uint64_t
SILC_CalculateGlobalLocationId( SILC_Thread_LocationData* locationData )
{
    assert( SILC_Mpi_IsInitialized() );
    uint64_t local_location_id = SILC_Thread_GetLocationId( locationData );
    uint64_t rank              = SILC_Mpi_GetRank();

    assert( rank >> 32 == 0 );
    assert( local_location_id >> 32 == 0 );

    uint64_t global_location_id = ( local_location_id << 32 ) | rank;
    return global_location_id;
}


void
SILC_DeferLocationInitialization( SILC_Thread_LocationData* locationData )
{
    silc_deferred_location* deferred_location = SILC_Memory_AllocForMisc( sizeof( silc_deferred_location ) );
    assert( deferred_location );

    deferred_location->location             = locationData;
    deferred_location->next                 = silc_deferred_locations_head_dummy.next;
    silc_deferred_locations_head_dummy.next = deferred_location;
}


void
SILC_ProcessDeferredLocations()
{
    SILC_Thread_LocationData* current_location                       = SILC_Thread_GetLocationData();
    silc_deferred_location*   deferred_location                      = silc_deferred_locations_head_dummy.next;
    bool                      current_location_in_deferred_locations = false;

    while ( deferred_location )
    {
        if ( deferred_location->location == current_location )
        {
            current_location_in_deferred_locations = true;
        }

        SILC_SetOtf2WriterLocationId( deferred_location->location );
        silc_update_location_definition_id( deferred_location->location );

        deferred_location = deferred_location->next;
    }

    assert( current_location_in_deferred_locations );
}


static void
silc_update_location_definition_id( SILC_Thread_LocationData* location )
{
    SILC_Trace_LocationData*  trace_data          = SILC_Thread_GetTraceLocationData( location );
    SILC_Location_Definition* location_definition =
        SILC_MEMORY_DEREF_MOVABLE( SILC_Thread_GetLocationHandle( location ),
                                   SILC_Location_Definition* );

    location_definition->global_location_id = trace_data->otf_location;
}


void
silc_create_directory( const char* dirname )
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    if ( mkdir( silc_experiment_dir_name, mode ) == -1 )
    {
        /// @todo We may check for EEXIST to provide a better error message.
        ///       But if the measurement succeeds, this case should not happen
        ///       because the directory will be renamed.
        SILC_ERROR_POSIX( "Can't create experiment directory \"%s\".",
                          silc_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }
}


void
SILC_RenameExperimentDir()
{
    SILC_Mpi_GlobalBarrier();
    if ( SILC_Mpi_GetRank() > 0 )
    {
        return;
    }

    if ( !SILC_IsExperimentDirCreated() )
    {
        return;
    }

    assert( !omp_in_parallel() ); // localtime() not reentrant
    time_t     now;
    struct tm* local_time;
    time( &now );
    local_time = localtime( &now );
    if ( local_time == NULL )
    {
        perror( "localtime should not fail." );
        exit( EXIT_FAILURE );
    }

    char new_experiment_dir_name[ dir_name_size ];
    strftime( new_experiment_dir_name, 20, "silc-%Y%m%d_%H%M_", local_time );
    snprintf( &( new_experiment_dir_name[ 19 ] ), 11,
              "%u", ( uint32_t )SILC_GetClockTicks() );

    if ( rename( "silc-measurement-tmp", new_experiment_dir_name ) != 0 )
    {
        SILC_ERROR_POSIX( "Can't rename experiment directory form silc-measurement-tmp to \"%s\".",
                          new_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }
}


uint64_t
SILC_OnTraceAndDefinitionPostFlush( void )
{
    /* remember that we have flushed the first time
     * after this point, we can't switch into MPI mode anymore
     */
    SILC_Otf2_OnFlush();

    return SILC_GetClockTicks();
}


OTF2_FlushType
SILC_OnTracePreFlush( void* evtWriter,
                      void* evtReader )
{
    if ( !SILC_Mpi_IsInitialized )
    {
        // flush before MPI_Init, we are lost.
        assert( false );
    }
    // master/slave and writer id already set during initialization
    return OTF2_FLUSH;
}
