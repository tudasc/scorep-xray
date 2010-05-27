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
/* *INDENT-ON* */


#define dir_name_size  30

char silc_experiment_dir_name[ dir_name_size ];

OTF2_ArchiveData* silc_otf2_archive = 0;


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
SILC_CalculateOTF2LocationId( SILC_Thread_LocationData* locationData )
{
    // When in MPI mode this function is called twice. During the first call, the
    // MPI rank is not known, but set to 0.
    SILC_Trace_LocationData* trace_data = SILC_Thread_GetTraceLocationData( locationData );
    assert( trace_data->otf_location == OTF2_UNDEFINED_UINT64 || // first call
            trace_data->otf_location == 0 );                     // first call was with rank == 0 and location == 0
                                                                 // re-call from SILC_InitMeasurementMPI()

    uint64_t rank = 0;
    if ( SILC_Mpi_IsInitialized() )
    {
        rank = SILC_Mpi_GetRank();
    }
    uint64_t location     = SILC_Thread_GetLocationId( locationData );
    uint64_t otf_location = ( rank << 32 ) | location;
    assert( rank     >> 32 == 0 );
    assert( location >> 32 == 0 );
    return otf_location;
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
