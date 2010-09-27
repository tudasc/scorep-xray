/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file scorep_definition_cube4.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Writes the definitions to Cube 4.
 */

#include <config.h>
#include "scorep_definition_cube4.h"

#include <SCOREP_Memory.h>

#include "scorep_mpi.h"
#include "scorep_types.h"
#include "scorep_definitions.h"
#include <cubew_services.h>

extern SCOREP_DefinitionManager scorep_definition_manager;

/* ****************************************************************************
 * Initialization / Finalization
 *****************************************************************************/

/**
   @def SCOREP_CUBE4_INIT_MAP(Type, type, tablesize)
   Defines a macro for the initialization of the mapping tables for one type
   of handles. This macro is used in scorep_cube4_create_definitions_map.
   @param Type      The definitions type with first letter capitalized. Values
                    be 'Region', 'Counter', 'Callpath'.
   @param type      The definition type in small letters. Values can be
                    'region', 'metric', 'callpath'.
   @param tablesize Defines the number of slots for the mapping table.
 */
/* *INDENT-OFF* */
#define SCOREP_CUBE4_INIT_MAP(Type, type, tablesize)                            \
    if ( sizeof( SCOREP_ ## Type ## Handle ) == 8 )                             \
    {                                                                         \
        map-> type ## _table_cube                                             \
            = SCOREP_Hashtab_CreateSize( tablesize,                             \
                                       &SCOREP_Hashtab_HashInt64,               \
                                       &SCOREP_Hashtab_CompareInt64 );          \
    }                                                                         \
    else if ( sizeof( SCOREP_ ## Type ## Handle ) == 4 )                        \
    {                                                                         \
        map-> type ## _table_cube                                             \
             = SCOREP_Hashtab_CreateSize( tablesize,                            \
                                        &SCOREP_Hashtab_HashInt32,              \
                                        &SCOREP_Hashtab_CompareInt32 );         \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        SCOREP_ASSERT( false );                                                 \
        goto cleanup;                                                         \
    }                                                                         \
    if ( map-> type ## _table_cube == NULL )                                  \
    {                                                                         \
        SCOREP_ERROR_POSIX( "Unable to create " #type " mapping table" );       \
        goto cleanup;                                                         \
    }                                                                         \
    map-> type ## _table_scorep                                                 \
        = SCOREP_Hashtab_CreateSize( tablesize,                                 \
                                   &SCOREP_Hashtab_HashPointer,                 \
                                   &SCOREP_Hashtab_ComparePointer );            \
    if ( map-> type ## _table_scorep == NULL )                                  \
    {                                                                         \
        SCOREP_ERROR_POSIX( "Unable to create " #type " mapping table" );       \
        goto cleanup;                                                         \
    }
/* *INDENT-ON* */

/* Creates a new scorep_cube4_definitions_map. */
scorep_cube4_definitions_map*
scorep_cube4_create_definitions_map()
{
    scorep_cube4_definitions_map* map = NULL;

    /* Allocate memory for the struct */
    map = ( scorep_cube4_definitions_map* )malloc( sizeof( scorep_cube4_definitions_map ) );
    if ( map == NULL )
    {
        SCOREP_ERROR_POSIX( "Unable to create mapping struct" );
        return NULL;
    }

    /* Initialize with NULL */
    map->region_table_cube     = NULL;
    map->metric_table_cube     = NULL;
    map->callpath_table_cube   = NULL;
    map->region_table_scorep   = NULL;
    map->metric_table_scorep   = NULL;
    map->callpath_table_scorep = NULL;

    /* Initialize region table */
    SCOREP_CUBE4_INIT_MAP( Region, region, 128 )

    /* Initialize metric table */
    SCOREP_CUBE4_INIT_MAP( Counter, metric, 8 )

    /* Initialize callpath table */
    SCOREP_CUBE4_INIT_MAP( Callpath, callpath, 256 )

    return map;

cleanup:
    if ( map->region_table_cube != NULL )
    {
        SCOREP_Hashtab_Free( map->region_table_cube );
    }
    if ( map->metric_table_cube != NULL )
    {
        SCOREP_Hashtab_Free( map->metric_table_cube );
    }
    if ( map->callpath_table_cube != NULL )
    {
        SCOREP_Hashtab_Free( map->callpath_table_cube );
    }
    if ( map->region_table_scorep != NULL )
    {
        SCOREP_Hashtab_Free( map->region_table_scorep );
    }
    if ( map->metric_table_scorep != NULL )
    {
        SCOREP_Hashtab_Free( map->metric_table_scorep );
    }
    if ( map->callpath_table_scorep != NULL )
    {
        SCOREP_Hashtab_Free( map->callpath_table_scorep );
    }
    free( map );
    return NULL;
}

#define SCOREP_Hashtab_DeleteRegionHandle SCOREP_Hashtab_DeleteInt64
#define SCOREP_Hashtab_DeleteMetricHandle SCOREP_Hashtab_DeleteInt64
#define SCOREP_Hashtab_DeleteCallpathHandle SCOREP_Hashtab_DeleteInt64

/* Deletes the scorep_cube4_definitions_map */
void
scorep_cube4_delete_definitions_map( scorep_cube4_definitions_map* map )
{
    /* Because the scorep handle is only duplicated once for both mappings, it
       must only be deleted once.
     */
    SCOREP_Hashtab_FreeAll( map->region_table_cube,
                            SCOREP_Hashtab_DeleteRegionHandle,
                            SCOREP_Hashtab_DeleteNone );
    SCOREP_Hashtab_FreeAll( map->metric_table_cube,
                            SCOREP_Hashtab_DeleteMetricHandle,
                            SCOREP_Hashtab_DeleteNone );
    SCOREP_Hashtab_FreeAll( map->callpath_table_cube,
                            SCOREP_Hashtab_DeleteCallpathHandle,
                            SCOREP_Hashtab_DeleteNone );

    SCOREP_Hashtab_Free( map->region_table_scorep );
    SCOREP_Hashtab_Free( map->metric_table_scorep );
    SCOREP_Hashtab_Free( map->callpath_table_scorep );

    free( map );
}

/* ****************************************************************************
 * Add entries
 *****************************************************************************/
void
scorep_cube4_add_region_mapping( scorep_cube4_definitions_map* map,
                                 cube_region*                  cube_handle,
                                 SCOREP_RegionHandle           scorep_handle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_RegionHandle* scorep_copy = malloc( sizeof( SCOREP_RegionHandle ) );
    *scorep_copy = scorep_handle;

    /* Store handle in hashtable */
    SCOREP_Hashtab_Insert( map->region_table_cube, scorep_copy,
                           ( void* )cube_handle, NULL );
    SCOREP_Hashtab_Insert( map->region_table_scorep, ( void* )cube_handle,
                           scorep_copy, NULL );
}

void
scorep_cube4_add_callpath_mapping( scorep_cube4_definitions_map* map,
                                   cube_cnode*                   cube_handle,
                                   SCOREP_CallpathHandle         scorep_handle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_CallpathHandle* scorep_copy = malloc( sizeof( SCOREP_CallpathHandle ) );
    *scorep_copy = scorep_handle;

    /* Store handle in hashtable */
    SCOREP_Hashtab_Insert( map->callpath_table_cube, scorep_copy,
                           ( void* )cube_handle, NULL );
    SCOREP_Hashtab_Insert( map->callpath_table_scorep, ( void* )cube_handle,
                           scorep_copy, NULL );
}

void
scorep_cube4_add_metric_mapping( scorep_cube4_definitions_map* map,
                                 cube_metric*                  cube_handle,
                                 SCOREP_CounterHandle          scorep_handle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_CounterHandle* scorep_copy = malloc( sizeof( SCOREP_CounterHandle ) );
    *scorep_copy = scorep_handle;

    /* Store handle in hashtable */
    SCOREP_Hashtab_Insert( map->metric_table_cube, scorep_copy,
                           ( void* )cube_handle, NULL );
    SCOREP_Hashtab_Insert( map->metric_table_scorep, ( void* )cube_handle,
                           scorep_copy, NULL );
}

/* ****************************************************************************
 * Get mappings
 *****************************************************************************/

/* *INDENT-OFF* */
#define SCOREP_GET_CUBE_MAPPING( ret_type, type, Type )                         \
ret_type *                                                                    \
scorep_get_cube4_ ## type (scorep_cube4_definitions_map* map,                     \
                         SCOREP_ ## Type ## Handle     handle )                 \
{                                                                             \
    SCOREP_Hashtab_Entry* entry = NULL;                                         \
    entry = SCOREP_Hashtab_Find( map->type ## _table_cube,                      \
                               &handle, NULL );                               \
    if ( entry == NULL )                                                      \
    {                                                                         \
        return NULL;                                                          \
    }                                                                         \
    return ( ret_type *) entry->value;                                        \
}

#define SCOREP_GET_SCOREP_MAPPING( in_type, type, Type, TYPE )                    \
SCOREP_ ## Type ## Handle                                                       \
scorep_get_ ## type ## _from_cube4 (scorep_cube4_definitions_map* map,            \
                                  in_type *                   handle)         \
{                                                                             \
    SCOREP_Hashtab_Entry* entry = NULL;                                         \
    entry = SCOREP_Hashtab_Find( map->type ## _table_scorep,                      \
                               handle, NULL );                                \
    if ( entry == NULL )                                                      \
    {                                                                         \
        return SCOREP_INVALID_ ## TYPE;                                         \
    }                                                                         \
    return *( SCOREP_ ## Type ## Handle *) entry->value;                        \
}
/* *INDENT-ON* */

SCOREP_GET_CUBE_MAPPING( cube_metric, metric, Counter );

SCOREP_GET_CUBE_MAPPING( cube_region, region, Region );

SCOREP_GET_CUBE_MAPPING( cube_cnode, callpath, Callpath );

SCOREP_GET_SCOREP_MAPPING( cube_metric, metric, Counter, COUNTER );

SCOREP_GET_SCOREP_MAPPING( cube_region, region, Region, REGION );

SCOREP_GET_SCOREP_MAPPING( cube_cnode, callpath, Callpath, CALLPATH );

uint64_t
scorep_cube4_get_number_of_callpathes( scorep_cube4_definitions_map* map )
{
    return SCOREP_Hashtab_Size( map->callpath_table_cube );
}

/* ****************************************************************************
 * Internal definition writer functions
 *****************************************************************************/
static void
scorep_write_counter_definitions_to_cube4( cube_t*                       my_cube,
                                           scorep_cube4_definitions_map* map )
{
    /** TODO: Write real counter definitions */
    SCOREP_DEBUG_PRINTF( SCOREP_WARNING | SCOREP_DEBUG_DEFINITIONS,
                         "Counter definitions writing not yet implemented." );
    cube_metric* metric;

    /* Write fake defintions for count and implicit time */
    metric = cube_def_met( my_cube, "Time", "time", "FLOAT", "sec", "",
                           "@mirror@patterns-2.1.html#execution",
                           "Total CPU allocation time", NULL, CUBE_METRIC_INCLUSIVE );
    scorep_cube4_add_metric_mapping( map, metric, ( SCOREP_CounterHandle )1 );

    metric = cube_def_met( my_cube, "Visits", "visits", "INTEGER", "occ", "",
                           "http://www.cs.utk.edu/usr.html",
                           "Number of visits", NULL, CUBE_METRIC_INCLUSIVE );
    scorep_cube4_add_metric_mapping( map, metric, ( SCOREP_CounterHandle )2 );
}

static void
scorep_write_region_definitions_to_cube4( cube_t*                       my_cube,
                                          scorep_cube4_definitions_map* map )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, Region, region )
    {
        /* Collect necessary data */
        const char* region_name = SCOREP_HANDLE_DEREF( definition->name_handle,
                                                       String )->string_data;
        const char* adapter   = scorep_adapter_type_to_string( definition->adapter_type );
        const char* file_name = "";
        if ( definition->file_handle != SCOREP_INVALID_SOURCE_FILE )
        {
            SCOREP_HANDLE_DEREF( definition->file_handle, String )->string_data;
        }

        /* Register region to cube */
        cube_region* cube_handle = cube_def_region( my_cube,
                                                    region_name,
                                                    definition->begin_line,
                                                    definition->end_line,
                                                    "", /* URL */
                                                    adapter,
                                                    file_name );

        /* Create entry in mapping table */
        scorep_cube4_add_region_mapping( map, cube_handle, handle );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_callpath_definitions_to_cube4( cube_t*                       my_cube,
                                            scorep_cube4_definitions_map* map )
{
    int                   index  = 0;
    cube_cnode*           cnode  = NULL;
    cube_region*          region = NULL;
    cube_cnode*           parent = NULL;
    SCOREP_RegionHandle   scorep_region;
    SCOREP_CallpathHandle scorep_callpath;

    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, Callpath, callpath )
    {
        /* Collect necessary data */
        scorep_region   = definition->callpath_argument.region_handle;
        region          = scorep_get_cube4_region( map, scorep_region );
        scorep_callpath = definition->parent_callpath_handle;
        parent          = scorep_get_cube4_callpath( map, scorep_callpath );

        /* Register region to cube */
        cnode = cube_def_cnode( my_cube, region, parent, index );
        index++;

        /* Create entry in mapping table */
        scorep_cube4_add_callpath_mapping( map, cnode, handle );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_location_definitions_to_cube4( cube_t*                       my_cube,
                                            scorep_cube4_definitions_map* map )
{
    char          name[ 32 ];
    cube_machine* machine = cube_def_mach( my_cube, "machine name", "no description" );
    cube_node*    node    = cube_def_node( my_cube, "node name", machine );

    sprintf( name, "rank %d", SCOREP_Mpi_GetRank() );
    cube_process* process = cube_def_proc( my_cube, name,
                                           SCOREP_Mpi_GetRank(), node );
    cube_thread*  thread = NULL;
    int           index  = 0;

    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, Location, location )
    {
        sprintf( name, "thread %d", index ),
        thread = cube_def_thrd( my_cube, name,
                                definition->global_location_id, process );
        index++;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

/* ****************************************************************************
 * Main definition writer function
 *****************************************************************************/
void
scorep_write_definitions_to_cube4( cube_t*                       my_cube,
                                   scorep_cube4_definitions_map* map )
{
    scorep_write_counter_definitions_to_cube4( my_cube, map );
    scorep_write_region_definitions_to_cube4( my_cube, map );
    scorep_write_callpath_definitions_to_cube4( my_cube, map );
    scorep_write_location_definitions_to_cube4( my_cube, map );
}
