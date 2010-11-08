/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       scorep_runtime_management.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "scorep_runtime_management.h"

#include <SCOREP_Timing.h>
#include <scorep_utility/SCOREP_Error.h>
#include <scorep_utility/SCOREP_Omp.h>
#include <SCOREP_Memory.h>
#include "scorep_status.h"
#include "scorep_mpi.h"
#include "scorep_thread.h"
#include "scorep_definition_structs.h"
#include "scorep_definition_macros.h"
#include <SCOREP_Thread_Types.h>

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>


/* *INDENT-OFF* */
extern bool scorep_create_experiment_dir(char* dirName, int dirNameSize, void (*createDir) (const char*) );
static void scorep_create_directory(const char* dirname);
static void scorep_create_experiment_dir_name();
static void scorep_set_event_writer_location_id(OTF2_EvtWriter* writer);
static bool scorep_dir_name_is_created();
static void scorep_set_otf2_event_writer_location_id();
/* *INDENT-ON* */


#define dir_name_size  34

char scorep_experiment_dir_name[ dir_name_size ];

OTF2_Archive* scorep_otf2_archive = 0;


char*
SCOREP_GetExperimentDirName()
{
    scorep_create_experiment_dir_name();
    return scorep_experiment_dir_name;
}


void
SCOREP_CreateExperimentDir()
{
    if ( SCOREP_IsExperimentDirCreated() )
    {
        return;
    }
    scorep_create_experiment_dir_name();

    if ( scorep_create_experiment_dir( scorep_experiment_dir_name,
                                       dir_name_size,
                                       scorep_create_directory ) )
    {
        SCOREP_OnExperimentDirCreation();
    }
}


void
scorep_create_experiment_dir_name()
{
    if ( scorep_dir_name_is_created() )
    {
        return;
    }
    snprintf( scorep_experiment_dir_name, 23, "%s", "scorep-measurement-tmp" );
}


bool
scorep_dir_name_is_created()
{
    return strlen( scorep_experiment_dir_name );
}


uint64_t
SCOREP_CalculateGlobalLocationId( SCOREP_Thread_LocationData* locationData )
{
    assert( SCOREP_Mpi_IsInitialized() );
    uint64_t local_location_id = SCOREP_Thread_GetLocationId( locationData );
    uint64_t rank              = SCOREP_Mpi_GetRank();

    assert( rank >> 32 == 0 );
    assert( local_location_id >> 32 == 0 );

    uint64_t global_location_id = ( local_location_id << 32 ) | rank;
    return global_location_id;
}


void
scorep_format_local_time( char* name )
{
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
    strftime( name, 15, "%Y%m%d_%H%M_", local_time );
    snprintf( &( name[ 14 ] ), 6,
              "%u", ( uint32_t )SCOREP_GetClockTicks() );
}


void
scorep_create_directory( const char* dirname )
{
    //first check to see if directory already exists.
    struct stat buf;
    mode_t      mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    if ( stat( scorep_experiment_dir_name, &buf ) == -1 )
    {
        if ( mkdir( scorep_experiment_dir_name, mode ) == -1 )
        {
            SCOREP_ERROR_POSIX( "Can't create experiment directory \"%s\".",
                                scorep_experiment_dir_name );
            _Exit( EXIT_FAILURE );
        }
    }
    else
    {
        char local_time[ dir_name_size ];
        scorep_format_local_time( local_time );
        char new_experiment_dir_name[ dir_name_size ] = "scorep-failed-";
        strcat( new_experiment_dir_name, local_time );
        if ( rename( scorep_experiment_dir_name, new_experiment_dir_name ) != 0 )
        {
            perror( "perror for rename." );
            SCOREP_ERROR_POSIX( "Can't rename experiment directory \"%s\" to \"%s\".",
                                scorep_experiment_dir_name, new_experiment_dir_name );
            _Exit( EXIT_FAILURE );
        }
        else
        {
            if ( mkdir( scorep_experiment_dir_name, mode ) == -1 )
            {
                SCOREP_ERROR_POSIX( "Can't create experiment directory \"%s\".",
                                    scorep_experiment_dir_name );
                _Exit( EXIT_FAILURE );
            }
        }
    }
}


void
SCOREP_RenameExperimentDir()
{
    SCOREP_Mpi_GlobalBarrier();
    if ( SCOREP_Mpi_GetRank() > 0 )
    {
        return;
    }

    if ( !SCOREP_IsExperimentDirCreated() )
    {
        return;
    }
    char local_time[ dir_name_size ];
    scorep_format_local_time( local_time );
    char new_experiment_dir_name[ dir_name_size ] = "scorep-";
    strcat( new_experiment_dir_name, local_time );
    if ( rename( "scorep-measurement-tmp", new_experiment_dir_name ) != 0 )
    {
        SCOREP_ERROR_POSIX( "Can't rename experiment directory form scorep-measurement-tmp to \"%s\".",
                            new_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }
}


uint64_t
SCOREP_OnTraceAndDefinitionPostFlush( void )
{
    /* remember that we have flushed the first time
     * after this point, we can't switch into MPI mode anymore
     */
    SCOREP_Otf2_OnFlush();

    return SCOREP_GetClockTicks();
}


OTF2_FlushType
SCOREP_OnTracePreFlush( void* evtWriter,
                        void* evtReader )
{
    if ( !SCOREP_Mpi_IsInitialized )
    {
        // flush before MPI_Init, we are lost.
        assert( false );
    }
    // master/slave and writer id already set during initialization
    return OTF2_FLUSH;
}
