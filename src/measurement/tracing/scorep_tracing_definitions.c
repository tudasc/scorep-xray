/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
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
 * @brief   Handles tracing specific definitions tasks.
 *
 *
 */


#include <config.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <otf2/otf2.h>


#include <UTILS_Error.h>


#include <UTILS_Debug.h>


#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_RuntimeManagement.h>
#include <scorep_runtime_management.h>
#include <scorep_environment.h>
#include <scorep_status.h>
#include <scorep_system_tree_sequence.h>
#include <scorep_type_utils.h>
#include <scorep_clock_synchronization.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_IoManagement.h>
#include <SCOREP_Bitstring.h>
#include <UTILS_Error.h>

#include "scorep_tracing_types.h"


static inline void
scorep_handle_definition_writing_error( OTF2_ErrorCode status,
                                        const char*    definitionType )
{
    // Do we really need to abort here?
    UTILS_WARNING( "Couldn't write %s definition: %s",
                   definitionType,
                   OTF2_Error_GetName( status ) );
}


static void
scorep_write_string_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( * def_string_pointer_t )( void*,
                                                       OTF2_StringRef,
                                                       const char* );
    def_string_pointer_t defString = ( def_string_pointer_t )
                                     OTF2_DefWriter_WriteString;

    if ( isGlobal )
    {
        defString = ( def_string_pointer_t )
                    OTF2_GlobalDefWriter_WriteString;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, String, string )
    {
        OTF2_ErrorCode status = defString(
            writerHandle,
            definition->sequence_number,
            definition->string_data );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "String" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_location_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( * def_location_pointer_t )( void*,
                                                         OTF2_LocationRef,
                                                         OTF2_StringRef,
                                                         OTF2_LocationType,
                                                         uint64_t,
                                                         OTF2_LocationGroupRef );
    def_location_pointer_t defLocation = ( def_location_pointer_t )
                                         OTF2_DefWriter_WriteLocation;

    if ( isGlobal )
    {
        defLocation = ( def_location_pointer_t )
                      OTF2_GlobalDefWriter_WriteLocation;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, Location, location )
    {
        OTF2_ErrorCode status = defLocation(
            writerHandle,
            definition->global_location_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_tracing_location_type_to_otf2( definition->location_type ),
            definition->number_of_events,
            definition->location_group_id );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Location" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_location_property_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( * def_location_property_pointer_t )( void*,
                                                                  OTF2_LocationRef,
                                                                  OTF2_StringRef,
                                                                  OTF2_Type,
                                                                  OTF2_AttributeValue );
    def_location_property_pointer_t defLocationProperty =
        ( def_location_property_pointer_t )OTF2_DefWriter_WriteLocationProperty;

    if ( isGlobal )
    {
        defLocationProperty = ( def_location_property_pointer_t )
                              OTF2_GlobalDefWriter_WriteLocationProperty;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, LocationProperty, location_property )
    {
        OTF2_AttributeValue value;
        value.stringRef = SCOREP_HANDLE_TO_ID( definition->value_handle, String, definitionManager->page_manager );
        OTF2_ErrorCode status = defLocationProperty(
            writerHandle,
            SCOREP_HANDLE_DEREF( definition->location_handle, Location, definitionManager->page_manager )->global_location_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            OTF2_TYPE_STRING,
            value );

        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "LocationProperty" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_location_group_definitions( void*                     writerHandle,
                                         SCOREP_DefinitionManager* definitionManager,
                                         bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( * def_location_group_pointer_t )( void*,
                                                               OTF2_LocationGroupRef,
                                                               OTF2_StringRef,
                                                               OTF2_LocationGroupType,
                                                               OTF2_SystemTreeNodeRef );
    def_location_group_pointer_t defLocationGroup =
        ( def_location_group_pointer_t )OTF2_DefWriter_WriteLocationGroup;
    if ( isGlobal )
    {
        defLocationGroup =  ( def_location_group_pointer_t )
                           OTF2_GlobalDefWriter_WriteLocationGroup;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, LocationGroup, location_group )
    {
        OTF2_SystemTreeNodeRef system_tree_parent = OTF2_UNDEFINED_SYSTEM_TREE_NODE;
        if ( definition->parent != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            system_tree_parent = SCOREP_HANDLE_TO_ID(
                definition->parent,
                SystemTreeNode,
                definitionManager->page_manager );
        }

        OTF2_ErrorCode status = defLocationGroup(
            writerHandle,
            definition->global_location_group_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_tracing_location_group_type_to_otf2( definition->location_group_type ),
            system_tree_parent );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "LocationGroup" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_system_tree_node_definitions( void*                     writerHandle,
                                           SCOREP_DefinitionManager* definitionManager,
                                           bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode
    ( * def_system_tree_node_pointer_t )( void*,
                                          OTF2_SystemTreeNodeRef,
                                          OTF2_StringRef,
                                          OTF2_StringRef,
                                          OTF2_SystemTreeNodeRef );
    def_system_tree_node_pointer_t defSystemTreeNode =
        ( def_system_tree_node_pointer_t )OTF2_DefWriter_WriteSystemTreeNode;

    typedef OTF2_ErrorCode
    ( * def_system_tree_node_domain_pointer_t )( void*,
                                                 OTF2_SystemTreeNodeRef,
                                                 OTF2_SystemTreeDomain );
    def_system_tree_node_domain_pointer_t defSystemTreeNodeDomain =
        ( def_system_tree_node_domain_pointer_t )OTF2_DefWriter_WriteSystemTreeNodeDomain;

    typedef OTF2_ErrorCode
    ( * def_system_tree_node_property_pointer_t )( void*,
                                                   OTF2_SystemTreeNodeRef,
                                                   OTF2_StringRef,
                                                   OTF2_Type,
                                                   OTF2_AttributeValue );
    def_system_tree_node_property_pointer_t defSystemTreeNodeProperty =
        ( def_system_tree_node_property_pointer_t )OTF2_DefWriter_WriteSystemTreeNodeProperty;

    if ( isGlobal )
    {
        defSystemTreeNode =
            ( def_system_tree_node_pointer_t )OTF2_GlobalDefWriter_WriteSystemTreeNode;
        defSystemTreeNodeDomain =
            ( def_system_tree_node_domain_pointer_t )OTF2_GlobalDefWriter_WriteSystemTreeNodeDomain;
        defSystemTreeNodeProperty =
            ( def_system_tree_node_property_pointer_t )OTF2_GlobalDefWriter_WriteSystemTreeNodeProperty;
    }


    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, SystemTreeNode, system_tree_node )
    {
        /* Determine parent id savely */
        uint32_t parent = OTF2_UNDEFINED_SYSTEM_TREE_NODE;
        if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            parent = SCOREP_HANDLE_TO_ID( definition->parent_handle, SystemTreeNode, definitionManager->page_manager );
        }

        /* Write definition */
        OTF2_ErrorCode status = defSystemTreeNode(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->class_handle, String, definitionManager->page_manager ),
            parent );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SystemTreeNode" );
        }

        /* write domain supplement defs */
        SCOREP_SystemTreeDomain domains = definition->domains;
        while ( domains != SCOREP_SYSTEM_TREE_DOMAIN_NONE )
        {
            OTF2_SystemTreeDomain domain = scorep_tracing_domain_to_otf2( &domains );
            status = defSystemTreeNodeDomain(
                writerHandle,
                definition->sequence_number,
                domain );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "SystemTreeNodeDomain" );
            }
        }

        SCOREP_SystemTreeNodePropertyHandle property_handle = definition->properties;
        while ( property_handle != SCOREP_INVALID_SYSTEM_TREE_NODE_PROPERTY )
        {
            SCOREP_SystemTreeNodePropertyDef* property =
                SCOREP_HANDLE_DEREF( property_handle,
                                     SystemTreeNodeProperty,
                                     definitionManager->page_manager );
            property_handle = property->properties_next;

            OTF2_AttributeValue value;
            value.stringRef = SCOREP_HANDLE_TO_ID(
                property->property_value_handle,
                String,
                definitionManager->page_manager );
            status = defSystemTreeNodeProperty(
                writerHandle,
                definition->sequence_number,
                SCOREP_HANDLE_TO_ID(
                    property->property_name_handle,
                    String,
                    definitionManager->page_manager ),
                OTF2_TYPE_STRING,
                value );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "SystemTreeNodeProperty" );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

typedef struct
{
    void*                        writer_handle;
    const uint32_t*              rank_mapping;
    const uint64_t*              location_mapping;
    const uint64_t*              number_of_events;
    uint32_t                     string_counter;
    uint32_t*                    location_name_start_ids;

    scorep_system_tree_seq_name* name_data;
} sequence_writer_data;

/**
 * Writes a string definition directly to the trace.
 * It is needed to define names during expansion of the sequence definitions.
 *
 * @param writerData Pointer to the writer data structure.
 * @param string     The string that is written to the definitions.
 */
static uint32_t
write_string_direct( sequence_writer_data* writerData,
                     const char*           string )
{
    OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteString( writerData->writer_handle,
                                                              writerData->string_counter,
                                                              string );
    if ( status != OTF2_SUCCESS )
    {
        scorep_handle_definition_writing_error( status, "String" );
    }
    writerData->string_counter++;
    return writerData->string_counter - 1;
}

static void
get_number_of_location_names( scorep_system_tree_seq* root,
                              uint32_t*               counts )
{
    if ( scorep_system_tree_seq_get_type( root ) == SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION )
    {
        uint64_t copies   = scorep_system_tree_seq_get_number_of_copies( root );
        uint64_t sub_type = scorep_system_tree_seq_get_sub_type( root );
        counts[ sub_type ] = copies > counts[ sub_type ] ? copies : counts[ sub_type ];
    }
    else
    {
        for ( uint64_t i = 0;
              i < scorep_system_tree_seq_get_number_of_children( root );
              i++ )
        {
            get_number_of_location_names( scorep_system_tree_seq_get_child( root, i ),
                                          counts );
        }
    }
}

static uint32_t*
construct_location_names( sequence_writer_data* writerData )
{
    scorep_system_tree_seq* root      = scorep_system_tree_seq_get_root();
    uint32_t*               start_ids = calloc( SCOREP_NUMBER_OF_LOCATION_TYPES, sizeof( uint32_t ) );
    get_number_of_location_names( root, start_ids );

    for ( uint64_t i = 0; i < SCOREP_NUMBER_OF_LOCATION_TYPES; i++ )
    {
        const char* class_name = scorep_location_type_to_string( i );
        uint32_t    start_id   = writerData->string_counter;

        for ( uint32_t j = 0; j < start_ids[ i ]; j++ )
        {
            size_t display_name_length = strlen( class_name ) + 20;
            char*  display_name        = malloc( display_name_length );
            UTILS_ASSERT( display_name );

            snprintf( display_name, display_name_length,
                      "%s %" PRIu32,
                      class_name, j );

            write_string_direct( writerData, display_name );
            free( display_name );
        }
        start_ids[ i ] = start_id;
    }

    return start_ids;
}

static scorep_system_tree_seq_child_param
write_location_seq( scorep_system_tree_seq*            definition,
                    uint64_t                           copy,
                    sequence_writer_data*              writerData,
                    scorep_system_tree_seq_child_param forChildren )
{
    uint64_t        parent     = forChildren.uint64;
    uint64_t        sub_type   = scorep_system_tree_seq_get_sub_type( definition );
    const char*     class_name = scorep_location_type_to_string( sub_type );
    static uint64_t counter    = 0;

    uint64_t global_location_id =
        parent + ( writerData->location_mapping[ counter ] << 32 );

    OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteLocation(
        writerData->writer_handle,
        global_location_id,
        writerData->location_name_start_ids[ sub_type ] + writerData->location_mapping[ counter ],
        scorep_tracing_location_type_to_otf2( sub_type ),
        writerData->number_of_events[ counter ],
        parent );
    if ( status != OTF2_SUCCESS )
    {
        scorep_handle_definition_writing_error( status, "Location" );
    }

    counter++;

    scorep_system_tree_seq_child_param for_children;
    for_children.uint64 = global_location_id;
    return for_children;
}

static scorep_system_tree_seq_child_param
write_location_group_seq( scorep_system_tree_seq*            definition,
                          uint64_t                           copy,
                          sequence_writer_data*              writerData,
                          scorep_system_tree_seq_child_param forChildren )
{
    uint64_t        parent   = forChildren.uint64;
    static uint64_t counter  = 0;
    uint64_t        sub_type = scorep_system_tree_seq_get_sub_type( definition );

    uint64_t sequence_no  = writerData->rank_mapping[ counter ];
    char*    display_name = scorep_system_tree_seq_get_name( definition, copy,
                                                             writerData->name_data );
    counter++;

    OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteLocationGroup(
        writerData->writer_handle,
        sequence_no,
        write_string_direct( writerData, display_name ),
        scorep_tracing_location_group_type_to_otf2( sub_type ),
        parent );
    if ( status != OTF2_SUCCESS )
    {
        scorep_handle_definition_writing_error( status, "LocationGroup" );
    }
    free( display_name );

    scorep_system_tree_seq_child_param for_children;
    for_children.uint64 = sequence_no;
    return for_children;
}

static scorep_system_tree_seq_child_param
write_system_tree_node_seq( scorep_system_tree_seq*            definition,
                            uint64_t                           copy,
                            sequence_writer_data*              writerData,
                            scorep_system_tree_seq_child_param forChildren )
{
    uint64_t    sub_type = scorep_system_tree_seq_get_sub_type( definition );
    const char* class    = SCOREP_StringHandle_GetById( sub_type );

    static uint32_t counter     = 0;
    uint32_t        sequence_no = counter;
    uint64_t        parent      = forChildren.uint64;
    counter++;
    char* display_name = scorep_system_tree_seq_get_name( definition, copy,
                                                          writerData->name_data );

    /* Write system tree node definition */
    OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteSystemTreeNode(
        writerData->writer_handle,
        sequence_no,
        write_string_direct( writerData, display_name ),
        sub_type,
        parent );
    if ( status != OTF2_SUCCESS )
    {
        scorep_handle_definition_writing_error( status, "SystemTreeNode" );
    }
    free( display_name );

    /* write domain supplement defs */
    SCOREP_SystemTreeDomain domains = scorep_system_tree_seq_get_domains( definition );
    while ( domains != SCOREP_SYSTEM_TREE_DOMAIN_NONE )
    {
        OTF2_SystemTreeDomain domain = scorep_tracing_domain_to_otf2( &domains );
        status = OTF2_GlobalDefWriter_WriteSystemTreeNodeDomain(
            writerData->writer_handle,
            sequence_no,
            domain );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SystemTreeNodeDomain" );
        }
    }

    /* Properties are lost */

    scorep_system_tree_seq_child_param for_children;
    for_children.uint64 = sequence_no;
    return for_children;
}

static scorep_system_tree_seq_child_param
write_system_tree_seq( scorep_system_tree_seq*            definition,
                       uint64_t                           copy,
                       void*                              param,
                       scorep_system_tree_seq_child_param forChildren )
{
    sequence_writer_data* writer_data = param;
    switch ( scorep_system_tree_seq_get_type( definition ) )
    {
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE:
            return write_system_tree_node_seq( definition, copy,
                                               writer_data,
                                               forChildren );
            break;
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP:
            return write_location_group_seq( definition, copy,
                                             writer_data,
                                             forChildren );
            break;
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION:
            return write_location_seq( definition, copy,
                                       writer_data,
                                       forChildren );
            break;
        default:
            UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                         "Child system tree node of unknown type" );
    }
    scorep_system_tree_seq_child_param for_children;
    for_children.uint64 = OTF2_UNDEFINED_SYSTEM_TREE_NODE;
    return for_children;
}

static void
write_system_tree_sequence_definitions( void*                     writerHandle,
                                        SCOREP_DefinitionManager* definitionManager )
{
    sequence_writer_data writer_data;
    writer_data.writer_handle    = writerHandle;
    writer_data.string_counter   = definitionManager->string.counter;
    writer_data.rank_mapping     = scorep_system_tree_seq_get_rank_order();
    writer_data.location_mapping = scorep_system_tree_seq_get_global_location_order();
    writer_data.number_of_events = scorep_system_tree_seq_get_event_numbers();
    writer_data.name_data        = scorep_system_tree_seq_create_name_data();

    writer_data.location_name_start_ids = construct_location_names( &writer_data );

    scorep_system_tree_seq_child_param to_root;
    to_root.uint64 = OTF2_UNDEFINED_SYSTEM_TREE_NODE;

    scorep_system_tree_seq_traverse_all( scorep_system_tree_seq_get_root(),
                                         &write_system_tree_seq,
                                         &writer_data,
                                         to_root );

    free( writer_data.location_name_start_ids );
    scorep_system_tree_seq_free_name_data( writer_data.name_data );
}


static void
scorep_write_region_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( * def_region_pointer_t )( void*,
                                                       OTF2_RegionRef,
                                                       OTF2_StringRef,
                                                       OTF2_StringRef,
                                                       OTF2_StringRef,
                                                       OTF2_RegionRole,
                                                       OTF2_Paradigm,
                                                       OTF2_RegionFlag,
                                                       OTF2_StringRef,
                                                       uint32_t,
                                                       uint32_t );

    def_region_pointer_t defRegion = ( def_region_pointer_t )
                                     OTF2_DefWriter_WriteRegion;
    if ( isGlobal )
    {
        defRegion = ( def_region_pointer_t )OTF2_GlobalDefWriter_WriteRegion;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, Region, region )
    {
        uint32_t source_file_id = OTF2_UNDEFINED_STRING;
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            source_file_id = SCOREP_HANDLE_TO_ID(
                definition->file_name_handle,
                String,
                definitionManager->page_manager );
        }

        OTF2_Paradigm paradigm =
            scorep_tracing_paradigm_to_otf2( definition->paradigm_type );

        OTF2_RegionRole region_role;
        OTF2_RegionFlag region_flags = OTF2_REGION_FLAG_NONE;

        switch ( definition->region_type )
        {
            case SCOREP_REGION_PHASE:
                region_role  = OTF2_REGION_ROLE_CODE;
                region_flags = OTF2_REGION_FLAG_PHASE;
                break;
            case SCOREP_REGION_DYNAMIC:
                region_role  = OTF2_REGION_ROLE_CODE;
                region_flags = OTF2_REGION_FLAG_DYNAMIC;
                break;
            case SCOREP_REGION_DYNAMIC_FUNCTION:
                region_role  = OTF2_REGION_ROLE_FUNCTION;
                region_flags = OTF2_REGION_FLAG_DYNAMIC;
                break;
            case SCOREP_REGION_DYNAMIC_LOOP:
                region_role  = OTF2_REGION_ROLE_LOOP;
                region_flags = OTF2_REGION_FLAG_DYNAMIC;
                break;
            case SCOREP_REGION_DYNAMIC_LOOP_PHASE:
                region_role  = OTF2_REGION_ROLE_LOOP;
                region_flags = OTF2_REGION_FLAG_DYNAMIC | OTF2_REGION_FLAG_PHASE;
                break;
            case SCOREP_REGION_DYNAMIC_PHASE:
                region_role  = OTF2_REGION_ROLE_CODE;
                region_flags = OTF2_REGION_FLAG_DYNAMIC | OTF2_REGION_FLAG_PHASE;
                break;

            case SCOREP_REGION_FUNCTION:
                region_role = OTF2_REGION_ROLE_FUNCTION;
                break;
            case SCOREP_REGION_LOOP:
                region_role = OTF2_REGION_ROLE_LOOP;
                break;
            case SCOREP_REGION_USER:
                region_role = OTF2_REGION_ROLE_FUNCTION;
                break;
            case SCOREP_REGION_CODE:
                region_role = OTF2_REGION_ROLE_CODE;
                break;

            #define SCOREP_REGION_TYPE( NAME, name_str ) \
    case SCOREP_REGION_ ## NAME:                     \
        region_role = OTF2_REGION_ROLE_ ## NAME;     \
        break;

                SCOREP_REGION_TYPES
            #undef SCOREP_REGION_TYPE

            default:
                region_role = OTF2_REGION_ROLE_UNKNOWN;
        }

        OTF2_ErrorCode status = defRegion(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->canonical_name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String, definitionManager->page_manager ),
            region_role,
            paradigm,
            region_flags,
            source_file_id,
            definition->begin_line,
            definition->end_line );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Region" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_communicator_definitions( void*                     writerHandle,
                                       SCOREP_DefinitionManager* definitionManager )
{
    UTILS_ASSERT( writerHandle );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, Communicator, communicator )
    {
        uint32_t comm_name_id = OTF2_UNDEFINED_STRING;
        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            comm_name_id = SCOREP_HANDLE_TO_ID( definition->name_handle,
                                                Communicator,
                                                definitionManager->page_manager );
        }

        uint32_t comm_parent_id = OTF2_UNDEFINED_COMM;
        if ( definition->parent_handle != SCOREP_INVALID_COMMUNICATOR )
        {
            comm_parent_id = SCOREP_HANDLE_TO_ID( definition->parent_handle,
                                                  Communicator,
                                                  definitionManager->page_manager );
        }

        OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteComm(
            writerHandle,
            definition->sequence_number,
            comm_name_id,
            SCOREP_HANDLE_TO_ID( definition->group_handle, Group, definitionManager->page_manager ),
            comm_parent_id );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Communicator" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_rma_window_definitions( void*                     writerHandle,
                                     SCOREP_DefinitionManager* definitionManager )
{
    UTILS_ASSERT( writerHandle );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, RmaWindow, rma_window )
    {
        uint32_t comm_id = OTF2_UNDEFINED_COMM;
        if ( definition->communicator_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            comm_id = SCOREP_HANDLE_TO_ID( definition->communicator_handle,
                                           InterimCommunicator,
                                           definitionManager->page_manager );
        }

        OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteRmaWin(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            comm_id );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "RmaWindow" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_group_definitions( void*                     writerHandle,
                                SCOREP_DefinitionManager* definitionManager,
                                bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );


    typedef OTF2_ErrorCode ( * def_group_pointer_t )( void*,
                                                      OTF2_GroupRef,
                                                      OTF2_StringRef,
                                                      OTF2_GroupType,
                                                      OTF2_Paradigm,
                                                      OTF2_GroupFlag,
                                                      uint32_t,
                                                      const uint64_t* );

    def_group_pointer_t defGroup = ( def_group_pointer_t )
                                   OTF2_DefWriter_WriteGroup;
    if ( isGlobal )
    {
        defGroup = ( def_group_pointer_t )OTF2_GlobalDefWriter_WriteGroup;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, Group, group )
    {
        OTF2_GroupType group_type
            = scorep_tracing_group_type_to_otf2( definition->group_type );
        OTF2_Paradigm paradigm     = OTF2_PARADIGM_UNKNOWN;
        OTF2_GroupFlag group_flags = OTF2_GROUP_FLAG_NONE;
        switch ( definition->group_type )
        {
            case SCOREP_GROUP_REGIONS:
                paradigm = OTF2_PARADIGM_NONE;
                break;

            case SCOREP_GROUP_MPI_LOCATIONS:
            case SCOREP_GROUP_MPI_GROUP:
            case SCOREP_GROUP_MPI_SELF:
                paradigm = OTF2_PARADIGM_MPI;
                break;

            case SCOREP_GROUP_OPENMP_LOCATIONS:
            case SCOREP_GROUP_OPENMP_THREAD_TEAM:
                paradigm = OTF2_PARADIGM_OPENMP;
                break;

            case SCOREP_GROUP_CUDA_LOCATIONS:
            case SCOREP_GROUP_CUDA_GROUP:
                paradigm = OTF2_PARADIGM_CUDA;
                break;

            case SCOREP_GROUP_OPENCL_LOCATIONS:
            case SCOREP_GROUP_OPENCL_GROUP:
                paradigm = OTF2_PARADIGM_OPENCL;
                break;

            case SCOREP_GROUP_SHMEM_LOCATIONS:
            case SCOREP_GROUP_SHMEM_GROUP:
            case SCOREP_GROUP_SHMEM_SELF:
                paradigm    = OTF2_PARADIGM_SHMEM;
                group_flags = OTF2_GROUP_FLAG_GLOBAL_MEMBERS;
                break;

            case SCOREP_GROUP_PTHREAD_LOCATIONS:
            case SCOREP_GROUP_PTHREAD_THREAD_TEAM:
                paradigm = OTF2_PARADIGM_PTHREAD;
                break;

            case SCOREP_GROUP_TOPOLOGY_HARDWARE_LOCATIONS:
            case SCOREP_GROUP_TOPOLOGY_HARDWARE_GROUP:
                paradigm = OTF2_PARADIGM_HARDWARE;
                break;

            case SCOREP_GROUP_TOPOLOGY_PROCESS_LOCATIONS:
            case SCOREP_GROUP_TOPOLOGY_PROCESS_GROUP:
                paradigm = OTF2_PARADIGM_MEASUREMENT_SYSTEM;
                break;

            case SCOREP_GROUP_TOPOLOGY_USER_LOCATIONS:
            case SCOREP_GROUP_TOPOLOGY_USER_GROUP:
                paradigm = OTF2_PARADIGM_USER;
                break;
        }

        const uint64_t* members = definition->members;
        if ( definition->group_type == SCOREP_GROUP_REGIONS )
        {
            uint64_t* resolved_members = calloc( definition->number_of_members, sizeof( *members ) );
            for ( uint64_t i = 0; i < definition->number_of_members; i++ )
            {
                resolved_members[ i ] = SCOREP_HANDLE_TO_ID( members[ i ], Region, definitionManager->page_manager );
            }
            members = resolved_members;
        }

        OTF2_ErrorCode status = defGroup(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            group_type,
            paradigm,
            group_flags,
            definition->number_of_members,
            members );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Group" );
        }
        if ( members != definition->members )
        {
            free( ( uint64_t* )members );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_metric_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode ( * def_metric_pointer_t )( void*,
                                                       OTF2_MetricMemberRef,
                                                       OTF2_StringRef,
                                                       OTF2_StringRef,
                                                       OTF2_MetricType,
                                                       OTF2_MetricMode,
                                                       OTF2_Type,
                                                       OTF2_Base,
                                                       int64_t,
                                                       OTF2_StringRef );
    def_metric_pointer_t defMetricMember = ( def_metric_pointer_t )
                                           OTF2_DefWriter_WriteMetricMember;
    if ( isGlobal )
    {
        defMetricMember = ( def_metric_pointer_t )
                          OTF2_GlobalDefWriter_WriteMetricMember;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         Metric,
                                                         metric )
    {
        OTF2_ErrorCode status = defMetricMember(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String, definitionManager->page_manager ),
            scorep_tracing_metric_source_type_to_otf2( definition->source_type ),
            scorep_tracing_metric_mode_to_otf2( definition->mode ),
            scorep_tracing_metric_value_type_to_otf2( definition->value_type ),
            scorep_tracing_base_to_otf2( definition->base ),
            definition->exponent,
            SCOREP_HANDLE_TO_ID( definition->unit_handle, String, definitionManager->page_manager ) );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Metric" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_sampling_set_definitions( void*                     writerHandle,
                                       SCOREP_DefinitionManager* definitionManager,
                                       bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode ( * def_metric_class_pointer_t )( void*,
                                                             OTF2_MetricRef,
                                                             uint8_t,
                                                             const uint32_t*,
                                                             OTF2_MetricOccurrence,
                                                             OTF2_RecorderKind );
    def_metric_class_pointer_t defMetricClass = ( def_metric_class_pointer_t )
                                                OTF2_DefWriter_WriteMetricClass;

    typedef OTF2_ErrorCode
    ( * def_metric_instance_pointer_t )( void*,
                                         OTF2_MetricRef,
                                         OTF2_MetricRef,
                                         OTF2_LocationRef,
                                         OTF2_MetricScope,
                                         uint64_t );
    def_metric_instance_pointer_t defMetricInstance =
        ( def_metric_instance_pointer_t )OTF2_DefWriter_WriteMetricInstance;

    typedef OTF2_ErrorCode
    ( * def_metric_class_recorder_pointer_t )( void*,
                                               OTF2_MetricRef,
                                               OTF2_LocationRef );
    def_metric_class_recorder_pointer_t defMetricClassRecorder =
        ( def_metric_class_recorder_pointer_t )OTF2_DefWriter_WriteMetricClassRecorder;

    if ( isGlobal )
    {
        defMetricClass =
            ( def_metric_class_pointer_t )OTF2_GlobalDefWriter_WriteMetricClass;
        defMetricInstance =
            ( def_metric_instance_pointer_t )OTF2_GlobalDefWriter_WriteMetricInstance;
        defMetricClassRecorder =
            ( def_metric_class_recorder_pointer_t )OTF2_GlobalDefWriter_WriteMetricClassRecorder;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         SamplingSet,
                                                         sampling_set )
    {
        OTF2_ErrorCode status;

        if ( !definition->is_scoped )
        {
            uint32_t metric_members[ definition->number_of_metrics ];
            for ( uint8_t i = 0; i < definition->number_of_metrics; i++ )
            {
                metric_members[ i ] = SCOREP_HANDLE_TO_ID(
                    definition->metric_handles[ i ],
                    Metric,
                    definitionManager->page_manager );
            }

            status = defMetricClass(
                writerHandle,
                definition->sequence_number,
                definition->number_of_metrics,
                metric_members,
                scorep_tracing_metric_occurrence_to_otf2( definition->occurrence ),
                scorep_tracing_sampling_set_class_to_otf2( definition->klass ) );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "SamplingSet" );
            }

            /* keep the recorders close to the metric class */
            SCOREP_SamplingSetRecorderHandle recorder_handle = definition->recorders;
            while ( recorder_handle != SCOREP_INVALID_SAMPLING_SET_RECORDER )
            {
                SCOREP_SamplingSetRecorderDef* recorder_definition =
                    SCOREP_HANDLE_DEREF( recorder_handle,
                                         SamplingSetRecorder,
                                         definitionManager->page_manager );
                recorder_handle = recorder_definition->recorders_next;

                status = defMetricClassRecorder(
                    writerHandle,
                    definition->sequence_number,
                    SCOREP_HANDLE_DEREF( recorder_definition->recorder_handle,
                                         Location,
                                         definitionManager->page_manager )->global_location_id );
                if ( status != OTF2_SUCCESS )
                {
                    scorep_handle_definition_writing_error( status, "SamplingSetRecorder" );
                }
            }
        }
        else
        {
            SCOREP_ScopedSamplingSetDef* scoped_definition
                = ( SCOREP_ScopedSamplingSetDef* )definition;

            uint64_t scope;
            switch ( scoped_definition->scope_type )
            {
                case SCOREP_METRIC_SCOPE_LOCATION:
                    scope = SCOREP_HANDLE_DEREF( scoped_definition->scope_handle,
                                                 Location,
                                                 definitionManager->page_manager )->global_location_id;
                    break;

                case SCOREP_METRIC_SCOPE_LOCATION_GROUP:
                    scope = SCOREP_HANDLE_DEREF( scoped_definition->scope_handle,
                                                 LocationGroup,
                                                 definitionManager->page_manager )->global_location_group_id;
                    break;

                case SCOREP_METRIC_SCOPE_SYSTEM_TREE_NODE:
                    scope = SCOREP_HANDLE_TO_ID( scoped_definition->scope_handle,
                                                 SystemTreeNode,
                                                 definitionManager->page_manager );
                    break;

                case SCOREP_METRIC_SCOPE_GROUP:
                    scope = SCOREP_HANDLE_TO_ID( scoped_definition->scope_handle,
                                                 Group,
                                                 definitionManager->page_manager );
                    break;

                default:
                    UTILS_BUG( "Invalid metric scope: %u",
                               scoped_definition->scope_type );
            }

            status = defMetricInstance(
                writerHandle,
                scoped_definition->sequence_number,
                SCOREP_HANDLE_TO_ID( scoped_definition->sampling_set_handle,
                                     SamplingSet,
                                     definitionManager->page_manager ),
                SCOREP_HANDLE_DEREF( scoped_definition->recorder_handle,
                                     Location,
                                     definitionManager->page_manager )->global_location_id,
                scorep_tracing_metric_scope_type_to_otf2(
                    scoped_definition->scope_type ),
                scope );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "ScopedSamplingSet" );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_parameter_definitions( void*                     writerHandle,
                                    SCOREP_DefinitionManager* definitionManager,
                                    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef  OTF2_ErrorCode ( * def_parameter_pointer_t )( void*,
                                                           OTF2_ParameterRef,
                                                           OTF2_StringRef,
                                                           OTF2_ParameterType );
    def_parameter_pointer_t defParameter = ( def_parameter_pointer_t )
                                           OTF2_DefWriter_WriteParameter;
    if ( isGlobal )
    {
        defParameter = ( def_parameter_pointer_t )
                       OTF2_GlobalDefWriter_WriteParameter;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         Parameter,
                                                         parameter )
    {
        OTF2_ErrorCode status = defParameter(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_tracing_parameter_type_to_otf2( definition->parameter_type ) );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Parameter" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_callpath_definitions( void*                     writerHandle,
                                   SCOREP_DefinitionManager* definitionManager,
                                   bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef  OTF2_ErrorCode ( * def_callpath_pointer_t )( void*,
                                                          OTF2_CallpathRef,
                                                          OTF2_CallpathRef,
                                                          OTF2_RegionRef );
    typedef  OTF2_ErrorCode ( * def_callpath_parameter_pointer_t )( void*,
                                                                    OTF2_CallpathRef,
                                                                    OTF2_ParameterRef,
                                                                    OTF2_Type,
                                                                    OTF2_AttributeValue );
    def_callpath_pointer_t defCallpath = ( def_callpath_pointer_t )
                                         OTF2_DefWriter_WriteCallpath;
    def_callpath_parameter_pointer_t defCallpathParameter = ( def_callpath_parameter_pointer_t )
                                                            OTF2_DefWriter_WriteCallpathParameter;
    if ( isGlobal )
    {
        defCallpath = ( def_callpath_pointer_t )
                      OTF2_GlobalDefWriter_WriteCallpath;
        defCallpathParameter = ( def_callpath_parameter_pointer_t )
                               OTF2_GlobalDefWriter_WriteCallpathParameter;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         Callpath,
                                                         callpath )
    {
        uint32_t parent_callpath_sequence_number = OTF2_UNDEFINED_CALLPATH;
        if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
        {
            parent_callpath_sequence_number = SCOREP_HANDLE_TO_ID( definition->parent_callpath_handle,
                                                                   Callpath,
                                                                   definitionManager->page_manager );
        }
        OTF2_ErrorCode status = defCallpath(
            writerHandle,
            definition->sequence_number,
            parent_callpath_sequence_number,
            SCOREP_HANDLE_TO_ID( definition->region_handle,
                                 Region,
                                 definitionManager->page_manager ) );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Callpath" );
        }

        /* Attach parameters */
        for ( uint32_t i = 0; i < definition->number_of_parameters; i++ )
        {
            SCOREP_ParameterType type =
                SCOREP_ParameterHandle_GetType( definition->parameters[ i ].parameter_handle );

            OTF2_AttributeValue value;
            if ( type == SCOREP_PARAMETER_STRING )
            {
                value.stringRef = SCOREP_HANDLE_TO_ID(
                    definition->parameters[ i ].parameter_value.string_handle,
                    String,
                    definitionManager->page_manager );
            }
            else if ( type == SCOREP_PARAMETER_UINT64 )
            {
                value.uint64 = definition->parameters[ i ].parameter_value.integer_value;
            }
            else if ( type == SCOREP_PARAMETER_INT64 )
            {
                value.int64 = definition->parameters[ i ].parameter_value.integer_value;
            }
            else
            {
                UTILS_BUG( "Unhandled parameter type: %u", type );
            }

            status = defCallpathParameter(
                writerHandle,
                definition->sequence_number,
                SCOREP_HANDLE_TO_ID( definition->parameters[ i ].parameter_handle,
                                     Parameter,
                                     definitionManager->page_manager ),
                scorep_tracing_parameter_type_to_otf2_type( type ),
                value );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "CallpathParameter" );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_attribute_definitions( void*                     writerHandle,
                                    SCOREP_DefinitionManager* definitionManager,
                                    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef  OTF2_ErrorCode ( * def_attribute_pointer_t )( void*,
                                                           OTF2_AttributeRef,
                                                           OTF2_StringRef,
                                                           OTF2_StringRef,
                                                           OTF2_Type );
    def_attribute_pointer_t defAttribute = ( def_attribute_pointer_t )
                                           OTF2_DefWriter_WriteAttribute;
    if ( isGlobal )
    {
        defAttribute = ( def_attribute_pointer_t )
                       OTF2_GlobalDefWriter_WriteAttribute;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         Attribute,
                                                         attribute )
    {
        OTF2_ErrorCode status = defAttribute(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String, definitionManager->page_manager ),
            scorep_tracing_attribute_type_to_otf2( definition->type ) );


        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Attribute" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_cartesian_coords_definitions( void*                     writerHandle,
                                           SCOREP_DefinitionManager* definitionManager,
                                           bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( * def_cartesian_coords_pointer_t )( void*,
                                                                 OTF2_CartTopologyRef,
                                                                 uint32_t,
                                                                 uint8_t,
                                                                 const uint32_t* );


    def_cartesian_coords_pointer_t defCartCoords = ( def_cartesian_coords_pointer_t )
                                                   OTF2_DefWriter_WriteCartCoordinate;
    if ( isGlobal )
    {
        defCartCoords = ( def_cartesian_coords_pointer_t )OTF2_GlobalDefWriter_WriteCartCoordinate;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, CartesianCoords, cartesian_coords )
    {
        SCOREP_CartesianTopologyHandle topo = definition->topology_handle;

        OTF2_ErrorCode status = defCartCoords(
            writerHandle,
            SCOREP_HANDLE_TO_ID( topo, CartesianTopology, definitionManager->page_manager ),
            definition->rank,
            definition->n_coords,
            definition->coords_of_current_rank );

        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "CartesianCartCoords" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_cartesian_topology_definitions( void*                     writerHandle,
                                             SCOREP_DefinitionManager* definitionManager,
                                             bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );


    typedef OTF2_ErrorCode ( * def_cartesian_topology_pointer_t )( void*,
                                                                   OTF2_CartTopologyRef,
                                                                   OTF2_StringRef,
                                                                   OTF2_CommRef,
                                                                   uint8_t,
                                                                   const OTF2_CartDimensionRef* );

    def_cartesian_topology_pointer_t defCartTopo = ( def_cartesian_topology_pointer_t )
                                                   OTF2_DefWriter_WriteCartTopology;

    typedef OTF2_ErrorCode ( * def_cartesian_dimension_pointer_t )( void*,
                                                                    OTF2_CartDimensionRef,
                                                                    OTF2_StringRef,
                                                                    uint32_t,
                                                                    OTF2_CartPeriodicity );


    def_cartesian_dimension_pointer_t defCartDim = ( def_cartesian_dimension_pointer_t )
                                                   OTF2_DefWriter_WriteCartDimension;
    if ( isGlobal )
    {
        defCartTopo = ( def_cartesian_topology_pointer_t )OTF2_GlobalDefWriter_WriteCartTopology;
        defCartDim  = ( def_cartesian_dimension_pointer_t )OTF2_GlobalDefWriter_WriteCartDimension;
    }

    /* dimensions don't have a handle in Score-P, but in OTF2, thus generate ids on the fly */
    uint32_t dimension_counter = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, CartesianTopology, cartesian_topology )
    {
        OTF2_CartDimensionRef tempRef[ definition->n_dimensions ];
        for ( int i = 0; i < definition->n_dimensions; i++ )
        {
            OTF2_ErrorCode status = defCartDim(
                writerHandle,
                dimension_counter,
                SCOREP_HANDLE_TO_ID( definition->cartesian_dims[ i ].dimension_name, String, definitionManager->page_manager ),
                definition->cartesian_dims[ i ].n_processes_per_dim,
                definition->cartesian_dims[ i ].periodicity_per_dim ? OTF2_CART_PERIODIC_TRUE : OTF2_CART_PERIODIC_FALSE );
            tempRef[ i ] = dimension_counter;
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "CartesianDimension" );
            }
            dimension_counter++;
        }
        OTF2_ErrorCode status = defCartTopo(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->topology_name, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->communicator_handle, InterimCommunicator, definitionManager->page_manager ),
            definition->n_dimensions,
            tempRef );

        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "CartesianTopology" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_source_code_location_definitions( void*                     writerHandle,
                                               SCOREP_DefinitionManager* definitionManager,
                                               bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef  OTF2_ErrorCode ( * def_source_code_location_pointer_t )( void*,
                                                                      OTF2_SourceCodeLocationRef,
                                                                      OTF2_StringRef,
                                                                      uint32_t );
    def_source_code_location_pointer_t defSourceCodeLocation = ( def_source_code_location_pointer_t )
                                                               OTF2_DefWriter_WriteSourceCodeLocation;
    if ( isGlobal )
    {
        defSourceCodeLocation = ( def_source_code_location_pointer_t )
                                OTF2_GlobalDefWriter_WriteSourceCodeLocation;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         SourceCodeLocation,
                                                         source_code_location )
    {
        OTF2_ErrorCode status = defSourceCodeLocation(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->file_handle, String, definitionManager->page_manager ),
            definition->line_number );

        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Source Code Location" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_calling_context_definitions( void*                     writerHandle,
                                          SCOREP_DefinitionManager* definitionManager,
                                          bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    extern SCOREP_StringHandle scorep_tracing_cct_file;
    extern SCOREP_StringHandle scorep_tracing_cct_ip_offset;
    extern SCOREP_StringHandle scorep_tracing_cct_ip;

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return;
    }

    typedef OTF2_ErrorCode
    ( * def_calling_context_pointer_t )( void*,
                                         OTF2_CallingContextRef,
                                         OTF2_RegionRef,
                                         OTF2_SourceCodeLocationRef,
                                         OTF2_CallingContextRef );
    def_calling_context_pointer_t defCallingContext =
        ( def_calling_context_pointer_t )OTF2_DefWriter_WriteCallingContext;

    typedef OTF2_ErrorCode
    ( * def_calling_context_property_pointer_t )( void*,
                                                  OTF2_CallingContextRef,
                                                  OTF2_StringRef,
                                                  OTF2_Type,
                                                  OTF2_AttributeValue );
    def_calling_context_property_pointer_t defCallingContextProperty =
        ( def_calling_context_property_pointer_t )OTF2_DefWriter_WriteCallingContextProperty;

    OTF2_StringRef file_property_ref      = SCOREP_HANDLE_TO_ID( scorep_tracing_cct_file, String, scorep_local_definition_manager.page_manager );
    OTF2_StringRef ip_offset_property_ref = SCOREP_HANDLE_TO_ID( scorep_tracing_cct_ip_offset, String, scorep_local_definition_manager.page_manager );
    OTF2_StringRef ip_property_ref        = SCOREP_HANDLE_TO_ID( scorep_tracing_cct_ip, String, scorep_local_definition_manager.page_manager );

    if ( isGlobal )
    {
        defCallingContext =
            ( def_calling_context_pointer_t )OTF2_GlobalDefWriter_WriteCallingContext;
        defCallingContextProperty =
            ( def_calling_context_property_pointer_t )OTF2_GlobalDefWriter_WriteCallingContextProperty;

        file_property_ref =
            scorep_local_definition_manager.string.mapping[ file_property_ref ];
        ip_offset_property_ref =
            scorep_local_definition_manager.string.mapping[ ip_offset_property_ref ];
        ip_property_ref =
            scorep_local_definition_manager.string.mapping[ ip_property_ref ];
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, CallingContext, calling_context )
    {
        /* Determine scl id savely */
        OTF2_SourceCodeLocationRef scl = OTF2_UNDEFINED_SOURCE_CODE_LOCATION;
        if ( definition->scl_handle != SCOREP_INVALID_SOURCE_CODE_LOCATION )
        {
            scl = SCOREP_HANDLE_TO_ID( definition->scl_handle, SourceCodeLocation, definitionManager->page_manager );
        }

        /* Determine parent id savely */
        OTF2_CallingContextRef parent = OTF2_UNDEFINED_CALLING_CONTEXT;
        if ( definition->parent_handle != SCOREP_INVALID_CALLING_CONTEXT )
        {
            parent = SCOREP_HANDLE_TO_ID( definition->parent_handle, CallingContext, definitionManager->page_manager );
        }

        /* Write definition */
        OTF2_ErrorCode status = defCallingContext(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->region_handle, Region, definitionManager->page_manager ),
            scl,
            parent );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "CallingContext" );
        }

        if ( definition->ip_offset )
        {
            OTF2_AttributeValue value;

            if ( definition->file_handle != SCOREP_INVALID_STRING )
            {
                value.stringRef = SCOREP_HANDLE_TO_ID( definition->file_handle, String, definitionManager->page_manager );
                defCallingContextProperty(
                    writerHandle,
                    definition->sequence_number,
                    file_property_ref,
                    OTF2_TYPE_STRING,
                    value );

                value.uint64 = definition->ip_offset;
                defCallingContextProperty(
                    writerHandle,
                    definition->sequence_number,
                    ip_offset_property_ref,
                    OTF2_TYPE_UINT64,
                    value );
            }
            else
            {
                value.uint64 = definition->ip_offset;
                defCallingContextProperty(
                    writerHandle,
                    definition->sequence_number,
                    ip_property_ref,
                    OTF2_TYPE_UINT64,
                    value );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_interrupt_generator_definitions( void*                     writerHandle,
                                              SCOREP_DefinitionManager* definitionManager,
                                              bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode
    ( * def_interrupt_generator_pointer_t )( void*,
                                             OTF2_InterruptGeneratorRef,
                                             OTF2_StringRef,
                                             OTF2_InterruptGeneratorMode,
                                             OTF2_Base,
                                             int64_t,
                                             uint64_t );
    def_interrupt_generator_pointer_t defInterruptGenerator =
        ( def_interrupt_generator_pointer_t )OTF2_DefWriter_WriteInterruptGenerator;

    if ( isGlobal )
    {
        defInterruptGenerator =
            ( def_interrupt_generator_pointer_t )OTF2_GlobalDefWriter_WriteInterruptGenerator;
    }


    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, InterruptGenerator, interrupt_generator )
    {
        /* Write definition */
        OTF2_ErrorCode status = defInterruptGenerator(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_tracing_interrupt_generator_mode_to_otf2( definition->mode ),
            scorep_tracing_base_to_otf2( definition->base ),
            definition->exponent,
            definition->period );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "InterruptGenerator" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_io_file_definitions( void*                     writerHandle,
                                  SCOREP_DefinitionManager* definitionManager,
                                  bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode
    ( * def_io_file_pointer_t )( void*,
                                 OTF2_IoFileRef,
                                 OTF2_StringRef,
                                 OTF2_SystemTreeNodeRef );

    def_io_file_pointer_t defIoFile =
        ( def_io_file_pointer_t )OTF2_DefWriter_WriteIoRegularFile;

    typedef OTF2_ErrorCode
    ( * def_io_file_property_pointer_t )( void*,
                                          OTF2_IoFileRef,
                                          OTF2_StringRef,
                                          OTF2_Type,
                                          OTF2_AttributeValue );
    def_io_file_property_pointer_t defIoFileProperty =
        ( def_io_file_property_pointer_t )OTF2_DefWriter_WriteIoFileProperty;

    if ( isGlobal )
    {
        defIoFile =
            ( def_io_file_pointer_t )OTF2_GlobalDefWriter_WriteIoRegularFile;
        defIoFileProperty =
            ( def_io_file_property_pointer_t )OTF2_GlobalDefWriter_WriteIoFileProperty;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, IoFile, io_file )
    {
        OTF2_SystemTreeNodeRef scope = OTF2_UNDEFINED_SYSTEM_TREE_NODE;
        if ( definition->scope != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            scope = SCOREP_HANDLE_TO_ID(
                definition->scope,
                SystemTreeNode,
                definitionManager->page_manager );
        }

        /* Write definition */
        OTF2_ErrorCode status = defIoFile(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->file_name_handle, String, definitionManager->page_manager ),
            scope );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "IoFile" );
        }

        SCOREP_IoFilePropertyHandle property_handle = definition->properties;
        while ( property_handle != SCOREP_INVALID_IO_FILE_PROPERTY )
        {
            SCOREP_IoFilePropertyDef* property =
                SCOREP_HANDLE_DEREF( property_handle,
                                     IoFileProperty,
                                     definitionManager->page_manager );
            property_handle = property->properties_next;

            OTF2_AttributeValue value;
            value.stringRef = SCOREP_HANDLE_TO_ID(
                property->property_value_handle,
                String,
                definitionManager->page_manager );
            status = defIoFileProperty(
                writerHandle,
                definition->sequence_number,
                SCOREP_HANDLE_TO_ID(
                    property->property_name_handle,
                    String,
                    definitionManager->page_manager ),
                OTF2_TYPE_STRING,
                value );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "IoFileProperty" );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_io_handle_definitions( void*                     writerHandle,
                                    SCOREP_DefinitionManager* definitionManager,
                                    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode
    ( * def_io_handle_pointer_t )( void*,
                                   OTF2_IoHandleRef,
                                   OTF2_StringRef,
                                   OTF2_IoFileRef,
                                   OTF2_IoParadigmRef,
                                   OTF2_IoHandleFlag,
                                   OTF2_CommRef,
                                   OTF2_IoHandleRef );

    def_io_handle_pointer_t defIoHandle =
        ( def_io_handle_pointer_t )OTF2_DefWriter_WriteIoHandle;

    if ( isGlobal )
    {
        defIoHandle =
            ( def_io_handle_pointer_t )OTF2_GlobalDefWriter_WriteIoHandle;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, IoHandle, io_handle )
    {
        OTF2_IoFileRef otf2_io_file     = OTF2_UNDEFINED_IO_FILE;
        OTF2_CommRef otf2_comm          = OTF2_UNDEFINED_COMM;
        OTF2_IoHandleRef otf2_io_handle = OTF2_UNDEFINED_IO_HANDLE;

        /* Determine file id safely */
        if ( definition->file_handle != SCOREP_INVALID_IO_FILE )
        {
            otf2_io_file = SCOREP_HANDLE_TO_ID( definition->file_handle,
                                                IoFile,
                                                definitionManager->page_manager );
        }

        /* Determine communicator id safely */
        if ( definition->scope_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            otf2_comm = SCOREP_HANDLE_TO_ID( definition->scope_handle,
                                             InterimCommunicator,
                                             definitionManager->page_manager );
        }

        /* Determine parent id safely */
        if ( definition->parent_handle != SCOREP_INVALID_IO_HANDLE )
        {
            otf2_io_handle = SCOREP_HANDLE_TO_ID( definition->parent_handle,
                                                  IoHandle,
                                                  definitionManager->page_manager );
        }

        /* Write definition */
        OTF2_ErrorCode status = defIoHandle(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            otf2_io_file,
            SCOREP_IoMgmt_GetParadigmId( definition->io_paradigm_type ),
            scorep_tracing_io_handle_flag_to_otf2( definition->flags ),
            otf2_comm,
            otf2_io_handle );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "IoHandle" );
        }

        /*
         * Check whether I/O handle is a pre-created one.
         * In this case we will write a supplement definition
         * to record its access mode and status flags.
         */
        if ( definition->flags & SCOREP_IO_HANDLE_FLAG_PRE_CREATED )
        {
            typedef OTF2_ErrorCode
            ( * def_io_pre_created_handle_state_pointer_t )( void*,
                                                             OTF2_IoHandleRef,
                                                             OTF2_IoAccessMode,
                                                             OTF2_IoStatusFlag );

            def_io_pre_created_handle_state_pointer_t defIoPreCreatedHandleState =
                ( def_io_pre_created_handle_state_pointer_t )OTF2_DefWriter_WriteIoPreCreatedHandleState;

            if ( isGlobal )
            {
                defIoPreCreatedHandleState =
                    ( def_io_pre_created_handle_state_pointer_t )OTF2_GlobalDefWriter_WriteIoPreCreatedHandleState;
            }

            /* Write supplement definition */
            status = defIoPreCreatedHandleState(
                writerHandle,
                definition->sequence_number,
                scorep_tracing_io_access_mode_to_otf2( definition->access_mode ),
                scorep_tracing_io_status_flags_to_otf2( definition->status_flags ) );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "IoIoPreCreatedHandleState" );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


/**
 * Generate and write the id mapping for definition type @a type into the
 * OTF2 local definition writer @a definition_writer.
 *
 * @note @a TYPE denotes the all-caps OTF2 name of the definition type.
 */
#define WRITE_MAPPING( definition_writer, bitSize, type, OTF2_TYPE ) \
    do \
    { \
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Writing %s mappings", #type ); \
        if ( scorep_local_definition_manager.type.mapping && \
             scorep_local_definition_manager.type.counter > 0 ) \
        { \
            OTF2_IdMap* map = OTF2_IdMap_CreateFromUint ## bitSize ## Array( \
                scorep_local_definition_manager.type.counter, \
                ( const uint ## bitSize ## _t* )scorep_local_definition_manager.type.mapping, \
                true ); \
            /* map can be NULL if the mapping is the identity */ \
            if ( map ) \
            { \
                OTF2_ErrorCode status = OTF2_DefWriter_WriteMappingTable( \
                    definition_writer, \
                    OTF2_MAPPING_ ## OTF2_TYPE, \
                    map ); \
                UTILS_ASSERT( status == OTF2_SUCCESS ); \
                OTF2_IdMap_Free( map ); \
            } \
        } \
    } \
    while ( 0 )


void
scorep_tracing_write_mappings( OTF2_DefWriter* localDefinitionWriter )
{
    WRITE_MAPPING( localDefinitionWriter, 32, string, STRING );

    /*
     * Location definitions need special treatment as their global id is 64bit
     * and they are not derived by the unification algorithm.
     */
    WRITE_MAPPING( localDefinitionWriter, 64, location, LOCATION );

    WRITE_MAPPING( localDefinitionWriter, 32, region, REGION );
    WRITE_MAPPING( localDefinitionWriter, 32, group, GROUP );
    WRITE_MAPPING( localDefinitionWriter, 32, interim_communicator, COMM );
    WRITE_MAPPING( localDefinitionWriter, 32, rma_window, RMA_WIN );
    WRITE_MAPPING( localDefinitionWriter, 32, sampling_set, METRIC );
    WRITE_MAPPING( localDefinitionWriter, 32, attribute, ATTRIBUTE );
    WRITE_MAPPING( localDefinitionWriter, 32, source_code_location, SOURCE_CODE_LOCATION );
    WRITE_MAPPING( localDefinitionWriter, 32, calling_context, CALLING_CONTEXT );
    WRITE_MAPPING( localDefinitionWriter, 32, interrupt_generator, INTERRUPT_GENERATOR );
    WRITE_MAPPING( localDefinitionWriter, 32, io_file, IO_FILE );
    WRITE_MAPPING( localDefinitionWriter, 32, io_handle, IO_HANDLE );

    // do we need Callpath and Parameter mappings for tracing?
    WRITE_MAPPING( localDefinitionWriter, 32, parameter, PARAMETER );
}

static void
write_clock_offset_cb( SCOREP_ClockOffset* clockOffset,
                       void*               userData )
{
    OTF2_DefWriter* writer = userData;
    OTF2_ErrorCode status  = OTF2_DefWriter_WriteClockOffset(
        writer,
        clockOffset->time,
        clockOffset->offset,
        clockOffset->stddev );
    UTILS_ASSERT( status == OTF2_SUCCESS );
}

void
scorep_tracing_write_clock_offsets( OTF2_DefWriter* localDefinitionWriter )
{
    SCOREP_ForAllClockOffsets( write_clock_offset_cb, localDefinitionWriter );
}

#if HAVE( SCOREP_DEBUG )
void
scorep_tracing_write_local_definitions( OTF2_DefWriter* localDefinitionWriter )
{
    extern bool scorep_debug_unify;
    if ( !scorep_debug_unify )
    {
        return;
    }

    scorep_write_string_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_system_tree_node_definitions(     localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_group_definitions(       localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_definitions(             localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_property_definitions(    localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_region_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_group_definitions(                localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_metric_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_sampling_set_definitions(         localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_parameter_definitions(            localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_callpath_definitions(             localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_attribute_definitions(            localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_source_code_location_definitions( localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_calling_context_definitions(      localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_interrupt_generator_definitions(  localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_cartesian_topology_definitions(   localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_cartesian_coords_definitions(     localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_io_file_definitions(              localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_io_handle_definitions(            localDefinitionWriter, &scorep_local_definition_manager, false );
}
#else
void
scorep_tracing_write_local_definitions( OTF2_DefWriter* localDefinitionWriter )
{
    return;
}
#endif


static void
write_paradigm_cb( SCOREP_Paradigm* paradigm,
                   void*            userData )
{
    void** args                                  = userData;
    OTF2_GlobalDefWriter* writer                 = args[ 0 ];
    SCOREP_DefinitionManager* definition_manager = args[ 1 ];

    OTF2_Paradigm otf2_paradigm =
        scorep_tracing_paradigm_to_otf2( paradigm->paradigm_type );

    OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteParadigm(
        writer,
        otf2_paradigm,
        SCOREP_HANDLE_TO_ID(
            SCOREP_LOCAL_HANDLE_DEREF(
                paradigm->name_handle,
                String )->unified,
            String,
            definition_manager->page_manager ),
        scorep_tracing_paradigm_class_to_otf2( paradigm->paradigm_class ) );
    UTILS_ASSERT( status == OTF2_SUCCESS );

    /* Each set paradigm flag is a boolean property with value true in OTF2. */
    OTF2_Type type;
    OTF2_AttributeValue value;
    OTF2_AttributeValue_SetBoolean( OTF2_TRUE,
                                    &type,
                                    &value );

    SCOREP_ParadigmFlags paradigm_flags = paradigm->paradigm_flags;
    SCOREP_ParadigmFlags flag_it        = 1;
    OTF2_ParadigmProperty property;
    while ( paradigm_flags )
    {
        if ( paradigm_flags & flag_it )
        {
            /* Clear flag */
            paradigm_flags &= ~flag_it;

            property = scorep_tracing_paradigm_boolean_property_to_otf2( flag_it );

            status = OTF2_GlobalDefWriter_WriteParadigmProperty( writer,
                                                                 otf2_paradigm,
                                                                 property,
                                                                 type,
                                                                 value );
            UTILS_ASSERT( status == OTF2_SUCCESS );
        }

        flag_it <<= 1;
    }

    /* Write other paradigm properties. */
    for ( int i = 0; i < SCOREP_INVALID_PARADIGM_PROPERTY; i++ )
    {
        if ( paradigm->property_handles[ i ] == SCOREP_MOVABLE_NULL )
        {
            continue;
        }

        switch ( i )
        {
            /* For now only String typed properties known. */
            case SCOREP_PARADIGM_PROPERTY_COMMUNICATOR_TEMPLATE:
            case SCOREP_PARADIGM_PROPERTY_RMA_WINDOW_TEMPLATE:
                property        = scorep_tracing_paradigm_property_to_otf2( i );
                type            = OTF2_TYPE_STRING;
                value.stringRef = SCOREP_HANDLE_TO_ID(
                    SCOREP_LOCAL_HANDLE_DEREF(
                        paradigm->property_handles[ i ],
                        String )->unified,
                    String,
                    definition_manager->page_manager );
                break;

            default:
                UTILS_BUG( "Unhandled paradigm property: %u", i );
                break;
        }

        status = OTF2_GlobalDefWriter_WriteParadigmProperty( writer,
                                                             otf2_paradigm,
                                                             property,
                                                             type,
                                                             value );
        UTILS_ASSERT( status == OTF2_SUCCESS );
    }
}

static void
write_paradigms( OTF2_GlobalDefWriter*     globalDefinitionWriter,
                 SCOREP_DefinitionManager* definitionManager )
{
    void* args[ 2 ] = {
        globalDefinitionWriter,
        definitionManager
    };
    SCOREP_ForAllParadigms( write_paradigm_cb, args );
}

static void
write_io_paradigm_cb( SCOREP_IoParadigm* paradigm,
                      void*              userData )
{
    void** args                                  = userData;
    OTF2_GlobalDefWriter* writer                 = args[ 0 ];
    SCOREP_DefinitionManager* definition_manager = args[ 1 ];

    /* Collect paradigm properties. */
    uint8_t number_of_properties = 0;
    OTF2_IoParadigmProperty properties[ SCOREP_INVALID_PARADIGM_PROPERTY ];
    OTF2_Type types[ SCOREP_INVALID_PARADIGM_PROPERTY ];
    OTF2_AttributeValue values[ SCOREP_INVALID_PARADIGM_PROPERTY ];
    for ( int i = 0; i < SCOREP_INVALID_IO_PARADIGM_PROPERTY; i++ )
    {
        if ( paradigm->property_handles[ i ] == SCOREP_MOVABLE_NULL )
        {
            continue;
        }

        switch ( i )
        {
            /* For now only String typed properties known. */
            case SCOREP_IO_PARADIGM_PROPERTY_VERSION:
                properties[ number_of_properties ]       = scorep_tracing_io_paradigm_property_to_otf2( i );
                types[ number_of_properties ]            = OTF2_TYPE_STRING;
                values[ number_of_properties ].stringRef = SCOREP_HANDLE_TO_ID(
                    SCOREP_LOCAL_HANDLE_DEREF(
                        paradigm->property_handles[ i ],
                        String )->unified,
                    String,
                    definition_manager->page_manager );
                break;

            default:
                UTILS_BUG( "Unhandled I/O paradigm property: %u", i );
                break;
        }

        number_of_properties++;
    }

    OTF2_ErrorCode status = OTF2_GlobalDefWriter_WriteIoParadigm(
        writer,
        paradigm->sequence_number,
        SCOREP_HANDLE_TO_ID(
            SCOREP_LOCAL_HANDLE_DEREF( paradigm->identification_handle, String )->unified,
            String, definition_manager->page_manager ),
        SCOREP_HANDLE_TO_ID(
            SCOREP_LOCAL_HANDLE_DEREF( paradigm->name_handle, String )->unified,
            String, definition_manager->page_manager ),
        scorep_tracing_io_paradigm_class_to_otf2( paradigm->paradigm_class ),
        scorep_tracing_io_paradigm_flags_to_otf2( paradigm->paradigm_flags ),
        number_of_properties, properties, types, values );
    UTILS_ASSERT( status == OTF2_SUCCESS );
}

static void
write_io_paradigms( OTF2_GlobalDefWriter*     globalDefinitionWriter,
                    SCOREP_DefinitionManager* definitionManager )
{
    void* args[ 2 ] = {
        globalDefinitionWriter,
        definitionManager
    };
    SCOREP_ForAllIoParadigms( write_io_paradigm_cb, args );
}

void
scorep_tracing_write_global_definitions( OTF2_GlobalDefWriter* global_definition_writer )
{
    UTILS_ASSERT( SCOREP_Status_GetRank() == 0 );
    UTILS_ASSERT( scorep_unified_definition_manager );

    scorep_write_string_definitions( global_definition_writer, scorep_unified_definition_manager, true );

    /* Write these after the strings, as they reference these already. */
    write_paradigms( global_definition_writer,
                     scorep_unified_definition_manager );
    write_io_paradigms( global_definition_writer,
                        scorep_unified_definition_manager );

    if ( SCOREP_Env_UseSystemTreeSequence() )
    {
        write_system_tree_sequence_definitions( global_definition_writer, scorep_unified_definition_manager );
    }
    else
    {
        scorep_write_system_tree_node_definitions(     global_definition_writer, scorep_unified_definition_manager, true );
        scorep_write_location_group_definitions(       global_definition_writer, scorep_unified_definition_manager, true );
        scorep_write_location_definitions(             global_definition_writer, scorep_unified_definition_manager, true );
        scorep_write_location_property_definitions(    global_definition_writer, scorep_unified_definition_manager, true );
    }
    scorep_write_region_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_group_definitions(                global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_communicator_definitions(         global_definition_writer, scorep_unified_definition_manager );
    scorep_write_rma_window_definitions(           global_definition_writer, scorep_unified_definition_manager );
    scorep_write_metric_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_sampling_set_definitions(         global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_parameter_definitions(            global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_callpath_definitions(             global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_attribute_definitions(            global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_source_code_location_definitions( global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_calling_context_definitions(      global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_interrupt_generator_definitions(  global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_cartesian_topology_definitions(   global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_cartesian_coords_definitions(     global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_io_file_definitions(              global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_io_handle_definitions(            global_definition_writer, scorep_unified_definition_manager, true );
}

void
scorep_tracing_set_properties( OTF2_Archive* scorep_otf2_archive )
{
    UTILS_ASSERT( scorep_unified_definition_manager );

    /* set all defined properties*/

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                         Property,
                                                         property )
    {
        /* convert scorep property enum value to otf2 string */

        char* property_name = scorep_tracing_property_to_otf2( definition->property );

        /* set property */

        OTF2_Archive_SetBoolProperty( scorep_otf2_archive,
                                      property_name,
                                      definition->invalidated
                                      ? !definition->initialValue
                                      : definition->initialValue,
                                      false );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}
