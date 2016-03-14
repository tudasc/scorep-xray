/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * Writes the definitions to Cube 4.
 */

#include <config.h>
#include <inttypes.h>
#include <assert.h>
#include "scorep_definition_cube4.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Metric_Management.h>

#include "scorep_ipc.h"
#include "scorep_types.h"
#include <SCOREP_Definitions.h>
#include <cubew_services.h>
#include <cubew_location.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

/* ****************************************************************************
 * Internal helper functions
 *****************************************************************************/

/**
 * Checks whether a @a metric is stored as dense metric.
 * @param metric  The metric which is checked.
 * @returns true if @a metric is stored as dense metric.
 */
static bool
is_dense_metric( SCOREP_MetricHandle metric )
{
    for ( uint64_t i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
    {
        SCOREP_MetricHandle strict
            = SCOREP_Metric_GetStrictlySynchronousMetricHandle( i );
        if ( SCOREP_MetricHandle_GetUnified( strict ) == metric )
        {
            return true;
        }
    }
    return false;
}

/**
 * Returns the cube_location_type for a given SCOREP_LocationType.
 */
static cube_location_type
convert_to_cube_location_type( SCOREP_LocationType location_type )
{
    switch ( location_type )
    {
        case SCOREP_LOCATION_TYPE_CPU_THREAD:
            return CUBE_LOCATION_TYPE_CPU_THREAD;
        case SCOREP_LOCATION_TYPE_GPU:
            return CUBE_LOCATION_TYPE_GPU;
        case SCOREP_LOCATION_TYPE_METRIC:
            return CUBE_LOCATION_TYPE_METRIC;
        case SCOREP_INVALID_LOCATION_TYPE:
            break;
            // By not having a default, we get a warning from the compiler if we add and forget to convert a type.
    }
    UTILS_BUG( "Can not convert location type to CUBE type." );
}

/**
 * Returns the cube_location_group_type for a given SCOREP_LocationGroupType
 */
static cube_location_group_type
convert_to_cube_location_group_type( SCOREP_LocationGroupType type )
{
    switch ( type )
    {
        case SCOREP_LOCATION_GROUP_TYPE_PROCESS:
            return CUBE_LOCATION_GROUP_TYPE_PROCESS;
        case SCOREP_INVALID_LOCATION_GROUP_TYPE:
            break;
    }

    UTILS_BUG( "Can not convert location group type to CUBE type." );
}

/* ****************************************************************************
 * Internal system tree representation
 *****************************************************************************/

/** Handles of both default metrics (number of visits and time) */
static cube_metric* time_sum_handle;
static cube_metric* time_max_handle;
static cube_metric* time_min_handle;
static cube_metric* visits_handle;
static cube_metric* hits_handle;

/**
   Node type definition for temporary internal system tree structure for Cube definition
   writing. It is needed to map Score-P and Cube system tree definitions.
 */
typedef struct scorep_cube_system_node
{
    struct scorep_cube_system_node* parent;          /**< Pointer to the parent node */
    SCOREP_SystemTreeNodeHandle     scorep_node;     /**< The Score-P handle */
    cube_system_tree_node*          cube_node;       /**< The Cube handle */
} scorep_cube_system_node;

/**
   Finds the node of the system tree mapping that belongs the the Score-P system tree
   node handle.
   @param system_tree Pointer to an array of scorep_cube_system_node nodes that contain
                      the system tree mapping structure.
   @param size        Number of elements in the system tree.
   @param node        Score-P system tree node handle that is seached in the mapping
                      tree.
   @returns A pointer to the mapping tree node that belong to @a node. If no matching
            Node is found, ir returns NULL.
 */
static scorep_cube_system_node*
find_system_node( scorep_cube_system_node* system_tree, uint32_t size,
                  SCOREP_SystemTreeNodeHandle node )
{
    assert( node );
    uint32_t pos = SCOREP_UNIFIED_HANDLE_DEREF( node, SystemTreeNode )->sequence_number;
    if ( pos >= size )
    {
        return NULL;
    }
    return &system_tree[ pos ];
}

/**
   Lookup the cube node definition for a Score-P system tree handle.
   In cases the Score-P system tree does not provide a node definition, a default node
   is defined to Cube and returned.
   @param my_cube     Pointer to the Cube instance.
   @param system_tree Pointer to an array of scorep_cube_system_node nodes that contain
                      the system tree mapping structure.
   @param size        Number of entries in @a system_tree.
   @param node        The Score-P handle of the node for which we look up the Cube
                      handle
   @returns A pointer to the Cube node definition.
 */
static cube_system_tree_node*
get_cube_node(  cube_t* my_cube,
                scorep_cube_system_node* system_tree,
                SCOREP_SystemTreeNodeHandle node, uint32_t size )
{
    /* Lookup the cube node  */
    scorep_cube_system_node* scorep_node = find_system_node( system_tree, size, node );
    assert( scorep_node );
    assert( scorep_node->cube_node );

    return scorep_node->cube_node;
}


/* ****************************************************************************
 * Initialization / Finalization
 *****************************************************************************/

/**
   @def SCOREP_CUBE4_INIT_MAP(Type, type, tablesize)
   Defines a macro for the initialization of the mapping tables for one type
   of handles. This macro is used in scorep_cube4_create_definitions_map.
   @param Type      The definitions type with first letter capitalized. Values
                    be 'Region', 'Metric', 'Callpath'.
   @param type      The definition type in small letters. Values can be
                    'region', 'metric', 'callpath'.
   @param tablesize Defines the number of slots for the mapping table.
 */
/* *INDENT-OFF* */
#define SCOREP_CUBE4_INIT_MAP(Type, type, tablesize)                          \
    if ( sizeof( SCOREP_ ## Type ## Handle ) == 8 )                           \
    {                                                                         \
        map-> type ## _table_cube                                             \
            = SCOREP_Hashtab_CreateSize( tablesize,                           \
                                       &SCOREP_Hashtab_HashInt64,             \
                                       &SCOREP_Hashtab_CompareInt64 );        \
    }                                                                         \
    else if ( sizeof( SCOREP_ ## Type ## Handle ) == 4 )                      \
    {                                                                         \
        map-> type ## _table_cube                                             \
             = SCOREP_Hashtab_CreateSize( tablesize,                          \
                                        &SCOREP_Hashtab_HashInt32,            \
                                        &SCOREP_Hashtab_CompareInt32 );       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        UTILS_ASSERT( false );                                                \
        goto cleanup;                                                         \
    }                                                                         \
    if ( map-> type ## _table_cube == NULL )                                  \
    {                                                                         \
        UTILS_ERROR_POSIX( "Unable to create " #type " mapping table" );      \
        goto cleanup;                                                         \
    }                                                                         \
    map-> type ## _table_scorep                                               \
        = SCOREP_Hashtab_CreateSize( tablesize,                               \
                                   &SCOREP_Hashtab_HashPointer,               \
                                   &SCOREP_Hashtab_ComparePointer );          \
    if ( map-> type ## _table_scorep == NULL )                                \
    {                                                                         \
        UTILS_ERROR_POSIX( "Unable to create " #type " mapping table" );      \
        goto cleanup;                                                         \
    }
/* *INDENT-ON* */

/* Creates a new scorep_cube4_definitions_map. */
scorep_cube4_definitions_map*
scorep_cube4_create_definitions_map( void )
{
    scorep_cube4_definitions_map* map = NULL;

    /* Allocate memory for the struct */
    map = ( scorep_cube4_definitions_map* )
          malloc( sizeof( scorep_cube4_definitions_map ) );
    if ( map == NULL )
    {
        UTILS_ERROR_POSIX( "Unable to create mapping struct" );
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
    SCOREP_CUBE4_INIT_MAP( Metric, metric, 8 )

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

#define SCOREP_Hashtab_DeleteRegionHandle SCOREP_Hashtab_DeleteFree
#define SCOREP_Hashtab_DeleteMetricHandle SCOREP_Hashtab_DeleteFree
#define SCOREP_Hashtab_DeleteCallpathHandle SCOREP_Hashtab_DeleteFree

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
static void
add_region_mapping( scorep_cube4_definitions_map* map,
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

static void
add_callpath_mapping( scorep_cube4_definitions_map* map,
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

static void
add_metric_mapping( scorep_cube4_definitions_map* map,
                    cube_metric*                  cube_handle,
                    SCOREP_MetricHandle           scorep_handle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_MetricHandle* scorep_copy = malloc( sizeof( SCOREP_MetricHandle ) );
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
#define SCOREP_GET_CUBE_MAPPING( ret_type, type, Type )                       \
ret_type *                                                                    \
scorep_get_cube4_ ## type (scorep_cube4_definitions_map* map,                 \
                         SCOREP_ ## Type ## Handle     handle )               \
{                                                                             \
    SCOREP_Hashtab_Entry* entry = NULL;                                       \
    entry = SCOREP_Hashtab_Find( map->type ## _table_cube,                    \
                               &handle, NULL );                               \
    if ( entry == NULL )                                                      \
    {                                                                         \
        return NULL;                                                          \
    }                                                                         \
    return ( ret_type *) entry->value;                                        \
}

#define SCOREP_GET_SCOREP_MAPPING( in_type, type, Type, TYPE )                \
SCOREP_ ## Type ## Handle                                                     \
scorep_get_ ## type ## _from_cube4 (scorep_cube4_definitions_map* map,        \
                                  in_type *                   handle)         \
{                                                                             \
    SCOREP_Hashtab_Entry* entry = NULL;                                       \
    entry = SCOREP_Hashtab_Find( map->type ## _table_scorep,                  \
                               handle, NULL );                                \
    if ( entry == NULL )                                                      \
    {                                                                         \
        return SCOREP_INVALID_ ## TYPE;                                       \
    }                                                                         \
    return *( SCOREP_ ## Type ## Handle *) entry->value;                      \
}
/* *INDENT-ON* */

SCOREP_GET_CUBE_MAPPING( cube_metric, metric, Metric )

SCOREP_GET_CUBE_MAPPING( cube_region, region, Region )

SCOREP_GET_CUBE_MAPPING( cube_cnode, callpath, Callpath )

SCOREP_GET_SCOREP_MAPPING( cube_metric, metric, Metric, METRIC )

SCOREP_GET_SCOREP_MAPPING( cube_region, region, Region, REGION )

SCOREP_GET_SCOREP_MAPPING( cube_cnode, callpath, Callpath, CALLPATH )

uint64_t
scorep_cube4_get_number_of_callpathes( scorep_cube4_definitions_map* map )
{
    return SCOREP_Hashtab_Size( map->callpath_table_cube );
}

cube_metric*
scorep_get_visits_handle( void )
{
    return visits_handle;
}


cube_metric*
scorep_get_hits_handle( void )
{
    return hits_handle;
}


cube_metric*
scorep_get_sum_time_handle( void )
{
    return time_sum_handle;
}

cube_metric*
scorep_get_max_time_handle( void )
{
    return time_max_handle;
}

cube_metric*
scorep_get_min_time_handle( void )
{
    return time_min_handle;
}

/* ****************************************************************************
 * Internal definition writer functions
 *****************************************************************************/

/**
   Writes metric definitions to Cube. The new Cube definitions are added to the
   mapping table @a map.
   @param my_cube Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param map     Pointer to mapping instance to map Score-P und Cube definitions.
 */
static void
write_metric_definitions( cube_t*                       my_cube,
                          SCOREP_DefinitionManager*     manager,
                          scorep_cube4_definitions_map* map,
                          bool                          write_task_metrics,
                          bool                          write_tuples )
{
    cube_metric* cube_handle;

    /* Add default profiling metrics for number of visits and implicit time */
    visits_handle = cube_def_met( my_cube, "Visits", "visits", "UINT64", "occ", "",
                                  "@mirror@scorep_metrics-" PACKAGE_VERSION ".html#visits",
                                  "Number of visits", NULL, CUBE_METRIC_EXCLUSIVE );

    if ( SCOREP_IsUnwindingEnabled() )
    {
        hits_handle = cube_def_met( my_cube, "Hits", "hits", "UINT64", "occ", "",
                                    "@mirror@scorep_metrics-" PACKAGE_VERSION ".html#hits",
                                    "Number of hits", NULL, CUBE_METRIC_EXCLUSIVE );
    }

    time_sum_handle = cube_def_met( my_cube, "Time", "time", "DOUBLE", "sec", "",
                                    "@mirror@scorep_metrics-" PACKAGE_VERSION ".html#time",
                                    "Total CPU allocation time", NULL,
                                    CUBE_METRIC_INCLUSIVE );

    time_min_handle = cube_def_met( my_cube, "Minimum Inclusive Time", "min_time",
                                    "MINDOUBLE", "sec", "",
                                    "", "Minimum inclusive CPU allocation time",
                                    NULL, CUBE_METRIC_EXCLUSIVE );

    time_max_handle = cube_def_met( my_cube, "Maximum Inclusive Time", "max_time",
                                    "MAXDOUBLE", "sec", "",
                                    "", "Maximum inclusive CPU allocation time",
                                    NULL, CUBE_METRIC_EXCLUSIVE );

    SCOREP_MetricDef*   metric_definition;
    char*               metric_name;
    char*               metric_unit;
    char*               metric_description;
    char*               data_type;
    bool                free_unit;
    enum CubeMetricType cube_metric_type;

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Metric, metric )
    {
        /* Collect necessary data */
        metric_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                   String )->string_data;
        metric_unit = SCOREP_UNIFIED_HANDLE_DEREF( definition->unit_handle,
                                                   String )->string_data;
        metric_description = SCOREP_UNIFIED_HANDLE_DEREF( definition->description_handle,
                                                          String )->string_data;

        free_unit = false;
        if ( definition->exponent != 0 )
        {
            free_unit = true;
            char*    unit = ( char* )malloc( strlen( metric_unit ) + 32 );
            uint32_t base = ( definition->base == SCOREP_METRIC_BASE_BINARY ? 2 : 10 );
            sprintf( unit, "%u^%" PRIi64 " %s", base, definition->exponent, metric_unit );
            metric_unit = unit;
        }

        if ( write_tuples && !is_dense_metric( handle ) )
        {
            data_type = "TAU_ATOMIC";
        }
        else
        {
            switch ( definition->value_type )
            {
                case SCOREP_METRIC_VALUE_INT64:
                    data_type = "INT64";
                    break;
                case SCOREP_METRIC_VALUE_UINT64:
                    data_type = "UINT64";
                    break;
                case SCOREP_METRIC_VALUE_DOUBLE:
                    data_type = "DOUBLE";
                    break;
                default:
                    UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                                 "Metric '%s' has unknown value type.",
                                 metric_name );
                    continue;
            }
        }

        switch ( definition->profiling_type )
        {
            case SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE:
                cube_metric_type = CUBE_METRIC_EXCLUSIVE;
                break;
            case SCOREP_METRIC_PROFILING_TYPE_INCLUSIVE:
                cube_metric_type = CUBE_METRIC_INCLUSIVE;
                break;
            case SCOREP_METRIC_PROFILING_TYPE_SIMPLE:
                cube_metric_type = CUBE_METRIC_SIMPLE;
                break;
            case SCOREP_METRIC_PROFILING_TYPE_MIN:
                cube_metric_type = CUBE_METRIC_EXCLUSIVE;
                if ( !write_tuples )
                {
                    data_type = "MINDOUBLE";
                }
                break;
            case SCOREP_METRIC_PROFILING_TYPE_MAX:
                cube_metric_type = CUBE_METRIC_EXCLUSIVE;
                if ( !write_tuples )
                {
                    data_type = "MAXDOUBLE";
                }
                break;
            default:
                UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                             "Metric '%s' has unknown profiling type.",
                             metric_name );
                continue;
        }

        if ( ( definition->source_type != SCOREP_METRIC_SOURCE_TYPE_TASK ) ||
             write_task_metrics )
        {
            cube_handle = cube_def_met( my_cube, metric_name, metric_name, data_type,
                                        metric_unit, "", "", metric_description, NULL,
                                        cube_metric_type );

            add_metric_mapping( map, cube_handle, handle );
        }

        if ( free_unit )
        {
            free( metric_unit );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/**
   Writes region definitions to Cube. The new Cube definitions are added to the
   mapping table @a map.
   @param my_cube Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param map     Pointer to mapping instance to map Score-P und Cube definitions.
 */
static void
write_region_definitions( cube_t*                       my_cube,
                          SCOREP_DefinitionManager*     manager,
                          scorep_cube4_definitions_map* map )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Region, region )
    {
        /* Collect necessary data */
        const char* region_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                               String )->string_data;
        const char* canonical_region_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->canonical_name_handle,
                                                                         String )->string_data;
        const char* paradigm  = scorep_paradigm_type_to_string( definition->paradigm_type );
        const char* role      = scorep_region_type_to_string( definition->region_type );
        const char* file_name = "";
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            file_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->file_name_handle,
                                                     String )->string_data;
        }

        /* Register region to cube */
        cube_region* cube_handle = cube_def_region( my_cube,
                                                    region_name,
                                                    canonical_region_name,
                                                    paradigm,
                                                    role,
                                                    definition->begin_line,
                                                    definition->end_line,
                                                    "", /* URL */
                                                    "",
                                                    file_name );

        /* Create entry in mapping table */
        add_region_mapping( map, cube_handle, handle );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/**
   Writes callpath definitions to Cube. The new Cube definitions are added to the
   mapping table @a map.
   @param my_cube Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param map     Pointer to mapping instance to map Score-P und Cube definitions.
 */
static void
write_callpath_definitions( cube_t*                       my_cube,
                            SCOREP_DefinitionManager*     manager,
                            scorep_cube4_definitions_map* map )
{
    cube_cnode*           cnode  = NULL;
    cube_region*          region = NULL;
    cube_cnode*           parent = NULL;
    SCOREP_RegionHandle   scorep_region;
    SCOREP_CallpathHandle scorep_callpath;

    /* We must write the callpathes in the order that the sequence_number of the
       unified definitions go from 0 to n-1. The unified definitions on rank zero
       are in the correct order.
     */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Callpath, callpath )
    {
        /* Collect necessary data */
        scorep_region   = definition->callpath_argument.region_handle;
        region          = scorep_get_cube4_region( map, scorep_region );
        scorep_callpath = definition->parent_callpath_handle;
        parent          = scorep_get_cube4_callpath( map, scorep_callpath );

        /* Register region to cube */
        cnode = cube_def_cnode( my_cube, region, parent );

        /* Create entry in mapping table */
        add_callpath_mapping( map, cnode, handle );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/**
   Writes the inner nodes of the Score-P system tree definitions to Cube.
   Because Cube assumes a fixed hierarchy, the system tree is compressed in the two Cube
   levels machine and node. In order to have a mapping between Cube and Score-P
   definitions a tree is constructed where each node contains the respective definitions.
   The tree's nodes are stored in a newly created array, which is returned from the
   function.
   @param my_cube Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @returns A Pointer to the array which contains the mapping system tree. This array
            must be freed by the caller of this function.
 */
static scorep_cube_system_node*
write_system_tree( cube_t*                   my_cube,
                   SCOREP_DefinitionManager* manager )
{
    uint32_t                 nodes       = manager->system_tree_node.counter;
    scorep_cube_system_node* system_tree = ( scorep_cube_system_node* )
                                           calloc( nodes, sizeof( scorep_cube_system_node ) );

    if ( system_tree == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for system tree translation." );
        return NULL;
    }

    char*  display_name          = 0;
    size_t display_name_capacity = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, SystemTreeNode, system_tree_node )
    {
        const uint32_t pos   = definition->sequence_number;
        const char*    class = SCOREP_UNIFIED_HANDLE_DEREF( definition->class_handle,
                                                            String )->string_data;
        const char* name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                        String )->string_data;

        size_t class_length        = strlen( class );
        size_t name_length         = strlen( name );
        size_t display_name_length = class_length + name_length + 2;
        if ( display_name_length > display_name_capacity )
        {
            display_name          = realloc( display_name, display_name_length );
            display_name_capacity = display_name_length;
            if ( !display_name )
            {
                UTILS_FATAL( "Failed to allocate memory for system tree display name generation." );
            }
        }
        strncpy( display_name, class, class_length );
        display_name[ class_length ] = ' ';
        strncpy( display_name + class_length + 1, name, name_length );
        display_name[ class_length + 1 + name_length ] = '\0';

        assert( pos < nodes );
        cube_system_tree_node* parent = NULL;
        system_tree[ pos ].scorep_node = handle;
        system_tree[ pos ].cube_node   = NULL;
        system_tree[ pos ].parent      = NULL;
        if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            system_tree[ pos ].parent = find_system_node( system_tree, nodes,
                                                          definition->parent_handle );
            parent = system_tree[ pos ].parent->cube_node;
        }

        system_tree[ pos ].cube_node =
            cube_def_system_tree_node( my_cube, display_name, "", class, parent );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    free( display_name );
    return system_tree;
}

/**
   Writes location group defintions to Cube.
   @param my_cube Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param ranks   Number of MPI ranks. It must equal the number of array elements in
                  @a threads.
   @param offsets Array of the offsets of threads in each rank. The ith entry contains
                  the sum of all threads of lower ranks. The number of elements must equal
                  @a ranks.
 */
static cube_location_group**
write_location_group_definitions( cube_t*                   my_cube,
                                  SCOREP_DefinitionManager* manager,
                                  uint32_t                  ranks )
{
    cube_location_group** processes =
        ( cube_location_group** )calloc( ranks, sizeof( cube_location_group* ) );
    assert( processes );
    scorep_cube_system_node* system_tree = write_system_tree( my_cube, manager );
    assert( system_tree );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, LocationGroup, location_group )
    {
        uint32_t   rank = definition->global_location_group_id;
        cube_node* node = get_cube_node( my_cube, system_tree, definition->parent,
                                         manager->system_tree_node.counter );

        const char* name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                        String )->string_data;
        cube_location_group_type type =
            convert_to_cube_location_group_type( definition->location_group_type );

        processes[ rank ] = cube_def_location_group( my_cube, name, rank, type, node );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    free( system_tree );
    return processes;
}

/**
   Writes location_definitions to Cube.
   @param my_cube   Pointer to Cube instance.
   @param manager   Pointer to Score-P definition manager with unified definitions.
   @param ranks     The number of processes.
   @param offsets   Array of the offsets of threads in each rank. The ith entry contains
                    the sum of all threads of lower ranks. The number of elements must
                    equal @a ranks.
   @retruns an array of cube_location pointers where the sequence number of the Score-P
            definitions is the index to the cube location.
 */
static cube_location**
write_location_definitions( cube_t*                   my_cube,
                            SCOREP_DefinitionManager* manager,
                            uint32_t                  ranks,
                            uint64_t                  number_of_threads )
{
    /* Counts the number of threads already registered for each rank */
    uint32_t* threads = calloc( ranks, sizeof( uint32_t ) );
    assert( threads );

    /* Location group (processes) mapping of global ids to cube defintions */
    cube_location_group** processes =
        write_location_group_definitions( my_cube, manager, ranks );

    /* Location mapping of global ids to cube defintion */
    cube_location** locations = calloc( number_of_threads,
                                        sizeof( cube_location* ) );
    assert( locations );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Location, location )
    {
        uint32_t    parent_id = definition->location_group_id;
        const char* name      = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                             String )->string_data;
        cube_location_type type =
            convert_to_cube_location_type( definition->location_type );

        locations[ definition->sequence_number ] =
            cube_def_location( my_cube,
                               name,
                               threads[ parent_id ],
                               type,
                               processes[ parent_id ] );
        threads[ parent_id ]++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    free( threads );
    free( processes );
    return locations;
}

static void
scorep_write_cube_location_property( cube_t*                   my_cube,
                                     SCOREP_DefinitionManager* manager,
                                     cube_location**           location_map )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, LocationProperty, location_property )
    {
        const char* name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                        String )->string_data;
        const char* value = SCOREP_UNIFIED_HANDLE_DEREF( definition->value_handle,
                                                         String )->string_data;
        uint64_t id = SCOREP_UNIFIED_HANDLE_DEREF( definition->location_handle,
                                                   Location )->sequence_number;

        cube_location_def_attr( location_map[ id ], name, value );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/* ****************************************************************************
 * Main definition writer function
 *****************************************************************************/
void
scorep_write_definitions_to_cube4( cube_t*                       myCube,
                                   scorep_cube4_definitions_map* map,
                                   uint32_t                      nRanks,
                                   uint64_t                      nLocations,
                                   bool                          writeTaskMetrics,
                                   bool                          writeTuples )
{
    /* The unification is always processed, even in serial case. Thus, we have
       always access to the unified definitions on rank 0.
       In non-mpi case SCOREP_Ipc_GetRank() returns always 0. Thus, we need only
       to test for the rank. */
    SCOREP_DefinitionManager* manager = scorep_unified_definition_manager;
    if ( SCOREP_Ipc_GetRank() != 0 )
    {
        return;
    }
    assert( scorep_unified_definition_manager );

    write_metric_definitions( myCube, manager, map, writeTaskMetrics, writeTuples );
    write_region_definitions( myCube, manager, map );
    write_callpath_definitions( myCube, manager, map );
    cube_location** location_map =
        write_location_definitions( myCube, manager, nRanks, nLocations );
    scorep_write_cube_location_property( myCube, manager, location_map );
    free( location_map );
}
