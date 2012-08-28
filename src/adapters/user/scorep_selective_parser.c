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
 *  configuration file.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>
#include <SCOREP_RuntimeManagement.h>
#include <UTILS_CStr.h>
#include <UTILS_Debug.h>
#include <UTILS_IO.h>
#include <scorep_selective_region.h>
#include <UTILS_Error.h>

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
        "config_file",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_selective_file_name,
        NULL,
        "",
        "A file name which configures selective tracing",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};

/**
   The list of traced regions. The regions are alphabetically sorted after their
   region name.
 */
SCOREP_Vector* scorep_selected_regions = NULL;


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
   Initializes the traced region list
 */
static SCOREP_Error_Code
scorep_selective_init_region_list()
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
   Adds an addtional interval to an existing traced region. It checks whether the new
   region overlaps and concatenates to an existing region. In this case regions are
   merged.
   @param region Pointer to the regio instance to which a new interval is added.
   @param first  First instance number of the new interval.
   @param last   Last instance number of the new interval.
 */
void
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
   Create a new new entry for the traced region list.
   @param region The region name of the new traced region.
   @param first  The first instance number of the initial instance interval.
   @param last   The last instance numbe of the initial instance interval.
   @param index  Index in the list where the new entry is inserted.
 */
static SCOREP_Error_Code
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
   Adds an traced region to the list of traced regions.
   @param name    The region name of the traced region.
   @param first   The first instance number of the traced interval of instances.
   @param last    The last instance number of the traced interval of instances.
 */
static void
scorep_selective_add( const char* name,
                      int         first,
                      int         last )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Add traced region %s %d:%d\n", name, first, last );
    assert( scorep_selected_regions != NULL );

    size_t                  index      = 0;
    scorep_selected_region* new_region = NULL;

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
    size_t            buffer_size = 0;
    char*             buffer      = NULL;
    SCOREP_Error_Code err         = SCOREP_SUCCESS;

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
                /* Check whether the interval contains only valid charakters */
                pos = strspn( interval, "0123456789:" );
                if ( pos < strlen( interval ) )
                {
                    UTILS_ERROR( SCOREP_ERROR_PARSE_INVALID_VALUE,
                                 "Invalid interval in selective tracing configuration "
                                 "file for region %s: '%s'. Ignore interval.",
                                 region_name, interval );
                }
                else
                {
                    /* Check wether it is a single iteration or an interval */
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
   Initialization of selective tracing
****************************************************************************************/

/**
   Initializes the selective tracing.
 */
void
scorep_selective_init()
{
    FILE* config_file = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Initialize selective tracing" );

    /* Initialize data structures */
    if ( scorep_selective_init_region_list() != SCOREP_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Failed to create traced region list" );
        return;
    }

    /* Check whether a configuraion file name was specified */
    if ( scorep_selective_file_name == NULL || *scorep_selective_file_name == '\0' )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                            "No configuration file for selective tracing specified.\n"
                            "Disable selective tracing." );
        return;
    }

    /* Open configuration file */
    config_file = fopen( scorep_selective_file_name, "r" );
    if ( config_file == NULL )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                            "Unable to open configuration file for selective tracing.\n"
                            "Disable selective tracing." );
        return;
    }

    /* Parse configuration file */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Reading selective tracing file %s.",
                        scorep_selective_file_name );

    SCOREP_Error_Code err = scorep_selective_parse_file( config_file );
    if ( err != SCOREP_SUCCESS )
    {
        UTILS_ERROR( err,
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
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG | SCOREP_DEBUG_USER,
                        "Register config variables for selective tracing" );
    return SCOREP_ConfigRegister( "selective", scorep_selective_configs );
}

/**
   Cleans up the data structures
 */
void
scorep_selective_finalize()
{
    SCOREP_Vector_Foreach( scorep_selected_regions,
                           scorep_selective_delete_selected_region );
    SCOREP_Vector_Free( scorep_selected_regions );
    scorep_selected_regions = NULL;
}
