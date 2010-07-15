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
 * @file silc_definition_cube4.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 * Writes the definitions to Cube 4.
 */

#include "SILC_Memory.h"
#include "SILC_Definitions.h"

#include "silc_mpi.h"
#include "silc_types.h"
#include "silc_definitions.h"
#include "silc_definition_cube4.h"

#include "cube.h"
#include "cubew.h"
#include "services.h"

extern SILC_DefinitionManager silc_definition_manager;

/* ****************************************************************************
 * Initialization / Finalization
 *****************************************************************************/

/* *INDENT-OFF* */
#define SILC_CUBE4_INIT_MAP(Type, type, tablesize)                            \
    if ( sizeof( SILC_ ## Type ## Handle ) == 8 )                             \
    {                                                                         \
        map-> type ## _table_cube                                             \
            = SILC_Hashtab_CreateSize( tablesize,                             \
                                       &SILC_Hashtab_HashInt64,               \
                                       &SILC_Hashtab_CompareInt64 );          \
    }                                                                         \
    else if ( sizeof( SILC_ ## Type ## Handle ) == 4 )                        \
    {                                                                         \
        map-> type ## _table_cube                                             \
             = SILC_Hashtab_CreateSize( tablesize,                            \
                                        &SILC_Hashtab_HashInt32,              \
                                        &SILC_Hashtab_CompareInt32 );         \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        SILC_ASSERT( false );                                                 \
        goto cleanup;                                                         \
    }                                                                         \
    if ( map-> type ## _table_cube == NULL )                                  \
    {                                                                         \
        SILC_ERROR_POSIX( "Unable to create " #type " mapping table" );       \
        goto cleanup;                                                         \
    }                                                                         \
    map-> type ## _table_silc                                                 \
        = SILC_Hashtab_CreateSize( tablesize,                                 \
                                   &SILC_Hashtab_HashPointer,                 \
                                   &SILC_Hashtab_ComparePointer );            \
    if ( map-> type ## _table_silc == NULL )                                  \
    {                                                                         \
        SILC_ERROR_POSIX( "Unable to create " #type " mapping table" );       \
        goto cleanup;                                                         \
    }
/* *INDENT-ON* */

silc_cube4_definitions_map*
silc_cube4_create_definitions_map()
{
    silc_cube4_definitions_map* map = NULL;

    /* Allocate memory for the struct */
    map = ( silc_cube4_definitions_map* )malloc( sizeof( silc_cube4_definitions_map ) );
    if ( map == NULL )
    {
        SILC_ERROR_POSIX( "Unable to create mapping struct" );
        return NULL;
    }

    /* Initialize with NULL */
    map->region_table_cube   = NULL;
    map->metric_table_cube   = NULL;
    map->callpath_table_cube = NULL;
    map->region_table_silc   = NULL;
    map->metric_table_silc   = NULL;
    map->callpath_table_silc = NULL;

    /* Initialize region table */
    SILC_CUBE4_INIT_MAP( Region, region, 128 )

    /* Initialize metric table */
    SILC_CUBE4_INIT_MAP( Counter, metric, 8 )

    /* Initialize callpath table */
    SILC_CUBE4_INIT_MAP( Callpath, callpath, 256 )

    return map;

cleanup:
    if ( map->region_table_cube != NULL )
    {
        SILC_Hashtab_Free( map->region_table_cube );
    }
    if ( map->metric_table_cube != NULL )
    {
        SILC_Hashtab_Free( map->metric_table_cube );
    }
    if ( map->callpath_table_cube != NULL )
    {
        SILC_Hashtab_Free( map->callpath_table_cube );
    }
    if ( map->region_table_silc != NULL )
    {
        SILC_Hashtab_Free( map->region_table_silc );
    }
    if ( map->metric_table_silc != NULL )
    {
        SILC_Hashtab_Free( map->metric_table_silc );
    }
    if ( map->callpath_table_silc != NULL )
    {
        SILC_Hashtab_Free( map->callpath_table_silc );
    }
    free( map );
    return NULL;
}

/* ****************************************************************************
 * Add entries
 *****************************************************************************/
void
silc_cube4_add_region_mapping( silc_cube4_definitions_map* map,
                               cube_region*                cube_handle,
                               SILC_RegionHandle           silc_handle )
{
    /* Create copy of the SILC region handle on the heap */
    SILC_RegionHandle* silc_copy = malloc( sizeof( SILC_RegionHandle ) );
    *silc_copy = silc_handle;

    /* Store handle in hashtable */
    SILC_Hashtab_Insert( map->region_table_cube, silc_copy,
                         ( void* )cube_handle, NULL );

    SILC_Hashtab_Insert( map->region_table_silc, ( void* )cube_handle,
                         silc_copy, NULL );
}

void
silc_cube4_add_callpath_mapping( silc_cube4_definitions_map* map,
                                 cube_cnode*                 cube_handle,
                                 SILC_CallpathHandle         silc_handle )
{
    /* Create copy of the SILC region handle on the heap */
    SILC_CallpathHandle* silc_copy = malloc( sizeof( SILC_CallpathHandle ) );
    *silc_copy = silc_handle;

    /* Store handle in hashtable */
    SILC_Hashtab_Insert( map->callpath_table_cube, silc_copy,
                         ( void* )cube_handle, NULL );

    SILC_Hashtab_Insert( map->callpath_table_silc, ( void* )cube_handle,
                         silc_copy, NULL );
}

void
silc_cube4_add_metric_mapping( silc_cube4_definitions_map* map,
                               cube_metric*                cube_handle,
                               SILC_CounterHandle          silc_handle )
{
    /* Create copy of the SILC region handle on the heap */
    SILC_CounterHandle* silc_copy = malloc( sizeof( SILC_CounterHandle ) );
    *silc_copy = silc_handle;

    /* Store handle in hashtable */
    SILC_Hashtab_Insert( map->callpath_table_cube, silc_copy,
                         ( void* )cube_handle, NULL );

    SILC_Hashtab_Insert( map->metric_table_silc, ( void* )cube_handle,
                         silc_copy, NULL );
}

/* ****************************************************************************
 * Get mappings
 *****************************************************************************/

/* *INDENT-OFF* */
#define SILC_GET_CUBE_MAPPING( ret_type, type, Type )                         \
ret_type *                                                                    \
silc_get_cube4_ ## type (silc_cube4_definitions_map* map,                     \
                         SILC_ ## Type ## Handle     handle )                 \
{                                                                             \
    SILC_Hashtab_Entry* entry = NULL;                                         \
    entry = SILC_Hashtab_Find( map->type ## _table_cube,                      \
                               &handle, NULL );                               \
    if ( entry == NULL ) { return NULL; }                                     \
    return ( ret_type *) entry->value;                                        \
}

#define SILC_GET_SILC_MAPPING( in_type, type, Type )                         \
SILC_ ## Type ## Handle                                                       \
silc_get_ ## type ## _from_cube4 (silc_cube4_definitions_map* map,            \
                                  in_type *                   handle)         \
{                                                                             \
    SILC_Hashtab_Entry* entry = NULL;                                         \
    entry = SILC_Hashtab_Find( map->type ## _table_silc,                      \
                               handle, NULL );                                \
    if ( entry == NULL ) { return NULL; }                                     \
    return *( SILC_ ## Type ## Handle *) entry->value;                        \
}
/* *INDENT-ON* */

SILC_GET_CUBE_MAPPING( cube_metric, metric, Counter );

SILC_GET_CUBE_MAPPING( cube_region, region, Region );

SILC_GET_CUBE_MAPPING( cube_cnode, callpath, Callpath );

SILC_GET_SILC_MAPPING( cube_metric, metric, Counter );

SILC_GET_SILC_MAPPING( cube_region, region, Region );

SILC_GET_SILC_MAPPING( cube_cnode, callpath, Callpath );

/* ****************************************************************************
 * Internal definition writer functions
 *****************************************************************************/
static void
silc_write_counter_definitions_to_cube4( cube_t*                     my_cube,
                                         silc_cube4_definitions_map* map )
{
    /** TODO: Write real counter definitions */
    SILC_DEBUG_PRINTF( SILC_WARNING | SILC_DEBUG_DEFINITIONS,
                       "Counter definitions writing not yet implemented." );
    cube_metric* metric;

    /* Write fake defintions for count and implicit time */
    metric = cube_def_met( my_cube, "Time", "time", "FLOAT", "sec", "",
                           "@mirror@patterns-2.1.html#execution",
                           "Total CPU allocation time", NULL, CUBE_METRIC_INCLUSIVE );
    silc_cube4_add_metric_mapping( map, metric, ( SILC_CounterHandle )1 );

    metric = cube_def_met( my_cube, "Visits", "visits", "INTEGER", "occ", "",
                           "http://www.cs.utk.edu/usr.html",
                           "Number of visits", NULL, CUBE_METRIC_INCLUSIVE );
    silc_cube4_add_metric_mapping( map, metric, ( SILC_CounterHandle )2 );
}

static void
silc_write_region_definitions_to_cube4( cube_t*                     my_cube,
                                        silc_cube4_definitions_map* map )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Region, region )
    {
        /* Collect necessary data */
        const char* region_name = SILC_HANDLE_DEREF( &definition->name_handle,
                                                     String )->string_data;
        const char* file_name = SILC_HANDLE_DEREF( &definition->file_handle,
                                                   String )->string_data;
        const char* adapter = silc_adapter_type_to_string( definition->adapter_type );

        /* Register region to cube */
        cube_region* cube_handle = cube_def_region( my_cube,
                                                    region_name,
                                                    definition->begin_line,
                                                    definition->end_line,
                                                    "", /* URL */
                                                    adapter,
                                                    file_name );

        /* Create entry in mapping table */
        silc_cube4_add_region_mapping( map, cube_handle, moveable );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}

static void
silc_write_callpath_definitions_to_cube4( cube_t*                     my_cube,
                                          silc_cube4_definitions_map* map )
{
    int                 index  = 0;
    cube_cnode*         cnode  = NULL;
    cube_region*        region = NULL;
    cube_cnode*         parent = NULL;
    SILC_RegionHandle   silc_region;
    SILC_CallpathHandle silc_callpath;

    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Callpath, callpath )
    {
        /* Collect necessary data */
        silc_region   = &definition->callpath_argument.region_handle;
        region        = silc_get_cube4_region( map, silc_region );
        silc_callpath = &definition->parent_callpath_handle;
        parent        = silc_get_cube4_callpath( map, silc_callpath );

        /* Register region to cube */
        cnode = cube_def_cnode( my_cube, region, parent, index );
        index++;

        /* Create entry in mapping table */
        silc_cube4_add_callpath_mapping( map, cnode, moveable );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}

static void
silc_write_location_definitions_to_cube4( cube_t*                     my_cube,
                                          silc_cube4_definitions_map* map )
{
    cube_machine* machine = cube_def_mach( my_cube, "no name", "no description" );
    cube_node*    node    = cube_def_node( my_cube, "no name", machine );
    cube_process* process = cube_def_proc( my_cube, "no name",
                                           SILC_Mpi_GetRank(), node );
    cube_thread*  thread = NULL;
    int           index  = 0;

    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Location, location )
    {
        thread = cube_def_thrd( my_cube, "no name",
                                definition->global_location_id, process );
        index++;
    }
    SILC_DEFINITION_FOREACH_WHILE();
}

/* ****************************************************************************
 * Main definition writer function
 *****************************************************************************/
void
silc_write_definitions_to_cube4( cube_t*                     my_cube,
                                 silc_cube4_definitions_map* map )
{
    silc_write_counter_definitions_to_cube4( my_cube, map );
    silc_write_region_definitions_to_cube4( my_cube, map );
    silc_write_callpath_definitions_to_cube4( my_cube, map );
    silc_write_location_definitions_to_cube4( my_cube, map );
}
