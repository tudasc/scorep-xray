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

static bool       silc_is_experiment_dir_created = false;

OTF2_ArchiveData* silc_otf2_archive = 0;

bool              silc_profiling_enabled = false;

bool              silc_tracing_enabled = true;

bool              flush_done = false;

char*
SILC_GetExperimentDirName()
{
    return silc_experiment_dir_name;
}


void
SILC_CreateExperimentDir()
{
    if ( silc_is_experiment_dir_created )
    {
        return;
    }
    silc_create_experiment_dir_name();
    silc_is_experiment_dir_created =
        silc_create_experiment_dir( silc_experiment_dir_name,
                                    dir_name_size,
                                    silc_create_directory );
}


void
silc_create_experiment_dir_name()
{
    if ( silc_dir_name_is_created() )
    {
        return;
    }
    assert( !omp_in_parallel() ); // localtime() not reentrant
    time_t now;
    time( &now );
    strftime( silc_experiment_dir_name, 20, "silc_%Y%m%d_%H%M_", localtime( &now ) );
    snprintf( &( silc_experiment_dir_name[ 19 ] ), 11, "%u",
              ( uint32_t )SILC_GetClockTicks() );
}


bool
silc_dir_name_is_created()
{
    return strlen( silc_experiment_dir_name );
}


bool
SILC_ExperimentDirIsCreated()
{
    return silc_is_experiment_dir_created;
}


uint64_t
SILC_CalculateOTF2LocationId( SILC_Thread_LocationData* locationData )
{
    SILC_Trace_LocationData* trace_data = SILC_Thread_GetTraceLocationData( locationData );
    assert( trace_data->otf_location == OTF2_UNDEFINED_UINT64 || // first call
            trace_data->otf_location == 0 );                     // first call was with rank == 0 and location == 0
                                                                 // re-call from SILC_InitMeasurementMPI()

    uint64_t rank         = SILC_Mpi_GetRank();
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
        SILC_ERROR_POSIX( "Can't create experiment directory \"%s\".",
                          silc_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }
}


uint64_t
SILC_OnTraceAndDefinitionPostFlush( void )
{
    /* remember that we have flushed the first time
     * after this point, we can't switch into MPI mode anymore
     */
    flush_done = true;

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
