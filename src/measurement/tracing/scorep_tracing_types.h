/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2016-2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_TRACING_INTERNAL_TYPES_H
#define SCOREP_TRACING_INTERNAL_TYPES_H

/**
 * @file
 *
 * @brief Holds functions for converting Score-P types to OTF2.
 */

static inline OTF2_LocationType
scorep_tracing_location_type_to_otf2( SCOREP_LocationType scorepType )
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
            UTILS_BUG( "Invalid location type: %u", scorepType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_LocationGroupType
scorep_tracing_location_group_type_to_otf2( SCOREP_LocationGroupType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_LOCATION_GROUP_TYPE_ ## SCOREP: \
        return OTF2_LOCATION_GROUP_TYPE_ ## OTF2

        case_return( PROCESS, PROCESS );

#undef case_return
        default:
            UTILS_BUG( "Invalid location group type: %u", scorepType );
    }

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_GroupType
scorep_tracing_group_type_to_otf2( SCOREP_GroupType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_GROUP_ ## SCOREP: \
        return OTF2_GROUP_TYPE_ ## OTF2

        case_return( UNKNOWN,            UNKNOWN );
        case_return( LOCATIONS,          LOCATIONS );
        case_return( REGIONS,            REGIONS );
        case_return( METRIC,             METRIC );

        // MPI
        case_return( MPI_LOCATIONS,      COMM_LOCATIONS );
        case_return( MPI_GROUP,          COMM_GROUP );
        case_return( MPI_SELF,           COMM_SELF );

        // OpenMP
        case_return( OPENMP_LOCATIONS,   COMM_LOCATIONS );
        case_return( OPENMP_THREAD_TEAM, COMM_GROUP );

        // CUDA
        case_return( CUDA_LOCATIONS,     COMM_LOCATIONS );
        case_return( CUDA_GROUP,         COMM_GROUP );

        // OpenCL
        case_return( OPENCL_LOCATIONS,   COMM_LOCATIONS );
        case_return( OPENCL_GROUP,       COMM_GROUP );

        // SHMEM
        case_return( SHMEM_LOCATIONS,    COMM_LOCATIONS );
        case_return( SHMEM_GROUP,        COMM_GROUP );
        case_return( SHMEM_SELF,         COMM_SELF );

        // Pthread
        case_return( PTHREAD_LOCATIONS,   COMM_LOCATIONS );
        case_return( PTHREAD_THREAD_TEAM, COMM_GROUP );

        //Topologies
        case_return( TOPOLOGY_HARDWARE_LOCATIONS, COMM_LOCATIONS );
        case_return( TOPOLOGY_HARDWARE_GROUP, COMM_GROUP );

        case_return( TOPOLOGY_PROCESS_LOCATIONS, COMM_LOCATIONS );
        case_return( TOPOLOGY_PROCESS_GROUP, COMM_GROUP );

        case_return( TOPOLOGY_USER_LOCATIONS, COMM_LOCATIONS );
        case_return( TOPOLOGY_USER_GROUP, COMM_GROUP );

#undef case_return
        default:
            UTILS_BUG( "Invalid group type: %u", scorepType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_MetricType
scorep_tracing_metric_source_type_to_otf2( SCOREP_MetricSourceType sourceType )
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
        case_return( TASK,   OTHER );
        /* RonnyT: @ todo Introduce own PLUGIN type in OTF2 ??? */
        case_return( PLUGIN, OTHER );
        case_return( PERF, OTHER );

#undef case_return
        default:
            UTILS_BUG( "Invalid metric source type: %u", sourceType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_MetricMode
scorep_tracing_metric_mode_to_otf2( SCOREP_MetricMode mode )
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
            UTILS_BUG( "Invalid metric mode: %u", mode );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_Type
scorep_tracing_metric_value_type_to_otf2( SCOREP_MetricValueType valueType )
{
    switch ( valueType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_METRIC_VALUE_ ## SCOREP: \
        return OTF2_TYPE_ ## OTF2

        case_return( INT64,  INT64 );
        case_return( UINT64, UINT64 );
        case_return( DOUBLE, DOUBLE );

#undef case_return
        default:
            UTILS_BUG( "Invalid metric value type: %u", valueType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_Base
scorep_tracing_base_to_otf2( SCOREP_MetricBase base )
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
            UTILS_BUG( "Invalid metric base: %u", base );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_MetricOccurrence
scorep_tracing_metric_occurrence_to_otf2( SCOREP_MetricOccurrence occurrence )
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
            UTILS_BUG( "Invalid metric occurrence: %u", occurrence );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_MetricScope
scorep_tracing_metric_scope_type_to_otf2( SCOREP_MetricScope scope )
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
            UTILS_BUG( "Invalid metric scope: %u", scope );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_ParameterType
scorep_tracing_parameter_type_to_otf2( SCOREP_ParameterType scorepType )
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
            UTILS_BUG( "Invalid parameter type: %u", scorepType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_Type
scorep_tracing_parameter_type_to_otf2_type( SCOREP_ParameterType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_PARAMETER_ ## SCOREP: \
        return OTF2_TYPE_ ## OTF2

        case_return( STRING, STRING );
        case_return( INT64,  INT64 );
        case_return( UINT64, UINT64 );

#undef case_return
        default:
            UTILS_BUG( "Invalid parameter type: %u", scorepType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_CollectiveOp
scorep_tracing_collective_type_to_otf2( SCOREP_CollectiveType scorepType )
{
    switch ( scorepType )
    {
#define case_return( name ) \
    case SCOREP_COLLECTIVE_ ## name: \
        return OTF2_COLLECTIVE_OP_ ## name

#define case_return2( SCOREP, OTF2 ) \
    case SCOREP_COLLECTIVE_ ## SCOREP: \
        return OTF2_COLLECTIVE_OP_ ## OTF2

        case_return( BARRIER );
        case_return2( BROADCAST, BCAST );
        case_return( GATHER );
        case_return( GATHERV );
        case_return( SCATTER );
        case_return( SCATTERV );
        case_return( ALLGATHER );
        case_return( ALLGATHERV );
        case_return( ALLTOALL );
        case_return( ALLTOALLV );
        case_return( ALLTOALLW );
        case_return( ALLREDUCE );
        case_return( REDUCE );
        case_return( REDUCE_SCATTER );
        case_return( REDUCE_SCATTER_BLOCK );
        case_return( SCAN );
        case_return( EXSCAN );
        case_return( CREATE_HANDLE );
        case_return( DESTROY_HANDLE );
        case_return( ALLOCATE );
        case_return( DEALLOCATE );
        case_return( CREATE_HANDLE_AND_ALLOCATE );
        case_return( DESTROY_HANDLE_AND_DEALLOCATE );

        default:
            UTILS_BUG( "Invalid collective type: %u", scorepType );

#undef case_return
#undef case_return2
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline char*
scorep_tracing_property_to_otf2( SCOREP_Property property )
{
    switch ( property )
    {
#define case_return( name ) \
    case SCOREP_PROPERTY_## name: \
        return "OTF2::" #name

        case_return( MPI_COMMUNICATION_COMPLETE );
        case_return( THREAD_FORK_JOIN_EVENT_COMPLETE );
        case_return( THREAD_CREATE_WAIT_EVENT_COMPLETE );
        case_return( THREAD_LOCK_EVENT_COMPLETE );
        case_return( PTHREAD_LOCATION_REUSED );

        default:
            UTILS_BUG( "Invalid property enum value: %u", property );
            return 0;

#undef case_return
    }
}

static inline OTF2_RecorderKind
scorep_tracing_sampling_set_class_to_otf2( SCOREP_SamplingSetClass samplingSetClass )
{
    switch ( samplingSetClass )
    {
#define case_return( name ) \
    case SCOREP_SAMPLING_SET_ ## name: \
        return OTF2_RECORDER_KIND_ ## name

        case_return( ABSTRACT );
        case_return( CPU );
        case_return( GPU );

        default:
            UTILS_BUG( "Invalid sampling set class: %u", samplingSetClass );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_SystemTreeDomain
scorep_tracing_domain_to_otf2( SCOREP_SystemTreeDomain* domains )
{
#define flag_return( domain ) \
    if ( *domains & SCOREP_SYSTEM_TREE_DOMAIN_ ## domain ) \
    { \
        *domains &= ~SCOREP_SYSTEM_TREE_DOMAIN_ ## domain; \
        return OTF2_SYSTEM_TREE_DOMAIN_ ## domain; \
    }

    flag_return( MACHINE )
    flag_return( SHARED_MEMORY )
    flag_return( NUMA )
    flag_return( SOCKET )
    flag_return( CACHE )
    flag_return( CORE )
    flag_return( PU )

#undef flag_return

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_RmaSyncLevel
scorep_tracing_rma_sync_level_to_otf2( SCOREP_RmaSyncLevel scorepLevel )
{
    OTF2_RmaSyncLevel sync_level = OTF2_RMA_SYNC_LEVEL_NONE;

#define if_flag( name ) \
    do { \
        if ( scorepLevel & SCOREP_RMA_SYNC_LEVEL_ ## name ) \
        { \
            sync_level  |= OTF2_RMA_SYNC_LEVEL_ ## name; \
            scorepLevel &= ~SCOREP_RMA_SYNC_LEVEL_ ## name; \
        } \
    } \
    while ( 0 )

    if_flag( PROCESS );
    if_flag( MEMORY );

#undef if_flag

    UTILS_BUG_ON( scorepLevel != SCOREP_RMA_SYNC_LEVEL_NONE,
                  "Unhandled RMA sync level flag" );

    return sync_level;
}


static inline OTF2_RmaSyncType
scorep_tracing_rma_sync_type_to_otf2( SCOREP_RmaSyncType scorepType )
{
    switch ( scorepType )
    {
#define case_return( name ) \
    case SCOREP_RMA_SYNC_TYPE_ ## name: \
        return OTF2_RMA_SYNC_TYPE_ ## name

        case_return( MEMORY );
        case_return( NOTIFY_IN );
        case_return( NOTIFY_OUT );

        default:
            UTILS_BUG( "Invalid RMA sync type: %u", scorepType );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_LockType
scorep_tracing_lock_type_to_otf2( SCOREP_LockType scorepType )
{
    switch ( scorepType )
    {
#define case_return( name ) \
    case SCOREP_LOCK_ ## name: \
        return OTF2_LOCK_ ## name

        case_return( EXCLUSIVE );
        case_return( SHARED );

        default:
            UTILS_BUG( "Invalid lock type: %u", scorepType );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_RmaAtomicType
scorep_tracing_rma_atomic_type_to_otf2( SCOREP_RmaAtomicType scorepType )
{
    switch ( scorepType )
    {
#define case_return( name ) \
    case SCOREP_RMA_ATOMIC_TYPE_ ## name: \
        return OTF2_RMA_ATOMIC_TYPE_ ## name

        case_return( ACCUMULATE );
        case_return( INCREMENT );
        case_return( TEST_AND_SET );
        case_return( COMPARE_AND_SWAP );
        case_return( SWAP );
        case_return( FETCH_AND_ADD );
        case_return( FETCH_AND_INCREMENT );
        case_return( FETCH_AND_ACCUMULATE );

        default:
            UTILS_BUG( "Invalid RMA atomic type: %u", scorepType );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_Paradigm
scorep_tracing_paradigm_to_otf2( SCOREP_ParadigmType paradigm )
{
    switch ( paradigm )
    {
#define SCOREP_PARADIGM( NAME, name_str, OTF2_NAME ) \
    case SCOREP_PARADIGM_ ## NAME: \
        return OTF2_PARADIGM_ ## OTF2_NAME;
        SCOREP_PARADIGMS
#undef SCOREP_PARADIGM

        default:
            UTILS_BUG( "Invalid paradigm: %u", paradigm );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_ParadigmClass
scorep_tracing_paradigm_class_to_otf2( SCOREP_ParadigmClass paradigmClass )
{
    switch ( paradigmClass )
    {
#define SCOREP_PARADIGM_CLASS( NAME, name_str, OTF2_NAME ) \
    case SCOREP_PARADIGM_CLASS_ ## NAME: \
        return OTF2_PARADIGM_CLASS_ ## OTF2_NAME;
        SCOREP_PARADIGM_CLASSES
#undef SCOREP_PARADIGM_CLASS

        default:
            UTILS_BUG( "Invalid paradigm class: %u", paradigmClass );
    }

    return OTF2_UNDEFINED_TYPE;
}


/* Score-P paradigm flags will be converted to OTF2 paradigm properties
   of type Boolean */
static inline OTF2_ParadigmProperty
scorep_tracing_paradigm_boolean_property_to_otf2( SCOREP_ParadigmFlags paradigmFlag )
{
    switch ( paradigmFlag )
    {
        case SCOREP_PARADIGM_FLAG_RMA_ONLY:
            return OTF2_PARADIGM_PROPERTY_RMA_ONLY;

        default:
            UTILS_BUG( "Invalid paradigm flag: %u", paradigmFlag );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_ParadigmProperty
scorep_tracing_paradigm_property_to_otf2( SCOREP_ParadigmProperty paradigmProperty )
{
    switch ( paradigmProperty )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_PARADIGM_PROPERTY_ ## SCOREP: \
        return OTF2_PARADIGM_PROPERTY_ ## OTF2

        case_return( COMMUNICATOR_TEMPLATE, COMM_NAME_TEMPLATE );
        case_return( RMA_WINDOW_TEMPLATE, RMA_WIN_NAME_TEMPLATE );

#undef case_return

        default:
            UTILS_BUG( "Invalid paradigm property: %u", paradigmProperty );
    }

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_Type
scorep_tracing_attribute_type_to_otf2( SCOREP_AttributeType attrType )
{
    switch ( attrType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_ATTRIBUTE_TYPE_ ## SCOREP: \
        return OTF2_TYPE_ ## OTF2

        case_return( UINT8,  UINT8 );
        case_return( UINT16, UINT16 );
        case_return( UINT32, UINT32 );
        case_return( UINT64, UINT64 );
        case_return( INT8,   INT8 );
        case_return( INT16,  INT16 );
        case_return( INT32,  INT32 );
        case_return( INT64,  INT64 );
        case_return( FLOAT, FLOAT );
        case_return( DOUBLE, DOUBLE );
        case_return( STRING, STRING );
        case_return( ATTRIBUTE, ATTRIBUTE );
        case_return( LOCATION, LOCATION );
        case_return( REGION, REGION );
        case_return( GROUP, GROUP );
        case_return( METRIC, METRIC );
        case_return( INTERIM_COMMUNICATOR, COMM );
        case_return( PARAMETER, PARAMETER );
        case_return( RMA_WINDOW, RMA_WIN );
        case_return( SOURCE_CODE_LOCATION, SOURCE_CODE_LOCATION );
        case_return( CALLING_CONTEXT, CALLING_CONTEXT );
        case_return( INTERRUPT_GENERATOR, INTERRUPT_GENERATOR );

#undef case_return
        default:
            UTILS_BUG( "Invalid attribute type: %u", attrType );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_InterruptGeneratorMode
scorep_tracing_interrupt_generator_mode_to_otf2( SCOREP_InterruptGeneratorMode mode )
{
    switch ( mode )
    {
#define case_return( MODE ) \
    case SCOREP_INTERRUPT_GENERATOR_MODE_ ## MODE: \
        return OTF2_INTERRUPT_GENERATOR_MODE_ ## MODE

        case_return( TIME );
        case_return( COUNT );

#undef case_return
        default:
            UTILS_BUG( "Invalid interrupt generator mode: %u", mode );
    }

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_IoParadigmClass
scorep_tracing_io_paradigm_class_to_otf2( SCOREP_IoParadigmClass class )
{
    switch ( class )
    {
#define case_return( CLASS ) \
    case SCOREP_IO_PARADIGM_CLASS_ ## CLASS: \
        return OTF2_IO_PARADIGM_CLASS_ ## CLASS

        case_return( SERIAL );
        case_return( PARALLEL );

#undef case_return
        default:
            UTILS_BUG( "Invalid I/O paradigm class: %u", class );
    }

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_IoParadigmFlag
scorep_tracing_io_paradigm_flags_to_otf2( SCOREP_IoParadigmFlag scorepFlag )
{
    OTF2_IoParadigmFlag otf2_flags = OTF2_IO_PARADIGM_FLAG_NONE;

#define if_flag( name ) \
    do { \
        if ( scorepFlag & SCOREP_IO_PARADIGM_FLAG_ ## name ) \
        { \
            otf2_flags |= OTF2_IO_PARADIGM_FLAG_ ## name; \
            scorepFlag &= ~SCOREP_IO_PARADIGM_FLAG_ ## name; \
        } \
    } \
    while ( 0 )

    if_flag( OS );

#undef if_flag

    UTILS_BUG_ON( scorepFlag != SCOREP_IO_PARADIGM_FLAG_NONE,
                  "Unhandled I/O paradigm flag" );

    return otf2_flags;
}

static inline OTF2_ParadigmProperty
scorep_tracing_io_paradigm_property_to_otf2( SCOREP_ParadigmProperty paradigmProperty )
{
    switch ( paradigmProperty )
    {
#define case_return( PROPERTY ) \
    case SCOREP_IO_PARADIGM_PROPERTY_ ## PROPERTY: \
        return OTF2_IO_PARADIGM_PROPERTY_ ## PROPERTY

        case_return( VERSION );

#undef case_return

        default:
            UTILS_BUG( "Invalid paradigm property: %u", paradigmProperty );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_IoHandleFlag
scorep_tracing_io_handle_flag_to_otf2( SCOREP_IoHandleFlag scorepFlag )
{
    OTF2_IoHandleFlag otf2_flags = OTF2_IO_HANDLE_FLAG_NONE;

#define if_flag( name ) \
    do { \
        if ( scorepFlag & SCOREP_IO_HANDLE_FLAG_ ## name ) \
        { \
            otf2_flags |= OTF2_IO_HANDLE_FLAG_ ## name; \
            scorepFlag &= ~SCOREP_IO_HANDLE_FLAG_ ## name; \
        } \
    } \
    while ( 0 )

    if_flag( PRE_CREATED );
    if_flag( ALL_PROXY );

#undef if_flag

    UTILS_BUG_ON( scorepFlag != SCOREP_IO_HANDLE_FLAG_NONE,
                  "Unhandled I/O handle flag" );

    return otf2_flags;
}


static inline OTF2_IoAccessMode
scorep_tracing_io_access_mode_to_otf2( SCOREP_IoAccessMode mode )
{
    switch ( mode )
    {
#define case_return( MODE ) \
    case SCOREP_IO_ACCESS_MODE_ ## MODE: \
        return OTF2_IO_ACCESS_MODE_ ## MODE

        case_return( READ_ONLY );
        case_return( WRITE_ONLY );
        case_return( READ_WRITE );
        case_return( EXECUTE_ONLY );
        case_return( SEARCH_ONLY );

#undef case_return
        default:
            UTILS_BUG( "Invalid I/O access mode: %u", mode );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_IoOperationMode
scorep_tracing_io_operation_mode_to_otf2( SCOREP_IoOperationMode mode )
{
    switch ( mode )
    {
#define case_return( MODE ) \
    case SCOREP_IO_OPERATION_MODE_ ## MODE: \
        return OTF2_IO_OPERATION_MODE_ ## MODE

        case_return( READ );
        case_return( WRITE );
        case_return( FLUSH );

#undef case_return
        default:
            UTILS_BUG( "Invalid I/O operation mode: %u", mode );
    }

    return OTF2_UNDEFINED_TYPE;
}

static inline OTF2_IoCreationFlag
scorep_tracing_io_creation_flags_to_otf2( SCOREP_IoCreationFlag scorepCreationFlag )
{
    OTF2_IoCreationFlag creation_flags = OTF2_IO_CREATION_FLAG_NONE;

#define if_flag( name ) \
    do { \
        if ( scorepCreationFlag & SCOREP_IO_CREATION_FLAG_ ## name ) \
        { \
            creation_flags     |= OTF2_IO_CREATION_FLAG_ ## name; \
            scorepCreationFlag &= ~SCOREP_IO_CREATION_FLAG_ ## name; \
        } \
    } \
    while ( 0 )

    if_flag( CREATE );
    if_flag( TRUNCATE );
    if_flag( DIRECTORY );
    if_flag( EXCLUSIVE );
    if_flag( NO_CONTROLLING_TERMINAL );
    if_flag( NO_FOLLOW );
    if_flag( PATH );
    if_flag( TEMPORARY_FILE );
    if_flag( LARGEFILE );
    if_flag( NO_SEEK );
    if_flag( UNIQUE );

#undef if_flag

    UTILS_BUG_ON( scorepCreationFlag != SCOREP_IO_CREATION_FLAG_NONE,
                  "Unhandled I/O creation flag" );

    return creation_flags;
}

static inline OTF2_IoStatusFlag
scorep_tracing_io_status_flags_to_otf2( SCOREP_IoStatusFlag scorepStatusFlag )
{
    OTF2_IoStatusFlag status_flags = OTF2_IO_STATUS_FLAG_NONE;

#define if_flag( name ) \
    do { \
        if ( scorepStatusFlag & SCOREP_IO_STATUS_FLAG_ ## name ) \
        { \
            status_flags     |= OTF2_IO_STATUS_FLAG_ ## name; \
            scorepStatusFlag &= ~SCOREP_IO_STATUS_FLAG_ ## name; \
        } \
    } \
    while ( 0 )

    if_flag( CLOSE_ON_EXEC );
    if_flag( APPEND );
    if_flag( NON_BLOCKING );
    if_flag( ASYNC );
    if_flag( SYNC );
    if_flag( DATA_SYNC );
    if_flag( AVOID_CACHING );
    if_flag( NO_ACCESS_TIME );
    if_flag( DELETE_ON_CLOSE );

#undef if_flag

    UTILS_BUG_ON( scorepStatusFlag != SCOREP_IO_STATUS_FLAG_NONE,
                  "Unhandled I/O status flag" );

    return status_flags;
}

static inline OTF2_IoOperationFlag
scorep_tracing_io_operation_flag_to_otf2( SCOREP_IoOperationFlag scorepOperationFlag )
{
    OTF2_IoOperationFlag operation_flags = OTF2_IO_OPERATION_FLAG_NONE;

#define if_flag( name ) \
    do { \
        if ( scorepOperationFlag & SCOREP_IO_OPERATION_FLAG_ ## name ) \
        { \
            operation_flags     |= OTF2_IO_OPERATION_FLAG_ ## name; \
            scorepOperationFlag &= ~SCOREP_IO_OPERATION_FLAG_ ## name; \
        } \
    } \
    while ( 0 )

    if_flag( NON_BLOCKING );
    if_flag( COLLECTIVE );

#undef if_flag

    UTILS_BUG_ON( scorepOperationFlag != SCOREP_IO_OPERATION_FLAG_NONE,
                  "Unhandled I/O operation flag" );

    return operation_flags;
}

static inline OTF2_IoSeekOption
scorep_tracing_io_seek_option_to_otf2( SCOREP_IoSeekOption option )
{
    switch ( option )
    {
#define case_return( OPTION ) \
    case SCOREP_IO_SEEK_ ## OPTION: \
        return OTF2_IO_SEEK_ ## OPTION

        case_return( FROM_START );
        case_return( FROM_CURRENT );
        case_return( FROM_END );
        case_return( DATA );
        case_return( HOLE );

#undef case_return
        default:
            UTILS_BUG( "Invalid I/O seek option: %u", option );
    }

    return OTF2_UNDEFINED_TYPE;
}

#endif /* SCOREP_TRACING_INTERNAL_TYPES_H */
