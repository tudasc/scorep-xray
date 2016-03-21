/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
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


#ifndef SCOREP_TYPES_H
#define SCOREP_TYPES_H


/**
 * @file
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
 * Phases the measurement can be in.
 */
typedef enum SCOREP_MeasurementPhase
{
    /** @breif The measurement was not initialized yet, or is currently initializing. */
    SCOREP_MEASUREMENT_PHASE_PRE    = -1,

    /** @breif The measurement is initialized and not finalized yet. */
    SCOREP_MEASUREMENT_PHASE_WITHIN = 0,

    /** @breif The measurement is finalizing or already finalized. */
    SCOREP_MEASUREMENT_PHASE_POST   = 1
} SCOREP_MeasurementPhase;


/** Activation and deactivation phase for CPU locations */
typedef enum SCOREP_CPULocationPhase
{
    SCOREP_CPU_LOCATION_PHASE_MGMT,
    SCOREP_CPU_LOCATION_PHASE_EVENTS,
    SCOREP_CPU_LOCATION_PHASE_PAUSE
} SCOREP_CPULocationPhase;


/**
 * Type used in specifying line numbers.
 * @see SCOREP_Definitions_NewRegion()
 */
typedef uint32_t SCOREP_LineNo;



/**
 * Symbolic constant representing an invalid or unknown line number.
 * @see SCOREP_Definitions_NewRegion()
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
#define SCOREP_INVALID_INTERIM_COMMUNICATOR SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI communicator
 * definition.
 */
#define SCOREP_INVALID_COMMUNICATOR SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI window definition.
 */
#define SCOREP_INVALID_INTERIM_RMA_WINDOW SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI window definition.
 */
#define SCOREP_INVALID_RMA_WINDOW SCOREP_MOVABLE_NULL


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
 * Symbolic constant representing an invalid or unknown system tree node property
 */
#define SCOREP_INVALID_SYSTEM_TREE_NODE_PROPERTY SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown metric class definition.
 */
#define SCOREP_INVALID_SAMPLING_SET_RECORDER SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown meta data entry.
 */
#define SCOREP_INVALID_LOCATION_PROPERTY SCOREP_MOVABLE_NULL

/**
 * Symbolic constant representing an invalid or unknown calling context entry.
 */
#define SCOREP_INVALID_CALLING_CONTEXT SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown interrupt generator entry.
 */
#define SCOREP_INVALID_INTERRUPT_GENERATOR SCOREP_MOVABLE_NULL


/**
 * Types to be used in defining a region (SCOREP_Definitions_NewRegion()). In order to
 * track the origin of a region definition, the adapter needs to provide @e
 * his type.
 *
 */

#define SCOREP_PARADIGM_CLASSES \
    SCOREP_PARADIGM_CLASS( MPP,                "multi-process", PROCESS ) \
    SCOREP_PARADIGM_CLASS( THREAD_FORK_JOIN,   "fork/join",     THREAD_FORK_JOIN ) \
    SCOREP_PARADIGM_CLASS( THREAD_CREATE_WAIT, "create/wait",   THREAD_CREATE_WAIT ) \
    SCOREP_PARADIGM_CLASS( ACCELERATOR,        "accelerator",   ACCELERATOR )


typedef enum SCOREP_ParadigmClass
{
#define SCOREP_PARADIGM_CLASS( NAME, name, OTF2_NAME ) \
    SCOREP_PARADIGM_CLASS_ ## NAME,
    SCOREP_PARADIGM_CLASSES

#undef SCOREP_PARADIGM_CLASS

    SCOREP_INVALID_PARADIGM_CLASS /**< For internal use only. */
} SCOREP_ParadigmClass;


/* Keep MPI first after the non-parallel paradigms */
#define SCOREP_PARADIGMS \
    SCOREP_PARADIGM( MEASUREMENT,        "measurement",        MEASUREMENT_SYSTEM ) \
    SCOREP_PARADIGM( USER,               "user",               USER ) \
    SCOREP_PARADIGM( COMPILER,           "compiler",           COMPILER ) \
    SCOREP_PARADIGM( SAMPLING,           "sampling",           SAMPLING ) \
    SCOREP_PARADIGM( MEMORY,             "memory",             NONE ) \
    SCOREP_PARADIGM( MPI,                "mpi",                MPI ) \
    SCOREP_PARADIGM( SHMEM,              "shmem",              SHMEM ) \
    SCOREP_PARADIGM( OPENMP,             "openmp",             OPENMP ) \
    SCOREP_PARADIGM( PTHREAD,            "pthread",            PTHREAD ) \
    SCOREP_PARADIGM( CUDA,               "cuda",               CUDA ) \
    SCOREP_PARADIGM( OPENCL,             "opencl",             OPENCL )


typedef enum SCOREP_ParadigmType
{
#define SCOREP_PARADIGM( NAME, name_str, OTF2_NAME ) \
    SCOREP_PARADIGM_ ## NAME,
    SCOREP_PARADIGMS

#undef SCOREP_PARADIGM

    SCOREP_INVALID_PARADIGM_TYPE /**< For internal use only. */
} SCOREP_ParadigmType;


/**
 *  Known flags for parallel paradigms.
 *
 *  Flags are essential boolean typed SCOREP_ParadigmProperty and thus
 *  exists as convenience.
 */
typedef enum SCOREP_ParadigmFlags
{
    /**
     *  Attests that the paradigm is purely RMA based.
     *
     *  As the definitions structure for RMA windows is based on communicators,
     *  these additional definitions don't reflect the actuall paradigm.
     *  By setting this flag for the paradigm it can therefore attest to the
     *  measurement data reader that the additional communicator definitions
     *  are not used.
     */
    SCOREP_PARADIGM_FLAG_RMA_ONLY = ( 1 << 0 ),

    /** Value for empty flags. */
    SCOREP_PARADIGM_FLAG_NONE     = 0
} SCOREP_ParadigmFlags;


/**
 *  Known properties for parallel paradigms.
 */
typedef enum SCOREP_ParadigmProperty
{
    /** Template for communicators without an explicit name.
     *  Must contain the string "${id}" to make each name unique.
     *  Mostly only useful if the paradigm allows user created communicators.
     *  Value is of type SCOREP_StringHandle.
     */
    SCOREP_PARADIGM_PROPERTY_COMMUNICATOR_TEMPLATE,

    /** Template for RMA windows without an explicit name.
     *  Must contain the string "${id}" to make each name unique.
     *  Mostly only useful if the paradigm allows user created rma windows.
     *  Value is of type SCOREP_StringHandle.
     */
    SCOREP_PARADIGM_PROPERTY_RMA_WINDOW_TEMPLATE,

    SCOREP_INVALID_PARADIGM_PROPERTY /**< For internal use only. */
} SCOREP_ParadigmProperty;


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
 * Types to be used in defining a location (SCOREP_Definitions_NewLocation()).
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
 * Types to be used in defining a location group (SCOREP_Definitions_NewLocationGroup()).
 *
 */
typedef enum SCOREP_LocationGroupType
{
    SCOREP_LOCATION_GROUP_TYPE_PROCESS = 0,

    SCOREP_INVALID_LOCATION_GROUP_TYPE /**< For internal use only. */
} SCOREP_LocationGroupType;


/**
 * Types to be used in defining a group (SCOREP_Definitions_NewGroup()).
 *
 */
typedef enum SCOREP_GroupType
{
    SCOREP_GROUP_UNKNOWN             = 0,
    SCOREP_GROUP_LOCATIONS           = 1,
    SCOREP_GROUP_REGIONS             = 2,
    SCOREP_GROUP_METRIC              = 3,

    SCOREP_GROUP_MPI_LOCATIONS       = 4,
    SCOREP_GROUP_MPI_GROUP           = 5,
    SCOREP_GROUP_MPI_SELF            = 6,

    SCOREP_GROUP_OPENMP_LOCATIONS    = 7,
    SCOREP_GROUP_OPENMP_THREAD_TEAM  = 8,

    SCOREP_GROUP_CUDA_LOCATIONS      = 9,
    SCOREP_GROUP_CUDA_GROUP          = 10,

    SCOREP_GROUP_SHMEM_LOCATIONS     = 11,
    SCOREP_GROUP_SHMEM_GROUP         = 12,
    SCOREP_GROUP_SHMEM_SELF          = 13,

    SCOREP_GROUP_PTHREAD_LOCATIONS   = 14,
    SCOREP_GROUP_PTHREAD_THREAD_TEAM = 15,

    SCOREP_GROUP_OPENCL_LOCATIONS    = 16,
    SCOREP_GROUP_OPENCL_GROUP        = 17,

    SCOREP_INVALID_GROUP_TYPE /**< For internal use only. */
} SCOREP_GroupType;



/**
 * Types to be used in defining a region (SCOREP_Definitions_NewRegion()). These types
 * are currently not used inside the measurement system. This @e may change in
 * future if we are going to implement phases/dynamic regions etc. inside the
 * measurement system as opposed to inside the adapters or as a postprocessing
 * step. The names should be self explanatory; most of them are already used
 * (whith a different prefix) in VampiTrace and Scalasca.
 *
 * @todo remove SCOREP_REGION_UNKNOWN
 */
#define SCOREP_REGION_TYPES \
    SCOREP_REGION_TYPE( COLL_ONE2ALL, "one2all" ) \
    SCOREP_REGION_TYPE( COLL_ALL2ONE, "all2one" ) \
    SCOREP_REGION_TYPE( COLL_ALL2ALL, "all2all" ) \
    SCOREP_REGION_TYPE( COLL_OTHER,   "other collective" ) \
    SCOREP_REGION_TYPE( POINT2POINT,  "point2point" ) \
    SCOREP_REGION_TYPE( PARALLEL,     "parallel" ) \
    SCOREP_REGION_TYPE( SECTIONS,     "sections" ) \
    SCOREP_REGION_TYPE( SECTION,      "section" ) \
    SCOREP_REGION_TYPE( WORKSHARE,    "workshare" ) \
    SCOREP_REGION_TYPE( SINGLE,       "single" ) \
    SCOREP_REGION_TYPE( MASTER,       "master" ) \
    SCOREP_REGION_TYPE( CRITICAL,     "critical" ) \
    SCOREP_REGION_TYPE( ATOMIC,       "atomic" ) \
    SCOREP_REGION_TYPE( BARRIER,      "barrier" ) \
    SCOREP_REGION_TYPE( IMPLICIT_BARRIER, "implicit barrier" ) \
    SCOREP_REGION_TYPE( FLUSH,        "flush" ) \
    SCOREP_REGION_TYPE( CRITICAL_SBLOCK, "critical sblock" ) \
    SCOREP_REGION_TYPE( SINGLE_SBLOCK, "single sblock" ) \
    SCOREP_REGION_TYPE( WRAPPER,      "wrapper" ) \
    SCOREP_REGION_TYPE( TASK,         "task" ) \
    SCOREP_REGION_TYPE( TASK_UNTIED,  "untied task" ) \
    SCOREP_REGION_TYPE( TASK_WAIT,    "taskwait" ) \
    SCOREP_REGION_TYPE( TASK_CREATE,  "task create" ) \
    SCOREP_REGION_TYPE( ORDERED,      "ordered" ) \
    SCOREP_REGION_TYPE( ORDERED_SBLOCK, "ordered sblock" ) \
    SCOREP_REGION_TYPE( ARTIFICIAL,   "artificial" ) \
    SCOREP_REGION_TYPE( RMA,          "rma" ) \
    SCOREP_REGION_TYPE( THREAD_CREATE, "thread create" ) \
    SCOREP_REGION_TYPE( THREAD_WAIT,  "thread wait" ) \
    SCOREP_REGION_TYPE( ALLOCATE,     "allocate" ) \
    SCOREP_REGION_TYPE( DEALLOCATE,   "deallocate" ) \
    SCOREP_REGION_TYPE( REALLOCATE,   "reallocate" )


#define SCOREP_REGION_TYPE( NAME, name_str ) \
    SCOREP_REGION_ ## NAME,

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

    SCOREP_REGION_TYPES

    SCOREP_INVALID_REGION_TYPE /**< For internal use only. */
} SCOREP_RegionType;

#undef SCOREP_REGION_TYPE


/**
 * Types to be used in defining a parameter for parameter based profiling
 * (SCOREP_Definitions_NewParameter()).
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
 * Types to specify the used collectives in calls to @a SCOREP_MpiCollectiveBegin
 * and @a SCOREP_RmaCollectiveBegin.
 */
typedef enum SCOREP_CollectiveType
{
    SCOREP_COLLECTIVE_BARRIER,
    SCOREP_COLLECTIVE_BROADCAST,
    SCOREP_COLLECTIVE_GATHER,
    SCOREP_COLLECTIVE_GATHERV,
    SCOREP_COLLECTIVE_SCATTER,
    SCOREP_COLLECTIVE_SCATTERV,
    SCOREP_COLLECTIVE_ALLGATHER,
    SCOREP_COLLECTIVE_ALLGATHERV,
    SCOREP_COLLECTIVE_ALLTOALL,
    SCOREP_COLLECTIVE_ALLTOALLV,
    SCOREP_COLLECTIVE_ALLTOALLW,
    SCOREP_COLLECTIVE_ALLREDUCE,
    SCOREP_COLLECTIVE_REDUCE,
    SCOREP_COLLECTIVE_REDUCE_SCATTER,
    SCOREP_COLLECTIVE_REDUCE_SCATTER_BLOCK,
    SCOREP_COLLECTIVE_SCAN,
    SCOREP_COLLECTIVE_EXSCAN,
    SCOREP_COLLECTIVE_CREATE_HANDLE,
    SCOREP_COLLECTIVE_DESTROY_HANDLE,
    SCOREP_COLLECTIVE_ALLOCATE,
    SCOREP_COLLECTIVE_DEALLOCATE,
    SCOREP_COLLECTIVE_CREATE_HANDLE_AND_ALLOCATE,
    SCOREP_COLLECTIVE_DESTROY_HANDLE_AND_DEALLOCATE
} SCOREP_CollectiveType;


/**
 * The type of a SCOREP_ConfigVariable.
 *
 */
typedef enum SCOREP_ConfigType
{
    /**
     * A string value.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type char*. The value of the variable should always be a valid pointer
     * returned from the @a malloc family of functions or @a NULL.
     */
    SCOREP_CONFIG_TYPE_STRING,

    /**
     * A string value. But the documentation indicates, that a path is expected.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type char*.
     */
    SCOREP_CONFIG_TYPE_PATH,

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
     * The strings can be separated by any of the following characters:
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
     * SCOREP_ConfigType_SetEntry with valid set members, their values,
     * and a description.
     * Aliases are allowed and should be separated by "/".
     * An empty value results in setting the variable to 0.
     * "none" and "no" are always considered as the empty set and should
     * not show up in this list, also entries with value zero are prohibited.
     * For better debugging, they should be in decreasing order of the value
     * field.
     * Terminate the array with an entry { NULL, 0, NULL }. Case doesn't matter.
     *
     * The entries can be separated by any of the following characters:
     *   @li " " - space
     *   @li "," - comma
     *   @li ":" - colon
     *   @li ";" - semicolon
     */
    SCOREP_CONFIG_TYPE_BITSET,

    /**
     * Selects an option out of possible values.
     *
     * SCOREP_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     *
     * SCOREP_ConfigVariable::variableContext should point to an array of type
     * SCOREP_ConfigType_SetEntry with valid set members, their values,
     * and a description.
     * Aliases are allowed and should be separated by "/".
     * An empty value results in setting the variable to 0.
     * Terminate the array with an entry { NULL, 0, NULL }. Case doesn't matter.
     */
    SCOREP_CONFIG_TYPE_OPTIONSET,

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
 * The type for SCOREP_CONFIG_TYPE_BITSET and SCOREP_CONFIG_TYPE_OPTIONSET entries.
 *
 */
typedef struct SCOREP_ConfigType_SetEntry
{
    const char* name;
    uint64_t    value;
    const char* description;
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

    SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE,
    SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE,
    SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE,
    SCOREP_PROPERTY_PTHREAD_LOCATION_REUSED,

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
 * Class of locations which recorded a sampling set.
 */
typedef enum SCOREP_SamplingSetClass
{
    SCOREP_SAMPLING_SET_ABSTRACT,
    SCOREP_SAMPLING_SET_CPU,
    SCOREP_SAMPLING_SET_GPU
} SCOREP_SamplingSetClass;


/**
 * Domains of a system tree.
 */
typedef enum SCOREP_SystemTreeDomain
{
    SCOREP_SYSTEM_TREE_DOMAIN_NONE          = 0,

    SCOREP_SYSTEM_TREE_DOMAIN_MACHINE       = ( 1 << 0 ),
    SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY = ( 1 << 1 ),
    SCOREP_SYSTEM_TREE_DOMAIN_NUMA          = ( 1 << 2 ),
    SCOREP_SYSTEM_TREE_DOMAIN_SOCKET        = ( 1 << 3 ),
    SCOREP_SYSTEM_TREE_DOMAIN_CACHE         = ( 1 << 4 ),
    SCOREP_SYSTEM_TREE_DOMAIN_CORE          = ( 1 << 5 ),
    SCOREP_SYSTEM_TREE_DOMAIN_PU            = ( 1 << 6 )
} SCOREP_SystemTreeDomain;


#define SCOREP_RMA_SYNC_TYPES \
    SCOREP_RMA_SYNC_TYPE( MEMORY, memory, "memory" )             /* Synchronize memory copy. */ \
    SCOREP_RMA_SYNC_TYPE( NOTIFY_IN, notify_in, "notify in" )    /* Incoming remote notification. */ \
    SCOREP_RMA_SYNC_TYPE( NOTIFY_OUT, notify_out, "notify out" ) /* Outgoing remote notification. */

/**
 * Type of direct RMA synchronization call.
 */
typedef enum SCOREP_RmaSyncType
{
#define SCOREP_RMA_SYNC_TYPE( upper, lower, name )  SCOREP_RMA_SYNC_TYPE_ ## upper,
    SCOREP_RMA_SYNC_TYPES
    #undef SCOREP_RMA_SYNC_TYPE

    SCOREP_INVALID_RMA_SYNC_TYPE /**< For internal use only. */
} SCOREP_RmaSyncType;


/*
 * NONE: No process synchronization or access completion (e.g.,
 * MPI_Win_post.
 *
 * PROCESS: Synchronize processes (e.g., MPI_Win_create/free)
 *
 * MEMORY: Complete memory accesses (e.g., MPI_Win_complete, MPI_Win_wait)
 */
#define SCOREP_RMA_SYNC_LEVELS \
    SCOREP_RMA_SYNC_LEVEL( NONE, none, "none", 0 ) \
    SCOREP_RMA_SYNC_LEVEL( PROCESS, process, "process", 1 << 0 ) \
    SCOREP_RMA_SYNC_LEVEL( MEMORY, memory, "memory", 1 << 1 )


/**
 * Types to be used by RMA records to be passed
 * to SCOREP_Rma*() functions.
 */
typedef enum SCOREP_RmaSyncLevel
{
#define SCOREP_RMA_SYNC_LEVEL( upper, lower, name, value ) \
    SCOREP_RMA_SYNC_LEVEL_ ## upper = value,

    SCOREP_RMA_SYNC_LEVELS
#undef SCOREP_RMA_SYNC_LEVEL
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

#define SCOREP_RMA_ATOMIC_TYPES \
    SCOREP_RMA_ATOMIC_TYPE( ACCUMULATE, accumulate,      "accumulate" )   \
    SCOREP_RMA_ATOMIC_TYPE( INCREMENT, increment,       "increment" )     \
    SCOREP_RMA_ATOMIC_TYPE( TEST_AND_SET, test_and_set,    "test and set" ) \
    SCOREP_RMA_ATOMIC_TYPE( COMPARE_AND_SWAP, compare_and_swap, "compare and swap" ) \
    SCOREP_RMA_ATOMIC_TYPE( SWAP, swap, "swap" ) \
    SCOREP_RMA_ATOMIC_TYPE( FETCH_AND_ADD, fetch_and_add, "fetch and add" ) \
    SCOREP_RMA_ATOMIC_TYPE( FETCH_AND_INCREMENT, fetch_and_increment, "fetch and increment" ) \
    SCOREP_RMA_ATOMIC_TYPE( ADD, add, "add" )


/**
 * RMA Atomic Operation Type.
 */
typedef enum SCOREP_RmaAtomicType
{
#define SCOREP_RMA_ATOMIC_TYPE( upper, lower, name ) SCOREP_RMA_ATOMIC_TYPE_ ## upper,
    SCOREP_RMA_ATOMIC_TYPES
    #undef SCOREP_RMA_ATOMIC_TYPE

    SCOREP_INVALID_RMA_ATOMIC_TYPE
} SCOREP_RmaAtomicType;


/**
 * Attribute type.
 */
typedef enum SCOREP_AttributeType
{
    SCOREP_ATTRIBUTE_TYPE_INT8,
    SCOREP_ATTRIBUTE_TYPE_INT16,
    SCOREP_ATTRIBUTE_TYPE_INT32,
    SCOREP_ATTRIBUTE_TYPE_INT64,
    SCOREP_ATTRIBUTE_TYPE_UINT8,
    SCOREP_ATTRIBUTE_TYPE_UINT16,
    SCOREP_ATTRIBUTE_TYPE_UINT32,
    SCOREP_ATTRIBUTE_TYPE_UINT64,
    SCOREP_ATTRIBUTE_TYPE_FLOAT,
    SCOREP_ATTRIBUTE_TYPE_DOUBLE,
    SCOREP_ATTRIBUTE_TYPE_STRING,
    SCOREP_ATTRIBUTE_TYPE_ATTRIBUTE,
    SCOREP_ATTRIBUTE_TYPE_LOCATION,
    SCOREP_ATTRIBUTE_TYPE_REGION,
    SCOREP_ATTRIBUTE_TYPE_GROUP,
    SCOREP_ATTRIBUTE_TYPE_METRIC,
    SCOREP_ATTRIBUTE_TYPE_INTERIM_COMMUNICATOR,
    SCOREP_ATTRIBUTE_TYPE_PARAMETER,
    SCOREP_ATTRIBUTE_TYPE_INTERIM_RMA_WINDOW,
    SCOREP_ATTRIBUTE_TYPE_SOURCE_CODE_LOCATION,
    SCOREP_ATTRIBUTE_TYPE_CALLING_CONTEXT,
    SCOREP_ATTRIBUTE_TYPE_INTERRUPT_GENERATOR
} SCOREP_AttributeType;

/**
 * Possible modes of a synchronization point.
 * Express the time when a synchronization happens.
 *
 */
typedef enum SCOREP_SynchronizationMode
{
    /** Synchronization at the beginning of the measurement */
    SCOREP_SYNCHRONIZATION_MODE_BEGIN,
    /** Synchronization at the initialization of a multi-process paradigm */
    SCOREP_SYNCHRONIZATION_MODE_BEGIN_MPP,
    /** Synchronization at the end of the measurement */
    SCOREP_SYNCHRONIZATION_MODE_END,

    SCOREP_SYNCHRONIZATION_MODE_MAX /**< NON-ABI, for internal use only. */
} SCOREP_SynchronizationMode;

/**
 * Interrupt generator modes.
 */
typedef enum SCOREP_InterruptGeneratorMode
{
    /** @brief Denotes that the interrupts generated are time based. */
    SCOREP_INTERRUPT_GENERATOR_MODE_TIME,
    /** @brief Denotes that the interrupts generated are count based. */
    SCOREP_INTERRUPT_GENERATOR_MODE_COUNT
} SCOREP_InterruptGeneratorMode;

/**
 * Task Handle
 */
typedef struct SCOREP_Task* SCOREP_TaskHandle;

/*@}*/


#endif /* SCOREP_TYPES_H */
