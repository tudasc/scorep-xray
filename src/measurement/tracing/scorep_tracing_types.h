/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/measurement/tracing/scorep_tracing_types.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief Holds functions for converting Score-P types to OTF2.
 */

#ifndef SCOREP_TRACING_INTERNAL_TYPES_H
#define SCOREP_TRACING_INTERNAL_TYPES_H


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
            UTILS_BUG( "Invalid location type" );
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
            UTILS_BUG( "Invalid location group type" );
    }

    return OTF2_UNDEFINED_TYPE;
}

#if 0

static inline OTF2_RegionType
scorep_tracing_region_type_to_otf2( SCOREP_RegionType scorepType )
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
        case_return( OMP_TASK_CREATE,      OMP_TASK_CREATE );
        case_return( OMP_ORDERED,          OMP_ORDERED );
        case_return( OMP_ORDERED_SBLOCK,   OMP_ORDERED_SBLOCK );
        case_return( PHASE,                PHASE );
        case_return( DYNAMIC,              DYNAMIC );
        case_return( DYNAMIC_PHASE,        DYNAMIC_PHASE );
        case_return( DYNAMIC_LOOP,         DYNAMIC_LOOP );
        case_return( DYNAMIC_FUNCTION,     DYNAMIC_FUNCTION );
        case_return( DYNAMIC_LOOP_PHASE,   DYNAMIC_LOOP_PHASE );

#undef case_return
        default:
            UTILS_BUG( "Invalid region type" );
    }

    return OTF2_UNDEFINED_TYPE;
}

#endif

static inline OTF2_GroupType
scorep_tracing_group_type_to_otf2( SCOREP_GroupType scorepType )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_GROUP_ ## SCOREP: \
        return OTF2_GROUP_TYPE_ ## OTF2

        case_return( UNKNOWN,       UNKNOWN );
        case_return( LOCATIONS,     LOCATIONS );
        case_return( REGIONS,       REGIONS );
        case_return( METRIC,        METRIC );
        case_return( COMM_SELF,     COMM_SELF );
        case_return( MPI_GROUP,     COMM_GROUP );
        case_return( MPI_LOCATIONS, COMM_LOCATIONS );

#undef case_return
        default:
            UTILS_BUG( "Invalid group type" );
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

#undef case_return
        default:
            UTILS_BUG( "Invalid metric source type" );
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
            UTILS_BUG( "Invalid metric mode" );
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
            UTILS_BUG( "Invalid metric value type" );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_MetricBase
scorep_tracing_metric_base_to_otf2( SCOREP_MetricBase base )
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
            UTILS_BUG( "Invalid metric base" );
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
            UTILS_BUG( "Invalid metric occurrence" );
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
            UTILS_BUG( "Invalid metric scope" );
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
            UTILS_BUG( "Invalid parameter type" );
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_CollectiveOp
scorep_tracing_collective_type_to_otf2( SCOREP_MpiCollectiveType scorep_type )
{
    switch ( scorep_type )
    {
#define case_return( name ) \
    case SCOREP_COLLECTIVE_MPI_ ## name: \
        return OTF2_COLLECTIVE_OP_ ## name

        case_return( BARRIER );
        case_return( BCAST );
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

        default:
            UTILS_BUG( "Invalid collective type" );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline char*
scorep_tracing_property_to_otf2( SCOREP_Property scorep_property )
{
    switch ( scorep_property )
    {
#define case_return( name ) \
    case SCOREP_PROPERTY_## name: \
        return "OTF2::" #name

        case_return( MPI_COMMUNICATION_COMPLETE );
        case_return( OPENMP_EVENT_COMPLETE );
        case_return( THREAD_UNIQUE_FORK_SEQUENCE_COUNTS );

        default:
            UTILS_BUG( "Invalid property enum value" );
            return 0;

#undef case_return
    }
}


static inline OTF2_RmaSyncLevel
scorep_tracing_rma_sync_level_to_otf2( SCOREP_RmaSyncLevel scorep_level )
{
    switch ( scorep_level )
    {
#define case_return( name ) \
    case SCOREP_RMA_SYNC_LEVEL_ ## name: \
        return OTF2_RMA_SYNC_LEVEL_ ## name

        case_return( NONE );
        case_return( PROCESS );
        case_return( MEMORY );

        default:
            UTILS_BUG( "Invalid RMA sync level" );

#undef case_return
    }

    return OTF2_UNDEFINED_UINT32;
}


static inline OTF2_RmaSyncType
scorep_tracing_rma_sync_type_to_otf2( SCOREP_RmaSyncType scorep_type )
{
    switch ( scorep_type )
    {
#define case_return( name ) \
    case SCOREP_RMA_SYNC_TYPE_ ## name: \
        return OTF2_RMA_SYNC_TYPE_ ## name

        case_return( MEMORY );
        case_return( NOTIFY_IN );
        case_return( NOTIFY_OUT );

        default:
            UTILS_BUG( "Invalid RMA sync type" );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_LockType
scorep_tracing_lock_type_to_otf2( SCOREP_LockType scorep_type )
{
    switch ( scorep_type )
    {
#define case_return( name ) \
    case SCOREP_LOCK_ ## name: \
        return OTF2_LOCK_ ## name

        case_return( EXCLUSIVE );
        case_return( SHARED );

        default:
            UTILS_BUG( "Invalid lock type" );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


static inline OTF2_RmaAtomicType
scorep_tracing_rma_atomic_type_to_otf2( SCOREP_RmaAtomicType scorep_type )
{
    switch ( scorep_type )
    {
#define case_return( name ) \
    case SCOREP_RMA_ATOMIC_TYPE_ ## name: \
        return OTF2_RMA_ATOMIC_TYPE_ ## name

        case_return( ACCUMULATE );
        case_return( INCREMENT );
        case_return( TEST_AND_SET );
        case_return( COMPARE_AND_SWAP );

        default:
            UTILS_BUG( "Invalid RMA atomic type" );

#undef case_return
    }

    return OTF2_UNDEFINED_TYPE;
}


#endif /* SCOREP_TRACING_INTERNAL_TYPES_H */
