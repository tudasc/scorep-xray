/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2017, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Darmstadt, Germany
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
#include "scorep_definition_cube4.h"
#include "scorep_system_tree_sequence.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Metric_Management.h>

#include "scorep_environment.h"
#include "scorep_ipc.h"
#include "scorep_type_utils.h"
#include <SCOREP_Definitions.h>
#include <cubew_services.h>
#include <cubew_location.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#define LOCATION_NAME_BUFFER_LENGTH 32

/**
 * Retrieve rank information from mapped rank index in location group.
 * Used during writing of CUBE profiles.
 *
 * @param index                   Index in the COM_LOCATIONS group for non-MPI topologies.
 * @param topoHandle              Select the Group depending on the topology.
 *
 * @return                        The global rank of this location index.
 *
 */
uint32_t
SCOREP_Topologies_CoordRankFromGroupIndex( uint32_t                       index,
                                           SCOREP_CartesianTopologyHandle topoHandle );

/**
 * Retrieve thread information from mapped rank index in location group.
 * Used during writing of CUBE profiles.
 *
 * @param index                   Index in the COM_LOCATIONS group for non-MPI topologies.
 * @param topoHandle              Select the Group depending on topology type.
 *
 * @return                        The thread of this location index.
 *
 */
uint32_t
SCOREP_Topologies_CoordThreadFromGroupIndex( uint32_t                       index,
                                             SCOREP_CartesianTopologyHandle topoHandle );

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
convert_to_cube_location_type( SCOREP_LocationType locationType )
{
    switch ( locationType )
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
static cube_metric* num_threads_handle;
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
   Finds the node of the system tree mapping that belongs the Score-P system tree
   node handle.
   @param systemTree  Pointer to an array of scorep_cube_system_node nodes that contain
                      the system tree mapping structure.
   @param size        Number of elements in the system tree.
   @param node        Score-P system tree node handle that is seached in the mapping
                      tree.
   @returns A pointer to the mapping tree node that belong to @a node. If no matching
            Node is found, ir returns NULL.
 */
static scorep_cube_system_node*
find_system_node( scorep_cube_system_node*    systemTree,
                  uint32_t                    size,
                  SCOREP_SystemTreeNodeHandle node )
{
    UTILS_ASSERT( node );
    uint32_t pos = SCOREP_UNIFIED_HANDLE_DEREF( node, SystemTreeNode )->sequence_number;
    if ( pos >= size )
    {
        return NULL;
    }
    return &systemTree[ pos ];
}

/**
   Lookup the cube node definition for a Score-P system tree handle.
   In cases the Score-P system tree does not provide a node definition, a default node
   is defined to Cube and returned.
   @param systemTree  Pointer to an array of scorep_cube_system_node nodes that contain
                      the system tree mapping structure.
   @param size        Number of entries in @a system_tree.
   @param node        The Score-P handle of the node for which we look up the Cube
                      handle
   @returns A pointer to the Cube node definition.
 */
static cube_system_tree_node*
get_cube_node( scorep_cube_system_node*    systemTree,
               SCOREP_SystemTreeNodeHandle node,
               uint32_t                    size )
{
    /* Lookup the cube node  */
    scorep_cube_system_node* scorep_node = find_system_node( systemTree, size, node );
    UTILS_ASSERT( scorep_node );
    UTILS_ASSERT( scorep_node->cube_node );

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
                    cube_region*                  cubeHandle,
                    SCOREP_RegionHandle           scorepHandle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_RegionHandle* scorep_copy = malloc( sizeof( SCOREP_RegionHandle ) );
    *scorep_copy = scorepHandle;

    /* Store handle in hashtable */
    SCOREP_Hashtab_InsertPtr( map->region_table_cube,
                              scorep_copy,
                              ( void* )cubeHandle,
                              NULL );

    SCOREP_Hashtab_InsertHandle( map->region_table_scorep,
                                 ( void* )cubeHandle,
                                 scorepHandle,
                                 NULL );
}

static void
add_callpath_mapping( scorep_cube4_definitions_map* map,
                      cube_cnode*                   cubeHandle,
                      SCOREP_CallpathHandle         scorepHandle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_CallpathHandle* scorep_copy = malloc( sizeof( SCOREP_CallpathHandle ) );
    *scorep_copy = scorepHandle;

    /* Store handle in hashtable */
    SCOREP_Hashtab_InsertPtr( map->callpath_table_cube,
                              scorep_copy,
                              ( void* )cubeHandle,
                              NULL );

    SCOREP_Hashtab_InsertHandle( map->callpath_table_scorep,
                                 ( void* )cubeHandle,
                                 scorepHandle,
                                 NULL );
}

static void
add_metric_mapping( scorep_cube4_definitions_map* map,
                    cube_metric*                  cubeHandle,
                    SCOREP_MetricHandle           scorepHandle )
{
    /* Create copy of the SCOREP region handle on the heap */
    SCOREP_MetricHandle* scorep_copy = malloc( sizeof( SCOREP_MetricHandle ) );
    *scorep_copy = scorepHandle;

    /* Store handle in hashtable */
    SCOREP_Hashtab_InsertPtr( map->metric_table_cube,
                              scorep_copy,
                              ( void* )cubeHandle,
                              NULL );

    SCOREP_Hashtab_InsertHandle( map->metric_table_scorep,
                                 ( void* )cubeHandle,
                                 scorepHandle,
                                 NULL );
}

/* ****************************************************************************
 * Get mappings
 *****************************************************************************/

/* *INDENT-OFF* */
#define SCOREP_GET_CUBE_MAPPING( ret_type, type, Type )                       \
ret_type *                                                                    \
scorep_get_cube4_ ## type (scorep_cube4_definitions_map* map,                 \
                           SCOREP_ ## Type ## Handle     handle )             \
{                                                                             \
    SCOREP_Hashtab_Entry* entry = NULL;                                       \
    entry = SCOREP_Hashtab_Find( map->type ## _table_cube,                    \
                               &handle, NULL );                               \
    if ( entry == NULL )                                                      \
    {                                                                         \
        return NULL;                                                          \
    }                                                                         \
    return ( ret_type *) entry->value.ptr;                                    \
}

#define SCOREP_GET_SCOREP_MAPPING( in_type, type, Type, TYPE )                \
SCOREP_ ## Type ## Handle                                                     \
scorep_get_ ## type ## _from_cube4 (scorep_cube4_definitions_map* map,        \
                                    in_type *                     handle)     \
{                                                                             \
    SCOREP_Hashtab_Entry* entry = NULL;                                       \
    entry = SCOREP_Hashtab_Find( map->type ## _table_scorep,                  \
                               handle, NULL );                                \
    if ( entry == NULL )                                                      \
    {                                                                         \
        return SCOREP_INVALID_ ## TYPE;                                       \
    }                                                                         \
    return entry->value.handle;                                               \
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

cube_metric*
scorep_get_num_threads_handle( void )
{
    return num_threads_handle;
}

/* ****************************************************************************
 * Internal definition writer functions
 *****************************************************************************/

/**
   Writes metric definitions to Cube. The new Cube definitions are added to the
   mapping table @a map.
   @param myCube  Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param map     Pointer to mapping instance to map Score-P und Cube definitions.
 */
static void
write_metric_definitions( cube_t*                       myCube,
                          SCOREP_DefinitionManager*     manager,
                          scorep_cube4_definitions_map* map,
                          const scorep_cube_layout*     layout )
{
    cube_metric* cube_handle;
    const char*  data_type = ( layout->dense_metric_type == SCOREP_CUBE_DATA_SCALAR ?
                               "UINT64" : "TAU_ATOMIC" );

    /* Add default profiling metrics for number of visits and implicit time */
    if ( layout->metric_list & SCOREP_CUBE_METRIC_VISITS )
    {
        visits_handle = cube_def_met( myCube, "Visits", "visits", data_type, "occ", "",
                                      "@mirror@scorep_metrics-" PACKAGE_VERSION
                                      ".html#visits",
                                      "Number of visits", NULL, CUBE_METRIC_EXCLUSIVE );
    }

    if ( layout->metric_list & SCOREP_CUBE_METRIC_NUM_THREADS )
    {
        num_threads_handle = cube_def_met( myCube, "number of threads",
                                           "num_threads", "UINT64", "threads", "",
                                           "@mirror@scorep_metrics-" PACKAGE_VERSION
                                           ".html#visits",
                                           "Number of threads", NULL, CUBE_METRIC_EXCLUSIVE );
    }

    if ( layout->dense_metric_type == SCOREP_CUBE_DATA_SCALAR )
    {
        time_sum_handle = cube_def_met( myCube, "Time", "time", "DOUBLE", "sec", "",
                                        "@mirror@scorep_metrics-" PACKAGE_VERSION
                                        ".html#time",
                                        "Total CPU allocation time", NULL,
                                        CUBE_METRIC_INCLUSIVE );

        time_min_handle = cube_def_met( myCube, "Minimum Inclusive Time", "min_time",
                                        "MINDOUBLE", "sec", "",
                                        "", "Minimum inclusive CPU allocation time",
                                        NULL, CUBE_METRIC_EXCLUSIVE );

        time_max_handle = cube_def_met( myCube, "Maximum Inclusive Time", "max_time",
                                        "MAXDOUBLE", "sec", "",
                                        "", "Maximum inclusive CPU allocation time",
                                        NULL, CUBE_METRIC_EXCLUSIVE );

        if ( SCOREP_IsUnwindingEnabled() )
        {
            hits_handle = cube_def_met( myCube, "Hits", "hits", "UINT64", "occ", "",
                                        "@mirror@scorep_metrics-" PACKAGE_VERSION
                                        ".html#hits",
                                        "Number of hits", NULL, CUBE_METRIC_EXCLUSIVE );
        }
    }
    else
    {
        time_sum_handle = cube_def_met( myCube, "Time", "time", "TAU_ATOMIC", "sec", "",
                                        "@mirror@scorep_metrics-" PACKAGE_VERSION
                                        ".html#time",
                                        "Total CPU allocation time", NULL,
                                        CUBE_METRIC_EXCLUSIVE );

        if ( SCOREP_IsUnwindingEnabled() )
        {
            hits_handle = cube_def_met( myCube, "Hits", "hits", "TAU_ATOMIC", "occ", "",
                                        "@mirror@scorep_metrics-" PACKAGE_VERSION
                                        ".html#hits",
                                        "Number of hits", NULL, CUBE_METRIC_EXCLUSIVE );
        }
    }

    char*               metric_name;
    char*               metric_unit;
    char*               metric_description;
    bool                free_unit;
    bool                write_tuples;
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

        cube_metric*        parent_handle = NULL;
        SCOREP_MetricHandle parent        = definition->parent_handle;
        if ( definition->parent_handle != SCOREP_INVALID_METRIC )
        {
            parent_handle = scorep_get_cube4_metric( map, parent );
        }

        free_unit = false;
        if ( definition->exponent != 0 )
        {
            free_unit = true;
            char*    unit = ( char* )malloc( strlen( metric_unit ) + 32 );
            uint32_t base = ( definition->base == SCOREP_METRIC_BASE_BINARY ? 2 : 10 );
            sprintf( unit, "%u^%" PRIi64 " %s", base, definition->exponent, metric_unit );
            metric_unit = unit;
        }

        if ( is_dense_metric( handle ) &&
             layout->dense_metric_type == SCOREP_CUBE_DATA_TUPLE )
        {
            data_type        = "TAU_ATOMIC";
            cube_metric_type = CUBE_METRIC_EXCLUSIVE;
            write_tuples     = true;
        }
        else if ( !is_dense_metric( handle ) &&
                  layout->sparse_metric_type == SCOREP_CUBE_DATA_TUPLE )
        {
            data_type        = "TAU_ATOMIC";
            cube_metric_type = CUBE_METRIC_EXCLUSIVE;
            write_tuples     = true;
        }
        else
        {
            write_tuples = false;
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
        }

        if ( ( definition->source_type != SCOREP_METRIC_SOURCE_TYPE_TASK ) ||
             ( layout->metric_list & SCOREP_CUBE_METRIC_TASK_METRICS ) )
        {
            char* uniq_name = metric_name;
            if ( parent_handle )
            {
                const char* parent_uniq_name = cube_metric_get_uniq_name( parent_handle );
                size_t      len              = strlen( parent_uniq_name ) + 2 + strlen( metric_name ) + 1;
                uniq_name = calloc( len + 1, 1 );
                snprintf( uniq_name, len, "%s::%s", parent_uniq_name, metric_name );
            }
            cube_handle = cube_def_met( myCube, metric_name, uniq_name, data_type,
                                        metric_unit, "", "", metric_description,
                                        parent_handle,
                                        cube_metric_type );
            if ( uniq_name != metric_name )
            {
                free( uniq_name );
            }

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
   @param myCube  Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param map     Pointer to mapping instance to map Score-P und Cube definitions.
 */
static void
write_region_definitions( cube_t*                       myCube,
                          SCOREP_DefinitionManager*     manager,
                          scorep_cube4_definitions_map* map )
{
    char*  paradigm_buffer          = NULL;
    size_t paradigm_buffer_capacity = 0;

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Region, region )
    {
        /* Collect necessary data */
        const char* region_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                               String )->string_data;
        const char* canonical_region_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->canonical_name_handle,
                                                                         String )->string_data;
        const char* paradigm = scorep_paradigm_type_to_string( definition->paradigm_type );
        if ( definition->group_name_handle != SCOREP_INVALID_STRING )
        {
            const char* group_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->group_name_handle,
                                                                  String )->string_data;
            size_t length = strlen( paradigm ) + strlen( ":" ) + strlen( group_name ) + 1;
            if ( length >= paradigm_buffer_capacity )
            {
                paradigm_buffer_capacity = length;
                paradigm_buffer          = realloc( paradigm_buffer, paradigm_buffer_capacity );
            }
            sprintf( paradigm_buffer, "%s:%s", paradigm, group_name );
            paradigm = paradigm_buffer;
        }
        const char* role      = scorep_region_type_to_string( definition->region_type );
        const char* file_name = "";
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            file_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->file_name_handle,
                                                     String )->string_data;
        }

        /* Register region to cube */
        cube_region* cube_handle = cube_def_region( myCube,
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

    free( paradigm_buffer );
}

/**
   Writes callpath definitions to Cube. The new Cube definitions are added to the
   mapping table @a map.
   @param myCube  Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param map     Pointer to mapping instance to map Score-P und Cube definitions.
 */
static void
write_callpath_definitions( cube_t*                       myCube,
                            SCOREP_DefinitionManager*     manager,
                            scorep_cube4_definitions_map* map,
                            uint32_t                      maxNumberOfProgramArgs )
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
        scorep_region   = definition->region_handle;
        region          = scorep_get_cube4_region( map, scorep_region );
        scorep_callpath = definition->parent_callpath_handle;
        parent          = scorep_get_cube4_callpath( map, scorep_callpath );

        /* Register region to cube */
        cnode = cube_def_cnode( myCube, region, parent );

        /* For program regions, add the number of arguments. Currently, the
         * maximum is used. Should be per MPMD program regions in conjunction
         * with a scorep-score improvement. */
        if ( parent == NULL )
        {
            char buffer[ 32 ];
            sprintf( buffer, "%u", maxNumberOfProgramArgs );
            cube_region_def_attr( region, "Score-P::ProgramArguments::numberOfArguments", buffer );
        }

        /* Attach parameters */
        for ( uint32_t i = 0; i < definition->number_of_parameters; i++ )
        {
            const char* parameter_name =
                SCOREP_ParameterHandle_GetName( definition->parameters[ i ].parameter_handle );
            SCOREP_ParameterType type =
                SCOREP_ParameterHandle_GetType( definition->parameters[ i ].parameter_handle );

            if ( type == SCOREP_PARAMETER_STRING )
            {
                cube_cnode_add_string_parameter(
                    cnode,
                    parameter_name,
                    SCOREP_StringHandle_Get(
                        definition->parameters[ i ].parameter_value.string_handle ) );
            }
            else
            {
                cube_cnode_add_numeric_parameter(
                    cnode,
                    parameter_name,
                    definition->parameters[ i ].parameter_value.integer_value );
            }
        }

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
   @param myCube  Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @returns A Pointer to the array which contains the mapping system tree. This array
            must be freed by the caller of this function.
 */
static scorep_cube_system_node*
write_system_tree( cube_t*                   myCube,
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

        UTILS_ASSERT( pos < nodes );
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
            cube_def_system_tree_node( myCube, display_name, "", class, parent );

        SCOREP_SystemTreeNodePropertyHandle property_handle = definition->properties;
        while ( property_handle != SCOREP_INVALID_SYSTEM_TREE_NODE_PROPERTY )
        {
            SCOREP_SystemTreeNodePropertyDef* property =
                SCOREP_HANDLE_DEREF( property_handle,
                                     SystemTreeNodeProperty,
                                     manager->page_manager );
            property_handle = property->properties_next;

            const char* key = SCOREP_HANDLE_DEREF( property->property_name_handle,
                                                   String,
                                                   manager->page_manager )->string_data;
            const char* value = SCOREP_HANDLE_DEREF( property->property_value_handle,
                                                     String,
                                                     manager->page_manager )->string_data;

            cube_system_tree_node_def_attr( system_tree[ pos ].cube_node, key, value );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    free( display_name );
    return system_tree;
}

/**
   Writes location group definitions to Cube.
   @param myCube  Pointer to Cube instance.
   @param manager Pointer to Score-P definition manager with unified definitions.
   @param ranks   Number of MPI ranks. It must equal the number of array elements in
                  @a threads.
   @param offsets Array of the offsets of threads in each rank. The ith entry contains
                  the sum of all threads of lower ranks. The number of elements must
                  equal @a ranks.
 */
static cube_location_group**
write_location_group_definitions( cube_t*                   myCube,
                                  SCOREP_DefinitionManager* manager,
                                  uint32_t                  ranks )
{
    cube_location_group** processes =
        ( cube_location_group** )calloc( ranks, sizeof( cube_location_group* ) );
    UTILS_ASSERT( processes );
    scorep_cube_system_node* system_tree = write_system_tree( myCube, manager );
    UTILS_ASSERT( system_tree );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, LocationGroup, location_group )
    {
        uint32_t   rank = definition->global_location_group_id;
        cube_node* node = get_cube_node( system_tree, definition->parent,
                                         manager->system_tree_node.counter );

        const char* name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                        String )->string_data;
        cube_location_group_type type =
            convert_to_cube_location_group_type( definition->location_group_type );

        processes[ rank ] = cube_def_location_group( myCube, name, rank, type, node );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    free( system_tree );
    return processes;
}

/**
   Writes all location definitions from the Score-P definitions to Cube.
   @param myCube    Pointer to Cube instance.
   @param manager   Pointer to Score-P definition manager with unified definitions.
   @param ranks     The number of processes.
   @param numberOfThreads  Global number of locations.
   @retruns an array of cube_location pointers where the sequence number of the Score-P
            definitions is the index to the cube location.
 */
static cube_location**
write_all_location_definitions( cube_t*                   myCube,
                                SCOREP_DefinitionManager* manager,
                                uint32_t                  ranks,
                                uint64_t                  numberOfThreads )
{
    /* Counts the number of threads already registered for each rank */
    uint32_t threads[ ranks ];
    memset( threads, 0, ranks * sizeof( uint32_t ) );
    UTILS_ASSERT( threads );

    /* Location group (processes) mapping of global ids to cube definitions */
    cube_location_group** processes =
        write_location_group_definitions( myCube, manager, ranks );

    /* Location mapping of global ids to cube definition */
    cube_location** locations = calloc( numberOfThreads,
                                        sizeof( cube_location* ) );
    UTILS_ASSERT( locations );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Location, location )
    {
        uint32_t    parent_id = definition->location_group_id;
        const char* name      = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                             String )->string_data;
        cube_location_type type =
            convert_to_cube_location_type( definition->location_type );

        UTILS_ASSERT( definition->sequence_number < numberOfThreads );

        locations[ definition->sequence_number ] =
            cube_def_location( myCube,
                               name,
                               threads[ parent_id ],
                               type,
                               processes[ parent_id ] );
        threads[ parent_id ]++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    free( processes );
    return locations;
}

/**
   Writes key location definitions for one processes to Cube.
   @param myCube  Pointer to Cube instance.
   @param process Pointer to CUBE location group definition for
                  which it writes locations.
 */
static void
write_aggregated_locations_for_one_process( cube_t*              myCube,
                                            cube_location_group* process )
{
    cube_def_location( myCube,
                       "aggregated threads",
                       0,
                       CUBE_LOCATION_TYPE_CPU_THREAD,
                       process );
}

/**
   Writes one location definitions per process to Cube.
   @param myCube    Pointer to Cube instance.
   @param manager   Pointer to Score-P definition manager with unified definitions.
   @param ranks     The number of processes.
 */
static void
write_one_location_per_process( cube_t*                   myCube,
                                SCOREP_DefinitionManager* manager,
                                uint32_t                  ranks )
{
    /* Location group (processes) mapping of global ids to cube definitions */
    cube_location_group** processes =
        write_location_group_definitions( myCube, manager, ranks );

    for ( uint32_t rank = 0; rank < ranks; rank++ )
    {
        write_aggregated_locations_for_one_process( myCube,
                                                    processes[ rank ] );
    }
    free( processes );
}

/**
   Writes key location definitions for one processes to Cube.
   @param myCube  Pointer to Cube instance.
   @param process Pointer to CUBE location group definition for
                  which it writes locations.
 */
static void
write_key_locations_for_one_process( cube_t*              myCube,
                                     cube_location_group* process )
{
    cube_def_location( myCube,
                       "master thread",
                       0,
                       CUBE_LOCATION_TYPE_CPU_THREAD,
                       process );
    cube_def_location( myCube,
                       "fastest thread",
                       1,
                       CUBE_LOCATION_TYPE_CPU_THREAD,
                       process );
    cube_def_location( myCube,
                       "slowest thread",
                       2,
                       CUBE_LOCATION_TYPE_CPU_THREAD,
                       process );
    cube_def_location( myCube,
                       "aggregated worker threads",
                       3,
                       CUBE_LOCATION_TYPE_CPU_THREAD,
                       process );
}


/**
   Writes key location definitions for all processes to Cube.
   @param myCube   Pointer to Cube instance.
   @param manager   Pointer to Score-P definition manager with unified definitions.
   @param ranks     The number of processes.
 */
static void
write_key_locations( cube_t*                   myCube,
                     SCOREP_DefinitionManager* manager,
                     uint32_t                  ranks )
{
    /* Location group (processes) mapping of global ids to cube definitions */
    cube_location_group** processes =
        write_location_group_definitions( myCube, manager, ranks );

    for ( uint32_t rank = 0; rank < ranks; rank++ )
    {
        write_key_locations_for_one_process( myCube, processes[ rank ] );
    }
    free( processes );
}

/**
   Writes cluster location definitions fro one process to Cube.
   @param myCube  Pointer to Cube instance.
   @param process Pointer to CUBE location group definition for
                  which it writes locations.
   @param number  The number of locations.
 */
static void
write_cluster_locations_per_process( cube_t*              myCube,
                                     cube_location_group* process,
                                     uint32_t             number )
{
    for ( uint32_t cluster = 0; cluster < number; cluster++ )
    {
        char name[ LOCATION_NAME_BUFFER_LENGTH ];
        snprintf( name, LOCATION_NAME_BUFFER_LENGTH, "cluster %" PRIu32, cluster );
        cube_def_location( myCube,
                           name,
                           cluster,
                           CUBE_LOCATION_TYPE_CPU_THREAD,
                           process );
    }
}

/**
   Writes one location definitions per process to Cube.
   @param myCube    Pointer to Cube instance.
   @param manager   Pointer to Score-P definition manager with unified definitions.
   @param ranks     The number of processes.
   @param nCluster  List of number of clusters per process.
 */
static void
write_cluster_locations( cube_t*                   myCube,
                         SCOREP_DefinitionManager* manager,
                         uint32_t                  ranks,
                         uint32_t*                 nCluster )
{
    /* Location group (processes) mapping of global ids to cube definitions */
    cube_location_group** processes =
        write_location_group_definitions( myCube, manager, ranks );

    for ( uint32_t rank = 0; rank < ranks; rank++ )
    {
        write_cluster_locations_per_process( myCube,
                                             processes[ rank ],
                                             nCluster[ rank ] );
    }
    free( processes );
}


static void
scorep_write_cube_location_property( cube_t*                   myCube,
                                     SCOREP_DefinitionManager* manager,
                                     cube_location**           locationMap )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, LocationProperty, location_property )
    {
        const char* name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                        String )->string_data;
        const char* value = SCOREP_UNIFIED_HANDLE_DEREF( definition->value_handle,
                                                         String )->string_data;
        uint64_t id = SCOREP_UNIFIED_HANDLE_DEREF( definition->location_handle,
                                                   Location )->sequence_number;

        cube_location_def_attr( locationMap[ id ], name, value );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/**
   Writes cartesian topology definitions to Cube. The new Cube definitions are added
   to the mapping table @a map.
   @param myCube      Pointer to Cube instance.
   @param manager     Pointer to Score-P definition manager with unified definitions.
   @param map         Pointer to mapping instance to map Score-P and Cube definitions.
   @param locationMap Mapping between Cube and Score-P locations
 */
static void
write_cartesian_definitions( cube_t*                       myCube,
                             SCOREP_DefinitionManager*     manager,
                             scorep_cube4_definitions_map* map,
                             cube_location**               locationMap )
{
    /* General information for the definition accesses. Since this is post unification
       we are in a serial part of Score-P. Therefore, no locking is required. */

    /* store the coord location id mapping to avoid the additional loop through the locations inside the coord loop */
    uint64_t coord_loc_id_per_coord_rank[ manager->location_group.counter ];
    memset( coord_loc_id_per_coord_rank, 0, manager->location_group.counter * sizeof( uint64_t ) );
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, Location, location )
    {
        /* is a thread */
        if ( cube_location_get_type( locationMap[ definition->sequence_number ] ) == CUBE_LOCATION_TYPE_CPU_THREAD )
        {
            if ( cube_location_get_rank( locationMap[ definition->sequence_number ] ) == 0 )
            {
                coord_loc_id_per_coord_rank[ cube_location_group_get_rank( cube_location_get_parent( locationMap[ definition->sequence_number ] ) ) ] = definition->sequence_number;
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /*  Create the cartesian topologies */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, CartesianTopology, cartesian_topology )
    {
        uint32_t             topo_id   = definition->sequence_number;
        uint32_t             n_dims    = definition->n_dimensions;
        SCOREP_Topology_Type topo_type = definition->topology_type;
        long                 dim_procs[ n_dims ];
        int                  dim_period[ n_dims ];

        const char* name = SCOREP_UNIFIED_HANDLE_DEREF( definition->topology_name,
                                                        String )->string_data;

        for ( uint32_t i = 0; i < n_dims; i++ )
        {
            dim_procs[ i ]  = definition->cartesian_dims[ i ].n_processes_per_dim;
            dim_period[ i ] = definition->cartesian_dims[ i ].periodicity_per_dim;
        }
        cube_cartesian* cube_handle = cube_def_cart( myCube,
                                                     n_dims,
                                                     dim_procs,
                                                     dim_period );
        cube_cart_set_name( cube_handle, name );

        for ( uint32_t i = 0; i < n_dims; i++ )
        {
            cube_cart_set_dim_name( cube_handle, SCOREP_UNIFIED_HANDLE_DEREF( definition->cartesian_dims[ i ].dimension_name,
                                                                              String )->string_data, i );
        }

        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager, CartesianCoords, cartesian_coords )
        {
            uint32_t coord_rank   = definition->rank;
            uint32_t coord_thread = definition->thread;
            uint32_t n_coords     = definition->n_coords;
            uint32_t coord_loc_id = 0;
            uint32_t cart_topo_id = SCOREP_HANDLE_TO_ID( definition->topology_handle,
                                                         CartesianTopology,
                                                         manager->page_manager );
            SCOREP_Topology_Type cart_topo_type = SCOREP_LOCAL_HANDLE_DEREF( definition->topology_handle,
                                                                             CartesianTopology )->topology_type;
            uint32_t global_rank = 0;
            if ( cart_topo_type == SCOREP_TOPOLOGIES_MPI )
            {
                SCOREP_GroupType group_type = SCOREP_UNIFIED_HANDLE_DEREF(
                    SCOREP_UNIFIED_HANDLE_DEREF(
                        SCOREP_UNIFIED_HANDLE_DEREF( definition->topology_handle, CartesianTopology )->communicator_handle,
                        Communicator )->group_handle,
                    Group )->group_type;

                if ( group_type !=  SCOREP_GROUP_MPI_SELF )
                {
                    global_rank = SCOREP_UNIFIED_HANDLE_DEREF(
                        SCOREP_UNIFIED_HANDLE_DEREF(
                            SCOREP_UNIFIED_HANDLE_DEREF( definition->topology_handle, CartesianTopology )->communicator_handle,
                            Communicator )->group_handle,
                        Group )->members[ coord_rank ];
                }
            }
            else
            {
                coord_rank   =  SCOREP_Topologies_CoordRankFromGroupIndex( definition->rank, definition->topology_handle );
                coord_thread =  SCOREP_Topologies_CoordThreadFromGroupIndex( definition->rank, definition->topology_handle );
            }

            if ( cart_topo_id == topo_id )
            {
                long temp_coords_of_current_rank[ definition->n_coords ];
                for ( uint32_t i = 0; i < definition->n_coords; i++ )
                {
                    temp_coords_of_current_rank[ i ] = definition->coords_of_current_rank[ i ];
                }

                /* globalRank for MPI, important for topologies on sub-communicators */
                if ( cart_topo_type == SCOREP_TOPOLOGIES_MPI )
                {
                    cube_def_coords( myCube,
                                     cube_handle,
                                     locationMap[ coord_loc_id_per_coord_rank[ global_rank ]  ],
                                     temp_coords_of_current_rank );
                }
                else /* thread for platform, processxthreads */
                {
                    cube_def_coords( myCube,
                                     cube_handle,
                                     locationMap[ coord_loc_id_per_coord_rank[ coord_rank ] + coord_thread ],
                                     temp_coords_of_current_rank );
                }
            }
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/* ****************************************************************************
 * System tree sequence definitions expansion.
 *****************************************************************************/

typedef struct
{
    cube_t*                      my_cube;
    const uint32_t*              rank_mapping;
    const uint32_t*              num_locations;
    scorep_system_tree_seq_name* name_data;
    const scorep_cube_layout*    layout;
} sequence_writer_data;

static scorep_system_tree_seq_child_param
write_location_to_cube( scorep_system_tree_seq*            definition,
                        uint64_t                           copy,
                        sequence_writer_data*              writerData,
                        scorep_system_tree_seq_child_param forChildren )
{
    cube_location* current = NULL;

    if ( writerData->layout->location_layout == SCOREP_CUBE_LOCATION_ALL )
    {
        cube_location_group* parent   = forChildren.ptr;
        uint64_t             sub_type = scorep_system_tree_seq_get_sub_type( definition );
        cube_location_type   class    = convert_to_cube_location_type( sub_type );

        char* display_name = scorep_system_tree_seq_get_name( definition, copy,
                                                              writerData->name_data );

        current = cube_def_location( writerData->my_cube,
                                     display_name, copy, class, parent );
        free( display_name );
    }

    scorep_system_tree_seq_child_param for_children;
    for_children.ptr = current;
    return for_children;
}

static scorep_system_tree_seq_child_param
write_location_group_to_cube( scorep_system_tree_seq*            definition,
                              uint64_t                           copy,
                              sequence_writer_data*              writerData,
                              scorep_system_tree_seq_child_param forChildren )
{
    cube_system_tree_node*   parent       = forChildren.ptr;
    static uint64_t          index        = 0;
    uint64_t                 sub_type     = scorep_system_tree_seq_get_sub_type( definition );
    cube_location_group_type class        = convert_to_cube_location_group_type( sub_type );
    uint64_t                 sequence_no  = writerData->rank_mapping[ index ];
    char*                    display_name = scorep_system_tree_seq_get_name( definition, copy,
                                                                             writerData->name_data );

    cube_location_group* current = cube_def_location_group( writerData->my_cube,
                                                            display_name,
                                                            sequence_no,
                                                            class,
                                                            parent );

    switch ( writerData->layout->location_layout )
    {
        case SCOREP_CUBE_LOCATION_ONE_PER_PROCESS:
            write_aggregated_locations_for_one_process( writerData->my_cube, current );
            break;

        case SCOREP_CUBE_LOCATION_KEY_THREADS:
            write_key_locations_for_one_process( writerData->my_cube, current );
            break;

        case SCOREP_CUBE_LOCATION_CLUSTER:
            write_cluster_locations_per_process( writerData->my_cube, current,
                                                 writerData->num_locations[ index ] );
            break;
    }

    index++;

    free( display_name );
    scorep_system_tree_seq_child_param for_children;
    for_children.ptr = current;
    return for_children;
}


static scorep_system_tree_seq_child_param
write_system_tree_node_to_cube( scorep_system_tree_seq*            definition,
                                uint64_t                           copy,
                                sequence_writer_data*              writerData,
                                scorep_system_tree_seq_child_param forChildren )
{
    uint64_t    sub_type = scorep_system_tree_seq_get_sub_type( definition );
    const char* class    = scorep_system_tree_seq_get_class( definition,
                                                             writerData->name_data );
    char* display_name = scorep_system_tree_seq_get_name( definition, copy,
                                                          writerData->name_data );

    cube_system_tree_node* parent = forChildren.ptr;


    cube_system_tree_node* current =
        cube_def_system_tree_node( writerData->my_cube, display_name, "", class, parent );

    free( display_name );
    scorep_system_tree_seq_child_param for_children;
    for_children.ptr = current;
    return for_children;
}

static scorep_system_tree_seq_child_param
write_system_tree_seq_to_cube( scorep_system_tree_seq*            definition,
                               uint64_t                           copy,
                               void*                              param,
                               scorep_system_tree_seq_child_param forChildren )
{
    sequence_writer_data* writer_data = param;
    switch ( scorep_system_tree_seq_get_type( definition ) )
    {
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE:
            return write_system_tree_node_to_cube( definition, copy,
                                                   writer_data,
                                                   forChildren );
            break;
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP:
            return write_location_group_to_cube( definition, copy,
                                                 writer_data,
                                                 forChildren );
            break;
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION:
            return write_location_to_cube( definition, copy,
                                           writer_data,
                                           forChildren );
            break;
        default:
            UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                         "Child system tree node of unknown type" );
    }
    scorep_system_tree_seq_child_param for_children;
    for_children.ptr = NULL;
    return for_children;
}


/* ****************************************************************************
 * Main definition writer function
 *****************************************************************************/
void
scorep_write_definitions_to_cube4( cube_t*                       myCube,
                                   scorep_cube4_definitions_map* map,
                                   uint32_t                      nRanks,
                                   uint64_t                      nLocations,
                                   uint32_t*                     locationsPerRank,
                                   const scorep_cube_layout*     layout,
                                   uint32_t                      maxNumberOfProgramArgs )
{
    cube_location** location_map = NULL;

    /* The unification is always processed, even in serial case. Thus, we have
       always access to the unified definitions on rank 0.
       In non-mpi case SCOREP_Ipc_GetRank() returns always 0. Thus, we need only
       to test for the rank. */
    SCOREP_DefinitionManager* manager = scorep_unified_definition_manager;
    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        UTILS_ASSERT( scorep_unified_definition_manager );

        write_metric_definitions( myCube, manager, map, layout );
        write_region_definitions( myCube, manager, map );
        write_callpath_definitions( myCube, manager, map, maxNumberOfProgramArgs );

        if ( SCOREP_Env_UseSystemTreeSequence() )
        {
            sequence_writer_data writer_data;
            writer_data.my_cube       = myCube;
            writer_data.rank_mapping  = scorep_system_tree_seq_get_rank_order();
            writer_data.name_data     = scorep_system_tree_seq_create_name_data();
            writer_data.num_locations = locationsPerRank;
            writer_data.layout        = layout;
            scorep_system_tree_seq_child_param to_root;
            to_root.ptr = NULL;

            scorep_system_tree_seq_traverse_all( scorep_system_tree_seq_get_root(),
                                                 &write_system_tree_seq_to_cube,
                                                 &writer_data,
                                                 to_root );
            scorep_system_tree_seq_free_name_data( writer_data.name_data );
        }
        else
        {
            switch ( layout->location_layout )
            {
                case SCOREP_CUBE_LOCATION_ALL:
                    location_map = write_all_location_definitions( myCube, manager,
                                                                   nRanks, nLocations );
                    scorep_write_cube_location_property( myCube, manager, location_map );
                    write_cartesian_definitions( myCube, manager, map, location_map );
                    free( location_map );
                    break;

                case SCOREP_CUBE_LOCATION_ONE_PER_PROCESS:
                    write_one_location_per_process( myCube, manager, nRanks );
                    break;

                case SCOREP_CUBE_LOCATION_KEY_THREADS:
                    write_key_locations( myCube, manager, nRanks );
                    break;
                case SCOREP_CUBE_LOCATION_CLUSTER:
                    write_cluster_locations( myCube, manager, nRanks, locationsPerRank );
                    break;
            }
        }
    }
}
