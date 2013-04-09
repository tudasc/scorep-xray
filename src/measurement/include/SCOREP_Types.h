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


#ifndef SCOREP_TYPES_H
#define SCOREP_TYPES_H


/**
 * @file        SCOREP_Types.h
 * @maintainer  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief Types needed by SCOREP_* API functions.
 *
 */


#include <stdint.h>


#include <scorep/SCOREP_PublicTypes.h>


/**
 * @defgroup SCOREP_Types SCOREP Types
 *
 * The adapters need to use special types to communicate with the measurement
 * layer functions. We can categorize them as follows:
 *
 * - Opaque handles returned by the @ref SCOREP_Definitions definition
     functions. Note that it is not intended for the adapter to do any
     operations on these handles. Their type may change in future.
 *
 * - Constants specifying invalid or unknown definition handles.
 *
 * - Enumerations specifying particular types of the defined entities.
 *
 * - Types used in configuring the measurement system.
 *
 * @todo Move INVALID defines that are not intended to be used by the adapter
 * layer to internal files.
 */
/*@{*/



/**
 * Type used in specifying line numbers.
 * @see SCOREP_DefineRegion()
 */
typedef uint32_t SCOREP_LineNo;



/**
 * Symbolic constant representing an invalid or unknown line number.
 * @see SCOREP_DefineRegion()
 */
#define SCOREP_INVALID_LINE_NO 0



/**
 * Symbolic constant representing an invalid or unknown I/O file group
 * definition.
 */
#define SCOREP_INVALID_IOFILE_GROUP SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown I/O file definition.
 */
#define SCOREP_INVALID_IOFILE SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown marker group
 * definition.
 */
#define SCOREP_INVALID_MARKER_GROUP SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown marker definition.
 */
#define SCOREP_INVALID_MARKER SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown group
 * definition.
 */
#define SCOREP_INVALID_GROUP SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown local MPI communicator
 * definition.
 */
#define SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI communicator
 * definition.
 */
#define SCOREP_INVALID_MPI_COMMUNICATOR SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI window definition.
 */
#define SCOREP_INVALID_MPI_WINDOW SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI cartesion topology
 * definition.
 */
#define SCOREP_INVALID_CART_TOPOLOGY SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI cartesion coordinates
 * definition.
 */
#define SCOREP_INVALID_CART_COORDS SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown callpath definition.
 */
#define SCOREP_INVALID_CALLPATH SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown parameter definition.
 */
#define SCOREP_INVALID_PARAMETER SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown string definition.
 */
#define SCOREP_INVALID_STRING SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown location definition.
 */
#define SCOREP_INVALID_LOCATION SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown location group definition.
 */
#define SCOREP_INVALID_LOCATION_GROUP SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown system tree node
 */
#define SCOREP_INVALID_SYSTEM_TREE_NODE SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing a filtered region
 */
#define SCOREP_FILTERED_REGION ( ( SCOREP_RegionHandle ) - 1 )


/**
 * Types to be used in defining a region (SCOREP_DefineRegion()). In order to
 * track the origin of a region definition, the adapter needs to provide @e
 * his type.
 *
 */
typedef enum SCOREP_AdapterType
{
    SCOREP_ADAPTER_USER,
    SCOREP_ADAPTER_COMPILER,
    SCOREP_ADAPTER_MPI,
    SCOREP_ADAPTER_POMP,
    SCOREP_ADAPTER_PTHREAD,
    SCOREP_ADAPTER_CUDA,

    SCOREP_INVALID_ADAPTER_TYPE /**< For internal use only. */
} SCOREP_AdapterType;



/**
 * Metric sources to be used in defining a metric member (SCOREP_DefineMetric()).
 *
 */
typedef enum SCOREP_MetricSourceType
{
    /** PAPI counter. */
    SCOREP_METRIC_SOURCE_TYPE_PAPI   = 0,
    /** Resource usage counter. */
    SCOREP_METRIC_SOURCE_TYPE_RUSAGE = 1,
    /** User metrics. */
    SCOREP_METRIC_SOURCE_TYPE_USER   = 2,
    /** Any other metrics. */
    SCOREP_METRIC_SOURCE_TYPE_OTHER  = 3,
    /** Generated by task profiling. */
    SCOREP_METRIC_SOURCE_TYPE_TASK   = 4,

    SCOREP_INVALID_METRIC_SOURCE_TYPE /**< For internal use only. */
} SCOREP_MetricSourceType;

/**
 * Types to be used in defining type of metric values (SCOREP_DefineMetric()).
 *
 */
typedef enum SCOREP_MetricValueType
{
    SCOREP_METRIC_VALUE_INT64,
    SCOREP_METRIC_VALUE_UINT64,
    SCOREP_METRIC_VALUE_DOUBLE,

    SCOREP_INVALID_METRIC_VALUE_TYPE /**< For internal use only. */
} SCOREP_MetricValueType;

/**
 * Types to be used in defining metric mode (SCOREP_DefineMetric()).
 *
 */
typedef enum SCOREP_MetricMode
{
    /** Accumulated metric, 'START' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_START = 0,
    /** Accumulated metric, 'POINT' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_POINT = 1,
    /** Accumulated metric, 'LAST' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_LAST  = 2,
    /** Accumulated metric, 'NEXT' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_NEXT  = 3,

    /** Absolute metric, 'POINT' timing. */
    SCOREP_METRIC_MODE_ABSOLUTE_POINT    = 4,
    /** Absolute metric, 'LAST' timing. */
    SCOREP_METRIC_MODE_ABSOLUTE_LAST     = 5,
    /** Absolute metric, 'NEXT' timing. */
    SCOREP_METRIC_MODE_ABSOLUTE_NEXT     = 6,

    /** Relative metric, 'POINT' timing. */
    SCOREP_METRIC_MODE_RELATIVE_POINT    = 7,
    /** Relative metric, 'LAST' timing. */
    SCOREP_METRIC_MODE_RELATIVE_LAST     = 8,
    /** Relative metric, 'NEXT' timing. */
    SCOREP_METRIC_MODE_RELATIVE_NEXT     = 9,

    SCOREP_INVALID_METRIC_MODE /**< For internal use only. */
} SCOREP_MetricMode;

/**
 * Types to be used in defining metric base (SCOREP_DefineMetric()).
 *
 */
typedef enum SCOREP_MetricBase
{
    /** Binary base. */
    SCOREP_METRIC_BASE_BINARY  = 0,
    /** Decimal base. */
    SCOREP_METRIC_BASE_DECIMAL = 1,

    SCOREP_INVALID_METRIC_BASE /**< For internal use only. */
} SCOREP_MetricBase;

/**
 * Types to be used in defining the occurrence of a sampling set.
 *
 */
typedef enum SCOREP_MetricOccurrence
{
    /** Metric occurs at every region enter and leave. */
    SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT = 0,
    /** Metric occurs only at a region enter and leave, but does not need to
     *  occur at every enter/leave. */
    SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS        = 1,
    /** Metric can occur at any place i.e. it is not related to region enter and
     *  leaves. */
    SCOREP_METRIC_OCCURRENCE_ASYNCHRONOUS       = 2,

    SCOREP_INVALID_METRIC_OCCURRENCE /**< For internal use only. */
} SCOREP_MetricOccurrence;

/**
 * Types to be used in defining the scope of a scoped sampling set.
 *
 */
typedef enum SCOREP_MetricScope
{
    /** Scope of a metric is another location. */
    SCOREP_METRIC_SCOPE_LOCATION         = 0,
    /** Scope of a metric is a location group. */
    SCOREP_METRIC_SCOPE_LOCATION_GROUP   = 1,
    /** Scope of a metric is a system tree node. */
    SCOREP_METRIC_SCOPE_SYSTEM_TREE_NODE = 2,
    /** Scope of a metric is a generic group of locations. */
    SCOREP_METRIC_SCOPE_GROUP            = 3,

    SCOREP_INVALID_METRIC_SCOPE /**< For internal use only. */
} SCOREP_MetricScope;

/**
 * Types used to define type of profiling.
 */
typedef enum SCOREP_MetricProfilingType
{
    /** Exclusive values (excludes values from subordinated items) */
    SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE = 0,
    /** Inclusive values (sum including values from subordinated items ) */
    SCOREP_METRIC_PROFILING_TYPE_INCLUSIVE = 1,
    /** Single value */
    SCOREP_METRIC_PROFILING_TYPE_SIMPLE    = 2,
    /** Maximum values */
    SCOREP_METRIC_PROFILING_TYPE_MAX       = 3,
    /** Minimum values */
    SCOREP_METRIC_PROFILING_TYPE_MIN       = 4,

    SCOREP_INVALID_METRIC_PROFILING_TYPE /**< For internal use only. */
} SCOREP_MetricProfilingType;

/**
 * Types to be used in defining a location (SCOREP_DefineLocation()).
 *
 */
typedef enum SCOREP_LocationType
{
    SCOREP_LOCATION_TYPE_CPU_THREAD = 0,
    SCOREP_LOCATION_TYPE_GPU        = 1,
    SCOREP_LOCATION_TYPE_METRIC     = 2,

    SCOREP_INVALID_LOCATION_TYPE /**< For internal use only. */
} SCOREP_LocationType;

/**
 * Types to be used in defining a location group (SCOREP_DefineLocationGroup()).
 *
 */
typedef enum SCOREP_LocationGroupType
{
    SCOREP_LOCATION_GROUP_TYPE_PROCESS = 0,

    SCOREP_INVALID_LOCATION_GROUP_TYPE /**< For internal use only. */
} SCOREP_LocationGroupType;


/**
 * Types to be used in defining a group (SCOREP_DefineGroup()).
 *
 */
typedef enum SCOREP_GroupType
{
    SCOREP_GROUP_UNKNOWN       = 0,
    SCOREP_GROUP_LOCATIONS     = 1,
    SCOREP_GROUP_REGIONS       = 2,
    SCOREP_GROUP_COMM_SELF     = 3,
    SCOREP_GROUP_METRIC        = 4,
    SCOREP_GROUP_MPI_GROUP     = 5,
    SCOREP_GROUP_MPI_LOCATIONS = 6,

    SCOREP_INVALID_GROUP_TYPE /**< For internal use only. */
} SCOREP_GroupType;



/**
 * Types to be used in defining a region (SCOREP_DefineRegion()). These types
 * are currently not used inside the measurement system. This @e may change in
 * future if we are going to implement phases/dynamic regions etc. inside the
 * measurement system as opposed to inside the adapters or as a postprocessing
 * step. The names should be self explanatory; most of them are already used
 * (whith a different prefix) in VampiTrace and Scalasca.
 *
 * @todo remove SCOREP_REGION_UNKNOWN
 */
typedef enum SCOREP_RegionType
{
    SCOREP_REGION_UNKNOWN = 0,
    SCOREP_REGION_FUNCTION,
    SCOREP_REGION_LOOP,
    SCOREP_REGION_USER,
    SCOREP_REGION_CODE,

    SCOREP_REGION_PHASE,
    SCOREP_REGION_DYNAMIC,
    SCOREP_REGION_DYNAMIC_PHASE,
    SCOREP_REGION_DYNAMIC_LOOP,
    SCOREP_REGION_DYNAMIC_FUNCTION,
    SCOREP_REGION_DYNAMIC_LOOP_PHASE,

    SCOREP_REGION_COLL_ONE2ALL,
    SCOREP_REGION_COLL_ALL2ONE,
    SCOREP_REGION_COLL_ALL2ALL,
    SCOREP_REGION_COLL_OTHER,
    SCOREP_REGION_POINT2POINT,

    SCOREP_REGION_PARALLEL,
    SCOREP_REGION_SECTIONS,
    SCOREP_REGION_SECTION,
    SCOREP_REGION_WORKSHARE,
    SCOREP_REGION_SINGLE,
    SCOREP_REGION_MASTER,
    SCOREP_REGION_CRITICAL,
    SCOREP_REGION_ATOMIC,
    SCOREP_REGION_BARRIER,
    SCOREP_REGION_IMPLICIT_BARRIER,
    SCOREP_REGION_FLUSH,
    SCOREP_REGION_CRITICAL_SBLOCK, /* type for the innerblock of criticals */
    SCOREP_REGION_SINGLE_SBLOCK,   /* type for the innerblock od singles */
    SCOREP_REGION_WRAPPER,
    SCOREP_REGION_TASK,
    SCOREP_REGION_TASKWAIT,
    SCOREP_REGION_TASK_CREATE,
    SCOREP_REGION_ORDERED,
    SCOREP_REGION_ORDERED_SBLOCK,

    SCOREP_INVALID_REGION_TYPE /**< For internal use only. */
} SCOREP_RegionType;



/**
 * Types to be used in defining a parameter for parameter based profiling
 * (SCOREP_DefineParameter()).
 *
 */
typedef enum SCOREP_ParameterType
{
    SCOREP_PARAMETER_INT64,
    SCOREP_PARAMETER_UINT64,
    SCOREP_PARAMETER_STRING,

    SCOREP_INVALID_PARAMETER_TYPE /**< For internal use only. */
} SCOREP_ParameterType;

/**
 * Types to specify the used MPI collectives in calls to @a SCOREP_MpiCollective.
 */
typedef enum SCOREP_MpiCollectiveType
{
    SCOREP_COLLECTIVE_MPI_BARRIER,
    SCOREP_COLLECTIVE_MPI_BCAST,
    SCOREP_COLLECTIVE_MPI_GATHER,
    SCOREP_COLLECTIVE_MPI_GATHERV,
    SCOREP_COLLECTIVE_MPI_SCATTER,
    SCOREP_COLLECTIVE_MPI_SCATTERV,
    SCOREP_COLLECTIVE_MPI_ALLGATHER,
    SCOREP_COLLECTIVE_MPI_ALLGATHERV,
    SCOREP_COLLECTIVE_MPI_ALLTOALL,
    SCOREP_COLLECTIVE_MPI_ALLTOALLV,
    SCOREP_COLLECTIVE_MPI_ALLTOALLW,
    SCOREP_COLLECTIVE_MPI_ALLREDUCE,
    SCOREP_COLLECTIVE_MPI_REDUCE,
    SCOREP_COLLECTIVE_MPI_REDUCE_SCATTER,
    SCOREP_COLLECTIVE_MPI_REDUCE_SCATTER_BLOCK,
    SCOREP_COLLECTIVE_MPI_SCAN,
    SCOREP_COLLECTIVE_MPI_EXSCAN
} SCOREP_MpiCollectiveType;


/**
 * The type of a SCOREP_ConfigVariable.
 *
 */
typedef enum SCOREP_ConfigType
{
    /**
     * A string value with variable expension (Ie. ${HOME})
     * (path normalization?)
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type char*.
     */
    SCOREP_CONFIG_TYPE_PATH,

    /**
     * A string value.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type char*. The value of the variable should always be a valid pointer
     * returned from the @a malloc family of functions or @a NULL.
     */
    SCOREP_CONFIG_TYPE_STRING,

    /**
     * A boolean value.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type bool.
     */
    SCOREP_CONFIG_TYPE_BOOL,

    /**
     * A numerical value
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     */
    SCOREP_CONFIG_TYPE_NUMBER,

    /**
     * A numerical value with datasize suffixes
     * (Ie. Kb, Gb, ..., 'b' may be omitted, case insensitive)
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     */
    SCOREP_CONFIG_TYPE_SIZE,

    /**
     * A symbolic set.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type char**.
     *
     * SCOREP_ConfigVariable::variableContext should point to a NULL terminated
     * string list with valid set members
     *
     * The strings can be separated by any of the following charachters:
     *   @li " " - space
     *   @li "," - comma
     *   @li ":" - colon
     *   @li ";" - semicolon
     * The resulting string list will be converted to lower case.
     * Only strings in the valid set members are allowed.
     */
    SCOREP_CONFIG_TYPE_SET,

    /**
     * A symbolic set, represented as a bitmask.
     *
     * @note At most 64 set members are supported.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     *
     * SCOREP_ConfigVariable::variableContext should point to an array of type
     * SCOREP_ConfigType_SetEntry with valid set members and there values.
     * For better debugging, they should be in decreasing order of the value
     * field.
     * Terminate the array with an entry { NULL, 0 }. Case doesn't matter.
     *
     * The strings can be separated by any of the following charachters:
     *   @li " " - space
     *   @li "," - comma
     *   @li ":" - colon
     *   @li ";" - semicolon
     */
    SCOREP_CONFIG_TYPE_BITSET,

    SCOREP_INVALID_CONFIG_TYPE /**< For internal use only. */
} SCOREP_ConfigType;


/**
 * Config variable object. Partially initialized objects of this type need to
 * be provided by the adapters if they need to access config variables.
 *
 * @see SCOREP_ConfigRegister()
 *
 */
typedef struct SCOREP_ConfigVariable
{
    const char*       name;
    SCOREP_ConfigType type;
    void*             variableReference;
    void*             variableContext;
    const char*       defaultValue;
    const char*       shortHelp;
    const char*       longHelp;
} SCOREP_ConfigVariable;


/**
 * The type for SCOREP_CONFIG_TYPE_BITSET entries.
 *
 */
typedef struct SCOREP_ConfigType_SetEntry
{
    const char* name;
    uint64_t    value;
} SCOREP_ConfigType_SetEntry;


/**
 * Type of MPI Ranks. Type of MPI ranks always int.
 */
typedef int SCOREP_MpiRank;

/**
 * Type of a MPI Non-blocking communication request id.
 */
typedef int32_t SCOREP_MpiRequestId;


/**
 * @def SCOREP_MPI_INVALID_RANK
 * Defines an invalid MPI rank.
 */
#define SCOREP_MPI_INVALID_RANK -1


/**
 * Symbolic constant representing an invalid or unknown rank.
 * @see SCOREP_MpiCollective()
 */
#define SCOREP_INVALID_ROOT_RANK -1


/**
 * List of known otf2-properties.
 */
typedef enum
{
    SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE,
    SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE,
    SCOREP_PROPERTY_THREAD_UNIQUE_FORK_SEQUENCE_COUNTS,
    SCOREP_PROPERTY_MAX
} SCOREP_Property;

/**
 * List of possible conditions for properties.
 */
typedef enum
{
    /** All processes must invalidated the property */
    SCOREP_PROPERTY_CONDITION_ALL,
    /** Any process must invalidated the property */
    SCOREP_PROPERTY_CONDITION_ANY
} SCOREP_PropertyCondition;


/**
 * Symbolic constant denoting that the fork sequence count
 * can't be provided by the adapter (and will get maintained
 * by a threading-model specific implementation). To be used
 * in SCOREP_Thread_Begin(), SCOREP_Thread_End(), and
 * SCOREP_Thread_Join().
 */
#define SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT UINT32_MAX


/**
 * Types to be used by threading-model adapters to be passed
 * to SCOREP_Thread_*() functions.
 */
typedef enum SCOREP_ThreadModel
{
    SCOREP_THREAD_MODEL_OPENMP,
    SCOREP_THREAD_MODEL_PTHREAD,
    SCOREP_THREAD_MODEL_OMPSS,
    SCOREP_THREAD_MODEL_EXTERNAL,

    SCOREP_INVALID_THREAD_MODEL /**< For internal use only. */
} SCOREP_ThreadModel;


/**
 * Type of direct RMA synchronization call.
 */
typedef enum SCOREP_RmaSyncType
{
    /** Synchronize memory copy. */
    SCOREP_RMA_SYNC_TYPE_MEMORY,
    /** Incoming remote notification. */
    SCOREP_RMA_SYNC_TYPE_NOTIFY_IN,
    /** Outgoing remote notification. */
    SCOREP_RMA_SYNC_TYPE_NOTIFY_OUT,

    SCOREP_INVALID_RMA_SYNC_TYPE /**< For internal use only. */
} SCOREP_RmaSyncType;


/**
 * Types to be used by RMA records to be passed
 * to SCOREP_Rma*() functions.
 */
typedef enum SCOREP_RmaSyncLevel
{
    /** @brief No process synchronization or access completion (e.g., MPI_Win_post,
     *  MPI_Win_start).
     */
    SCOREP_RMA_SYNC_LEVEL_NONE    = 0,
    /** @brief Synchronize processes (e.g., MPI_Win_create/free).
     */
    SCOREP_RMA_SYNC_LEVEL_PROCESS = ( 1 << 0 ),
    /** @brief Complete memory accesses (e.g., MPI_Win_complete, MPI_Win_wait).
     */
    SCOREP_RMA_SYNC_LEVEL_MEMORY  = ( 1 << 1 )
} SCOREP_RmaSyncLevel;


/**
 * General Lock Type.
 */
typedef enum SCOREP_LockType
{
    /** Exclusive lock. No other lock will be granted.
     */
    SCOREP_LOCK_EXCLUSIVE,
    /** Shared lock. Other shared locks will be granted, but no exclusive
     *  locks.
     */
    SCOREP_LOCK_SHARED,

    SCOREP_INVALID_LOCK_TYPE /**< For internal use only. */
} SCOREP_LockType;


/**
 * RMA Atomic Operation Type.
 */
typedef enum SCOREP_RmaAtomicType
{
    SCOREP_RMA_ATOMIC_TYPE_ACCUMULATE,
    SCOREP_RMA_ATOMIC_TYPE_INCREMENT,
    SCOREP_RMA_ATOMIC_TYPE_TEST_AND_SET,
    SCOREP_RMA_ATOMIC_TYPE_COMPARE_AND_SWAP,

    SCOREP_INVALID_RMA_ATOMIC_TYPE
} SCOREP_RmaAtomicType;


/**
 * Type used to indicate a reference to a Group definition
 */
typedef uint32_t SCOREP_GroupRef;

/*@}*/


#endif /* SCOREP_TYPES_H */
