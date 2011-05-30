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


#include <scorep_utility/SCOREP_Allocator.h>


#include <SCOREP_PublicTypes.h>


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


// scorep_meets_tau_begin
/**
 * Type used in specifying line numbers.
 * @see SCOREP_DefineRegion()
 */
typedef uint32_t SCOREP_LineNo;
// scorep_meets_tau_end

// scorep_meets_tau_begin
/**
 * Symbolic constant representing an invalid or unknown line number.
 * @see SCOREP_DefineRegion()
 */
#define SCOREP_INVALID_LINE_NO 0
// scorep_meets_tau_end


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



// scorep_meets_tau_begin
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

    SCOREP_INVALID_ADAPTER_TYPE /**< For internal use only. */
} SCOREP_AdapterType;
// scorep_meets_tau_end


/**
 * Types to be used in defining a counter (SCOREP_DefineCounter()).
 *
 */
typedef enum SCOREP_CounterType
{
    SCOREP_COUNTER_INT64,
    SCOREP_COUNTER_DOUBLE,

    SCOREP_INVALID_COUNTER_TYPE /**< For internal use only. */
} SCOREP_CounterType;


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


// scorep_meets_tau_begin
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
    SCOREP_REGION_PHASE,
    SCOREP_REGION_DYNAMIC,

    SCOREP_REGION_DYNAMIC_PHASE,
    SCOREP_REGION_DYNAMIC_LOOP,
    SCOREP_REGION_DYNAMIC_FUNCTION,
    SCOREP_REGION_DYNAMIC_LOOP_PHASE,

    SCOREP_REGION_MPI_COLL_BARRIER,
    SCOREP_REGION_MPI_COLL_ONE2ALL,
    SCOREP_REGION_MPI_COLL_ALL2ONE,
    SCOREP_REGION_MPI_COLL_ALL2ALL,
    SCOREP_REGION_MPI_COLL_OTHER,

    SCOREP_REGION_OMP_PARALLEL,
    SCOREP_REGION_OMP_LOOP,
    SCOREP_REGION_OMP_SECTIONS,
    SCOREP_REGION_OMP_SECTION,
    SCOREP_REGION_OMP_WORKSHARE,
    SCOREP_REGION_OMP_SINGLE,
    SCOREP_REGION_OMP_MASTER,
    SCOREP_REGION_OMP_CRITICAL,
    SCOREP_REGION_OMP_ATOMIC,
    SCOREP_REGION_OMP_BARRIER,
    SCOREP_REGION_OMP_IMPLICIT_BARRIER,
    SCOREP_REGION_OMP_FLUSH,
    SCOREP_REGION_OMP_CRITICAL_SBLOCK, /* type for the innerblock of criticals */
    SCOREP_REGION_OMP_SINGLE_SBLOCK,   /* type for the innerblock od singles */
    SCOREP_REGION_OMP_WRAPPER,

    SCOREP_INVALID_REGION_TYPE /**< For internal use only. */
} SCOREP_RegionType;
// scorep_meets_tau_end


/**
 * Types to be used in defining a parameter for parameter based profiling
 * (SCOREP_DefineParameter()).
 *
 */
typedef enum SCOREP_ParameterType
{
    SCOREP_PARAMETER_INT64,
    SCOREP_PARAMETER_DOUBLE,
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
     * Terminate the array with an entry { NULL, 0 }.
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


/*@}*/


#endif /* SCOREP_TYPES_H */
