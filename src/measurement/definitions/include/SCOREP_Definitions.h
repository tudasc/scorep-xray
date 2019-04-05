/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015-2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#define SCOREP_INTERNAL_DEFINITIONS_H


/**
 * @file
 *
 *
 */

/**
 * @defgroup SCOREP_Definitions SCOREP Definitions
 *
 * - Before calling one of the @ref SCOREP_Events event functions the adapter
     needs to define (i.e. register) entities like regions to be used in
     subsequent definition calls or event function calls.
 *
 * - The definition function calls return opaque handles. The adapter must not
     make any assumptions on the type or the operations that are allowed on
     this type.
 *
 * - A call to a definition function creates internally a process local
     definition. All these process local definitions are unified at the end of
     the measurement.
 *
 * - Definitions are stored per process (as opposed to per loccation) in the
     measurement system. This renders storing of redundant information
     unnecessary and reduces unification expense.
 *
 * - Note that calls to the definition functions need to be synchronized. See
     SCOREP_DefinitionLocking.h for the synchronization interface.
 *
 * - Note that the MPI definition functions also return handles now if
     appropriate (they previously returned void what caused some troubles
     during unification).
 *
 * - Note that the definition functions don't check for uniqueness of their
     arguments but create a new handle for each call. Checking for uniqueness
     is the responsibility of the adapter. Uniqueness of argument tuples is
     required for unification.
 *
 */
/*@{*/

#include "SCOREP_DefinitionHandles.h"

/* forward decl */
struct SCOREP_DefinitionManager;
typedef struct SCOREP_DefinitionManager SCOREP_DefinitionManager;
struct SCOREP_Allocator_PageManager;

#include <SCOREP_Types.h>
#include <UTILS_Error.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/**
 * Just a convenience macro to access the @e real memory a
 * SCOREP_Allocator_MovableMemory object is referring to.
 *
 * @param definition_memory_ptr Pointer to some SCOREP_Memory_DefinitionMemory
 * object.
 * @param targetType The type @a definition_memory_ptr should be converted to.
 *
 * @return A pointer to an object of type @a target_type.
 */
#define SCOREP_MEMORY_DEREF_MOVABLE( movableMemory, movablePageManager, targetType ) \
    ( ( targetType )SCOREP_Memory_GetAddressFromMovableMemory( movableMemory, \
                                                               movablePageManager ) )

/**
 *  Dereferences a moveable memory pointer to the definition struct.
 *
 */
#define SCOREP_HANDLE_DEREF( handle, Type, movablePageManager ) \
    SCOREP_MEMORY_DEREF_MOVABLE( handle, \
                                 movablePageManager, \
                                 SCOREP_ ## Type ## Def* )

#define SCOREP_LOCAL_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetLocalDefinitionPageManager() )

#define SCOREP_UNIFIED_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetLocalDefinitionPageManager() )

/**
 *  Extracts the ID out of an handle pointer.
 *
 *  @note This is only the process local sequence number, which
 *        may happen to be the OTF2 definition id.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_HANDLE_TO_ID( handle, Type, movablePageManager ) \
    ( SCOREP_HANDLE_DEREF( handle, Type, movablePageManager )->sequence_number )

#define SCOREP_LOCAL_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_LOCAL_HANDLE_DEREF( handle, Type )->sequence_number )

#define SCOREP_UNIFIED_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_UNIFIED_HANDLE_DEREF( handle, Type )->sequence_number )

/**
 * Defines a new type of definition for use in @a SCOREP_DefinitionManager.
 */
typedef struct scorep_definitions_manager_entry
{
    SCOREP_AnyHandle  head;
    SCOREP_AnyHandle* tail;
    SCOREP_AnyHandle* hash_table;
    uint32_t          hash_table_mask;
    uint32_t          counter;
    uint32_t*         mapping;
} scorep_definitions_manager_entry;



/**
 * Initialize a definition type in a @a SCOREP_DefinitionManager @a
 * definition_manager.
 *
 */
static inline void
scorep_definitions_manager_init_entry( scorep_definitions_manager_entry* entry )
{
    entry->head            = SCOREP_MOVABLE_NULL;
    entry->tail            = &entry->head;
    entry->hash_table      = 0;
    entry->hash_table_mask = 0;
    entry->counter         = 0;
    entry->mapping         = 0;
}


/**
 * Allocate memory for a definition type's hash_table in a
 * scorep_definitions_manager_entry @a entry, setting size
 * to 2^hashTablePower.
 */
void
scorep_definitions_manager_entry_alloc_hash_table( scorep_definitions_manager_entry* entry,
                                                   uint32_t                          hashTablePower );


/**
 * Iterator functions for definition. The iterator variable is named
 * @definition.
 * @{
 *
 * Iterates over all definitions of definition type @a Type from the
 * definition manager @a definition_manager.
 *
 * Example:
 * @code
 *  SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_definition_manager, String, string )
 *  {
 *      :
 *      definition->member = ...
 *      :
 *  }
 *  SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
 * @endcode
 *
 * You can still use break and continue statements.
 *
 * @declares Variable with name @a definition of definition type @a Type*.
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN( entry, Type, page_manager ) \
    do \
    { \
        SCOREP_ ## Type ## Def*   definition; \
        SCOREP_ ## Type ## Handle handle; \
        for ( handle = ( entry )->head; \
              handle != SCOREP_MOVABLE_NULL; \
              handle = definition->next ) \
        { \
            definition = SCOREP_HANDLE_DEREF( handle, Type, page_manager ); \

#define SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END() \
        } \
    } \
    while ( 0 )

#define SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definition_manager, Type, type ) \
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN( &( definition_manager )->type, \
                                                               Type, \
                                                               ( definition_manager )->page_manager )

#define SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END() \
        SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END()
/* *INDENT-ON* */
/** @} */


/**
 * Use this macro to define a definition struct.
 *
 * Usage:
 * @code
 *     SCOREP_DEFINE_DEFINITION_TYPE( Type )
 *     {
 *         SCOREP_DEFINE_DEFINITION_HEADER( Type ); // must be first
 *         // definition specfic members
 *         :
 *     };
 * @endcode
 *
 * @see SCOREP_DEFINE_DEFINITION_HEADER
 */
#define SCOREP_DEFINE_DEFINITION_TYPE( Type ) \
    typedef struct SCOREP_ ## Type ## Def SCOREP_ ## Type ## Def; \
    struct SCOREP_ ## Type ## Def

/**
 * Provides a stub for the definition struct header.
 *
 * The sequence_number member is mostly used as the id for the local definitions
 *
 * @see SCOREP_DEFINE_DEFINITION_TYPE
 *
 * @note No ';' after
 */
#define SCOREP_DEFINE_DEFINITION_HEADER( Type ) \
    SCOREP_ ## Type ## Handle next;             \
    SCOREP_ ## Type ## Handle unified;          \
    SCOREP_ ## Type ## Handle hash_next;        \
    uint32_t hash_value;                        \
    uint32_t sequence_number


/**
 * Allocates the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
static inline void
scorep_definitions_manager_entry_alloc_mapping( scorep_definitions_manager_entry* entry )
{
    entry->mapping = NULL;
    if ( entry->counter > 0 )
    {
        entry->mapping = malloc( entry->counter * sizeof( *entry->mapping ) );
        UTILS_BUG_ON( entry->mapping == 0, "Allocation failed." );
        memset( entry->mapping, 0xff, entry->counter * sizeof( *entry->mapping ) );
    }
}


/**
 * Frees the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
static inline void
scorep_definitions_manager_entry_free_mapping( scorep_definitions_manager_entry* entry )
{
    free( entry->mapping );
    entry->mapping = NULL;
}


#include "scorep_definitions_string.h"
#include "scorep_definitions_source_file.h"
#include "scorep_definitions_location_group.h"
#include "scorep_definitions_location.h"
#include "scorep_definitions_system_tree_node.h"
#include "scorep_definitions_system_tree_node_property.h"
#include "scorep_definitions_region.h"
#include "scorep_definitions_communicator.h"
#include "scorep_definitions_group.h"
#include "scorep_definitions_metric.h"
#include "scorep_definitions_sampling_set.h"
#include "scorep_definitions_sampling_set_recorder.h"
#include "scorep_definitions_paradigm.h"
#include "scorep_definitions_parameter.h"
#include "scorep_definitions_callpath.h"
#include "scorep_definitions_clock_offset.h"
#include "scorep_definitions_property.h"
#include "scorep_definitions_rma_window.h"
#include "scorep_definitions_topology.h"
#include "scorep_definitions_io_handle.h"
#include "scorep_definitions_io_file.h"
#include "scorep_definitions_io_file_property.h"
#include "scorep_definitions_io_paradigm.h"
#include "scorep_definitions_marker_group.h"
#include "scorep_definitions_marker.h"
#include "scorep_definitions_attribute.h"
#include "scorep_definitions_location_property.h"
#include "scorep_definitions_source_code_location.h"
#include "scorep_definitions_calling_context.h"

/* super object for all definitions */
SCOREP_DEFINE_DEFINITION_TYPE( Any )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Any );
};


/* Return the unique id associated with @a handle.
 * So far only used in UTILS_DEBUG* statements, no need to introduce
 * dependency to SCOREP_Memory by inlining it.  */
uint32_t
SCOREP_Definitions_HandleToId( SCOREP_AnyHandle handle );


// the list of definitions for what we generate mappings to global
// ids in the unifier
#define SCOREP_LIST_OF_DEFS_WITH_MAPPINGS \
    DEF_WITH_MAPPING( String, string ) \
    DEF_WITH_MAPPING( Region, region ) \
    DEF_WITH_MAPPING( Group, group ) \
    DEF_WITH_MAPPING( Communicator, communicator ) \
    DEF_WITH_MAPPING( RmaWindow, rma_window ) \
    DEF_WITH_MAPPING( Metric, metric ) \
    DEF_WITH_MAPPING( SamplingSet, sampling_set ) \
    DEF_WITH_MAPPING( Parameter, parameter ) \
    DEF_WITH_MAPPING( Callpath, callpath ) \
    DEF_WITH_MAPPING( Attribute, attribute ) \
    DEF_WITH_MAPPING( SourceCodeLocation, source_code_location ) \
    DEF_WITH_MAPPING( CallingContext, calling_context ) \
    DEF_WITH_MAPPING( InterruptGenerator, interrupt_generator ) \
    DEF_WITH_MAPPING( CartesianTopology, cartesian_topology ) \
    DEF_WITH_MAPPING( IoFile, io_file ) \
    DEF_WITH_MAPPING( IoHandle, io_handle )


/**
 * Declares a definition manager entry of type @a type.
 */
#define SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( type ) \
    scorep_definitions_manager_entry type


/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
struct SCOREP_DefinitionManager
{
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( string );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( system_tree_node );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( system_tree_node_property );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( location_group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( location );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( source_file );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( region );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( interim_communicator );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( communicator );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( rma_window );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( cartesian_topology );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( cartesian_coords );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( metric );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( sampling_set );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( sampling_set_recorder );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( io_handle );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( io_file );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( io_file_property );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( marker_group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( marker );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( parameter );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( callpath );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( property );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( attribute );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( location_property );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( source_code_location );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( calling_context );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( interrupt_generator );

    /** The pager manager where all definition objects resides */
    struct SCOREP_Allocator_PageManager* page_manager;
};

/* *INDENT-ON* */

extern SCOREP_DefinitionManager scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;

void
SCOREP_Definitions_Initialize( void );


bool
SCOREP_Definitions_Initialized( void );


void
SCOREP_Definitions_Finalize( void );


void
SCOREP_Definitions_Lock( void );

void
SCOREP_Definitions_Unlock( void );


void
SCOREP_Definitions_InitializeDefinitionManager(
    SCOREP_DefinitionManager**           manager,
    struct SCOREP_Allocator_PageManager* pageManager,
    bool                                 allocHashTables );


/**
 * Returns the number of unified callpaths definitions.
 */
uint32_t
SCOREP_Definitions_GetNumberOfUnifiedCallpathDefinitions( void );

/*@}*/

#endif /* SCOREP_INTERNAL_DEFINITIONS_H */
