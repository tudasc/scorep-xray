/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
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
 * @brief   Handles tracing specific definitions tasks.
 *
 *
 */


#include <config.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#include <otf2/otf2.h>


#include <UTILS_Error.h>


#include <UTILS_Debug.h>


#include <scorep/SCOREP_PublicTypes.h>
#include <scorep_runtime_management.h>
#include <scorep_environment.h>
#include <scorep_status.h>
#include <scorep_clock_synchronization.h>
#include <SCOREP_Memory.h>
#include <definitions/SCOREP_Definitions.h>
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
    typedef OTF2_ErrorCode ( *def_string_pointer_t )( void*,
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
    typedef OTF2_ErrorCode ( *def_location_pointer_t )( void*,
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
    typedef OTF2_ErrorCode ( *def_location_property_pointer_t )( void*,
                                                                 OTF2_LocationRef,
                                                                 OTF2_StringRef,
                                                                 OTF2_StringRef );
    def_location_property_pointer_t defLocationProperty =
        ( def_location_property_pointer_t )OTF2_DefWriter_WriteLocationProperty;

    if ( isGlobal )
    {
        defLocationProperty = ( def_location_property_pointer_t )
                              OTF2_GlobalDefWriter_WriteLocationProperty;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager, LocationProperty, location_property )
    {
        OTF2_ErrorCode status = defLocationProperty(
            writerHandle,
            SCOREP_HANDLE_DEREF( definition->location_handle, Location, definitionManager->page_manager )->global_location_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->value_handle, String, definitionManager->page_manager ) );

        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "LocationProperty" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
scorep_write_location_group_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( *def_location_group_pointer_t )( void*,
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
scorep_write_system_tree_node_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );

    typedef OTF2_ErrorCode
    ( *def_system_tree_node_pointer_t )( void*,
                                         OTF2_SystemTreeNodeRef,
                                         OTF2_StringRef,
                                         OTF2_StringRef,
                                         OTF2_SystemTreeNodeRef );
    def_system_tree_node_pointer_t defSystemTreeNode =
        ( def_system_tree_node_pointer_t )OTF2_DefWriter_WriteSystemTreeNode;

    typedef OTF2_ErrorCode
    ( *def_system_tree_node_domain_pointer_t )( void*,
                                                OTF2_SystemTreeNodeRef,
                                                OTF2_SystemTreeDomain );
    def_system_tree_node_domain_pointer_t defSystemTreeNodeDomain =
        ( def_system_tree_node_domain_pointer_t )OTF2_DefWriter_WriteSystemTreeNodeDomain;

    typedef OTF2_ErrorCode
    ( *def_system_tree_node_property_pointer_t )( void*,
                                                  OTF2_SystemTreeNodeRef,
                                                  OTF2_StringRef,
                                                  OTF2_StringRef );
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

            status = defSystemTreeNodeProperty(
                writerHandle,
                definition->sequence_number,
                SCOREP_HANDLE_TO_ID(
                    property->property_name_handle,
                    String,
                    definitionManager->page_manager ),
                SCOREP_HANDLE_TO_ID(
                    property->property_value_handle,
                    String,
                    definitionManager->page_manager ) );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "SystemTreeNodeProperty" );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}


static void
scorep_write_region_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    UTILS_ASSERT( writerHandle );
    typedef OTF2_ErrorCode ( *def_region_pointer_t )( void*,
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
            scorep_tracing_get_otf2_paradigm( definition->paradigm_type );

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


    typedef OTF2_ErrorCode ( *def_group_pointer_t )( void*,
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
        }

        OTF2_ErrorCode status = defGroup(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            group_type,
            paradigm,
            group_flags,
            definition->number_of_members,
            definition->members );
        if ( status != OTF2_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "Group" );
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

    typedef OTF2_ErrorCode ( *def_metric_pointer_t )( void*,
                                                      OTF2_MetricMemberRef,
                                                      OTF2_StringRef,
                                                      OTF2_StringRef,
                                                      OTF2_MetricType,
                                                      OTF2_MetricMode,
                                                      OTF2_Type,
                                                      OTF2_MetricBase,
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
            scorep_tracing_metric_base_to_otf2( definition->base ),
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

    typedef OTF2_ErrorCode ( *def_metric_class_pointer_t )( void*,
                                                            OTF2_MetricRef,
                                                            uint8_t,
                                                            const uint32_t*,
                                                            OTF2_MetricOccurrence,
                                                            OTF2_RecorderKind );
    def_metric_class_pointer_t defMetricClass = ( def_metric_class_pointer_t )
                                                OTF2_DefWriter_WriteMetricClass;

    typedef OTF2_ErrorCode
    ( *def_metric_instance_pointer_t )( void*,
                                        OTF2_MetricRef,
                                        OTF2_MetricRef,
                                        OTF2_LocationRef,
                                        OTF2_MetricScope,
                                        uint64_t );
    def_metric_instance_pointer_t defMetricInstance =
        ( def_metric_instance_pointer_t )OTF2_DefWriter_WriteMetricInstance;

    typedef OTF2_ErrorCode
    ( *def_metric_class_recorder_pointer_t )( void*,
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
            ( def_metric_class_recorder_pointer_t )OTF2_GlobalDefWriter_WriteMetricInstance;
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
                    UTILS_BUG( "Invalid metric scope" );
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

    typedef  OTF2_ErrorCode ( *def_parameter_pointer_t )( void*,
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

    typedef  OTF2_ErrorCode ( *def_callpath_pointer_t )( void*,
                                                         OTF2_CallpathRef,
                                                         OTF2_CallpathRef,
                                                         OTF2_RegionRef );
    def_callpath_pointer_t defCallpath = ( def_callpath_pointer_t )
                                         OTF2_DefWriter_WriteCallpath;
    if ( isGlobal )
    {
        defCallpath = ( def_callpath_pointer_t )
                      OTF2_GlobalDefWriter_WriteCallpath;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definitionManager,
                                                         Callpath,
                                                         callpath )
    {
        if ( !definition->with_parameter )
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
                SCOREP_HANDLE_TO_ID( definition->callpath_argument.region_handle,
                                     Region,
                                     definitionManager->page_manager ) );
            if ( status != OTF2_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "Callpath" );
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
#define WRITE_MAPPING( definition_writer, type, OTF2_TYPE ) \
    do \
    { \
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Write mappings to OTF2 for %s", #type ); \
        if ( scorep_local_definition_manager.type.mapping && \
             scorep_local_definition_manager.type.counter > 0 ) \
        { \
            OTF2_IdMap* map = OTF2_IdMap_CreateFromUint32Array( \
                scorep_local_definition_manager.type.counter, \
                scorep_local_definition_manager.type.mapping, \
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
    WRITE_MAPPING( localDefinitionWriter, string, STRING );
    WRITE_MAPPING( localDefinitionWriter, region, REGION );
    WRITE_MAPPING( localDefinitionWriter, group, GROUP );
    WRITE_MAPPING( localDefinitionWriter, interim_communicator, COMM );
    WRITE_MAPPING( localDefinitionWriter, interim_rma_window, RMA_WIN );
    WRITE_MAPPING( localDefinitionWriter, sampling_set, METRIC );

    // do we need Callpath and Parameter mappings for tracing?
    WRITE_MAPPING( localDefinitionWriter, parameter, PARAMETER );
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

    scorep_write_string_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_system_tree_node_definitions(       localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_group_definitions(         localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_property_definitions(      localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_region_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_group_definitions(                  localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_metric_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_sampling_set_definitions(           localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_parameter_definitions(              localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_callpath_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
}
#else
void
scorep_tracing_write_local_definitions( OTF2_DefWriter* localDefinitionWriter )
{
    return;
}
#endif


void
scorep_tracing_write_global_definitions( OTF2_GlobalDefWriter* global_definition_writer )
{
    UTILS_ASSERT( SCOREP_Status_GetRank() == 0 );
    UTILS_ASSERT( scorep_unified_definition_manager );

    scorep_write_string_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_system_tree_node_definitions(       global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_group_definitions(         global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_property_definitions(      global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_region_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_group_definitions(                  global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_communicator_definitions(           global_definition_writer, scorep_unified_definition_manager );
    scorep_write_rma_window_definitions(             global_definition_writer, scorep_unified_definition_manager );
    scorep_write_metric_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_sampling_set_definitions(           global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_parameter_definitions(              global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_callpath_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
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
