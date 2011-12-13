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
 * @status      alpha
 * @file        src/measurement/tracing/scorep_tracing_definitions.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Handles tracing specific definitions tasks.
 *
 *
 */


#include <config.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <otf2/otf2.h>


#include <scorep_utility/SCOREP_Debug.h>


#include <scorep/SCOREP_PublicTypes.h>
#include <scorep_runtime_management.h>
#include <scorep_environment.h>
#include <scorep_mpi.h>
#include <scorep_clock_synchronization.h>

#include <scorep_definitions.h>
#include <scorep_definition_structs.h>
#include <scorep_definition_macros.h>


extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;


static void
scorep_handle_definition_writing_error( SCOREP_Error_Code status,
                                        const char*       definitionType )
{
    assert( false ); // implement me
}


static inline OTF2_LocationType
scorep_location_type_to_otf_location_type( SCOREP_LocationType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_LOCATION_TYPE_ ## SCOREP: \
        return OTF2_LOCATION_TYPE_ ## OTF2

        case_return( CPU_THREAD, CPU_THREAD );
        case_return( GPU, GPU );
        case_return( METRIC, METRIC );

#undef case_return
        default:
            SCOREP_BUG( "Invalid location type" );
    }
}

static inline OTF2_LocationGroupType
scorep_location_group_type_to_otf_location_group_type( SCOREP_LocationGroupType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_LOCATION_GROUP_TYPE_ ## SCOREP: \
        return OTF2_LOCATION_GROUP_TYPE_ ## OTF2

        case_return( PROCESS, PROCESS );

#undef case_return
        default:
            SCOREP_BUG( "Invalid location group type" );
    }
}

static inline OTF2_RegionType
scorep_region_type_to_otf_region_type( SCOREP_RegionType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_REGION_ ## SCOREP: \
        return OTF2_REGION_TYPE_ ## OTF2

        case_return( UNKNOWN,              UNKNOWN );
        case_return( FUNCTION,             FUNCTION );
        case_return( LOOP,                 LOOP );
        case_return( USER,                 USER_REGION );
        case_return( MPI_COLL_BARRIER,     MPI_COLL_BARRIER );
        case_return( MPI_COLL_ONE2ALL,     MPI_COLL_ONE2ALL );
        case_return( MPI_COLL_ALL2ONE,     MPI_COLL_ALL2ONE );
        case_return( MPI_COLL_ALL2ALL,     MPI_COLL_ALL2ALL );
        case_return( MPI_COLL_OTHER,       MPI_COLL_OTHER );
        case_return( OMP_PARALLEL,         OMP_PARALLEL );
        case_return( OMP_LOOP,             OMP_LOOP );
        case_return( OMP_SECTIONS,         OMP_SECTIONS );
        case_return( OMP_SECTION,          OMP_SECTION );
        case_return( OMP_WORKSHARE,        OMP_WORKSHARE );
        case_return( OMP_SINGLE,           OMP_SINGLE );
        case_return( OMP_MASTER,           OMP_MASTER );
        case_return( OMP_CRITICAL,         OMP_CRITICAL );
        case_return( OMP_ATOMIC,           OMP_ATOMIC );
        case_return( OMP_BARRIER,          OMP_BARRIER );
        case_return( OMP_IMPLICIT_BARRIER, OMP_IBARRIER );
        case_return( OMP_FLUSH,            OMP_FLUSH );
        case_return( OMP_CRITICAL_SBLOCK,  OMP_CRITICAL_SBLOCK );
        case_return( OMP_SINGLE_SBLOCK,    OMP_SINGLE_SBLOCK );
        case_return( OMP_WRAPPER,          OMP_WRAPPER );
        case_return( OMP_TASK,             OMP_TASK );
        case_return( OMP_TASKWAIT,         OMP_TASK_WAIT );
        case_return( PHASE,                PHASE );
        case_return( DYNAMIC,              DYNAMIC );
        case_return( DYNAMIC_PHASE,        DYNAMIC_PHASE );
        case_return( DYNAMIC_LOOP,         DYNAMIC_LOOP );
        case_return( DYNAMIC_FUNCTION,     DYNAMIC_FUNCTION );
        case_return( DYNAMIC_LOOP_PHASE,   DYNAMIC_LOOP_PHASE );

#undef case_return
        default:
            SCOREP_BUG( "Invalid region type" );
    }
}

static inline OTF2_GroupType
scorep_group_type_to_otf_group_type( SCOREP_GroupType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_GROUP_ ## SCOREP: \
        return OTF2_GROUPTYPE_ ## OTF2

        case_return( UNKNOWN,       UNKNOWN );
        case_return( LOCATIONS,     LOCATIONS );
        case_return( REGIONS,       REGIONS );
        case_return( METRIC,        METRIC );
        case_return( COMM_SELF,     COMMUNICATOR_SELF );
        case_return( MPI_GROUP,     MPI_GROUP );
        case_return( MPI_LOCATIONS, MPI_LOCATIONS );

#undef case_return
        default:
            SCOREP_BUG( "Invalid group type" );
    }
}

static inline OTF2_MetricType
scorep_metric_source_type_to_otf_metric_type( SCOREP_MetricSourceType sourceType )
{
    switch ( sourceType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_SOURCE_TYPE_ ## SCOREP: \
        return OTF2_METRIC_TYPE_ ## OTF2

        case_return( PAPI,   PAPI );
        case_return( RUSAGE, RUSAGE );
        case_return( USER,   USER );
        case_return( OTHER,  OTHER );

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric source type" );
    }
}

static inline OTF2_MetricMode
scorep_metric_mode_to_otf_metric_mode( SCOREP_MetricMode mode )
{
    switch ( mode )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_MODE_ ## SCOREP: \
        return OTF2_METRIC_ ## OTF2

        case_return( ACCUMULATED_START, ACCUMULATED_START );
        case_return( ACCUMULATED_POINT, ACCUMULATED_POINT );
        case_return( ACCUMULATED_LAST,  ACCUMULATED_LAST );
        case_return( ACCUMULATED_NEXT,  ACCUMULATED_NEXT );

        case_return( ABSOLUTE_POINT, ABSOLUTE_POINT );
        case_return( ABSOLUTE_LAST,  ABSOLUTE_LAST );
        case_return( ABSOLUTE_NEXT,  ABSOLUTE_NEXT );

        case_return( RELATIVE_POINT, RELATIVE_POINT );
        case_return( RELATIVE_LAST,  RELATIVE_LAST );
        case_return( RELATIVE_NEXT,  RELATIVE_NEXT );

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric mode" );
    }
}

static inline OTF2_TypeID
scorep_metric_value_type_to_otf_metric_value_type( SCOREP_MetricValueType valueType )
{
    switch ( valueType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_VALUE_ ## SCOREP: \
        return OTF2_ ## OTF2

        case_return( INT64,  INT64_T );
        case_return( UINT64, UINT64_T );
        case_return( DOUBLE, DOUBLE );

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric value type" );
    }
}

static inline OTF2_MetricBase
scorep_metric_base_to_otf_metric_base( SCOREP_MetricBase base )
{
    switch ( base )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_BASE_ ## SCOREP: \
        return OTF2_BASE_ ## OTF2

        case_return( BINARY,  BINARY );
        case_return( DECIMAL, DECIMAL );

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric base" );
    }
}

static inline OTF2_MetricOccurrence
scorep_metric_occurrence_to_otf_metric_occurrence( SCOREP_MetricOccurrence occurrence )
{
    switch ( occurrence )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_OCCURRENCE_ ## SCOREP: \
        return OTF2_METRIC_ ## OTF2

        case_return( SYNCHRONOUS_STRICT,  SYNCHRONOUS_STRICT );
        case_return( SYNCHRONOUS, SYNCHRONOUS );
        case_return( ASYNCHRONOUS, ASYNCHRONOUS );

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric occurrence" );
    }
}

static inline OTF2_MetricScope
scorep_metric_scope_type_to_otf_metric_scope_type( SCOREP_MetricScope scope )
{
    switch ( scope )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_SCOPE_ ## SCOREP: \
        return OTF2_SCOPE_ ## OTF2

        case_return( LOCATION, LOCATION );
        case_return( LOCATION_GROUP, LOCATION_GROUP );
        case_return( SYSTEM_TREE_NODE, SYSTEM_TREE_NODE );
        case_return( GROUP, GROUP );

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric scope" );
    }
}

static uint8_t
scorep_parameter_type_to_otf_parameter_type( SCOREP_ParameterType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_PARAMETER_ ## SCOREP: \
        return OTF2_PARAMETER_TYPE_ ## OTF2

        case_return( STRING, STRING );
        case_return( INT64,  INT64 );
        case_return( UINT64, UINT64 );

#undef case_return
        default:
            SCOREP_BUG( "Invalid parameter type" );
    }
}


static void
scorep_write_string_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    assert( writerHandle );
    typedef SCOREP_Error_Code ( *def_string_pointer_t )( void*,
                                                         uint32_t,
                                                         char* );
    def_string_pointer_t defString = ( def_string_pointer_t )
                                     OTF2_DefWriter_WriteString;

    if ( isGlobal )
    {
        defString = ( def_string_pointer_t )
                    OTF2_GlobalDefWriter_WriteString;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, String, string )
    {
        SCOREP_Error_Code status = defString(
            writerHandle,
            definition->sequence_number,
            definition->string_data );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_String_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_location_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    assert( writerHandle );
    typedef SCOREP_Error_Code ( *def_location_pointer_t )( void*,
                                                           uint64_t,
                                                           uint32_t,
                                                           OTF2_LocationType,
                                                           uint64_t,
                                                           uint64_t,
                                                           uint64_t,
                                                           uint64_t );
    def_location_pointer_t defLocation = ( def_location_pointer_t )
                                         OTF2_DefWriter_WriteLocation;

    if ( isGlobal )
    {
        defLocation = ( def_location_pointer_t )
                      OTF2_GlobalDefWriter_WriteLocation;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, Location, location )
    {
        SCOREP_Error_Code status = defLocation(
            writerHandle,
            definition->global_location_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_location_type_to_otf_location_type( definition->location_type ),
            definition->number_of_events,
            definition->number_of_definitions,
            definition->timer_resolution,
            definition->location_group_id );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Location_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_location_group_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    assert( writerHandle );
    typedef SCOREP_Error_Code ( *def_location_group_pointer_t )( void*,
                                                                 uint64_t,
                                                                 uint32_t,
                                                                 OTF2_LocationGroupType,
                                                                 uint32_t );
    def_location_group_pointer_t defLocationGroup =
        ( def_location_group_pointer_t )OTF2_DefWriter_WriteLocationGroup;
    if ( isGlobal )
    {
        defLocationGroup =  ( def_location_group_pointer_t )
                           OTF2_GlobalDefWriter_WriteLocationGroup;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, LocationGroup, location_group )
    {
        SCOREP_Error_Code status = defLocationGroup(
            writerHandle,
            definition->global_location_group_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_location_group_type_to_otf_location_group_type( definition->location_group_type ),
            SCOREP_HANDLE_TO_ID( definition->parent, SystemTreeNode, definitionManager->page_manager ) );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_LocationGroup_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_system_tree_node_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    assert( writerHandle );
    typedef SCOREP_Error_Code ( *def_system_tree_node_pointer_t )( void*,
                                                                   uint32_t,
                                                                   uint32_t,
                                                                   uint32_t,
                                                                   uint32_t );
    def_system_tree_node_pointer_t defSystemTreeNode =
        ( def_system_tree_node_pointer_t )OTF2_DefWriter_WriteSystemTreeNode;
    if ( isGlobal )
    {
        defSystemTreeNode =  ( def_system_tree_node_pointer_t )
                            OTF2_GlobalDefWriter_WriteSystemTreeNode;
    }


    SCOREP_DEFINITION_FOREACH_DO( definitionManager, SystemTreeNode, system_tree_node )
    {
        /* Determine parent id savely */
        uint32_t parent = OTF2_UNDEFINED_UINT32;
        if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            parent = SCOREP_HANDLE_TO_ID( definition->parent_handle, SystemTreeNode, definitionManager->page_manager );
        }

        /* Write defintion */
        SCOREP_Error_Code status = defSystemTreeNode(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->class_handle, String, definitionManager->page_manager ),
            parent );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_SystemTreeNode_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_region_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    assert( writerHandle );
    typedef SCOREP_Error_Code ( *def_region_pointer_t )( void*,
                                                         uint32_t,
                                                         uint32_t,
                                                         uint32_t,
                                                         OTF2_RegionType,
                                                         uint32_t,
                                                         uint32_t,
                                                         uint32_t );

    def_region_pointer_t defRegion = ( def_region_pointer_t )
                                     OTF2_DefWriter_WriteRegion;
    if ( isGlobal )
    {
        defRegion = ( def_region_pointer_t )OTF2_GlobalDefWriter_WriteRegion;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, Region, region )
    {
        uint32_t source_file_id = OTF2_UNDEFINED_UINT32;
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            source_file_id = SCOREP_HANDLE_TO_ID(
                definition->file_name_handle,
                String,
                definitionManager->page_manager );
        }

        SCOREP_Error_Code status = defRegion(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String, definitionManager->page_manager ),
            scorep_region_type_to_otf_region_type( definition->region_type ),
            source_file_id,
            definition->begin_line,
            definition->end_line );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Region_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_communicator_definitions( void*                     writerHandle,
                                       SCOREP_DefinitionManager* definitionManager )
{
    assert( writerHandle );

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, MPICommunicator, mpi_communicator )
    {
        SCOREP_Error_Code status = OTF2_GlobalDefWriter_WriteMpiComm(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->group, Group, definitionManager->page_manager ) );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Communicator_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_group_definitions( void*                     writerHandle,
                                SCOREP_DefinitionManager* definitionManager,
                                bool                      isGlobal )
{
    assert( writerHandle );


    typedef SCOREP_Error_Code ( *def_group_pointer_t )( void*,
                                                        uint64_t,
                                                        OTF2_GroupType,
                                                        uint32_t,
                                                        uint64_t,
                                                        uint64_t* );

    def_group_pointer_t defGroup = ( def_group_pointer_t )
                                   OTF2_DefWriter_WriteGroup;
    if ( isGlobal )
    {
        defGroup = ( def_group_pointer_t )OTF2_GlobalDefWriter_WriteGroup;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, Group, group )
    {
        SCOREP_Error_Code status = defGroup(
            writerHandle,
            definition->sequence_number,
            scorep_group_type_to_otf_group_type( definition->group_type ),
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            definition->number_of_members,
            definition->members );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Group_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_metric_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    assert( writerHandle );

    typedef SCOREP_Error_Code ( *def_metric_pointer_t )( void*,
                                                         uint64_t,
                                                         uint32_t,
                                                         uint32_t,
                                                         OTF2_MetricType,
                                                         OTF2_MetricMode,
                                                         OTF2_TypeID,
                                                         OTF2_MetricBase,
                                                         int64_t,
                                                         uint32_t );
    def_metric_pointer_t defMetricMember = ( def_metric_pointer_t )
                                           OTF2_DefWriter_WriteMetricMember;
    if ( isGlobal )
    {
        defMetricMember = ( def_metric_pointer_t )
                          OTF2_GlobalDefWriter_WriteMetricMember;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Metric,
                                  metric )
    {
        SCOREP_Error_Code status = defMetricMember(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String, definitionManager->page_manager ),
            scorep_metric_source_type_to_otf_metric_type( definition->source_type ),
            scorep_metric_mode_to_otf_metric_mode( definition->mode ),
            scorep_metric_value_type_to_otf_metric_value_type( definition->value_type ),
            scorep_metric_base_to_otf_metric_base( definition->base ),
            definition->exponent,
            SCOREP_HANDLE_TO_ID( definition->unit_handle, String, definitionManager->page_manager ) );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Metric_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_sampling_set_definitions( void*                     writerHandle,
                                       SCOREP_DefinitionManager* definitionManager,
                                       bool                      isGlobal )
{
    assert( writerHandle );

    typedef SCOREP_Error_Code ( *def_metric_class_pointer_t )( void*,
                                                               uint64_t,
                                                               uint8_t,
                                                               uint64_t*,
                                                               OTF2_MetricOccurrence );
    def_metric_class_pointer_t defMetricClass = ( def_metric_class_pointer_t )
                                                OTF2_DefWriter_WriteMetricClass;

    typedef SCOREP_Error_Code ( *def_metric_instance_pointer_t )( void*,
                                                                  uint64_t,
                                                                  uint64_t,
                                                                  uint64_t,
                                                                  OTF2_MetricScope,
                                                                  uint64_t );
    def_metric_instance_pointer_t defMetricInstance = ( def_metric_instance_pointer_t )
                                                      OTF2_DefWriter_WriteMetricInstance;
    if ( isGlobal )
    {
        defMetricClass = ( def_metric_class_pointer_t )
                         OTF2_GlobalDefWriter_WriteMetricClass;
        defMetricInstance = ( def_metric_instance_pointer_t )
                            OTF2_GlobalDefWriter_WriteMetricInstance;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  SamplingSet,
                                  sampling_set )
    {
        SCOREP_Error_Code status;

        if ( !definition->is_scoped )
        {
            uint64_t metric_members[ definition->number_of_metrics ];
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
                scorep_metric_occurrence_to_otf_metric_occurrence( definition->occurrence ) );
        }
        else
        {
            SCOREP_ScopedSamplingSet_Definition* scoped_definition
                = ( SCOREP_ScopedSamplingSet_Definition* )definition;

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
                scorep_metric_scope_type_to_otf_metric_scope_type(
                    scoped_definition->scope_type ),
                scope );
        }
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_SamplingSet_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_parameter_definitions( void*                     writerHandle,
                                    SCOREP_DefinitionManager* definitionManager,
                                    bool                      isGlobal )
{
    assert( writerHandle );

    typedef  SCOREP_Error_Code ( *def_parameter_pointer_t )( void*,
                                                             uint32_t,
                                                             uint32_t,
                                                             uint8_t );
    def_parameter_pointer_t defParameter = ( def_parameter_pointer_t )
                                           OTF2_DefWriter_WriteParameter;
    if ( isGlobal )
    {
        defParameter = ( def_parameter_pointer_t )
                       OTF2_GlobalDefWriter_WriteParameter;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Parameter,
                                  parameter )
    {
        SCOREP_Error_Code status = defParameter(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_parameter_type_to_otf_parameter_type( definition->parameter_type ) );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Parameter_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_callpath_definitions( void*                     writerHandle,
                                   SCOREP_DefinitionManager* definitionManager,
                                   bool                      isGlobal )
{
    assert( writerHandle );

    typedef  SCOREP_Error_Code ( *def_callpath_pointer_t )( void*,
                                                            uint32_t,
                                                            uint32_t,
                                                            uint32_t,
                                                            uint8_t );
    def_callpath_pointer_t defCallpath = ( def_callpath_pointer_t )
                                         OTF2_DefWriter_WriteCallpath;
    if ( isGlobal )
    {
        defCallpath = ( def_callpath_pointer_t )
                      OTF2_GlobalDefWriter_WriteCallpath;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Callpath,
                                  callpath )
    {
        if ( !definition->with_parameter )
        {
            uint32_t parent_callpath_sequence_number = OTF2_UNDEFINED_UINT32;
            if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
            {
                parent_callpath_sequence_number = SCOREP_HANDLE_TO_ID( definition->parent_callpath_handle,
                                                                       Callpath,
                                                                       definitionManager->page_manager );
            }
            SCOREP_Error_Code status = defCallpath(
                writerHandle,
                definition->sequence_number,
                parent_callpath_sequence_number,
                SCOREP_HANDLE_TO_ID( definition->callpath_argument.region_handle,
                                     Region,
                                     definitionManager->page_manager ),
                0 /// @todo (uint8_t call_path_order) what are we supposed to pass here?
                );

            if ( status != SCOREP_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "SCOREP_Callpath_Definition" );
            }
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mappings( OTF2_DefWriter* localDefinitionWriter )
{
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( string, STRING, localDefinitionWriter );
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( region, REGION, localDefinitionWriter );
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( group, GROUP, localDefinitionWriter );
    if ( SCOREP_Mpi_HasMpi() )
    {
        SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( local_mpi_communicator,
                                                 MPI_COMMUNICATOR,
                                                 localDefinitionWriter );
    }
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( sampling_set, METRIC, localDefinitionWriter );

    // do we need Callpath and Parameter mappings for tracing?
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( parameter, PARAMETER, localDefinitionWriter );
}

static void
scorep_write_clock_offsets( OTF2_DefWriter* localDefinitionWriter )
{
    extern SCOREP_ClockOffset* scorep_clock_offset_head;
    for ( SCOREP_ClockOffset* clock_offset = scorep_clock_offset_head;
          clock_offset;
          clock_offset = clock_offset->next )
    {
        SCOREP_Error_Code status = OTF2_DefWriter_WriteClockOffset(
            localDefinitionWriter,
            clock_offset->time,
            clock_offset->offset,
            clock_offset->stddev );
        assert( status == SCOREP_SUCCESS );
    }
}

#if HAVE( SCOREP_DEBUG )
static void
scorep_write_local_definitions( OTF2_DefWriter* localDefinitionWriter )
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
    scorep_write_region_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_group_definitions(                  localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_metric_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_sampling_set_definitions(           localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_parameter_definitions(              localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_callpath_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
}
#else
static inline void
scorep_write_local_definitions( OTF2_DefWriter* localDefinitionWriter )
{
    return;
}
#endif


static void
scorep_write_global_definitions( OTF2_GlobalDefWriter* global_definition_writer )
{
    assert( SCOREP_Mpi_GetRank() == 0 );
    assert( scorep_unified_definition_manager );

    scorep_write_string_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_system_tree_node_definitions(       global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_group_definitions(         global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_region_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_group_definitions(                  global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_metric_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_sampling_set_definitions(           global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_parameter_definitions(              global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_callpath_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_communicator_definitions(           global_definition_writer, scorep_unified_definition_manager );
}


static void
scorep_write_number_of_definitions_per_location( OTF2_GlobalDefWriter* global_definition_writer )
{
    int* n_locations_per_rank = SCOREP_Mpi_GatherNumberOfLocationsPerRank();
    int  n_global_locations   = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        for ( int rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
        {
            n_global_locations += n_locations_per_rank[ rank ];
        }
    }

    int* n_definitions_per_location = 0;
    n_definitions_per_location = SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( n_locations_per_rank, n_global_locations );

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        OTF2_Archive_SetNumberOfLocations( scorep_otf2_archive, n_global_locations );

        if ( 0 /* unify failed => fallback */ )
        {
            SCOREP_Error_Code status = OTF2_GlobalDefWriter_WriteString( global_definition_writer, 0, "" );
            assert( status == SCOREP_SUCCESS );
            uint32_t          location_name_id = 0;

            int               location_index = 0; // index into n_definitions_per_location[]
            for ( int rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
            {
                for ( int local_location_id = 0; local_location_id < n_locations_per_rank[ rank ]; ++local_location_id )
                {
                    uint64_t          global_location_id = ( ( ( uint64_t )local_location_id ) << 32 ) | ( uint64_t )rank;
                    SCOREP_Error_Code status             = OTF2_GlobalDefWriter_WriteLocation(
                        global_definition_writer,
                        global_location_id,
                        location_name_id,
                        OTF2_LOCATION_TYPE_CPU_THREAD,
                        0 /* dummy number of events */,
                        n_definitions_per_location[ location_index ],
                        1 /* dummy timer resolution */,
                        rank /*assume that the rank is global id of process group */  );
                    assert( status == SCOREP_SUCCESS );
                    ++location_index;
                }
            }
        }
    }

    free( n_definitions_per_location );
    free( n_locations_per_rank );
}


static OTF2_DefWriter*
scorep_create_local_definition_writer( SCOREP_Location_Definition* definition )
{
    OTF2_DefWriter* definition_writer =
        OTF2_Archive_GetDefWriter( scorep_otf2_archive,
                                   definition->global_location_id );

    assert( definition_writer );
    return definition_writer;
}


static OTF2_GlobalDefWriter*
scorep_create_global_definition_writer()
{
    OTF2_GlobalDefWriter* global_definition_writer =
        OTF2_Archive_GetGlobalDefWriter( scorep_otf2_archive );
    assert( global_definition_writer );
    return global_definition_writer;
}


void
SCOREP_Tracing_WriteDefinitions()
{
    assert( scorep_otf2_archive );

    /* Write for all local locations the same local definition file */
    SCOREP_CreateExperimentDir();
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Location, location )
    {
        OTF2_DefWriter* local_definition_writer =
            scorep_create_local_definition_writer( definition );
        scorep_write_mappings( local_definition_writer );
        scorep_write_clock_offsets( local_definition_writer );
        scorep_write_local_definitions( local_definition_writer );
        OTF2_Archive_CloseDefWriter( scorep_otf2_archive,
                                     local_definition_writer );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();


    OTF2_GlobalDefWriter* global_definition_writer = NULL;
    uint64_t              epoch_begin;
    uint64_t              epoch_end;
    SCOREP_GetGlobalEpoch( &epoch_begin, &epoch_end );
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        global_definition_writer = scorep_create_global_definition_writer();
        OTF2_GlobalDefWriter_WriteTimeRange( global_definition_writer, epoch_begin, epoch_end - epoch_begin );
        scorep_write_global_definitions( global_definition_writer );
    }
    // uses MPI communication. references string handle, so write after strings
    // this may become obsolete, see comment in scorep_write_location_definitions()
    // only rank 0 will reference global_definition_writer
    scorep_write_number_of_definitions_per_location( global_definition_writer );
}
