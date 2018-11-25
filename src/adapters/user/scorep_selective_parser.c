/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014, 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file contains the implementation of the parser for selective recording
 *  configuration file.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <SCOREP_Vector.h>
#include "scorep_selective_region.h"
#include <SCOREP_RuntimeManagement.h>

#include <UTILS_Error.h>
#include <UTILS_CStr.h>
#include <UTILS_Debug.h>
#include <UTILS_IO.h>

/* **************************************************************************************
   Variable definitions
****************************************************************************************/


#include "scorep_user_selective_confvars.inc.c"


/**
   The list of recorded regions. The regions are alphabetically sorted after their
   region name.
 */
static SCOREP_Vector* scorep_selected_regions = NULL;


/* **************************************************************************************
   Local helper functions
****************************************************************************************/

/**
   Frees the memory for an interval entry.
 */
static void
scorep_selectiv_delete_interval( void* item )
{
    free( item );
}

/**
   Frees the memory for an selected region entry.
 */
static void
scorep_selective_delete_selected_region( void* item )
{
    scorep_selected_region* region = ( scorep_selected_region* )item;
    SCOREP_Vector_Foreach( region->intervals, &scorep_selectiv_delete_interval );
    SCOREP_Vector_Free( region->intervals );
    free( region );
}

/**
   Compares a pointer to a 64 bit integer with the first entry of an interval.
 */
static int8_t
scorep_selective_compare_intervals( const void* value,
                                    const void* item )
{
    return *( uint64_t* )value - ( ( scorep_selected_interval* )item )->first;
}

/**
   Compares an string to the region name of a selected region.
 */
static int8_t
scorep_selective_compare_regions( const void* value,
                                  const void* item )
{
    return strcmp( ( const char* )value, ( ( scorep_selected_region* )item )->region_name );
}

/**
   Initializes the recorded region list
 */
static SCOREP_ErrorCode
scorep_selective_init_region_list( void )
{
    scorep_selected_regions = SCOREP_Vector_CreateSize( 4 );
    if ( scorep_selected_regions == NULL )
    {
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }
    else
    {
        return SCOREP_SUCCESS;
    }
}

/**
   Adds an additional interval to an existing recorded region. It checks whether the new
   region overlaps and concatenates to an existing region. In this case regions are
   merged.
   @param region Pointer to the region instance to which a new interval is added.
   @param first  First instance number of the new interval.
   @param last   Last instance number of the new interval.
 */
static void
scorep_selective_add_interval( scorep_selected_region* region,
                               uint64_t                first,
                               uint64_t                last )
{
    size_t pos = 0;

    /* Create a new entry */
    scorep_selected_interval* new_interval =
        ( scorep_selected_interval* )malloc( sizeof( scorep_selected_interval ) );
    new_interval->first = first;
    new_interval->last  = last;

    SCOREP_Vector_LowerBound( region->intervals,
                              &first,
                              scorep_selective_compare_intervals,
                              &pos );
    SCOREP_Vector_Insert( region->intervals, pos, new_interval );
}

/**
   Create a new new entry for the recorded region list.
   @param region The region name of the new recorded region.
   @param first  The first instance number of the initial instance interval.
   @param last   The last instance number of the initial instance interval.
   @param index  Index in the list where the new entry is inserted.
 */
static SCOREP_ErrorCode
scorep_selective_insert_new_region( const char* region,
                                    int         first,
                                    int         last,
                                    size_t      index )
{
    /* Create region */
    scorep_selected_region* new_region =
        ( scorep_selected_region* )malloc( sizeof( scorep_selected_region ) );
    if ( new_region == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for new entry" );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_region->region_name = NULL;
    new_region->intervals   = NULL;

    new_region->region_name = UTILS_CStr_dup( region );
    new_region->intervals   = SCOREP_Vector_CreateSize( 1 );

    if ( new_region->region_name == NULL || new_region->intervals == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for new entry" );
        free( new_region->region_name );
        free( new_region->intervals );
        free( new_region );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    SCOREP_Vector_Insert( scorep_selected_regions, index, new_region );

    /* Insert initial interval */
    scorep_selective_add_interval( new_region, first, last );

    return SCOREP_SUCCESS;
}

/**
   Adds an recorded region to the list of recorded regions.
   @param name    The region name of the recorded region.
   @param first   The first instance number of the recorded interval of instances.
   @param last    The last instance number of the recorded interval of instances.
 */
static void
scorep_selective_add( const char* name,
                      int         first,
                      int         last )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Add recorded region %s %d:%d\n", name, first, last );
    assert( scorep_selected_regions != NULL );

    size_t index = 0;

    /* Region does already exist */
    if ( SCOREP_Vector_Find( scorep_selected_regions,
                             name,
                             scorep_selective_compare_regions,
                             &index ) )
    {
        scorep_selected_region* region =
            ( scorep_selected_region* )SCOREP_Vector_At( scorep_selected_regions, index );
        scorep_selective_add_interval( region, first, last );
        return;
    }

    /* It is a new region */
    if ( SCOREP_Vector_UpperBound( scorep_selected_regions,
                                   name,
                                   scorep_selective_compare_regions,
                                   &index ) )
    {
        scorep_selective_insert_new_region( name, first, last, index );
    }
    else
    {
        index = SCOREP_Vector_Size( scorep_selected_regions );
        scorep_selective_insert_new_region( name, first, last, index );
    }
}

/**
   Parses a configuration file for selective recording. It expected format is:
   One recorded region per line followed by one or more comma separated instance numbers or
   instance intervals:
   <region_name> [<first_instance>[:<last_instance>][,....]]
   @param file An open file handle for the configuration file with its reading pointer set
               to the start position.
   @returns SCOREP_SUCCESS on successful execution. Else an error code is returned.
            Possible error codes are: SCOREP_ERROR_MEM_ALLOC_FAILED and
            SCOREP_ERROR_FILE_INTERACTION.
 */
static SCOREP_ErrorCode
scorep_selective_parse_file( FILE* file )
{
    size_t           buffer_size = 0;
    char*            buffer      = NULL;
    SCOREP_ErrorCode err         = SCOREP_SUCCESS;

    /* Validity assertions */
    assert( file );

    /* Read file line by line */
    while ( !feof( file ) )
    {
        err = UTILS_IO_GetLine( &buffer, &buffer_size, file );
        if ( ( err != SCOREP_SUCCESS ) && ( err != SCOREP_ERROR_END_OF_BUFFER ) )
        {
            goto cleanup;
        }

        /* Process line */
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
                /* Check whether the interval contains only valid charakters */
                pos = strspn( interval, "0123456789:" );
                if ( pos < strlen( interval ) )
                {
                    UTILS_ERROR( SCOREP_ERROR_PARSE_INVALID_VALUE,
                                 "Invalid interval in selective recording configuration "
                                 "file for region '%s': '%s'. Ignore interval.",
                                 region_name, interval );
                }
                else
                {
                    /* Check whether it is a single iteration or an interval */
                    pos   = strcspn( interval, ":" );
                    start = 0;
                    end   = -1;
                    if ( pos >= strlen( interval ) )
                    {
                        /* Single instance number */
                        sscanf( interval, "%d", &start );
                        end = start;
                    }
                    else
                    {
                        /* Interval */
                        interval[ pos ] = '\0';
                        sscanf( interval, "%d", &start );
                        sscanf( &interval[ pos + 1 ], "%d", &end );
                    }
                    scorep_selective_add( region_name, start, end );
                }
                interval = strtok( NULL, " \t\n," );
            }
        }
        //*buffer = '\0';
    }

    /* We do not want the EOF error, but a success when we reached the end of the file */
    err = SCOREP_SUCCESS;

    /* Clean up */
cleanup:
    free( buffer );
    return err;
}

/* **************************************************************************************
   Functions used in the adapter
****************************************************************************************/
scorep_selected_region*
scorep_selective_get_region( const char* name )
{
    size_t index = 0;
    if ( SCOREP_Vector_Find( scorep_selected_regions, name,
                             scorep_selective_compare_regions, &index ) )
    {
        return ( scorep_selected_region* )SCOREP_Vector_At( scorep_selected_regions, index );
    }
    return NULL;
}

/* **************************************************************************************
   Initialization of selective recording
****************************************************************************************/

/**
   Initializes the selective recording.
 */
void
scorep_selective_init( void )
{
    FILE* config_file = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Initialize selective recording" );

    /* Initialize data structures */
    if ( scorep_selective_init_region_list() != SCOREP_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Failed to create recorded region list" );
        return;
    }

    /* Check whether a configuration file name was specified */
    if ( scorep_selective_file_name == NULL || *scorep_selective_file_name == '\0' )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                            "No configuration file for selective recording specified.\n"
                            "Disable selective recording." );
        return;
    }

    /* Open configuration file */
    config_file = fopen( scorep_selective_file_name, "r" );
    if ( config_file == NULL )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                            "Unable to open configuration file for selective recording.\n"
                            "Disable selective recording." );
        return;
    }

    /* Parse configuration file */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Reading selective recording file %s.",
                        scorep_selective_file_name );

    SCOREP_ErrorCode err = scorep_selective_parse_file( config_file );
    if ( err != SCOREP_SUCCESS )
    {
        UTILS_ERROR( err,
                     "Unable to read configuration file for selective recording.\n"
                     "Disable selective recording." );
        fclose( config_file );
        return;
    }

    /* Switch off recording */
    SCOREP_SetDefaultRecordingMode( false );

    /* Clean up */
    fclose( config_file );
}

/**
   Registers the config variables for selective recording.
 */
SCOREP_ErrorCode
scorep_selective_register( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Register config variables for selective recording" );
    return SCOREP_ConfigRegister( "selective", scorep_selective_confvars );
}

/**
   Cleans up the data structures
 */
void
scorep_selective_finalize( void )
{
    SCOREP_Vector_Foreach( scorep_selected_regions,
                           scorep_selective_delete_selected_region );
    SCOREP_Vector_Free( scorep_selected_regions );
    scorep_selected_regions = NULL;
}

void
scorep_user_subsystem_dump_manifest( FILE*       manifestFile,
                                     const char* relativeSourceDir,
                                     const char* targetDir )
{
    if ( SCOREP_ConfigCopyFile( "selective", "config_file", relativeSourceDir, targetDir ) )
    {
        SCOREP_ConfigManifestSectionHeader( manifestFile, "User instrumentation" );
        SCOREP_ConfigManifestSectionEntry( manifestFile, "scorep.selective", "Copy of the applied config file for selective recording, source file `%s`.", scorep_selective_file_name );
    }
}
