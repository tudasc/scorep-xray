/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
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
 * @status          alpha
 * @file            SCOREP_GetExe.c
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief           Provides functions to get executable and executable path.
 */

#include <config.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SCOREP_IO.h>
#include <SCOREP_CStr.h>
#include <SCOREP_Error.h>

bool
MANGLE_NAME( DoesFileExist ) ( const char* filename )
{
    FILE* file = fopen( filename, "r" );
    if ( file != NULL )
    {
        fclose( file );
        return true;
    }
    return false;
}

char*
MANGLE_NAME( GetExecutablePath ) ( const char* exe )
{
    char* executable_name = MANGLE_NAME( CStr_dup ) ( exe );
    char* current_pos     = executable_name;
    if ( exe == NULL )
    {
        return NULL;
    }

    /* If the name already contains slashes, it is probably already the
       path. Thus, only strip the content after the last slash
     */
    for (; *current_pos != '\0'; current_pos++ )
    {
        ;                                      // Seek end of string;
    }
    while ( current_pos != executable_name )
    {
        if ( *current_pos == '/' )
        {
            *current_pos = '\0';              // Truncate name
            return executable_name;
        }
        current_pos--;
    }
    free( executable_name );

    /* If we reach this point the executable_name contains no slashes.
       Thus, it must be in the PATH. Try all directories in the PATH
       environment variable.
     */
    char* path_list      = MANGLE_NAME( CStr_dup ) ( getenv( "PATH" ) );
    char* current_path   = path_list;
    char* full_file_name = NULL;
    int   path_len       = 0;

    current_pos = path_list;
    if ( path_list == NULL )
    {
        return NULL;
    }

    bool done = ( *current_pos == '\0' );
    while ( !done )
    {
        if ( *current_pos == ':' || *current_pos == '\0' )
        {
            if ( *current_pos == '\0' )
            {
                done = true;
            }

            // Truncate current_path
            *current_pos = '\0';

            // Construct full path name
            path_len       = strlen( current_path );
            full_file_name = ( char* )malloc( path_len + strlen( exe ) + 2 );
            if ( full_file_name == NULL )
            {
                MANGLE_NAME( ERROR ) ( MANGLE_NAME( ERROR_MEM_ALLOC_FAILED ), "" );
                free( path_list );
                return NULL;
            }
            strcpy( full_file_name, current_path );
            full_file_name[ path_len ] = '/';
            strcpy( &full_file_name[ path_len + 1 ], exe );
            full_file_name[ path_len + 1 + strlen( exe ) ] = '\0';

            // Copy return path, because the list will be freed.
            if ( MANGLE_NAME( DoesFileExist ) ( full_file_name ) )
            {
                current_path = MANGLE_NAME( CStr_dup ) ( current_path );
                free( path_list );
                free( full_file_name );
                return current_path;
            }

            // Setup for next path
            free( full_file_name );
            full_file_name = NULL;
            if ( !done )
            {
                current_pos++;
                current_path = current_pos;
            }
        }
        else
        {
            current_pos++;
        }
    }

    free( path_list );
    return NULL;
}
