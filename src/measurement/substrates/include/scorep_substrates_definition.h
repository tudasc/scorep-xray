/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015-2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SUBSTRATES_DEFINITION_H
#define SCOREP_SUBSTRATES_DEFINITION_H

/**
 * @file
 *
 * This file contains datatype definitions for SCOREP substrate handling
 *
 */
#include <scorep/SCOREP_SubstrateEvents.h>

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Macro defining substrate call construct
 */
#define SCOREP_CALL_SUBSTRATE( Event, EVENT, ARGS ) \
    do \
    { \
        SCOREP_Substrates_##Event##Cb * substrate_cb = ( SCOREP_Substrates_##Event##Cb* ) & ( scorep_substrates[ SCOREP_EVENT_##EVENT * scorep_substrates_max_substrates ] ); \
        while ( *substrate_cb ) \
        { \
            ( *substrate_cb )ARGS; \
            ++substrate_cb; \
        } \
    } while ( 0 )

/**
 * Macro initializing substrate callback
 */
#ifdef __GNUC__
#define SCOREP_ASSIGN_SUBSTRATE_CALLBACK( Event, EVENT, cb ) \
    [ SCOREP_EVENT_##EVENT ] = \
        __builtin_choose_expr( \
            __builtin_types_compatible_p( __typeof__( 0 ? ( cb ) : ( cb ) ), \
                                          SCOREP_Substrates_##Event##Cb ), \
            ( SCOREP_Substrates_Callback )( cb ), ( cb ) )
#else
#define SCOREP_ASSIGN_SUBSTRATE_CALLBACK( Event, EVENT, cb ) \
    [ SCOREP_EVENT_##EVENT ] = ( SCOREP_Substrates_Callback )( cb )
#endif

/**
 * Macro initializing substrate callback
 */
#ifdef __GNUC__
#define SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( Event, EVENT, cb ) \
    [ SCOREP_MGMT_##EVENT ] = \
        __builtin_choose_expr( \
            __builtin_types_compatible_p( __typeof__( 0 ? ( cb ) : ( cb ) ), \
                                          SCOREP_Substrates_##Event##Cb ), \
            ( SCOREP_Substrates_Callback )( cb ), ( cb ) )
#else
#define SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( Event, EVENT, cb ) \
    [ SCOREP_MGMT_##EVENT ] = ( SCOREP_Substrates_Callback )( cb )
#endif

/**
 * Macro defining substrate call construct
 */
#define SCOREP_CALL_SUBSTRATE_MGMT( Event, EVENT, ARGS ) \
    do \
    { \
        SCOREP_Substrates_##Event##Cb * substrate_cb = ( SCOREP_Substrates_##Event##Cb* ) & ( scorep_substrates_mgmt[ SCOREP_MGMT_##EVENT * scorep_substrates_max_mgmt_substrates ] ); \
        while ( *substrate_cb ) \
        { \
            ( *substrate_cb )ARGS; \
            ++substrate_cb; \
        } \
    } while ( 0 )

/**
 * Macro for checking any consumers of events
 * Usage:
 * if ( SCOREP_SUBSTRATE_EVENT_IS_CONSUMED( WRITE_METRIC_BEFORE_EVENT ) )
 * {
 *   function_that_writes_metrics();
 * }
 * Both macros will return NULL if there is no consumer for the event, otherwise it will be the pointer to the first substrate function
 */
#define SCOREP_SUBSTRATE_EVENT_IS_CONSUMED( EVENT ) \
    scorep_substrates[ SCOREP_EVENT_##EVENT * scorep_substrates_max_substrates ]

#define SCOREP_SUBSTRATE_MGMT_EVENT_IS_CONSUMED( EVENT ) \
    scorep_substrates_mgmt[ SCOREP_EVENT_##EVENT * scorep_substrates_max_mgmt_substrates ]

/**
 * Macro for checking requirements
 */
#define SCOREP_SUBSTRATE_REQUIREMENT_CHECK( REQUIREMENT, RESULT,  OP, U64_TMP ) \
    do \
    { \
        RESULT = 0; \
        SCOREP_Substrates_GetRequirementCb* substrate_cb = ( SCOREP_Substrates_GetRequirementCb* )&( scorep_substrates_mgmt[ SCOREP_MGMT_GET_REQUIREMENT * scorep_substrates_max_mgmt_substrates ] ); \
        while ( *substrate_cb ) \
        { \
            U64_TMP = ( ( *substrate_cb )( REQUIREMENT ) ); \
            RESULT OP U64_TMP; \
            ++substrate_cb; \
        } \
    } while ( 0 )

#define SCOREP_SUBSTRATE_REQUIREMENT_CHECK_BIT_ALL( REQUIREMENT, RESULT ) \
    do \
    { \
        uint64_t __requirement_tmp = 0; \
        SCOREP_SUBSTRATE_REQUIREMENT_CHECK( REQUIREMENT, RESULT, &=, __requirement_tmp ); \
    } while ( 0 )

#define SCOREP_SUBSTRATE_REQUIREMENT_CHECK_BIT_ANY( REQUIREMENT, RESULT ) \
    do \
    { \
        uint64_t __requirement_tmp = 0; \
        SCOREP_SUBSTRATE_REQUIREMENT_CHECK( REQUIREMENT, RESULT, |=, __requirement_tmp ); \
    } while ( 0 )

#define SCOREP_SUBSTRATE_REQUIREMENT_CHECK_BIT_NONE( REQUIREMENT, RESULT ) \
    do \
    { \
        SCOREP_SUBSTRATE_REQUIREMENT_CHECK_BIT_ANY( RESULT ); \
        ( REQUIREMENT != 0 ) ? 0 : 1; \
    } while ( 0 )

/**
 * Substrate events. List every event that is going to be used by the
 * substrate mechanism.
 */
typedef enum SCOREP_Substrates_MgmtType
{
    SCOREP_MGMT_INIT_SUBSTRATE = 0,
    SCOREP_MGMT_INITIALIZE_MPP,
    SCOREP_MGMT_ENSURE_GLOBAL_ID,
    SCOREP_MGMT_ON_LOCATION_CREATION,
    SCOREP_MGMT_ON_LOCATION_DELETION,
    SCOREP_MGMT_ON_CPU_LOCATION_ACTIVATION,
    SCOREP_MGMT_ON_CPU_LOCATION_DEACTIVATION,
    SCOREP_MGMT_CORE_TASK_CREATE,
    SCOREP_MGMT_CORE_TASK_COMPLETE,
    SCOREP_MGMT_WRITE_DATA,
    SCOREP_MGMT_FINALIZE_SUBSTRATE,
    SCOREP_MGMT_PRE_UNIFY_SUBSTRATE,
    SCOREP_MGMT_NEW_DEFINITION_HANDLE,
    SCOREP_MGMT_ADD_ATTRIBUTE,
    SCOREP_MGMT_LEAKED_MEMORY,
    SCOREP_MGMT_GET_REQUIREMENT,

    SCOREP_SUBSTRATES_NUM_MGMT_EVENTS
} SCOREP_Substrates_MgmtType;



/**
 * Number of the substrates for a particular event. Currently we support at
 * most three simultaneous substrates: profiling, tracing and invalidate property.
 * See SCOREP_Substrates_DisableRecording for the example where three simultaneous
 * substrates are used.
 */
#define SCOREP_SUBSTRATES_NUM_SUBSTRATES 3

/* Global array holding the currently active substrate callbacks per event.
 * See 'enum SCOREP_Substrates_EventType'. The last element for each event is always NULL
 * and serves as the while-loop's exit condition. Consider read-only except
 * in SCOREP_Substrates_Management.c. */

extern const SCOREP_Substrates_Callback* scorep_substrates;
extern int                               scorep_substrates_max_substrates;


extern SCOREP_Substrates_Callback* scorep_substrates_mgmt;
extern int                         scorep_substrates_max_mgmt_substrates;

// Per-event function pointer prototypes

/**
 * Initialize the substrate (SCOREP_MGMT_INIT_SUBSTRATE).
 *
 * At this point substrates should initialize their data structures and check whether all prerequisites are met.
 * @param substrateId a unique ID for the substrate that can be used to store and retrieve thread local data.
 * The substrateId must be stored and returned at SCOREP_MGMT_FINALIZE_SUBSTRATE if the respective function is implemented.
 *
 * More details in SCOREP_Subsystem.subsystem_init
 *
 */
typedef void ( * SCOREP_Substrates_InitSubstrateCb )(
    size_t substrateId );

/**
 * Finalize the substrate (SCOREP_MGMT_FINALIZE_SUBSTRATE).
 *
 * At this point substrates should free their resources.
 *
 * More details in SCOREP_Subsystem.subsystem_finalize
 *
 * @return the substrate id retrieved at SCOREP_MGMT_FINALIZE_SUBSTRATE
 */
typedef size_t ( * SCOREP_Substrates_FinalizeSubstrateCb )( void );

/**
 * Create a location (SCOREP_MGMT_ON_LOCATION_CREATION).
 *
 * Called whenever a new location is registered in Score-P.
 * Beginning with this call, the locations thread local storage can be used.
 *
 * More details in SCOREP_Subsystem.subsystem_init_location
 *
 * @param location the new location
 * @param parentLocation parent of the new location
 */
typedef void ( * SCOREP_Substrates_OnLocationCreationCb )(
    struct SCOREP_Location* location,
    struct SCOREP_Location* parentLocation );


/**
 * Delete a location (SCOREP_MGMT_ON_LOCATION_DELETION).
 *
 * Called whenever a new location is unregistered in Score-P (e.g., a thread is closed).
 * Thread local storage attached to this location should be deleted here.
 *
 * More details in SCOREP_Subsystem.subsystem_finalize_location
 *
 * @param location the closed location
 */
typedef void ( * SCOREP_Substrates_OnLocationDeletionCb )(
    struct SCOREP_Location* location );

/**
 * Activate a CPU location (SCOREP_MGMT_ON_CPU_LOCATION_ACTIVATION).
 *
 * Called whenever a CPU location is activated in Score-P.
 * This happens right after its creation (SCOREP_MGMT_ON_LOCATION_CREATION) and when a location becomes active again (e.g., a collapsed OpenMP thread is re-used).
 *
 * More details in SCOREP_Subsystem.subsystem_activate_cpu_location, only called for MGMT
 *
 * @param location the activated location
 * @param parentLocation parent of the activated location
 * @param forkSequenceCount specifies how many fork sequences (e.g., OpenMP parallel regions) have been encountered until now
 */
typedef void ( * SCOREP_Substrates_OnCpuLocationActivationCb )(
    struct SCOREP_Location* location,
    struct SCOREP_Location* parentLocation,
    uint32_t                forkSequenceCount );

/**
 * Deactivate a CPU location (SCOREP_MGMT_ON_CPU_LOCATION_DEACTIVATION).
 *
 * Called whenever a CPU location is deactivated in Score-P.
 * This happens right before its deletion (SCOREP_MGMT_ON_LOCATION_DELETION) and when a location becomes inactive (e.g., a OpenMP thread collapses).
 *
 * More details in SCOREP_Subsystem.subsystem_deactivate_cpu_location, only called for MGMT
 *
 * @param location the deactivated location
 * @param parentLocation parent of the activated location
 */
typedef void ( * SCOREP_Substrates_OnCpuLocationDeactivationCb )(
    struct SCOREP_Location* location,
    struct SCOREP_Location* parentLocation );

/**
 * Create a new task (SCOREP_MGMT_CORE_TASK_CREATE).
 *
 * Called whenever a new task is created in Score-P.
 * This happens for example with OpenMP tasks.
 *
 * When the task ends, SCOREP_MGMT_CORE_TASK_COMPLETE/SCOREP_Substrates_CoreTaskCompleteCb is triggered.
 *
 * Called at the end of scorep_task_create
 *
 * @param location the location where the task is created
 * @param taskHandle the new task
 */
typedef void ( * SCOREP_Substrates_CoreTaskCreateCb )(
    struct SCOREP_Location* location,
    SCOREP_TaskHandle       taskHandle );

/**
 * Complete an existing task (SCOREP_MGMT_CORE_TASK_COMPLETE).
 *
 * Called whenever a new task finishes.
 * This happens for example with OpenMP tasks.
 *
 * Called at the beginning of scorep_task_complete
 *
 * @param location the location that completes the task
 * @param taskHandle the completed task
 */
typedef void ( * SCOREP_Substrates_CoreTaskCompleteCb )(
    struct SCOREP_Location* locationData,
    SCOREP_TaskHandle       taskHandle );

/**
 * Tell substrate that the unification step is near (SCOREP_MGMT_PRE_UNIFY_SUBSTRATE).
 *
 * Called near the end of a measurement run, before the unification.
 *
 * See also SCOREP_Subsystem.subsystem_pre_unify
 */
typedef void ( * SCOREP_Substrates_PreUnifySubstrateCb )( void );

/**
 * Tell substrate that it should flush its data (SCOREP_MGMT_WRITE_DATA).
 *
 * Called after unification.
 */
typedef void ( * SCOREP_Substrates_WriteDataCb )( void );

/**
 * Called when the used MPP is initialized (SCOREP_MGMT_INITIALIZE_MPP).
 *
 * When this event is called and any time after, MPP depending functionality can be used.
 * If no MPP is used, this is called as well, right at the initialization.
 *
 * See also SCOREP_Subsystem.subsystem_init_mpp
 */
typedef void ( * SCOREP_Substrates_InitializeMppCb )( void );


/**
 * Called when a global ID for a location is available (SCOREP_MGMT_ENSURE_GLOBAL_ID).
 *
 * Called when a locations definitions are finalized
 *
 */
typedef void ( * SCOREP_Substrates_EnsureGlobalIdCb )(
    struct SCOREP_Location* location );

/**
 * Called whenever there is a new definition (SCOREP_MGMT_NEW_DEFINITION_HANDLE).
 *
 * This is not thread safe. Thus, substrates must handle locking.
 * @param handle handle to the new definition
 * @param type type of the handle
 */
typedef void ( * SCOREP_Substrates_NewDefinitionHandleCb )(
    SCOREP_AnyHandle  handle,
    SCOREP_HandleType type );


/**
 * Add an attribute to the attribute list of a location
 * note: The datatype of value depends on the given attributeHandle
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param attributeHandle a previously defined attribute handle
 *
 * @param value the new value for the given attributeHandle
 *
 */
typedef void ( * SCOREP_Substrates_AddAttributeCb )(
    struct SCOREP_Location* location,
    SCOREP_AttributeHandle  attributeHandle,
    void*                   value );


/**
 * Called after a process ended, declares a chunk of data that should have been freed but was not.
 *
 * @param addrLeaked address that has not been freed
 *
 * @param bytesLeaked number of bytes leaked
 *
 * @param substrateData ???
 *
 */
typedef void ( * SCOREP_Substrates_LeakedMemoryCb )(
    uint64_t addrLeaked,
    size_t   bytesLeaked,
    void*    substrateData[] );

/**
 * Provide Score-P with additional information about requirements, see SCOREP_SubstrateRequirementFlag for details.
 * These are called right after SCOREP_SubstratePluginInfo.init
 * There is one call for any SCOREP_Substrates_RequirementFlag provided by the Score-P installation.
 * Substrates must take care that they return 0 if flag is greater than SCOREP_SUBSTRATES_NUM_REQUIREMENT
 * @param flag the requirement flag that is queried
 * @return the setting for the requirement flag, which highly depends on the type of flag
 */
typedef int64_t ( * SCOREP_Substrates_GetRequirementCb)(
    SCOREP_Substrates_RequirementFlag flag );

#endif /* SCOREP_SUBSTRATES_DEFINITION_H */
