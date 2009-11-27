/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SILC_TYPES_H
#define SILC_TYPES_H


/**
 * @file        SILC_Types.h
 * @maintainer  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief Types needed by SILC_* API functions.
 *
 */


#include <stdint.h>


/**
 * @defgroup SILC_Types SILC Types
 *
 * The adapters need to use special types to communicate with the measurement
 * layer functions. We can categorize them as follows:
 *
 * - Opaque handles returned by the @ref SILC_Definitions definition
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
 * Type of a opaque handle to a source file definition.
 * @see SILC_DefineSourceFile()
 */
typedef uint32_t SILC_SourceFileHandle;


/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SILC_INVALID_SOURCE_FILE UINT32_MAX


/**
 * Type used in specifying line numbers.
 * @see SILC_DefineRegion()
 */
typedef uint32_t SILC_LineNo;


/**
 * Symbolic constant representing an invalid or unknown line number.
 * @see SILC_DefineRegion()
 */
#define SILC_INVALID_LINE_NO 0


/**
 * Type of a opaque handle to a counter group definition.
 * @see SILC_DefineCounterGroup()
 */
typedef uint32_t SILC_CounterGroupHandle;


/**
 * Symbolic constant representing an invalid or unknown counter group
 * definition.
 */
#define SILC_INVALID_COUNTER_GROUP UINT32_MAX


/**
 * Type of a opaque handle to a counter definition.
 * @see SILC_DefineCounter()
 */
typedef uint32_t SILC_CounterHandle;


/**
 * Symbolic constant representing an invalid or unknown counter definition.
 */
#define SILC_INVALID_COUNTER UINT32_MAX


/**
 * Type of a opaque handle to a I/O file group definition.
 * @see SILC_DefineIOFileGroup()
 */
typedef uint32_t SILC_IOFileGroupHandle;


/**
 * Symbolic constant representing an invalid or unknown I/O file group
 * definition.
 */
#define SILC_INVALID_IOFILE_GROUP UINT32_MAX


/**
 * Type of a opaque handle to a I/O file definition.
 * @see SILC_DefineIOFile()
 */
typedef uint32_t SILC_IOFileHandle;


/**
 * Symbolic constant representing an invalid or unknown I/O file definition.
 */
#define SILC_INVALID_IOFILE UINT32_MAX


/**
 * Type of a opaque handle to a marker group definition.
 * @see SILC_DefineMarkerGroup()
 */
typedef uint32_t SILC_MarkerGroupHandle;


/**
 * Symbolic constant representing an invalid or unknown marker group
 * definition.
 */
#define SILC_INVALID_MARKER_GROUP UINT32_MAX


/**
 * Type of a opaque handle to a marker definition.
 * @see SILC_DefineMarker()
 */
typedef uint32_t SILC_MarkerHandle;


/**
 * Symbolic constant representing an invalid or unknown marker definition.
 */
#define SILC_INVALID_MARKER UINT32_MAX


/**
 * Type of a opaque handle to a region definition.
 * @see SILC_DefineRegion()
 */
typedef uint32_t SILC_RegionHandle;


/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SILC_INVALID_REGION UINT32_MAX


/**
 * Type of a opaque handle to a MPI communicator definition.
 * @see SILC_DefineMPICommunicator()
 */
typedef uint32_t SILC_MPICommunicatorHandle;


/**
 * Symbolic constant representing an invalid or unknown MPI communicator
 * definition.
 */
#define SILC_INVALID_MPI_COMMUNICATOR UINT32_MAX


/**
 * Type of a opaque handle to a MPI window definition.
 * @see SILC_DefineMPIWindow()
 */
typedef uint32_t SILC_MPIWindowHandle;


/**
 * Symbolic constant representing an invalid or unknown MPI window definition.
 */
#define SILC_INVALID_MPI_WINDOW UINT32_MAX


/**
 * Type of a opaque handle to a MPI cartesion topology definition.
 * @see SILC_DefineMPICartesianTopology()
 */
typedef uint32_t SILC_MPICartTopolHandle;


/**
 * Symbolic constant representing an invalid or unknown MPI cartesion topology
 * definition.
 */
#define SILC_INVALID_CART_TOPOLOGY UINT32_MAX


/**
 * Type of a opaque handle to a parameter definition.
 * @see SILC_DefineParameter()
 */
typedef uint32_t SILC_ParameterHandle;


/**
 * Symbolic constant representing an invalid or unknown parameter definition.
 */
#define SILC_INVALID_PARAMETER UINT32_MAX


/**
 * Symbolic constant representing an invalid or unknown rank.
 * @see SILC_MpiCollective()
 */
#define SILC_INVALID_ROOT_RANK INT32_MAX


/**
 * Types to be used in defining a region (SILC_DefineRegion()). In order to
 * track the origin of a region definition, the adapter needs to provide @e
 * his type.
 *
 */
typedef enum SILC_AdapterType
{
    SILC_ADAPTER_USER,
    SILC_ADAPTER_COMPILER,
    SILC_ADAPTER_MPI,
    SILC_ADAPTER_POMP,
    SILC_ADAPTER_PTHREAD,

    SILC_INVALID_ADAPTER_TYPE /**< For internal use only. */
} SILC_AdapterType;


/**
 * Types to be used in defining a region (SILC_DefineRegion()). These types
 * are currently not used inside the measurement system. This @e may change in
 * future if we are going to implement phases/dynamic regions etc. inside the
 * measurement system as opposed to inside the adapters or as a postprocessing
 * step. The names should be self explanatory; most of them are already used
 * (whith a different prefix) in VampiTrace and Scalasca.
 *
 * @todo remove SILC_REGION_UNKNOWN
 */
typedef enum SILC_RegionType
{
    SILC_REGION_UNKNOWN = 0,
    SILC_REGION_FUNCTION,
    SILC_REGION_LOOP,
    SILC_REGION_USER,
    SILC_REGION_PHASE,
    SILC_REGION_DYNAMIC,

    SILC_REGION_DYNAMIC_PHASE,
    SILC_REGION_DYNAMIC_LOOP,
    SILC_REGION_DYNAMIC_FUNCTION,
    SILC_REGION_DYNAMIC_LOOP_PHASE,

    SILC_REGION_MPI_COLL_BARRIER,
    SILC_REGION_MPI_COLL_ONE2ALL,
    SILC_REGION_MPI_COLL_ALL2ONE,
    SILC_REGION_MPI_COLL_ALL2ALL,
    SILC_REGION_MPI_COLL_OTHER,

    SILC_REGION_OMP_PARALLEL,
    SILC_REGION_OMP_LOOP,
    SILC_REGION_OMP_SECTIONS,
    SILC_REGION_OMP_SECTION,
    SILC_REGION_OMP_WORKSHARE,
    SILC_REGION_OMP_SINGLE,
    SILC_REGION_OMP_MASTER,
    SILC_REGION_OMP_CRITICAL,
    SILC_REGION_OMP_ATOMIC,
    SILC_REGION_OMP_BARRIER,
    SILC_REGION_OMP_IMPLICIT_BARRIER,
    SILC_REGION_OMP_FLUSH,
    SILC_REGION_OMP_CRITICAL_SBLOCK, /**< @todo what is SBLOCK? */
    SILC_REGION_OMP_SINGLE_SBLOCK,

    SILC_INVALID_REGION_TYPE /**< For internal use only. */
} SILC_RegionType;


/**
 * Types to be used in defining a counter (SILC_DefineCounter()).
 *
 */
typedef enum SILC_CounterType
{
    SILC_COUNTER_INT64,
    SILC_COUNTER_DOUBLE,

    SILC_INVALID_COUNTER_TYPE /**< For internal use only. */
} SILC_CounterType;


/**
 * Types to be used in defining a parameter for parameter based profiling
 * (SILC_DefineParameter()).
 *
 */
typedef enum SILC_ParameterType
{
    SILC_PARAMETER_INT64,
    SILC_PARAMETER_DOUBLE,
    SILC_PARAMETER_STRING,

    SILC_INVALID_PARAMETER_TYPE /**< For internal use only. */
} SILC_ParameterType;


/**
 * The type of a SILC_ConfigVariable.
 *
 */
typedef enum SILC_ConfigType
{
    /**
     * A string value with variable expension (Ie. ${HOME})
     * (path normalization?)
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type char*.
     */
    SILC_CONFIG_TYPE_PATH,

    /**
     * A string value.
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type char*.
     */
    SILC_CONFIG_TYPE_STRING,

    /**
     * A boolean value.
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type bool.
     */
    SILC_CONFIG_TYPE_BOOL,

    /**
     * A numerical value
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     */
    SILC_CONFIG_TYPE_NUMBER,

    /**
     * A numerical value with size suffixes (Ie. Kb, Gb, KiBi, ...)
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     */
    SILC_CONFIG_TYPE_SIZE,

    /**
     * A symbolic set.
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type char**.
     *
     * SILC_ConfigVariable::variableContext should point to a NULL terminated
     * string list with valid set members
     */
    SILC_CONFIG_TYPE_SET,

    /**
     * A symbolic set, represented as a bitmask.
     *
     * @note At most 64 set members are supported.
     *
     * SILC_ConfigVariable::variableReference should point to a variable of
     * type uint64_t.
     *
     * SILC_ConfigVariable::variableContext should point to an array of type
     * SILC_ConfigType_SetEntry with valid set members and there values.
     * Terminate the array with an entry { NULL, 0 }.
     */
    SILC_CONFIG_TYPE_BITSET,

    SILC_INVALID_CONFIG_TYPE /**< For internal use only. */
} SILC_ConfigType;


/**
 * Config variable object. Partially initialized objects of this type need to
 * be provided by the adapters if they need to access config variables.
 *
 * @see SILC_ConfigRegister()
 *
 */
typedef struct SILC_ConfigVariable
{
    const char*     nameSpace;     /** NULL for @e root/global namespace */
    const char*     name;
    SILC_ConfigType type;
    void*           variableReference;
    void*           variableContext;
    const char*     defaultValue;
    const char*     shortHelp;
    const char*     longHelp;
} SILC_ConfigVariable;


/**
 * The type for SILC_CONFIG_TYPE_BITSET entries.
 *
 */
typedef struct SILC_ConfigType_SetEntry
{
    const char* name;
    uint64_t    value;
} SILC_ConfigType_SetEntry;


/*@}*/


#endif /* SILC_TYPES_H */
