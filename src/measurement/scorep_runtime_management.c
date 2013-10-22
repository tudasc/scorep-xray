/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

#include <config.h>
#include "scorep_runtime_management.h"

#include <SCOREP_Timing.h>
#include <UTILS_Error.h>
#include <SCOREP_ThreadForkJoin_Mgmt.h>
#include <SCOREP_Memory.h>
#include "scorep_status.h"
#include "scorep_ipc.h"
#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Location.h>
#include "scorep_environment.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>

/* *INDENT-OFF* */
extern bool scorep_create_experiment_dir(void (*createDir) (void) );
static void scorep_create_directory( void );
static void scorep_create_experiment_dir_name( void );
static bool scorep_dir_name_is_created( void );
/* *INDENT-ON* */


static const char* scorep_experiment_dir_name;
static bool scorep_experiment_dir_needs_rename;
/* length for generated experiment directory names based on timestamp */


const char*
SCOREP_GetExperimentDirName( void )
{
    scorep_create_experiment_dir_name();
    return scorep_experiment_dir_name;
}


void
SCOREP_CreateExperimentDir( void )
{
    if ( SCOREP_Status_IsExperimentDirCreated() )
    {
        return;
    }
    scorep_create_experiment_dir_name();

    if ( scorep_create_experiment_dir( scorep_create_directory ) )
    {
        SCOREP_OnExperimentDirCreation();
    }
}


void
scorep_create_experiment_dir_name( void )
{
    if ( scorep_dir_name_is_created() )
    {
        return;
    }

    scorep_experiment_dir_name = SCOREP_Env_GetExperimentDirectory();

    /* if the user does not specified an experiment name,
     * use scorep-measurement-tmp and rename it to a timestamp based
     * at the end.
     */
    if ( 0 == strlen( scorep_experiment_dir_name ) )
    {
        static char tmp_experiment_dir_name_buf[] = "scorep-measurement-tmp";
        scorep_experiment_dir_name         = tmp_experiment_dir_name_buf;
        scorep_experiment_dir_needs_rename = true;
    }
}


bool
scorep_dir_name_is_created( void )
{
    return scorep_experiment_dir_name && 0 < strlen( scorep_experiment_dir_name );
}


#define format_time_size  128
static const char*
scorep_format_time( time_t* timestamp )
{
    assert( !SCOREP_Thread_InParallel() ); // localtime() not reentrant
    static char local_time_buf[ format_time_size ];
    time_t      now;
    struct tm*  local_time;

    if ( timestamp == NULL )
    {
        time( &now );
        timestamp = &now;
    }

    local_time = localtime( timestamp );
    if ( local_time == NULL )
    {
        perror( "localtime should not fail." );
        _Exit( EXIT_FAILURE );
    }

    strftime( local_time_buf, format_time_size - 1, "%Y%m%d_%H%M_", local_time );
    snprintf( &( local_time_buf[ strlen( local_time_buf ) ] ),
              format_time_size - strlen( local_time_buf ) - 1,
              "%" PRIu64, SCOREP_GetClockTicks() );
    local_time_buf[ format_time_size - 1 ] = '\0';

    return local_time_buf;
}


void
scorep_create_directory( void )
{
    //first check to see if directory already exists.
    struct stat buf;
    mode_t      mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    /* rename an existing experiment directory */
    if ( stat( scorep_experiment_dir_name, &buf ) != -1 )
    {
        if ( scorep_experiment_dir_needs_rename )
        {
            /*
             * we use the default scorep-measurement-tmp directory,
             * rename previous failed runs away
             */
            char failed_experiment_dir_name[ sizeof( "scorep-failed-" ) + format_time_size ] = "scorep-failed-";
            strcat( failed_experiment_dir_name, scorep_format_time( NULL ) );
            if ( rename( scorep_experiment_dir_name, failed_experiment_dir_name ) != 0 )
            {
                UTILS_ERROR_POSIX( "Can't rename experiment directory \"%s\" to \"%s\".",
                                   scorep_experiment_dir_name, failed_experiment_dir_name );
                _Exit( EXIT_FAILURE );
            }
        }
        else
        {
            /*
             * fail if this experiment directory exists and the user
             * commands us to do so
             */
            if ( !SCOREP_Env_DoOverwriteExperimentDirectory() )
            {
                UTILS_ERROR( SCOREP_ERROR_EEXIST,
                             "Experiment directory \"%s\" exists and overwriting is disabled.",
                             scorep_experiment_dir_name );
                _Exit( EXIT_FAILURE );
            }

            /* rename a previous run away by appending a timestamp */
            const char* local_time_buf              = scorep_format_time( &buf.st_mtime );
            char*       old_experiment_dir_name_buf = calloc( strlen( scorep_experiment_dir_name )
                                                              + 1 + strlen( local_time_buf )
                                                              + 1,
                                                              sizeof( char ) );
            assert( old_experiment_dir_name_buf );
            strcpy( old_experiment_dir_name_buf, scorep_experiment_dir_name );
            strcat( old_experiment_dir_name_buf, "-" );
            strcat( old_experiment_dir_name_buf, local_time_buf );
            if ( rename( scorep_experiment_dir_name, old_experiment_dir_name_buf ) != 0 )
            {
                UTILS_ERROR_POSIX( "Can't rename old experiment directory \"%s\" to \"%s\".",
                                   scorep_experiment_dir_name, old_experiment_dir_name_buf );
                _Exit( EXIT_FAILURE );
            }
            if ( SCOREP_Env_RunVerbose() )
            {
                printf( "[Score-P] previous experiment directory: %s\n", old_experiment_dir_name_buf );
            }
            free( old_experiment_dir_name_buf );
        }
    }

    if ( mkdir( scorep_experiment_dir_name, mode ) == -1 )
    {
        UTILS_ERROR_POSIX( "Can't create experiment directory \"%s\".",
                           scorep_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }

    if ( SCOREP_Env_RunVerbose() )
    {
        printf( "[Score-P] experiment directory: %s\n", scorep_experiment_dir_name );
    }
}


void
SCOREP_RenameExperimentDir( void )
{
    SCOREP_Ipc_Barrier();
    if ( SCOREP_Status_GetRank() > 0 )
    {
        return;
    }

    if ( !SCOREP_Status_IsExperimentDirCreated() )
    {
        return;
    }

    if ( !scorep_experiment_dir_needs_rename )
    {
        return;
    }

    /*
     * we use the default scorep-measurement-tmp,
     * thus rename it to a timestamped based directory name
     */
    char new_experiment_dir_name[ sizeof( "scorep-" ) + format_time_size ] = "scorep-";
    strcat( new_experiment_dir_name, scorep_format_time( NULL ) );
    if ( rename( scorep_experiment_dir_name, new_experiment_dir_name ) != 0 )
    {
        UTILS_ERROR_POSIX( "Can't rename experiment directory from \"%s\" to \"%s\".",
                           scorep_experiment_dir_name, new_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }

    if ( SCOREP_Env_RunVerbose() )
    {
        printf( "[Score-P] final experiment directory: %s\n", new_experiment_dir_name );
    }
}
