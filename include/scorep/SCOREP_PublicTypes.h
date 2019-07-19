/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2018,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2018,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Defines public definitions that are used internally and externally (e.g., by metric plugins, user functions, substrate plugins)
 */

#ifndef SCOREP_PUBLICTYPES_H
#define SCOREP_PUBLICTYPES_H

#include <stdint.h>

/** \defgroup Public type definitions and enums used in Score-P
 */
/*@{*/

/**
 * Type used in specify the exit status of the program.
 * @see ProgramEnd event
 */
typedef int64_t SCOREP_ExitStatus;


/**
 * Symbolic constant representing an invalid or unknown exit status.
 * Do not use INT64_C here, as this maybe accessed by C++
 */
#define SCOREP_INVALID_EXIT_STATUS ( ( int64_t )( ~( ( ~( ( uint64_t )0u ) ) >> 1 ) ) )


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
 * Opaque handle to memory that can be easily moved between
 * processes. Used for definitions as they have to be moved during
 * unification.
 */
typedef uint32_t SCOREP_Allocator_MovableMemory;

/**
 * Symbolic constant representing an invalid or NULL handle of type
 * SCOREP_Allocator_MovableMemory.
 *
 */
#define SCOREP_MOVABLE_NULL 0

/**
 * Type of a opaque handle to any definition.
 */
typedef SCOREP_Allocator_MovableMemory SCOREP_AnyHandle;

/**
 * Type of a opaque handle to a source file definition.
 * @see SCOREP_Definitions_NewSourceFile()
 */
typedef SCOREP_AnyHandle SCOREP_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SCOREP_INVALID_SOURCE_FILE SCOREP_MOVABLE_NULL

/**
 * Type of a opaque handle to a metric definition.
 * @see SCOREP_Definitions_NewMetric()
 */
typedef SCOREP_AnyHandle SCOREP_MetricHandle;

/**
 * Symbolic constant representing an invalid or unknown metric
 * definition.
 */
#define SCOREP_INVALID_METRIC SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a sampling set definition.
 * @see SCOREP_Definitions_NewSamplingSet()
 */
typedef SCOREP_AnyHandle SCOREP_SamplingSetHandle;

/**
 * Symbolic constant representing an invalid or unknown metric class definition.
 */
#define SCOREP_INVALID_SAMPLING_SET SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a region definition.
 * @see SCOREP_Definitions_NewRegion()
 */
typedef SCOREP_AnyHandle SCOREP_RegionHandle;

/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SCOREP_INVALID_REGION SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a paradigm definition.
 * @see SCOREP_Definitions_NewParadigm()
 */
typedef SCOREP_AnyHandle SCOREP_ParadigmHandle;

/**
 * Symbolic constant representing an invalid or unknown paradigm definition.
 */
#define SCOREP_INVALID_PARADIGM SCOREP_MOVABLE_NULL



/**
 * Task Handle
 */
typedef struct SCOREP_Task* SCOREP_TaskHandle;

/**
 * \enum SCOREP_CollectiveType
 * \brief Types to specify the used collectives in calls to @a SCOREP_MpiCollectiveBegin and @a SCOREP_RmaCollectiveBegin
 */
typedef enum SCOREP_CollectiveType
{
    SCOREP_COLLECTIVE_BARRIER,                      /**< The collective is a barrier, e.g., MPI_Barrier(...), shmem_barrier(...), or shmem_barrier_all(...) */
    SCOREP_COLLECTIVE_BROADCAST,                    /**< The collective is a barrier, e.g., MPI_Bcast(...), or shmem_broadcast32(...) */
    SCOREP_COLLECTIVE_GATHER,                       /**< The collective is a simple gather operation, e.g., MPI_Gather(...) */
    SCOREP_COLLECTIVE_GATHERV,                      /**< The collective is a complex gather operation, e.g., MPI_Gatherv(...) */
    SCOREP_COLLECTIVE_SCATTER,                      /**< The collective is a simple scatter operation, e.g., MPI_Scatter(...) */
    SCOREP_COLLECTIVE_SCATTERV,                     /**< The collective is a complex scatter operation, e.g., MPI_Scatterv(...) */
    SCOREP_COLLECTIVE_ALLGATHER,                    /**< The collective is a simple allgather operation, e.g., MPI_Allgather(...), or shmem_collect64(...) */
    SCOREP_COLLECTIVE_ALLGATHERV,                   /**< The collective is a complex allgather operation, e.g., MPI_Allgatherv(...) */
    SCOREP_COLLECTIVE_ALLTOALL,                     /**< The collective is a simple all-to-all communication, e.g., MPI_Alltoall(...) */
    SCOREP_COLLECTIVE_ALLTOALLV,                    /**< The collective is a all-to-all communication with more options for sizes and displacements, e.g., MPI_Alltoallv(...) */
    SCOREP_COLLECTIVE_ALLTOALLW,                    /**< The collective is a generalized all-to-all communication, e.g., MPI_Alltoallw(...) */
    SCOREP_COLLECTIVE_ALLREDUCE,                    /**< The collective is an allreduce operation, e.g., MPI_Allreduce(...) */
    SCOREP_COLLECTIVE_REDUCE,                       /**< The collective is a reduce operation, e.g., MPI_Reduce(...), or shmem_longlong_max_to_all(...) */
    SCOREP_COLLECTIVE_REDUCE_SCATTER,               /**< The collective is a reduce-scatter operation, which combines some values and scatters the results, e.g., MPI_Reduce_scatter(...) */
    SCOREP_COLLECTIVE_REDUCE_SCATTER_BLOCK,         /**< The collective is a reduce scatter block operation, e.g., MPI_Reduce_scatter_block(...) */
    SCOREP_COLLECTIVE_SCAN,                         /**< The collective is a scan operation, where partial reductions of data is computed, e.g., MPI_Scan(...) */
    SCOREP_COLLECTIVE_EXSCAN,                       /**< The collective is an exclusive scan operation, e.g., MPI_Exscan(...) */
    SCOREP_COLLECTIVE_CREATE_HANDLE,                /**< This is used by the tracing substrate to work together with OTF2  */
    SCOREP_COLLECTIVE_DESTROY_HANDLE,               /**< This is used by the tracing substrate to work together with OTF2 */
    SCOREP_COLLECTIVE_ALLOCATE,                     /**< This is used by the tracing substrate to work together with OTF2 */
    SCOREP_COLLECTIVE_DEALLOCATE,                   /**< This is used by the tracing substrate to work together with OTF2 */
    SCOREP_COLLECTIVE_CREATE_HANDLE_AND_ALLOCATE,   /**< This is used by the tracing substrate to work together with OTF2 */
    SCOREP_COLLECTIVE_DESTROY_HANDLE_AND_DEALLOCATE /**< This is used by the tracing substrate to work together with OTF2 */
} SCOREP_CollectiveType;



/**
 * Types to be used in defining a location (SCOREP_Definitions_NewLocation()).
 *
 */
#define SCOREP_LOCATION_TYPES \
    SCOREP_LOCATION_TYPE( CPU_THREAD, "CPU thread" ) \
    SCOREP_LOCATION_TYPE( GPU,        "GPU" ) \
    SCOREP_LOCATION_TYPE( METRIC,     "metric location" ) \


typedef enum SCOREP_LocationType
{
    #define SCOREP_LOCATION_TYPE( NAME, name_string ) SCOREP_LOCATION_TYPE_ ## NAME,
    SCOREP_LOCATION_TYPES
    #undef SCOREP_LOCATION_TYPE

    SCOREP_NUMBER_OF_LOCATION_TYPES,
    SCOREP_INVALID_LOCATION_TYPE /**< For internal use only. */
} SCOREP_LocationType;

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
 * Type of MPI Ranks. Type of MPI ranks always int.
 */
typedef int SCOREP_MpiRank;


/**
 * Type of a MPI Non-blocking communication request id.
 */
typedef uint64_t SCOREP_MpiRequestId;


/**
 * Symbolic constant representing an invalid or inapplicable target
 * rank for a lock event. @see SCOREP_RmaAcquireLock() and similar
 * calls.
 */
#define SCOREP_ALL_TARGET_RANKS -1

/**
 * Symbolic constant representing an invalid or unknown rank.
 * @see SCOREP_MpiCollective()
 */
#define SCOREP_INVALID_ROOT_RANK -1


/**
 * Types to be used in defining a region (SCOREP_Definitions_NewRegion()). In order to
 * track the origin of a region definition, the adapter needs to provide @e
 * his type.
 *
 */

/**
 * \enum SCOREP_ParadigmClass
 * \brief defines classes of paradigms that are monitored
 * Types:
 * - SCOREP_PARADIGM_CLASS_MPP refers to any multi processing based paradigms (e.g., MPI, SHMEM)
 * - SCOREP_PARADIGM_CLASS_THREAD_FORK_JOIN refers to any thread parallel fork-join based paradigms (e.g., OpenMP)
 * - SCOREP_PARADIGM_CLASS_THREAD_CREATE_WAIT refers to any thread parallel create wait based paradigms (e.g., PThreads)
 * - SCOREP_PARADIGM_CLASS_ACCELERATOR refers to any accelerator based paradigms
 * - SCOREP_INVALID_PARADIGM_CLASS for internal use only
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


/**
 * \enum SCOREP_ParadigmType
 * \brief defines paradigms that are be monitored
 *
 * Types:
 * - SCOREP_PARADIGM_MEASUREMENT refers to Score-P internals
 * - SCOREP_PARADIGM_USER refers to user instrumentation
 * - SCOREP_PARADIGM_COMPILER refers to compiler instrumentation
 * - SCOREP_PARADIGM_SAMPLING refers to sampling
 * - SCOREP_PARADIGM_MEMORY refers to a memory region (malloc/realloc/...)
 * - SCOREP_PARADIGM_LIBWRAP refers to region instrumented by user library wrapping
 * - SCOREP_PARADIGM_MPI refers to MPI instrumentation
 * - SCOREP_PARADIGM_SHMEM refers to SHMEM instrumentation
 * - SCOREP_PARADIGM_OPENMP refers to OpenMP instrumentation
 * - SCOREP_PARADIGM_PTHREAD refers to Pthread instrumentation
 * - SCOREP_PARADIGM_ORPHAN_THREAD refers to Pthreads that are not instrumented
 * - SCOREP_PARADIGM_CUDA refers to CUDA instrumentation
 * - SCOREP_PARADIGM_OPENCL refers to OpenCL instrumentation
 * - SCOREP_PARADIGM_OPENACC refers to OpenACC instrumentation
 * - SCOREP_PARADIGM_IO refers to I/O instrumentation
 * - SCOREP_INVALID_PARADIGM_TYPE for internal use only
 */
#define SCOREP_PARADIGMS \
    SCOREP_PARADIGM( MEASUREMENT,        "measurement",        MEASUREMENT_SYSTEM ) \
    SCOREP_PARADIGM( USER,               "user",               USER ) \
    SCOREP_PARADIGM( COMPILER,           "compiler",           COMPILER ) \
    SCOREP_PARADIGM( SAMPLING,           "sampling",           SAMPLING ) \
    SCOREP_PARADIGM( MEMORY,             "memory",             NONE ) \
    SCOREP_PARADIGM( LIBWRAP,            "libwrap",            NONE ) \
    SCOREP_PARADIGM( MPI,                "mpi",                MPI ) \
    SCOREP_PARADIGM( SHMEM,              "shmem",              SHMEM ) \
    SCOREP_PARADIGM( OPENMP,             "openmp",             OPENMP ) \
    SCOREP_PARADIGM( PTHREAD,            "pthread",            PTHREAD ) \
    SCOREP_PARADIGM( ORPHAN_THREAD,      "orphan thread",      UNKNOWN ) \
    SCOREP_PARADIGM( CUDA,               "cuda",               CUDA ) \
    SCOREP_PARADIGM( OPENCL,             "opencl",             OPENCL ) \
    SCOREP_PARADIGM( OPENACC,            "openacc",            OPENACC ) \
    SCOREP_PARADIGM( IO,                 "io",                 NONE )


typedef enum SCOREP_ParadigmType
{
#define SCOREP_PARADIGM( NAME, name_str, OTF2_NAME ) \
    SCOREP_PARADIGM_ ## NAME,
    SCOREP_PARADIGMS

#undef SCOREP_PARADIGM

    SCOREP_INVALID_PARADIGM_TYPE /**< For internal use only. */
} SCOREP_ParadigmType;

/**
 * \enum SCOREP_ParameterType
 * \brief defines types to be used in defining a parameter for parameter based profiling
 * (SCOREP_Definitions_NewParameter()).
 *
 */
typedef enum SCOREP_ParameterType
{
    SCOREP_PARAMETER_INT64,       /**< The parameter is an int64_t */
    SCOREP_PARAMETER_UINT64,      /**< The parameter is an uint64_t */
    SCOREP_PARAMETER_STRING,      /**< The parameter is a string */

    SCOREP_INVALID_PARAMETER_TYPE /**< For internal use only. */
} SCOREP_ParameterType;


/**
 * \enum SCOREP_RegionType
 * \brief specifies a Region
 *
 * Types to be used in defining a region (SCOREP_Definitions_NewRegion()). These types
 * are currently not used inside the measurement system. This @e may change in
 * future if we are going to implement phases/dynamic regions etc. inside the
 * measurement system as opposed to inside the adapters or as a postprocessing
 * step. The names should be self explanatory; most of them are already used
 * (whith a different prefix) in VampiTrace and Scalasca.
 *
 * Types:
 * - SCOREP_REGION_UNKNOWN The type of the region is unknown / not defined
 * - SCOREP_REGION_FUNCTION The region is defined by compiler instrumentation/sampling and defines a code function
 * - SCOREP_REGION_LOOP The region represents a loop in the source code (used by Opari)
 * - SCOREP_REGION_USER The region is a user region, e.g., an Opari user region
 * - SCOREP_REGION_CODE The region represents a code region
 * - SCOREP_REGION_PHASE (Currently not used)
 * - SCOREP_REGION_DYNAMIC (Currently not used)
 * - SCOREP_REGION_DYNAMIC_PHASE (Currently not used)
 * - SCOREP_REGION_DYNAMIC_LOOP (Currently not used)
 * - SCOREP_REGION_DYNAMIC_FUNCTION (Currently not used)
 * - SCOREP_REGION_DYNAMIC_LOOP_PHASE (Currently not used)
 * - SCOREP_REGION_COLL_ONE2ALL Represents a collective communication region with one2all communication
 * - SCOREP_REGION_COLL_ALL2ONE Represents a collective communication region with all2one communication
 * - SCOREP_REGION_COLL_ALL2ALL Represents a collective communication region with all2all communication
 * - SCOREP_REGION_COLL_OTHER Represents a collective communication region that is neither one2all, nor all2one, nor all2all
 * - SCOREP_REGION_POINT2POINT Represents a point2point communication region
 * - SCOREP_REGION_PARALLEL Represents an (OpenMP) parallel region
 * - SCOREP_REGION_SECTIONS Represents an (OpenMP) sections region
 * - SCOREP_REGION_SECTION Represents an (OpenMP) section region
 * - SCOREP_REGION_WORKSHARE Represents an (OpenMP) workshare region
 * - SCOREP_REGION_SINGLE Represents an (OpenMP) single region
 * - SCOREP_REGION_MASTER Represents an (OpenMP) master region
 * - SCOREP_REGION_CRITICAL Represents an (OpenMP) critical region
 * - SCOREP_REGION_ATOMIC Represents an atomic region
 * - SCOREP_REGION_BARRIER Represents a barrier
 * - SCOREP_REGION_IMPLICIT_BARRIER Represents an implicit barrier (that is implicitely given but not explicitely defined)
 * - SCOREP_REGION_FLUSH Represents an (OpenMP) flush region
 * - SCOREP_REGION_CRITICAL_SBLOCK Represents an sblock within a (OpenMP) critical region
 * - SCOREP_REGION_SINGLE_SBLOCK Represents an sblock within a (OpenMP) single region
 * - SCOREP_REGION_WRAPPER Represents a wrapper region (e.g., from interpositioning)
 * - SCOREP_REGION_TASK Represents a (OpenMP) task region, within SCOREP_REGION_TASK_CREATE
 * - SCOREP_REGION_TASK_UNTIED Represents a (OpenMP) untied task region
 * - SCOREP_REGION_TASK_WAIT Represents a (OpenMP) taskwait region
 * - SCOREP_REGION_TASK_CREATE Represents a created (OpenMP) task region
 * - SCOREP_REGION_ORDERED Represents an (OpenMP) ordered region
 * - SCOREP_REGION_ORDERED_SBLOCK Represents an sblock within a (OpenMP) ordered region
 * - SCOREP_REGION_ARTIFICIAL Represents an artificial region
 * - SCOREP_REGION_RMA Represents an RMA region
 * - SCOREP_REGION_THREAD_CREATE Represents the creation of a thread
 * - SCOREP_REGION_THREAD_WAIT Represents the creation of a thread
 * - SCOREP_REGION_ALLOCATE Represents a region where memory is allocated, e.g., MPI_Alloc_mem
 * - SCOREP_REGION_DEALLOCATE Represents a region where memory is deallocated
 * - SCOREP_REGION_REALLOCATE Represents a region where memory is reallocated
 * - SCOREP_REGION_FILE_IO Represents an I/O data operation region
 * - SCOREP_REGION_FILE_IO_METADATA Represents an I/O metadata operation region (e.g., seek)
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
    SCOREP_REGION_TYPE( REALLOCATE,   "reallocate" ) \
    SCOREP_REGION_TYPE( FILE_IO,      "file_io" ) \
    SCOREP_REGION_TYPE( FILE_IO_METADATA,  "file_io metadata" )


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
 * \enum SCOREP_RmaSyncType
 * \brief Type of direct RMA synchronization call
 *
 * Types:
 * - SCOREP_RMA_SYNC_TYPE_MEMORY Synchronize memory copy.
 * - SCOREP_RMA_SYNC_TYPE_NOTIFY_IN  Incoming remote notification.
 * - SCOREP_RMA_SYNC_TYPE_NOTIFY_OUT Outgoing remote notification
 * - SCOREP_INVALID_RMA_SYNC_TYPE for internal use only
 */

#define SCOREP_RMA_SYNC_TYPES \
    SCOREP_RMA_SYNC_TYPE( MEMORY, memory, "memory" )             \
    SCOREP_RMA_SYNC_TYPE( NOTIFY_IN, notify_in, "notify in" )    \
    SCOREP_RMA_SYNC_TYPE( NOTIFY_OUT, notify_out, "notify out" )

typedef enum SCOREP_RmaSyncType
{
#define SCOREP_RMA_SYNC_TYPE( upper, lower, name )  SCOREP_RMA_SYNC_TYPE_ ## upper,
    SCOREP_RMA_SYNC_TYPES
    #undef SCOREP_RMA_SYNC_TYPE

    SCOREP_INVALID_RMA_SYNC_TYPE /**< For internal use only. */
} SCOREP_RmaSyncType;



/**
 * \enum SCOREP_RmaSyncLevel
 * \brief specifies a RMA synchronization level, used by RMA records to be passed to SCOREP_Rma*() functions.
 *
 * Types:
 * - SCOREP_RMA_SYNC_LEVEL_NONE No process synchronization or access completion (e.g., * MPI_Win_post)
 * - SCOREP_RMA_SYNC_LEVEL_PROCESS Synchronize processes (e.g., MPI_Win_create/free)
 * - SCOREP_RMA_SYNC_LEVEL_MEMORY Complete memory accesses (e.g., MPI_Win_complete, MPI_Win_wait)
 * - SCOREP_RMA_SYNC_LEVELS for internal use only
 */

#define SCOREP_RMA_SYNC_LEVELS \
    SCOREP_RMA_SYNC_LEVEL( NONE, none, "none", 0 ) \
    SCOREP_RMA_SYNC_LEVEL( PROCESS, process, "process", 1 << 0 ) \
    SCOREP_RMA_SYNC_LEVEL( MEMORY, memory, "memory", 1 << 1 )

typedef enum SCOREP_RmaSyncLevel
{
#define SCOREP_RMA_SYNC_LEVEL( upper, lower, name, value ) \
    SCOREP_RMA_SYNC_LEVEL_ ## upper = value,

    SCOREP_RMA_SYNC_LEVELS
#undef SCOREP_RMA_SYNC_LEVEL
} SCOREP_RmaSyncLevel;


/**
 * \enum SCOREP_RmaAtomicType
 * \brief specifies a RMA Atomic Operation Type.
 *
 * Types:
 * - SCOREP_RMA_ATOMIC_TYPE_ACCUMULATE accumulate
 * - SCOREP_RMA_ATOMIC_TYPE_INCREMENT increment
 * - SCOREP_RMA_ATOMIC_TYPE_TEST_AND_SET test and set
 * - SCOREP_RMA_ATOMIC_TYPE_COMPARE_AND_SWAP compare and swap
 * - SCOREP_RMA_ATOMIC_TYPE_SWAP swap
 * - SCOREP_RMA_ATOMIC_TYPE_FETCH_AND_ADD fetch and add
 * - SCOREP_RMA_ATOMIC_TYPE_FETCH_AND_INCREMENT fetch and increment
 * - SCOREP_RMA_ATOMIC_TYPE_ADD add
 * - SCOREP_INVALID_RMA_ATOMIC_TYPE for internal use only
 */


#define SCOREP_RMA_ATOMIC_TYPES \
    SCOREP_RMA_ATOMIC_TYPE( ACCUMULATE, accumulate,      "accumulate" )   \
    SCOREP_RMA_ATOMIC_TYPE( INCREMENT, increment,       "increment" )     \
    SCOREP_RMA_ATOMIC_TYPE( TEST_AND_SET, test_and_set,    "test and set" ) \
    SCOREP_RMA_ATOMIC_TYPE( COMPARE_AND_SWAP, compare_and_swap, "compare and swap" ) \
    SCOREP_RMA_ATOMIC_TYPE( SWAP, swap, "swap" ) \
    SCOREP_RMA_ATOMIC_TYPE( FETCH_AND_ADD, fetch_and_add, "fetch and add" ) \
    SCOREP_RMA_ATOMIC_TYPE( FETCH_AND_INCREMENT, fetch_and_increment, "fetch and increment" ) \
    SCOREP_RMA_ATOMIC_TYPE( ADD, add, "add" ) \
    SCOREP_RMA_ATOMIC_TYPE( FETCH_AND_ACCUMULATE, fetch_and_op, "fetch and accumulate with user-specified operator" )

typedef enum SCOREP_RmaAtomicType
{
#define SCOREP_RMA_ATOMIC_TYPE( upper, lower, name ) SCOREP_RMA_ATOMIC_TYPE_ ## upper,
    SCOREP_RMA_ATOMIC_TYPES
    #undef SCOREP_RMA_ATOMIC_TYPE

    SCOREP_INVALID_RMA_ATOMIC_TYPE
} SCOREP_RmaAtomicType;


/**
 * \enum SCOREP_SamplingSetClass
 * \brief Class of locations which recorded a sampling set.
 */
typedef enum SCOREP_SamplingSetClass
{
    SCOREP_SAMPLING_SET_ABSTRACT, /**< The sampling set is more complicated, e.g., refers to a number of locations. */
    SCOREP_SAMPLING_SET_CPU,      /**< The sampling set refers to a CPU. */
    SCOREP_SAMPLING_SET_GPU       /**< The sampling set refers to a GPU. */
} SCOREP_SamplingSetClass;

/**
 * \enum SCOREP_MetricScope
 * \brief Types to be used in defining the scope of a scoped sampling set.
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
 * \enum SCOREP_MetricOccurrence
 * \brief Types to be used in defining the occurrence of a sampling set.
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

#define SCOREP_IO_PARADIGMS \
    SCOREP_IO_PARADIGM( POSIX,           posix,           "POSIX" ) \
    SCOREP_IO_PARADIGM( ISOC,            isoc,            "ISOC" ) \
    SCOREP_IO_PARADIGM( MPI,             mpi,             "MPI-IO" )

/**
 * I/O paradigm types.
 */
typedef enum SCOREP_IoParadigmType
{
#define SCOREP_IO_PARADIGM( upper, lower, id_name ) SCOREP_IO_PARADIGM_ ## upper,
    SCOREP_IO_PARADIGMS
    #undef SCOREP_IO_PARADIGM
    SCOREP_INVALID_IO_PARADIGM_TYPE
} SCOREP_IoParadigmType;

/* The compile time static number of known parallel paradigms */
#define SCOREP_NUM_IO_PARADIGMS SCOREP_INVALID_IO_PARADIGM_TYPE

/**
 * Access mode of an I/O handle in subsequent I/O operations.
 */
typedef enum SCOREP_IoAccessMode
{
    /** @brief Unspecified access mode. */
    SCOREP_IO_ACCESS_MODE_NONE = 0,
    /** @brief Read-only access. */
    SCOREP_IO_ACCESS_MODE_READ_ONLY,
    /** @brief Write-only access. */
    SCOREP_IO_ACCESS_MODE_WRITE_ONLY,
    /** @brief Read-write access. */
    SCOREP_IO_ACCESS_MODE_READ_WRITE,
    /** @brief Execute-only access. */
    SCOREP_IO_ACCESS_MODE_EXECUTE_ONLY,
    /** @brief Search-only access. */
    SCOREP_IO_ACCESS_MODE_SEARCH_ONLY,
} SCOREP_IoAccessMode;

/**
 * Additional flags specified while creation of an I/O handle.
 */
typedef enum SCOREP_IoCreationFlag
{
    /** @brief No flag is set. */
    SCOREP_IO_CREATION_FLAG_NONE                    = 0,
    /** @brief If the file does not exist, it will be created. */
    SCOREP_IO_CREATION_FLAG_CREATE                  = ( 1 << 0 ),
    /** @brief Truncate file to length 0 if possible. */
    SCOREP_IO_CREATION_FLAG_TRUNCATE                = ( 1 << 1 ),
    /** @brief Open operation will fail if pathname is not a directory. */
    SCOREP_IO_CREATION_FLAG_DIRECTORY               = ( 1 << 2 ),
    /** @brief Ensure that this call creates the file. */
    SCOREP_IO_CREATION_FLAG_EXCLUSIVE               = ( 1 << 3 ),
    /** @brief File is a terminal device and should not be promoted
     *  to a controlling terminal, if none existed before. */
    SCOREP_IO_CREATION_FLAG_NO_CONTROLLING_TERMINAL = ( 1 << 4 ),
    /** @brief If pathname is a symbolic link, then the open operation will fail. */
    SCOREP_IO_CREATION_FLAG_NO_FOLLOW               = ( 1 << 5 ),
    /** @brief File is only a location in the filesystem tree and
     *  is not suitable for reading and writing */
    SCOREP_IO_CREATION_FLAG_PATH                    = ( 1 << 6 ),
    /** @brief Create an unnamed temporary file. */
    SCOREP_IO_CREATION_FLAG_TEMPORARY_FILE          = ( 1 << 7 ),
    /** @brief Ensure that the file size can be represented by a 64-bit datatype. */
    SCOREP_IO_CREATION_FLAG_LARGEFILE               = ( 1 << 8 ),
    /** @brief Gives the advice that no reposition will happen on this I/O handle.
     *  E.g., no seek operation or similar, only sequential read or
     *  write operations. */
    SCOREP_IO_CREATION_FLAG_NO_SEEK                 = ( 1 << 9 ),
    /** @brief Gives the advice that this will be the only @emph{active}
     *  @eref{IoHandle} of the @eref{IoParadigmType} which will operate on
     *  the referenced @eref{IoFile} at any time. E.g., no other
     *  @eref{IoHandle} of the same @eref{IoParadigmType} and the same
     *  @eref{IoFile} will be @emph{active}. */
    SCOREP_IO_CREATION_FLAG_UNIQUE                  = ( 1 << 10 )
} SCOREP_IoCreationFlag;

/**
 * Additional status flags of an I/O handle. Status flags can be set
 * when an I/O handle is created and changed during its lifetime.
 */
typedef enum SCOREP_IoStatusFlag
{
    /** @brief  No flag is set. */
    SCOREP_IO_STATUS_FLAG_NONE            = 0,
    /** @brief Enable close-on-exec flag. */
    SCOREP_IO_STATUS_FLAG_CLOSE_ON_EXEC   = ( 1 << 0 ),
    /** @brief Open file in append mode which means I/O write operations
     *  are automatically performed at the end of the file.  */
    SCOREP_IO_STATUS_FLAG_APPEND          = ( 1 << 1 ),
    /** @brief I/O operations (including the creation) will fail if they
     *  would block the issuing process. */
    SCOREP_IO_STATUS_FLAG_NON_BLOCKING    = ( 1 << 2 ),
    /** @brief  Enable signal-driven I/O. */
    SCOREP_IO_STATUS_FLAG_ASYNC           = ( 1 << 3 ),
    /** @brief Write operations on the file will complete according to the
     *  requirements of synchronized I/O file integrity completion
     *  (data and metadata) */
    SCOREP_IO_STATUS_FLAG_SYNC            = ( 1 << 4 ),
    /** @brief Write operations on the file will complete according to the
     *  requirements of synchronized I/O data integrity completion. */
    SCOREP_IO_STATUS_FLAG_DATA_SYNC       = ( 1 << 5 ),
    /** @brief Instruct I/O operations to reduce caching effects,
     *  e.g., direct file I/O. */
    SCOREP_IO_STATUS_FLAG_AVOID_CACHING   = ( 1 << 6 ),
    /** @brief Read access to a file won't update its last access time. */
    SCOREP_IO_STATUS_FLAG_NO_ACCESS_TIME  = ( 1 << 7 ),
    /** @brief Delete the file when closing the @eref{IoHandle}. */
    SCOREP_IO_STATUS_FLAG_DELETE_ON_CLOSE = ( 1 << 8 )
} SCOREP_IoStatusFlag;

/**
 * Options for repositioning a file offset with file seek operations.
 */
typedef enum SCOREP_IoSeekOption
{
    /** The offset is set to offset bytes. */
    SCOREP_IO_SEEK_FROM_START = 0,
    /** The offset is set to its current location plus offset bytes. */
    SCOREP_IO_SEEK_FROM_CURRENT,
    /** The offset is set to the size of the file plus offset bytes. */
    SCOREP_IO_SEEK_FROM_END,
    /** The offset is set to the next location in the file greater than or equal to offset containing data. */
    SCOREP_IO_SEEK_DATA,
    /** The offset is set to the next hole in the file greater than or equal to offset. */
    SCOREP_IO_SEEK_HOLE,

    SCOREP_IO_SEEK_INVALID /**< NON-ABI, for internal use only. */
} SCOREP_IoSeekOption;

/**
 * Mode of an I/O operation.
 */
typedef enum SCOREP_IoOperationMode
{
    /** @brief Read operation. */
    SCOREP_IO_OPERATION_MODE_READ = 0,
    /** @brief Write operation. */
    SCOREP_IO_OPERATION_MODE_WRITE,
    /** @brief Synchronization/flush operation (request and completion). */
    SCOREP_IO_OPERATION_MODE_FLUSH
} SCOREP_IoOperationMode;

/**
 * Flags for I/O operations to indicate specific semantics of the operation.
 * Per default any I/O operation is assumed as blocking and non-collective.
 * You can set appropriate flag bits to indicate a deviation from this
 * default semantic.
 */
typedef enum SCOREP_IoOperationFlag
{
    /** @brief No special semantics. */
    SCOREP_IO_OPERATION_FLAG_NONE           = 0,
    /** @brief The I/O operation was performed in a blocking mode (default). */
    SCOREP_IO_OPERATION_FLAG_BLOCKING       = 0,
    /** @brief The I/O operation was performed in a non-blocking mode. */
    SCOREP_IO_OPERATION_FLAG_NON_BLOCKING   = ( 1 << 0 ),
    /** @brief The I/O operation was performed collectively over
     *  the communicator of the referenced @eref{IoHandle} handle. */
    SCOREP_IO_OPERATION_FLAG_COLLECTIVE     = ( 1 << 1 ),
    /** @brief The I/O operation was performed in a non-collective mode. (default) */
    SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE = 0
} SCOREP_IoOperationFlag;


/**
 * \enum SCOREP_Ipc_Datatype
 * \brief specifies an inter process communication data types
 *
 * Types:
 * - SCOREP_IPC_BYTE byte
 * - SCOREP_IPC_CHAR char
 * - SCOREP_IPC_UNSIGNED_CHAR unsigned char
 * - SCOREP_IPC_INT  int
 * - SCOREP_IPC_UNSIGNED unsigned int
 * - SCOREP_IPC_INT32_T int32_t
 * - SCOREP_IPC_UINT32_T uint32_t
 * - SCOREP_IPC_INT64_T int64_t
 * - SCOREP_IPC_UINT64_T uint64_t
 * - SCOREP_IPC_DOUBLE double
 */

#define SCOREP_IPC_DATATYPES \
    SCOREP_IPC_DATATYPE( BYTE ) \
    SCOREP_IPC_DATATYPE( CHAR ) \
    SCOREP_IPC_DATATYPE( UNSIGNED_CHAR ) \
    SCOREP_IPC_DATATYPE( INT ) \
    SCOREP_IPC_DATATYPE( UNSIGNED ) \
    SCOREP_IPC_DATATYPE( INT32_T ) \
    SCOREP_IPC_DATATYPE( UINT32_T ) \
    SCOREP_IPC_DATATYPE( INT64_T ) \
    SCOREP_IPC_DATATYPE( UINT64_T ) \
    SCOREP_IPC_DATATYPE( DOUBLE )

typedef enum SCOREP_Ipc_Datatype
{
#define SCOREP_IPC_DATATYPE( datatype ) \
    SCOREP_IPC_ ## datatype,
    SCOREP_IPC_DATATYPES
#undef SCOREP_IPC_DATATYPE
    SCOREP_IPC_NUMBER_OF_DATATYPES
} SCOREP_Ipc_Datatype;

/**
 * \enum SCOREP_Ipc_Operation
 * \brief specifies an inter process communication operation for reduce function
 *
 * Types:
 * - SCOREP_IPC_BAND binary and
 * - SCOREP_IPC_BOR binary or
 * - SCOREP_IPC_MIN minimum
 * - SCOREP_IPC_MAX maximum
 * - SCOREP_IPC_SUM sum
 */

#define SCOREP_IPC_OPERATIONS \
    SCOREP_IPC_OPERATION( BAND ) \
    SCOREP_IPC_OPERATION( BOR ) \
    SCOREP_IPC_OPERATION( MIN ) \
    SCOREP_IPC_OPERATION( MAX ) \
    SCOREP_IPC_OPERATION( SUM )

typedef enum SCOREP_Ipc_Operation
{
#define SCOREP_IPC_OPERATION( op ) \
    SCOREP_IPC_ ## op,
    SCOREP_IPC_OPERATIONS
#undef SCOREP_IPC_OPERATION
    SCOREP_IPC_NUMBER_OF_OPERATIONS
} SCOREP_Ipc_Operation;


/**
 * \enum SCOREP_SubstratesRequirementFlags
 * \brief Substrates can require features by returning true when asked; they
 * return false when they don't care about or are unaware of the feature.
 * Substrates are asked about a feature via the GET_REQUIREMENT MGMT substrate
 * callback (internal substrates) or via
 * SCOREP_SubstratePluginInfo.get_requirement() (external substrates).
 * The feature itself decides how to deal with the requirements by evaluating
 * all substrate's answers about a feature using
 * SCOREP_SUBSTRATE_REQUIREMENT_CHECK_ANY
 */
typedef enum SCOREP_Substrates_RequirementFlag
{
    SCOREP_SUBSTRATES_REQUIREMENT_CREATE_EXPERIMENT_DIRECTORY,       /**< Return true on this feature if your substrate needs the experiment directory. There will be no directory nor configuration log file if no substrate requests it. */
    SCOREP_SUBSTRATES_REQUIREMENT_PREVENT_ASYNC_METRICS,             /**< Return true on this feature if your substrate can't handle asynchronous metrics. No asynchronous metrics will be recorded if at least on substrate prevents it. */
    SCOREP_SUBSTRATES_REQUIREMENT_PREVENT_PER_HOST_AND_ONCE_METRICS, /**< Return true on this feature if your substrate can't handle PER_HOST or ONCE metrics. No PER_HOST or ONCE metrics will be recorded if at least on substrate prevents it. */

    SCOREP_SUBSTRATES_NUM_REQUIREMENTS                               /**< Non-ABI used internally  */
} SCOREP_Substrates_RequirementFlag;

/** @} */

#endif /* SCOREP_PUBLICTYPES_H */
