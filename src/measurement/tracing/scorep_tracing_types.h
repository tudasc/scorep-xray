/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
            SCOREP_BUG( "Invalid location type" );
    }
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
            SCOREP_BUG( "Invalid location group type" );
    }
}


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
scorep_tracing_group_type_to_otf2( SCOREP_GroupType scorepType )
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

#undef case_return
        default:
            SCOREP_BUG( "Invalid metric source type" );
    }
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
            SCOREP_BUG( "Invalid metric mode" );
    }
}


static inline OTF2_TypeID
scorep_tracing_metric_value_type_to_otf2( SCOREP_MetricValueType valueType )
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
            SCOREP_BUG( "Invalid metric base" );
    }
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
            SCOREP_BUG( "Invalid metric occurrence" );
    }
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
            SCOREP_BUG( "Invalid metric scope" );
    }
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
            SCOREP_BUG( "Invalid parameter type" );
    }
}


static inline OTF2_Mpi_CollectiveType
scorep_tracing_collective_type_to_otf2( SCOREP_MpiCollectiveType scorep_type )
{
    switch ( scorep_type )
    {
#define case_return( name ) \
    case SCOREP_COLLECTIVE_MPI_ ## name: \
        return OTF2_MPI_ ## name

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
            SCOREP_BUG( "Invalid collective type" );
            return 0;

#undef case_return
    }
}


#endif /* SCOREP_TRACING_INTERNAL_TYPES_H */
