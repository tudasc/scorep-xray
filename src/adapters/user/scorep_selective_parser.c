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
 *  @status     alpha
 *  @file       scorep_selective_parser.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 *  This file contains the implementation of the parser for selective tracing
 *  configuration file
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <SCOREP_Adapter.h>
#include <SCOREP_Config.h>
#include <SCOREP_RuntimeManagement.h>
#include <scorep_utility/SCOREP_Utils.h>

#define BUFFER_SIZE 1024

/* **************************************************************************************
   Variable definitions
****************************************************************************************/

/**
   Contains the file name for the selective tracing configuration file. This filename
   is filled out by the configration system. In oartcular it is set by the environment
   variable 'SCOREP_SELECTIVE_CONFIG_FILE'.
 */
char* scorep_selective_file_name = NULL;

/**
   Array of configuration variables for the selecetive tracing.
 */
SCOREP_ConfigVariable scorep_selective_configs[] = {
    {
        "SELECTIVE_CONFIG_FILE",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_selective_file_name,
        NULL,
        "",
        "A file name which configures selective tracing"
    }
};


#if 0
char*
scorep_strtok( char* str,
               char* delimiters )
{
    static size_t length  = 0;
    static size_t pos     = 0;
    static char*  buffer  = NULL;
    char*         ret_val = NULL;

    /* Remember new string data */
    if ( str != NULL )
    {
        buffer = str;
        length = strlen( buffer );
        pos    = 0;
    }

    if ( buffer == NULL )
    {
        return NULL;
    }

    /* Skip leading delimiters */
    pos = strspn( buffer, delimiters );
    if ( pos == length )
    {
        buffer = NULL;
        return NULL;
    }
    buffer  = &buffer[ pos ];
    length -= pos;

    /* Find next delimiter */
    pos     = strcspn( buffer, delimiters );
    ret_val = buffer;
    if ( pos == length )
    {
        buffer = NULL;
    }
    else
    {
        buffer[ pos ] = '\0';
        buffer        = &buffer[ pos + 1 ];
    }
    return ret_val;
}
#endif

/* **************************************************************************************
   Local helper functions
****************************************************************************************/

/**
   Adds an traced region to the list of traced regions.
   @param region  The region name of the traced region.
   @param start   The first instance number of the traced interval of instances.
   @param end     The last instance number of the traced interval of instances.
 */
static void
scorep_selective_add( const char* region,
                      int         start,
                      int         end )
{
    printf(
        //SCOREP_DEBUG_PRINTF(SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
        "Add treced region %s %d:%d\n", region, start, end );
    printf( "Implementment me:scorep_selective_add\n" );
}

/**
   Parses a configuration file for selective tracing. It expected format is:
   One traced region per line followed by one or more comma separated instance numbers or
   instance intervals:
   <region_name> [<first_instance>[:<last_instance>][,....]]
   @param file An open file handle for the configuration file with its reading pointer set
               to the start position.
   @returns SCOREP_SUCCESS on successful execution. Else an error code is returned.
            Possible error codes are: SCOREP_ERROR_MEM_ALLOC_FAILED and
            SCOREP_ERROR_FILE_INTERACTION.
 */
static SCOREP_Error_Code
scorep_selective_parse_file( FILE* file )
{
    size_t buffer_size = BUFFER_SIZE;
    char*  buffer      = ( char* )malloc( buffer_size );

    /* Validity assertions */
    assert( file );
    if ( buffer == NULL )
    {
        SCOREP_ERROR_POSIX( "Failed to allocate memory for string buffer" );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    /* Read file line by line */
    while ( !feof( file ) )
    {
        /* Reads a line of arbitrary length*/
        fgets( buffer, buffer_size, file );
        if ( feof( file ) )
        {
            break;
        }
        while ( strlen( buffer ) == buffer_size - 1 )
        {
            buffer_size += BUFFER_SIZE;
            buffer       = ( char* )realloc( buffer, buffer_size );
            if ( buffer == NULL )
            {
                SCOREP_ERROR_POSIX( "Failed to increase memory for string buffer" );
                return SCOREP_ERROR_MEM_ALLOC_FAILED;
            }
            fgets( &buffer[ buffer_size - BUFFER_SIZE - 1 ], BUFFER_SIZE + 1, file );
        }
        if ( ferror( file ) )
        {
            SCOREP_ERROR_POSIX( "Error while reading from file" );
            return SCOREP_ERROR_FILE_INTERACTION;
        }

        /* Process line */
        size_t length      = strlen( buffer );
        size_t pos         = 0;
        char*  region_name = strtok( buffer, " \t\n" );
        char*  interval    = strtok( NULL, " \t\n," );
        int    start       = 0;
        int    end         = -1;

        /* Do not process empty region names */
        if ( region_name == NULL || *region_name == '\0' )
        {
            continue;
        }

        /* If no instances are specified register whole run */
        if ( interval == NULL )
        {
            scorep_selective_add( region_name, start, end );
        }
        /* Process instance selection */
        else
        {
            while ( interval != NULL )
            {
                pos   = strcspn( interval, ":" );
                start = 0;
                end   = -1;
                if ( pos >= strlen( interval ) )
                {
                    sscanf( interval, "%d", &start );
                    end = start;
                }
                else
                {
                    interval[ pos ] = '\0';
                    sscanf( interval, "%d", &start );
                    sscanf( &interval[ pos + 1 ], "%d", &end );
                }
                scorep_selective_add( region_name, start, end );
                interval = strtok( NULL, " \t\n," );
            }
        }
    }


    /* Clean up */
    free( buffer );
    return SCOREP_SUCCESS;
}

/* **************************************************************************************
   Initialization of selective tracing
****************************************************************************************/

/**
   Initializes the selective tracing.
 */
void
scorep_selective_init()
{
    FILE* config_file = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                         "Initialize selective tracing" );

    /* Check whether a configuraion file name was specified */
    if ( scorep_selective_file_name == NULL || *scorep_selective_file_name == '\0' )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                             "No configuration file for selective tracing specified.\n"
                             "Disable selective tracing." );
        return;
    }

    /* Open configuration file */
    config_file = fopen( scorep_selective_file_name, "r" );
    if ( config_file == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                             "Unable to open configuration file for selective tracing.\n"
                             "Disable selective tracing." );
        return;
    }

    /* Parse configuration file */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                         "Reading selective tracing file %s.",
                         scorep_selective_file_name );

    SCOREP_Error_Code err = scorep_selective_parse_file( config_file );
    if ( err != SCOREP_SUCCESS )
    {
        SCOREP_ERROR( err,
                      "Unable to read configration file for selective tracing.\n"
                      "Disable selective tracing." );
        fclose( config_file );
        return;
    }

    /* Switch off recording */
    SCOREP_DisableRecording();

    /* Clean up */
    fclose( config_file );
}

/**
   Registers the config variables for selective tracing.
 */
SCOREP_Error_Code
scorep_selective_register()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                         "Register config variables for selective tracing" );
    return SCOREP_ConfigRegister( NULL, scorep_selective_configs );
}
