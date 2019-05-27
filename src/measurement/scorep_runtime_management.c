/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017-2018,
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

#include <SCOREP_Timer_Ticks.h>
#include <UTILS_Error.h>
#include <UTILS_IO.h>
#include <SCOREP_Thread_Mgmt.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Config.h>
#include "scorep_status.h"
#include "scorep_ipc.h"
#include <SCOREP_Definitions.h>
#include <SCOREP_Location.h>
#include "scorep_environment.h"
#include "scorep_subsystem_management.h"

#include <scorep_substrates_definition.h>
#include <SCOREP_Substrates_Management.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>


/** @def FORMAT_TIME_SIZE length for generated experiment directory names based on timestamp */
#define FORMAT_TIME_SIZE  128

/** @def SCOREP_FAILED_DIR_NAME_PREFIX directory name prefix of failed Score-P measurement runs */
#define SCOREP_FAILED_DIR_NAME_PREFIX "scorep-failed-"

/** @def SCOREP_DIR_NAME_PREFIX directory name prefix of Score-P measurement runs */
#define SCOREP_DIR_NAME_PREFIX "scorep-"

/** @def SCOREP_TMP_DIR_NAME name of temporary Score-P directory */
#define SCOREP_TMP_DIR_NAME "scorep-measurement-tmp"


/* *INDENT-OFF* */
static void scorep_create_directory( void );
static void scorep_create_experiment_dir_name( void );
static bool scorep_dir_name_is_created( void );
static void scorep_dump_config( void );
static void dump_manifest_and_subsystem_configs( const char* relativeSourceDir, const char* targetDir );
/* *INDENT-ON* */


static char* scorep_experiment_dir_name;
static bool scorep_experiment_dir_needs_rename;
static bool scorep_experiment_dir_created;

static bool scorep_experiment_dir_not_needed;

const char*
SCOREP_GetExperimentDirName( void )
{
    if ( scorep_experiment_dir_not_needed )
    {
        return NULL;
    }

    scorep_create_experiment_dir_name();
    return scorep_experiment_dir_name;
}


void
SCOREP_CreateExperimentDir( void )
{
    UTILS_BUG_ON( scorep_experiment_dir_created,
                  "SCOREP_CreateExperimentDir called multiple times." );

    /* check if any substrate actually needs the directory. If not, skip it. */
    /* directory_required must be initialized 0, according to SCOREP_SUBSTRATE_REQUIREMENT_CHECK_ANY doc */
    uint64_t directory_required = 0;
    SCOREP_SUBSTRATE_REQUIREMENT_CHECK_ANY( CREATE_EXPERIMENT_DIRECTORY, directory_required );
    if ( !directory_required && !SCOREP_Env_DoForceCfgFiles() )
    {
        /* If tracing and profiling is disabled and no other substrate except for scorep-properties (which is always there) is present */
        if ( SCOREP_Substrates_NumberOfRegisteredSubstrates() == 1 && SCOREP_Status_GetRank() == 0 )
        {
            UTILS_WARNING( "Score-P will not create an experiment directory. "
                           "If you want to change this, set the environment variable SCOREP_FORCE_CFG_FILES to true "
                           "or register any substrate (e.g., profiling or tracing) that writes results." );
        }
        scorep_experiment_dir_not_needed   = true;
        scorep_experiment_dir_created      = true;
        scorep_experiment_dir_needs_rename = false;
        return;
    }

    scorep_create_experiment_dir_name();

    if ( SCOREP_Status_GetRank() == 0 )
    {
        scorep_create_directory();

        /* dump the measurement configuration, filter and manifest early, so that it is also
           available in case of failure */
        scorep_dump_config();
        dump_manifest_and_subsystem_configs( SCOREP_GetWorkingDirectory(), scorep_experiment_dir_name );
    }

    scorep_experiment_dir_created = true;
}


const char*
SCOREP_GetWorkingDirectory( void )
{
    static char* working_directory;
    static bool  been_visited = false;
    if ( !been_visited )
    {
        working_directory = UTILS_IO_GetCwd( NULL, 0 );
        if ( working_directory == NULL )
        {
            UTILS_ERROR_POSIX( "Error while getting absolute path name of the current working directory." );
            _Exit( EXIT_FAILURE );
        }
        been_visited = true;
    }
    return working_directory;
}


void
scorep_create_experiment_dir_name( void )
{
    if ( scorep_dir_name_is_created() )
    {
        return;
    }

    const char* env_get_experiment_dir_name;
    env_get_experiment_dir_name = SCOREP_Env_GetExperimentDirectory();

    if ( strlen( env_get_experiment_dir_name ) == 0 )
    {
        /*
         * if the user does not specified an experiment name,
         * use default temporary directory name and rename it to a timestamp based
         * at the end.
         */

        scorep_experiment_dir_name = UTILS_IO_JoinPath( 2, SCOREP_GetWorkingDirectory(),
                                                        SCOREP_TMP_DIR_NAME );
        scorep_experiment_dir_needs_rename = true;
    }
    else
    {
        /*
         * if the user specified an experiment name, use this one,
         * no need to rename it at the end
         */

        /*
         * If env_get_experiment_dir_name contains an absolute path,
         * SCOREP_GetWorkingDirectory() will be skipped by UTILS_IO_JoinPath,
         * otherwise scorep_experiment_dir_name is set to SCOREP_GetWorkingDirectory() +
         * path separator + env_get_experiment_dir_name
         */
        scorep_experiment_dir_name = UTILS_IO_JoinPath( 2, SCOREP_GetWorkingDirectory(),
                                                        env_get_experiment_dir_name );
        scorep_experiment_dir_needs_rename = false;
    }
}


bool
scorep_dir_name_is_created( void )
{
    return scorep_experiment_dir_name && 0 < strlen( scorep_experiment_dir_name );
}


static const char*
scorep_format_time( time_t* timestamp )
{
    static char local_time_buf[ FORMAT_TIME_SIZE ];
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

    strftime( local_time_buf, FORMAT_TIME_SIZE - 1, "%Y%m%d_%H%M_", local_time );
    snprintf( &( local_time_buf[ strlen( local_time_buf ) ] ),
              FORMAT_TIME_SIZE - strlen( local_time_buf ) - 1,
              "%" PRIu64, SCOREP_Timer_GetClockTicks() );
    local_time_buf[ FORMAT_TIME_SIZE - 1 ] = '\0';

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
        /*
         * fail if the previous entry exists and is not an directory at all
         */
        if ( !S_ISDIR( buf.st_mode ) )
        {
            UTILS_ERROR( SCOREP_ERROR_ENOTDIR,
                         "Experiment directory \"%s\" exists but is not an directory.",
                         scorep_experiment_dir_name );
            _Exit( EXIT_FAILURE );
        }


        if ( scorep_experiment_dir_needs_rename )
        {
            /*
             * we use the default temporary directory,
             * rename previous failed runs away
             */
            char* tmp = calloc( strlen( SCOREP_FAILED_DIR_NAME_PREFIX ) + FORMAT_TIME_SIZE + 1,
                                sizeof( char ) );
            UTILS_ASSERT( tmp );
            strncat( tmp, SCOREP_FAILED_DIR_NAME_PREFIX, strlen( SCOREP_FAILED_DIR_NAME_PREFIX ) );
            strncat( tmp, scorep_format_time( NULL ), FORMAT_TIME_SIZE );
            char* failed_experiment_dir_name = UTILS_IO_JoinPath( 2, SCOREP_GetWorkingDirectory(), tmp );

            if ( rename( scorep_experiment_dir_name, failed_experiment_dir_name ) != 0 )
            {
                UTILS_ERROR_POSIX( "Can't rename experiment directory \"%s\" to \"%s\".",
                                   scorep_experiment_dir_name, failed_experiment_dir_name );
                _Exit( EXIT_FAILURE );
            }
            free( failed_experiment_dir_name );
            free( tmp );
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
    UTILS_BUG_ON( !scorep_experiment_dir_created,
                  "SCOREP_CreateExperimentDir not yet called." );

    SCOREP_Ipc_Barrier();

    if ( SCOREP_Status_GetRank() > 0 )
    {
        return;
    }

    if ( !scorep_experiment_dir_needs_rename )
    {
        return;
    }

    /*
     * we use the default temporary directory,
     * thus rename it to a timestamped based directory name
     */
    char* tmp = calloc( strlen( SCOREP_DIR_NAME_PREFIX ) + FORMAT_TIME_SIZE + 1,
                        sizeof( char ) );
    UTILS_ASSERT( tmp );
    strncat( tmp, SCOREP_DIR_NAME_PREFIX, strlen( SCOREP_DIR_NAME_PREFIX ) );
    strncat( tmp, scorep_format_time( NULL ), FORMAT_TIME_SIZE );
    char* new_experiment_dir_name = UTILS_IO_JoinPath( 2, SCOREP_GetWorkingDirectory(), tmp );

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
    free( new_experiment_dir_name );
    free( tmp );
}

static void
scorep_dump_config( void )
{
    char* dump_file_name = UTILS_IO_JoinPath(
        2, SCOREP_GetExperimentDirName(), "scorep.cfg" );
    if ( !dump_file_name )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Can't write measurement configuration" );

        return;
    }

    FILE* dump_file = fopen( dump_file_name, "w" );
    if ( !dump_file )
    {
        UTILS_ERROR( SCOREP_ERROR_FILE_CAN_NOT_OPEN,
                     "Can't write measurement configuration into '%s'",
                     dump_file_name );

        free( dump_file_name );
        return;
    }
    free( dump_file_name );

    SCOREP_ConfigDump( dump_file );
    fclose( dump_file );
}

static void
dump_manifest_and_subsystem_configs( const char* relativeSourceDir, const char* targetDir )
{
    char* dump_file_name = UTILS_IO_JoinPath(
        2, scorep_experiment_dir_name, "MANIFEST.md" );
    if ( !dump_file_name )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Cannot allocate memory for MANIFEST.md file name" );
        return;
    }

    FILE* dump_file = fopen( dump_file_name, "w" );
    if ( !dump_file )
    {
        UTILS_ERROR( SCOREP_ERROR_FILE_CAN_NOT_OPEN,
                     "Cannot write MANIFEST.md into '%s'",
                     dump_file_name );
        return;
    }
    free( dump_file_name );

    /* Note, further additions to the MANIFEST.md file or triggered file copies
     * should use the following scheme:
     * 1) If the information is part of a subsystem it should use the
     *    scorep_subsystems_dump_manifest callback and let the subsystem handle the
     *    information.
     * 2) If it is subsystem independent, use the provided SCOREP_ConfigCopyFile to
     *    trigger a file copy. If the new information is configuration independent,
     *    it can be added directly as part of the static header.
     */

    /* static header */
    fprintf( dump_file, "# Experiment directory overview\n" );
    fprintf( dump_file, "\n" );

    fprintf( dump_file, "The result directory of this measurement should contain the following files:\n" );
    fprintf( dump_file, "\n" );
    fprintf( dump_file, "   1. Files that should be present even if the measurement aborted:\n" );
    fprintf( dump_file, "\n" );
    fprintf( dump_file, "      * `MANIFEST.md`           This manifest file.\n" );
    fprintf( dump_file, "      * `scorep.cfg`            Listing of used environment variables.\n" );

    /* write dynamic information */
    /* add info for a possible filter file since it isn't controlled by subsystem
       Only on SCOREP_SUCCESS a file has been copied and the respective text has to be
       generated.
     */
    if ( SCOREP_ConfigCopyFile( "filtering", "file", relativeSourceDir, targetDir ) )
    {
        fprintf( dump_file, "      * `scorep.filter`         Copy of the applied filter file.\n" );
    }

    fprintf( dump_file, "\n" );
    fprintf( dump_file, "   2. Files that will be created by subsystems of the measurement core:\n" );

    /* collect information from subsystems*/
    scorep_subsystems_dump_manifest( dump_file, relativeSourceDir, targetDir );

    fprintf( dump_file, "\n" );
    fprintf( dump_file, "# List of Score-P variables that were explicitly set for this measurement\n" );
    fprintf( dump_file, "\n" );
    fprintf( dump_file, "The complete list of Score-P variables used, incl. current default values,\n" );
    fprintf( dump_file, "can be found in `scorep.cfg`.\n" );
    fprintf( dump_file, "\n" );

    SCOREP_ConfigDumpChangedVars( dump_file );
    fclose( dump_file );
}
