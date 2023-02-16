/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015, 2018, 2021-2023,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_TASK_H
#define SCOREP_TASK_H

/**
 * @file
 * This files defines functions of the per-task region stack that may be
 * called by non-measurement core components.
 */

#include <SCOREP_Types.h>
#include <SCOREP_FastHashtab.h>

#include <stddef.h>

struct SCOREP_Location;

/**
 * Returns the thread id of the @a task object, i.e., the id that was
 * passed to SCOREP_Task_Create().
 * Together with the generation number it identifies the task instance.
 * @param task  The task handle.
 * @returns the thread id of the @a task object.
 */
uint32_t
SCOREP_Task_GetThreadId( SCOREP_TaskHandle task );

/**
 * Returns the generation number of the @a task object, i.e., the
 * number that was passed to SCOREP_Task_Create().
 * Together with the thread id it identifies the task instance.
 * @param task  The task handle.
 * @returns the generation number of the @a task object.
 */
uint32_t
SCOREP_Task_GetGenerationNumber( SCOREP_TaskHandle task );

/**
 * Returns the currently executed task for @a location.
 * @param location The location.
 * @returns the currently executed task for @a location.
 */
SCOREP_TaskHandle
SCOREP_Task_GetCurrentTask( struct SCOREP_Location* location );

/**
 * Returns the region handle on top of the stack for @a task.
 * @param task The task.
 * @returns the region handle on top of the stack for @a task.
 */
SCOREP_RegionHandle
SCOREP_Task_GetTopRegion( SCOREP_TaskHandle task );

/**
 * Removes all regions from the stack of @a task.
 * @param location  The location which is used for memory operations.
 * @param task      The task.
 */
void
SCOREP_Task_ClearStack( struct SCOREP_Location* location,
                        SCOREP_TaskHandle       task );

/**
 * Removes all regions from the stack of the current task.
 */
void
SCOREP_Task_ClearCurrent( void );

/**
 * Adds @a region to the region stack of the current task executed by @a location.
 * @param location The location which executes the enter event.
 * @param region   The region handle of the entered region.
 */
void
SCOREP_Task_Enter( struct SCOREP_Location* location,
                   SCOREP_RegionHandle     region );

/**
 * Moves the stack pointer of the current task one element down.
 * @param location The location that executes the exit.
 */
void
SCOREP_Task_Exit( struct SCOREP_Location* location );

/**
 * Creates exit events for all regions on the stack for @a task using
 * SCOREP_ExitRegion(), assuming current location equals passed
 * @a location.
 * @param location  The location that processes the exits.
 * @param task      The task.
 */
void
SCOREP_Task_ExitAllRegions( struct SCOREP_Location* location,
                            SCOREP_TaskHandle       task );

/**
 * Creates exit events for all regions on the stack for @a task using
 * SCOREP_Location_ExitRegion(). Current location and passed @a location
 *  may differ in phase POST only.
 * @param location  The location for which the exits are triggered.
 * @param task      The task.
 * @param timestamp The timestamp for all exits.
 */
void
SCOREP_Location_Task_ExitAllRegions( struct SCOREP_Location* location,
                                     SCOREP_TaskHandle       task,
                                     uint64_t                timestamp );

/**
 * Returns the substrate specific data for @a task.
 * @param task        The task handle from which we get the substrate data.
 * @param substrateId The Id of the substrate.
 *
 * @returns the substrate specific data for @a task.
 *          If the substrate not enabled, it returns NULL.
 */
void*
SCOREP_Task_GetSubstrateData( SCOREP_TaskHandle task,
                              size_t            substrateId );

/**
 * Sets the profiling specific data for @a task.
 * @param task        The task for which we set the data.
 * @param substrateId The Id of the substrate.
 * @param data        The profiling data.
 */
void
SCOREP_Task_SetSubstrateData( SCOREP_TaskHandle task,
                              size_t            substrateId,
                              void*             data );

/**
 * Returns the jenkinshash of the current region handle stack for @a task.
 * @param task The task.
 * @returns the hash value of the region handle stack for @a task.
 */
uint32_t
SCOREP_Task_GetRegionStackHash( SCOREP_TaskHandle task );

/* *INDENT-OFF* */

/**
 * Standardized FastHashTab for callsites based on region stack hashes.
 * The SCOREP_CALLSITE_HASH_TABLE is instantiated in the launch site context
 * with static access to insert to the `callsite_hash_get_and_insert` function.
 * The callsitePrefix creates wrapper for the get and remove functions for
 * the execution side, which may be called in a different compilation unit.
 * The `keyType` may vary depending on use case, but the value type is the same
 * for any callsites based on the region stack hash.
 * Requires inclusion of `SCOREP_FastHashtab.h` and `jenkins_hash.h` on use.
 */
#define SCOREP_CALLSITE_HASH_TABLE( callsitePrefix, keyType ) \
\
    typedef keyType  callsite_hash_key_t; \
    typedef uint32_t callsite_hash_value_t; \
\
    static inline bool \
    callsite_hash_equals( callsite_hash_key_t key1, callsite_hash_key_t key2 ) \
    { \
        return key1 == key2; \
    } \
\
    static void* \
    callsite_hash_allocate_chunk( size_t chunkSize ) \
    { \
        void* chunk = SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize ); \
        return chunk; \
    } \
\
    static void \
    callsite_hash_free_chunk( void* chunk ) \
    { \
    } \
\
    static callsite_hash_value_t \
    callsite_hash_value_ctor( callsite_hash_key_t* addr, \
                              const void*          ctorDataUnused ) \
    { \
        return SCOREP_Task_GetRegionStackHash( SCOREP_Task_GetCurrentTask( SCOREP_Location_GetCurrentCPULocation() ) ); \
    } \
\
    static void \
    callsite_hash_value_dtor( callsite_hash_key_t key, callsite_hash_value_t value ) \
    { \
    } \
\
    static inline uint32_t \
    callsite_hash_bucket_idx( callsite_hash_key_t key ) \
    { \
        uint32_t idx = jenkins_hash( &key, sizeof( key ), 0 ) & hashmask( 9 ); \
        return idx; \
    } \
\
    SCOREP_HASH_TABLE_NON_MONOTONIC(  callsite_hash, 15, hashsize( 9 ) ) \
\
    bool \
    scorep_ ## callsitePrefix ## _callsite_hash_get_and_remove( keyType                key, \
                                                                callsite_hash_value_t* value ) \
    { \
        return callsite_hash_get_and_remove( key, value ); \
    } \
\
    callsite_hash_value_t \
    scorep_ ## callsitePrefix ## _callsite_hash_get_and_insert( keyType idx ) \
    { \
        callsite_hash_value_t value = 0; \
        callsite_hash_get_and_insert( idx, NULL, &value ); \
        return value; \
    } \


#endif /* SCOREP_TASK_H */
