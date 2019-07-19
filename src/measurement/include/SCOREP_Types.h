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
 * Copyright (c) 2009-2014, 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
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
    /** @brief The measurement was not initialized yet, or is currently initializing. */
    SCOREP_MEASUREMENT_PHASE_PRE    = -1,

    /** @brief The measurement is initialized and not finalized yet. */
    SCOREP_MEASUREMENT_PHASE_WITHIN = 0,

    /** @brief The measurement is finalizing or already finalized. */
    SCOREP_MEASUREMENT_PHASE_POST   = 1
} SCOREP_MeasurementPhase;


/** Activation and deactivation phase for CPU locations
 *
 * (1) SCOREP_CPU_LOCATION_PHASE_MGMT::
 *     A general notification about the intention to activate the location.
 *     No events are allowed, just maintenance stuff should be done with
 *     CPU the location.
 * (2) SCOREP_CPU_LOCATION_PHASE_EVENTS::
 *     The location was successfully activated. Events can now be triggered.
 *
 * Additionally, a CPU location can be in a PAUSE phase where no events are
 * allowed.
 *     SCOREP_CPU_LOCATION_PHASE_PAUSE::
 *     Location goes into an PAUSE phase were no events are allowed.
 */
typedef enum SCOREP_CPULocationPhase
{
    SCOREP_CPU_LOCATION_PHASE_MGMT,
    SCOREP_CPU_LOCATION_PHASE_EVENTS,
    SCOREP_CPU_LOCATION_PHASE_PAUSE
} SCOREP_CPULocationPhase;


/**
 * Symbolic constant representing an unknown number of transferred bytes in I/O operation.
 */
#define SCOREP_IO_UNKOWN_TRANSFER_SIZE UINT64_MAX

/**
 * Symbolic constant representing an invalid or unknown I/O file handle
 * definition.
 */
#define SCOREP_INVALID_IO_HANDLE SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown I/O file definition.
 */
#define SCOREP_INVALID_IO_FILE SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown I/O file property
 */
#define SCOREP_INVALID_IO_FILE_PROPERTY SCOREP_MOVABLE_NULL

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
 * Symbolic constant representing an invalid or unknown interim communicator
 * definition.
 */
#define SCOREP_INVALID_INTERIM_COMMUNICATOR SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown communicator
 * definition.
 */
#define SCOREP_INVALID_COMMUNICATOR SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown RMA window definition.
 */
#define SCOREP_INVALID_RMA_WINDOW SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI cartesian topology
 * definition.
 */
#define SCOREP_INVALID_CART_TOPOLOGY SCOREP_MOVABLE_NULL


/**
 * Symbolic constant representing an invalid or unknown MPI cartesian coordinates
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
 * Symbolic constant denoting that the fork sequence count
 * is not in a valid state.
 */
#define SCOREP_THREAD_INVALID_SEQUENCE_COUNT UINT32_MAX

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
     *  these additional definitions don't reflect the actual paradigm.
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
 * Types to be used in defining a location group (SCOREP_Definitions_NewLocationGroup()).
 *
 */
#define SCOREP_LOCATION_GROUP_TYPES \
    SCOREP_LOCATION_GROUP_TYPE( PROCESS, "process" )

typedef enum SCOREP_LocationGroupType
{
    #define SCOREP_LOCATION_GROUP_TYPE( NAME, name_string ) \
    SCOREP_LOCATION_GROUP_TYPE_ ## NAME,

    SCOREP_LOCATION_GROUP_TYPES

    #undef SCOREP_LOCATION_GROUP_TYPE

    SCOREP_INVALID_LOCATION_GROUP_TYPE /**< For internal use only. */
} SCOREP_LocationGroupType;


/**
 * Types to be used in defining a group (SCOREP_Definitions_NewGroup()).
 *
 */
typedef enum SCOREP_GroupType
{
    SCOREP_GROUP_UNKNOWN                     = 0,
    SCOREP_GROUP_LOCATIONS                   = 1,
    SCOREP_GROUP_REGIONS                     = 2,
    SCOREP_GROUP_METRIC                      = 3,

    SCOREP_GROUP_MPI_LOCATIONS               = 4,
    SCOREP_GROUP_MPI_GROUP                   = 5,
    SCOREP_GROUP_MPI_SELF                    = 6,

    SCOREP_GROUP_OPENMP_LOCATIONS            = 7,
    SCOREP_GROUP_OPENMP_THREAD_TEAM          = 8,

    SCOREP_GROUP_CUDA_LOCATIONS              = 9,
    SCOREP_GROUP_CUDA_GROUP                  = 10,

    SCOREP_GROUP_SHMEM_LOCATIONS             = 11,
    SCOREP_GROUP_SHMEM_GROUP                 = 12,
    SCOREP_GROUP_SHMEM_SELF                  = 13,

    SCOREP_GROUP_PTHREAD_LOCATIONS           = 14,
    SCOREP_GROUP_PTHREAD_THREAD_TEAM         = 15,

    SCOREP_GROUP_OPENCL_LOCATIONS            = 16,
    SCOREP_GROUP_OPENCL_GROUP                = 17,

    SCOREP_GROUP_TOPOLOGY_HARDWARE_LOCATIONS = 18,
    SCOREP_GROUP_TOPOLOGY_HARDWARE_GROUP     = 19,

    SCOREP_GROUP_TOPOLOGY_PROCESS_LOCATIONS  = 20,
    SCOREP_GROUP_TOPOLOGY_PROCESS_GROUP      = 21,

    SCOREP_GROUP_TOPOLOGY_USER_LOCATIONS     = 22,
    SCOREP_GROUP_TOPOLOGY_USER_GROUP         = 23,

    SCOREP_INVALID_GROUP_TYPE /**< For internal use only. */
} SCOREP_GroupType;

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
     *
     * SCOREP_ConfigVariable::variableContext if it contains a default file name
     * the respective file will be copied to the measurement archive by use of
     * SCOREP_ConfigCopyFile(). If it is NULL it will be skipped.
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
    SCOREP_ATTRIBUTE_TYPE_RMA_WINDOW,
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
 * Types to specify the used file operation mode in calls to @a SCOREP_FileOperationEnd.
 */
typedef enum SCOREP_FileOperationType
{
    SCOREP_FILE_OPERATION_OPEN,
    SCOREP_FILE_OPERATION_CLOSE,
    SCOREP_FILE_OPERATION_READ,
    SCOREP_FILE_OPERATION_WRITE
} SCOREP_FileOperationType;

/**
 * Possible cartesian topology types differentiated by their source:
 *  - MPI topologies created by each MPI_Cart_create
    - Process X Threads 2 dimensional topology of used processes and threads
 *  - Platform/Hardware topology if available for the current system
 *  - Topologies created through user instrumentation.
 */
typedef enum
{
    SCOREP_TOPOLOGIES_MPI = 0,
    SCOREP_TOPOLOGIES_PROCESS,
    SCOREP_TOPOLOGIES_PLATFORM,
    SCOREP_TOPOLOGIES_USER,
    SCOREP_TOPOLOGIES_NONE,
} SCOREP_Topology_Type;

/**
 * List of I/O paradigm classes.
 */
typedef enum SCOREP_IoParadigmClass
{
    /** Serial.
     */
    SCOREP_IO_PARADIGM_CLASS_SERIAL   = 0,
    /** Parallel.
     */
    SCOREP_IO_PARADIGM_CLASS_PARALLEL = 1
} SCOREP_IoParadigmClass;

/**
 * List of I/O paradigm flags.
 */
typedef enum SCOREP_IoParadigmFlag
{
    /** @brief No flags set.
     */
    SCOREP_IO_PARADIGM_FLAG_NONE = 0,
    /** @brief This I/O paradigm denotes an interface to the operating system. Thus,
     *  no further I/O paradigms below exists.
     */
    SCOREP_IO_PARADIGM_FLAG_OS   = ( 1 << 0 )
} SCOREP_IoParadigmFlag;

/**
 * List of I/O paradigm properties.
 */
typedef enum SCOREP_IoParadigmProperty
{
    /** @brief Version of the implementation.
     *
     *  Type: String
     */
    SCOREP_IO_PARADIGM_PROPERTY_VERSION = 0,

    SCOREP_INVALID_IO_PARADIGM_PROPERTY
} SCOREP_IoParadigmProperty;

/**
 * List of I/O handle flags.
 */
typedef enum SCOREP_IoHandleFlag
{
    /** @brief No flags set.
     */
    SCOREP_IO_HANDLE_FLAG_NONE        = 0,
    /** @brief This I/O handle was already created by the system.
     */
    SCOREP_IO_HANDLE_FLAG_PRE_CREATED = ( 1 << 0 ),
    /** @brief This I/O handle is a proxy to a group of I/O paradigm specific handles.
     */
    SCOREP_IO_HANDLE_FLAG_ALL_PROXY   = ( 1 << 1 )
} SCOREP_IoHandleFlag;

/*@}*/


#endif /* SCOREP_TYPES_H */
