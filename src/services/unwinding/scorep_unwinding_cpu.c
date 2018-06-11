/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_unwinding_cpu.h"

#include <SCOREP_Unwinding.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Location.h>
#include <SCOREP_Events.h>

#define SCOREP_DEBUG_MODULE_NAME UNWINDING
#include <UTILS_Debug.h>

#include <unistd.h>
#include <string.h>

#include "scorep_unwinding_region.h"
#include "scorep_unwinding_cct.h"

SCOREP_Unwinding_CpuLocationData*
scorep_unwinding_cpu_get_location_data( SCOREP_Location* location )
{
    /* Create per-location unwinding management data */
    SCOREP_Unwinding_CpuLocationData* cpu_unwind_data =
        SCOREP_Location_AllocForMisc( location,
                                      sizeof( *cpu_unwind_data ) );

    /* Initialize the object */
    memset( cpu_unwind_data, 0, sizeof( *cpu_unwind_data ) );
    cpu_unwind_data->location                 = location;
    cpu_unwind_data->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;

    return cpu_unwind_data;
}

/**
 * Determine whether @region is the main region. If it is, we can
 * stop going up the stack.
 *
 * @param unwindData    Unwinding data of this location
 * @param region        Region to check
 *
 * @return True if @region is the main region, otherwise false
 */
static bool
check_is_main( SCOREP_Unwinding_CpuLocationData* unwindData,
               scorep_unwinding_region*          region )
{
    if ( 0 == unwindData->start_ip_of_main &&
         ( 0 == strcmp( "main",   region->name ) ||
           0 == strcmp( "MAIN__", region->name ) ) )
    {
        unwindData->start_ip_of_main = region->start;
    }

    return unwindData->start_ip_of_main == region->start;
}

/**
 * Determine whether @region represents a thread fork event.
 * If it does, we can stop going up the stack.
 *
 * @param unwindData    Unwinding data of this location
 * @param region        Region to check
 *
 * @return True if @region represents a thread fork event,
 *         otherwise false
 */
static bool
check_is_fork( SCOREP_Unwinding_CpuLocationData* unwindData,
               scorep_unwinding_region*          region )
{
    if ( ( 0 == unwindData->start_ip_of_fork ) &&
         ( 0 == strcmp( "GOMP_taskwait",          region->name ) ||
           0 == strcmp( "GOMP_single_start",      region->name ) ||
           0 == strcmp( "gomp_thread_start",      region->name ) ||
           0 == strcmp( "__kmp_invoke_microtask", region->name ) ||
           0 == strcmp( "__kmp_launch_thread",    region->name ) ||
           0 == strcmp( "start_thread",           region->name ) ||
           0 == strcmp( "clone",                  region->name ) ) )
    {
        unwindData->start_ip_of_fork = region->start;
    }

    return unwindData->start_ip_of_fork == region->start;
}

/**
 * Checks the function name and returns true if the function should
 * be skipped in the backtrace generation.
 *
 * @param regionName Region name to check
 *
 * @return True if this regin should be ignored in the backtrace.
 */
static bool
region_to_skip( const char* regionName )
{
    if ( 0 == strncmp( "scorep_", regionName, 7 ) ||
         0 == strncmp( "SCOREP_", regionName, 7 ) ||
         NULL != strstr( regionName, "._omp_fn." ) )
    {
        return true;
    }

    return false;
}

/**
 * Create a new known region
 *
 * @param unwindData    Unwinding data of this location
 * @param startIp       Instruction pointer marking the beginning of a function
 * @param endIp         Instruction pointer marking the end of a function
 * @param regionName    Name of the region
 *
 * @return The newly created region.
 */
static scorep_unwinding_region*
create_region( SCOREP_Unwinding_CpuLocationData* unwindData,
               uint64_t                          startIp,
               uint64_t                          endIp,
               const char*                       regionName )
{
    UTILS_DEBUG_ENTRY( "name=%s@[%p,%p)", regionName, startIp, endIp );

    scorep_unwinding_region* region =
        scorep_unwinding_region_insert( unwindData,
                                        startIp,
                                        endIp,
                                        regionName );

    region->skip    = region_to_skip( regionName );
    region->is_main = check_is_main( unwindData, region );
    region->is_fork = check_is_fork( unwindData, region );

    return region;
}

/**
 * Looks-up the region by IP. If not fownd create one.
 *
 * @param unwindData    Unwinding data of this location
 * @param cursor        The current unwinding cursor
 * @param ip            The instruction address
 *
 * @return The region belonging to the instruction address.
 */
static scorep_unwinding_region*
get_region( SCOREP_Unwinding_CpuLocationData* unwindData,
            unw_cursor_t*                     cursor,
            unw_word_t                        ip )
{
    /* Look for the region belonging to ip */
    scorep_unwinding_region* region = scorep_unwinding_region_find( unwindData, ip );

    if ( region )
    {
        return region;
    }

    /* region not known, get info and name from libunwind */
    /* get the IP range of the function */
    unw_proc_info_t proc_info;
    int             ret = unw_get_proc_info( cursor, &proc_info );
    if ( ret < 0 )
    {
        UTILS_DEBUG( "unw_get_proc_info() failed for IP %#tx: %s", ip, unw_strerror( ret ) );
        return NULL;
    }

    /* FIXME: libunwind bug workaround */
    /* This has been introduced by Zoltan, We use it because it might fix something */
    if ( proc_info.end_ip == 0 || proc_info.end_ip == ip )
    {
        UTILS_DEBUG( "workaround active: proc_info.end_ip == ip: %#tx, start=%#tx", ip, proc_info.start_ip );
        return NULL;
    }

    UTILS_BUG_ON( proc_info.start_ip > ip || ip >= proc_info.end_ip,
                  "IP %#tx does not is insie region [%#tx,%#tx)",
                  ip, proc_info.start_ip, proc_info.end_ip );

    // the function name, libunwind can give us
    /* char       SCOREP_Unwinding_LocationData::region_name_buffer[ MAX_FUNC_NAME_LENGTH ]; */
    // the offset of the current instruction
    unw_word_t offset;

    ret = unw_get_proc_name( cursor,
                             unwindData->region_name_buffer,
                             MAX_FUNC_NAME_LENGTH,
                             &offset );
    if ( ret < 0 )
    {
        UTILS_DEBUG( "error while retrieving function name for IP %#tx: %s",
                     proc_info.start_ip, unw_strerror( ret ) );
        snprintf( unwindData->region_name_buffer, MAX_FUNC_NAME_LENGTH,
                  "UNKNOWN@[%#tx,%#tx)", proc_info.start_ip, proc_info.end_ip );
        // ??? return NULL;
    }

    return create_region( unwindData,
                          proc_info.start_ip,
                          proc_info.end_ip,
                          unwindData->region_name_buffer );
}

static void
put_unused( SCOREP_Unwinding_CpuLocationData* unwindData,
            void*                             unusedObject )
{
    scorep_unwinding_unused_object* unused_object = unusedObject;
    unused_object->next        = unwindData->unused_objects;
    unwindData->unused_objects = unused_object;
}

static void*
get_unused( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    scorep_unwinding_unused_object* used_object = unwindData->unused_objects;
    if ( used_object )
    {
        unwindData->unused_objects = used_object->next;
    }
    else
    {
        used_object = SCOREP_Location_AllocForMisc( unwindData->location, sizeof( *used_object ) );
    }
    memset( used_object, 0, sizeof( *used_object ) );

    return used_object;
}

/**
 * Push a region onto the stack.
 *
 * @param unwindData    Unwinding data of the location
 * @param[inout] stack  The top of the stack.
 * @param region        The region to push
 * @param ip            The instruction address inside the @p region
 */
static void
push_stack( SCOREP_Unwinding_CpuLocationData* unwindData,
            scorep_unwinding_frame**          stack,
            scorep_unwinding_region*          region,
            unw_word_t                        ip )
{
    scorep_unwinding_frame* frame = get_unused( unwindData );
    frame->ip     = ip;
    frame->region = region;
    frame->next   = *stack;
    *stack        = frame;
}

/**
 * Clears the stack.
 *
 * @param unwindData  Unwinding data of the location
 * @param stack       The top of the stack.
 */
static void
drop_stack( SCOREP_Unwinding_CpuLocationData* unwindData,
            scorep_unwinding_frame*           stack )
{
    /* clear the last current stack */
    while ( stack )
    {
        scorep_unwinding_frame* top = stack;
        stack = top->next;
        put_unused( unwindData, top );
    }
}

/** Gets the IP from the current stack frame
 *
 *  @param unwindData             Unwinding data of the location
 */
static uint64_t
get_current_ip( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    /* the current instruction pointer */
    unw_word_t ip;
    /* get the instruction pointer for the current instruction on the thread */
    int ret = unw_get_reg( &unwindData->cursor, UNW_REG_IP, &ip );
    if ( ret < 0 )
    {
        UTILS_DEBUG( "Could not get IP register (unw_get_reg() returned %s)", unw_strerror( ret ) );
        return 0;
    }
    UTILS_DEBUG( "unwinding: IP %p", ( void* )ip );
    return ip;
}

/** Creates the current stack out of the unwind cursor
 *
 *  @param unwindData             Unwinding data of the location
 *
 *  @return the stack
 */
static void
drop_signal_context( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    UTILS_DEBUG_ENTRY();

    int ret = 1;
    for (; ret > 0; ret = unw_step( &unwindData->cursor ) )
    {
        if ( unw_is_signal_frame( &unwindData->cursor ) )
        {
            break;
        }
    }
    if ( ret < 0 )
    {
        UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
    }
    if ( 0 == ret )
    {
        UTILS_DEBUG( "unwinding: unw_step() returned 0" );
    }
}

/** Creates the current stack out of the unwind cursor
 *
 *  @param unwindData             Unwinding data of the location
 *
 *  @return the stack
 */
static void
pop_skipped_frames( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    UTILS_DEBUG_ENTRY();

    int ret = 1;
    for (; ret > 0; ret = unw_step( &unwindData->cursor ) )
    {
        /* the current instruction pointer */
        unw_word_t ip = get_current_ip( unwindData );
        if ( 0 == ip )
        {
            UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
            break;
        }

        /* lock-up the region by the IP */
        scorep_unwinding_region* region = get_region( unwindData, &unwindData->cursor, ip );

        /* if we could not recognize a region (because it has been in kernel space for example)
         * or we know we can skip the region (e.g., because its within Score-P), skip it */
        if ( !region || region->skip )
        {
            continue;
        }

        /* First frame we did not skipped */
        break;
    }
    if ( ret < 0 )
    {
        UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
    }
    if ( 0 == ret )
    {
        UTILS_DEBUG( "unwinding: unw_step() returned 0" );
    }
}

/** Creates the current stack out of the unwind cursor
 *
 *  @param unwindData             Unwinding data of the location
 *
 *  @return the stack
 */
static scorep_unwinding_frame*
get_current_stack( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    scorep_unwinding_frame* current_stack = NULL;

    UTILS_DEBUG_ENTRY();

    int ret = 1;
    for (; ret > 0; ret = unw_step( &unwindData->cursor ) )
    {
        int use_prev_instr = 1;
        if ( unw_is_signal_frame( &unwindData->cursor ) )
        {
            use_prev_instr = 0;
        }

        /* the current instruction pointer */
        unw_word_t ip = get_current_ip( unwindData );
        if ( 0 == ip )
        {
            break;
        }

        /* lock-up the region by the IP */
        scorep_unwinding_region* region = get_region( unwindData, &unwindData->cursor, ip );

        /* if we could not recognize a region (because it has been in kernel space for example)
         * or we know we can skip the region (e.g., because its within Score-P), skip it */
        if ( !region || region->skip )
        {
            continue;
        }

        /* Honor this frame in the backtrace */
        push_stack( unwindData, &current_stack, region, ip - use_prev_instr );

        /* Break if this is a compiler-specific fork region */
        if ( region->is_fork )
        {
            UTILS_DEBUG( " Break on is_fork" );
            break;
        }

        /* Stop unwinding if the current region is main */
        if ( region->is_main )
        {
            UTILS_DEBUG( " Break on main" );
            break;
        }
    }
    if ( ret < 0 )
    {
        UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
    }
    if ( 0 == ret )
    {
        UTILS_DEBUG( "unwinding %s: unw_step() returned 0" );
    }

    return current_stack;
}

static scorep_unwinding_surrogate*
get_surrogate( SCOREP_Unwinding_CpuLocationData* unwindData,
               uint64_t                          ip,
               SCOREP_RegionHandle               instrumentedRegionHandle,
               bool                              isWrapped )
{
    scorep_unwinding_surrogate* surrogate = get_unused( unwindData );

    surrogate->ip            = ip;
    surrogate->region_handle = instrumentedRegionHandle;
    surrogate->is_wrapped    = isWrapped;

    return surrogate;
}

static void
push_surrogate( SCOREP_Unwinding_CpuLocationData*            unwindData,
                scorep_unwinding_surrogate*                  surrogate,
                scorep_unwinding_calling_context_tree_node** unwindContext,
                uint32_t*                                    unwindDistance )
{
    surrogate->unwind_context               = *unwindContext;
    surrogate->prev                         = unwindData->augmented_stack->surrogates;
    unwindData->augmented_stack->surrogates = surrogate;

    /* Now descent into the instrumented region */
    calling_context_descent( unwindData->location,
                             unwindContext,
                             unwindDistance,
                             surrogate->ip,
                             surrogate->region_handle );
}


static void
pop_surrogate( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    scorep_unwinding_augmented_frame* frame     = unwindData->augmented_stack;
    scorep_unwinding_surrogate*       surrogate = frame->surrogates;
    frame->surrogates = surrogate->prev;
    put_unused( unwindData, surrogate );

    /*
     * pop also all non-surrogate frames from the augmented stack until the next
     * instrumented region or drop the whole argumented stack if this was the
     * last instrumented region we left
     */
    while ( unwindData->augmented_stack && unwindData->augmented_stack->surrogates == NULL )
    {
        /* This is a real stack region, remove from augmented stack */
        frame = unwindData->augmented_stack;
        if ( frame == frame->prev )
        {
            /* The last one */
            unwindData->augmented_stack = NULL;
        }
        else
        {
            /* remove frame from double-linked list */
            frame->prev->next           = frame->next;
            frame->next->prev           = frame->prev;
            unwindData->augmented_stack = frame->next;
        }
        put_unused( unwindData, frame );
    }
}

static scorep_unwinding_surrogate*
resolve_unhandled_wrappers( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    scorep_unwinding_surrogate* new_surrogates    = NULL;
    bool                        next_is_caller_ip = false;
    int                         use_prev_instr    = 1;
    if ( SCOREP_IN_SIGNAL_CONTEXT() )
    {
        next_is_caller_ip = true;
        use_prev_instr    = 0;
    }

    scorep_unwinding_unhandled_wrapper* wrapper = unwindData->unhandled_wrappers;
    while ( wrapper )
    {
        UTILS_DEBUG( "Handle wrapper: %p %zu",
                     wrapper->wrapper_ip,
                     wrapper->n_wrapper_frames );

        uint64_t caller_ip = 0;

        /* now step through the stack, until we find the wrapper */
        int ret = 1;
        for (; ret > 0; ret = unw_step( &unwindData->cursor ) )
        {
            /* the current instruction pointer */
            unw_word_t ip = get_current_ip( unwindData );
            if ( 0 == ip )
            {
                break;
            }

            if ( next_is_caller_ip )
            {
                caller_ip         = ip - use_prev_instr;
                next_is_caller_ip = false;
            }

            /* lock-up the region by the IP */
            scorep_unwinding_region* region = get_region( unwindData, &unwindData->cursor, ip );
            if ( !region )
            {
                continue;
            }

            UTILS_DEBUG( "Handle wrapper: region on stack %s@[%p,%p)",
                         region->name,
                         region->start,
                         region->end );

            if ( wrapper->wrapper_ip < region->start || wrapper->wrapper_ip >= region->end )
            {
                continue;
            }

            /* we found the wrapper, now skip it and any possible pre-wrappers */
            while ( wrapper->n_wrapper_frames-- )
            {
                ret = unw_step( &unwindData->cursor );
                if ( ret < 0 )
                {
                    UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
                    break;
                }
                if ( 0 == ret )
                {
                    UTILS_DEBUG( "unwinding: unw_step() returned 0" );
                    break;
                }
            }
            break;
        }
        if ( ret < 0 )
        {
            UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
            goto out;
        }
        if ( 0 == ret )
        {
            UTILS_DEBUG( "unwinding: unw_step() returned 0" );
            goto out;
        }

        scorep_unwinding_surrogate* surrogate = get_surrogate( unwindData,
                                                               caller_ip,
                                                               wrapper->wrappee_handle,
                                                               true );
        surrogate->prev = new_surrogates;
        new_surrogates  = surrogate;

        next_is_caller_ip = true;

        wrapper = wrapper->next;
    }

    return new_surrogates;

out:
    /* we could not resolve all unhandled wrappers, drop the newly resolved one */
    while ( new_surrogates )
    {
        scorep_unwinding_surrogate* surrogate = new_surrogates;
        new_surrogates = surrogate->prev;
        put_unused( unwindData, surrogate );
    }
    return NULL;
}

static void
drop_unhandled_wrappers( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    while ( unwindData->unhandled_wrappers )
    {
        scorep_unwinding_unhandled_wrapper* wrapper = unwindData->unhandled_wrappers;
        unwindData->unhandled_wrappers = wrapper->next;
        put_unused( unwindData, wrapper );
    }
}

SCOREP_ErrorCode
scorep_unwinding_cpu_handle_enter( SCOREP_Unwinding_CpuLocationData* unwindData,
                                   void*                             contextPtr,
                                   SCOREP_RegionHandle               instrumentedRegionHandle,
                                   SCOREP_CallingContextHandle*      callingContext,
                                   uint32_t*                         unwindDistance,
                                   SCOREP_CallingContextHandle*      previousCallingContext )
{
    if ( !unwindData )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    UTILS_DEBUG_ENTRY( "%p instrumentedRegionHandle=%u[%s]",
                       unwindData->location,
                       instrumentedRegionHandle,
                       instrumentedRegionHandle
                       ? SCOREP_RegionHandle_GetName( instrumentedRegionHandle )
                       : "" );

    /* export the previous calling context, but do not reset our previous yet,
       as we may fail to get a backtrace */
    *previousCallingContext = unwindData->previous_calling_context;

#if HAVE( DECL_UNW_INIT_LOCAL2 ) || HAVE( DECL_UNW_INIT_LOCAL_SIGNAL )
    if ( contextPtr )
    {
#if HAVE( DECL_UNW_INIT_LOCAL2 )
        int ret = unw_init_local2( &unwindData->cursor, contextPtr, UNW_INIT_SIGNAL_FRAME );
#elif HAVE( DECL_UNW_INIT_LOCAL_SIGNAL )
        int ret = unw_init_local_signal( &unwindData->cursor, contextPtr );
#endif
        if ( ret < 0 )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not get libunwind cursor from signal context: %s", unw_strerror( ret ) );
        }
    }
    else
#endif
    {
        int ret = unw_getcontext( &unwindData->context );
        if ( ret < 0 )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not get libunwind context: %s", unw_strerror( ret ) );
        }
        ret = unw_init_local( &unwindData->cursor, &unwindData->context );
        if ( ret < 0 )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not get libunwind cursor: %s", unw_strerror( ret ) );
        }

        if ( SCOREP_IN_SIGNAL_CONTEXT() )
        {
            drop_signal_context( unwindData );
        }
        else
        {
            pop_skipped_frames( unwindData );
        }
    }

    //UTILS_BUG_ON( "Entering an instrumented region while in a wrapper." );

    scorep_unwinding_surrogate* new_surrogates = NULL;
    if ( unwindData->unhandled_wrappers )
    {
        UTILS_BUG_ON( instrumentedRegionHandle && unwindData->unhandled_wrappers->wrappee_handle != instrumentedRegionHandle,
                      "Entering the wrong wrapped region." );

        new_surrogates = resolve_unhandled_wrappers( unwindData );

        if ( new_surrogates == NULL )
        {
            UTILS_BUG_ON( instrumentedRegionHandle, "Could not resolve unhandled wrappers for enter event." );
            /* Ignore sample */
            return SCOREP_SUCCESS;
        }

        /* If we are already in a wrapped region, than we just need to push all new
         * surrogates onto the augmented stack and can return */
        if ( unwindData->augmented_stack
             && unwindData->augmented_stack->surrogates->is_wrapped )
        {
            /*
             * we could successfully resolve all unhandled wrappers, we can now
             * safely drop them all
             */
            drop_unhandled_wrappers( unwindData );

            scorep_unwinding_calling_context_tree_node* unwind_context = unwindData->augmented_stack->surrogates->unwind_context;
            *unwindDistance = 0;

            /* The previous wrapper made progress */
            calling_context_descent( unwindData->location,
                                     &unwind_context,
                                     unwindDistance,
                                     get_current_ip( unwindData ),
                                     unwindData->augmented_stack->surrogates->region_handle );

            /* We enter nested wrapped regions, just push all surrogates */
            while ( new_surrogates )
            {
                scorep_unwinding_surrogate* surrogate = new_surrogates;
                new_surrogates = surrogate->prev;

                UTILS_DEBUG( "Entering nested-wrapper: %s",
                             SCOREP_RegionHandle_GetName( surrogate->region_handle ) );

                push_surrogate( unwindData,
                                surrogate,
                                &unwind_context,
                                unwindDistance );
            }

            *callingContext                      = unwind_context->handle;
            unwindData->previous_calling_context = *callingContext;

            return SCOREP_SUCCESS;
        }
    }
    if ( unwindData->augmented_stack
         && unwindData->augmented_stack->surrogates->is_wrapped )
    {
        scorep_unwinding_calling_context_tree_node* unwind_context = unwindData->augmented_stack->surrogates->unwind_context;
        *unwindDistance = 0;

        if ( instrumentedRegionHandle != SCOREP_INVALID_REGION )
        {
            /* The previous wrapper made progress */
            calling_context_descent( unwindData->location,
                                     &unwind_context,
                                     unwindDistance,
                                     0, /* no IP yet, if instrumented region was triggered by wrappers, they are off anyway */
                                     unwindData->augmented_stack->surrogates->region_handle );

            /* instrumented region inside a wrapped region */
            new_surrogates = get_surrogate( unwindData,
                                            0, /* no IP yet, if instrumented region was triggered by wrappers, they are off anyway */
                                            instrumentedRegionHandle,
                                            false );

            push_surrogate( unwindData,
                            new_surrogates,
                            &unwind_context,
                            unwindDistance );
        }
        else
        {
            /* sample inside wrapped region */
            calling_context_descent( unwindData->location,
                                     &unwind_context,
                                     unwindDistance,
                                     get_current_ip( unwindData ),
                                     unwindData->augmented_stack->surrogates->region_handle );
        }

        *callingContext                      = unwind_context->handle;
        unwindData->previous_calling_context = *callingContext;

        return SCOREP_SUCCESS;
    }

    scorep_unwinding_frame* current_stack = get_current_stack( unwindData );
    if ( !current_stack )
    {
        UTILS_BUG_ON( instrumentedRegionHandle, "Empty stack for enter" );

        /* Just ignore this sample */
        return SCOREP_SUCCESS;
    }

    /* As this is the virtual root, we need to assume that it made progress,
       thus start with an unwind distance of 1 */
    *unwindDistance = 1;
    scorep_unwinding_calling_context_tree_node* unwind_context = &unwindData->calling_context_root;

    /* If we have instrumented regions on the stack, determine the unwind context
     * and the tail of the current unwind stack. */
    if ( unwindData->augmented_stack )
    {
        uint64_t                          previous_ip = unwindData->augmented_stack->ip;
        scorep_unwinding_augmented_frame* frame       = unwindData->augmented_stack->prev;
        while ( current_stack )
        {
            if ( frame->region != current_stack->region )
            {
                break;
            }
            previous_ip = current_stack->ip;

            scorep_unwinding_frame* top = current_stack;
            current_stack = current_stack->next;
            put_unused( unwindData, top );

            if ( frame == unwindData->augmented_stack )
            {
                break;
            }
            frame = frame->prev;
        }

        /* Use the unwind context from the last instrumented region, as this
           is a real node, we start the unwind distance with 0 again */
        unwind_context                  = unwindData->augmented_stack->surrogates->unwind_context;
        *unwindDistance                 = 0;
        unwindData->augmented_stack->ip = previous_ip;

        /* Decent into the instrumented region */
        calling_context_descent( unwindData->location,
                                 &unwind_context,
                                 unwindDistance,
                                 unwindData->augmented_stack->ip,
                                 unwindData->augmented_stack->surrogates->region_handle );
    }

    /* Descent with the tail of the current stack down the unwind context
       Build the augmented stack, if needed */
    while ( current_stack )
    {
        UTILS_BUG_ON( current_stack->region == NULL, "Missing region for stack frame" );

        if ( current_stack->region->handle == SCOREP_INVALID_REGION )
        {
            /* Need to define the region first */
            current_stack->region->handle = SCOREP_Definitions_NewRegion(
                current_stack->region->name,
                NULL,
                SCOREP_INVALID_SOURCE_FILE,
                SCOREP_INVALID_LINE_NO,
                SCOREP_INVALID_LINE_NO,
                SCOREP_PARADIGM_SAMPLING,
                SCOREP_REGION_FUNCTION );
        }

        /* Decent into the calling context tree */
        calling_context_descent( unwindData->location,
                                 &unwind_context,
                                 unwindDistance,
                                 current_stack->ip,
                                 current_stack->region->handle );

        /* We want to enter an instrumented region, thus we need to create
           the augmented stack, thus convert the current frame to an augmented
           one */
        if ( instrumentedRegionHandle != SCOREP_INVALID_REGION
             || new_surrogates )
        {
            scorep_unwinding_augmented_frame* augmented_frame = get_unused( unwindData );
            augmented_frame->ip     = current_stack->ip;
            augmented_frame->region = current_stack->region;

            if ( unwindData->augmented_stack == NULL )
            {
                /* First frame */
                augmented_frame->next = augmented_frame;
                augmented_frame->prev = augmented_frame;
            }
            else
            {
                augmented_frame->next       = unwindData->augmented_stack;
                augmented_frame->prev       = unwindData->augmented_stack->prev;
                augmented_frame->prev->next = augmented_frame;
                augmented_frame->next->prev = augmented_frame;
            }
            unwindData->augmented_stack = augmented_frame;
        }

        /* Move stack frame to the unused list */
        scorep_unwinding_frame* frame = current_stack;
        current_stack = current_stack->next;
        put_unused( unwindData, frame );
    }

    /*
     * we could successfully resolve all unhandled wrappers, we can now safely
     * drop them all
     */
    drop_unhandled_wrappers( unwindData );

    /* create the surrogate for the instrumented (non-wrapped) region */
    if ( instrumentedRegionHandle != SCOREP_INVALID_REGION && !new_surrogates )
    {
        new_surrogates = get_surrogate( unwindData,
                                        unwindData->augmented_stack->ip,
                                        instrumentedRegionHandle,
                                        false );
    }

    /* We now have the calling context for the current CPU stack, now enter
       all provided instrumented region */
    while ( new_surrogates )
    {
        scorep_unwinding_surrogate* surrogate = new_surrogates;
        new_surrogates = surrogate->prev;
        push_surrogate( unwindData,
                        surrogate,
                        &unwind_context,
                        unwindDistance );
    }

    *callingContext                      = unwind_context->handle;
    unwindData->previous_calling_context = *callingContext;

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_unwinding_cpu_handle_exit( SCOREP_Unwinding_CpuLocationData* unwindData,
                                  SCOREP_CallingContextHandle*      callingContext,
                                  uint32_t*                         unwindDistance,
                                  SCOREP_CallingContextHandle*      previousCallingContext )
{
    if ( !unwindData )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    UTILS_DEBUG_ENTRY( "%p",
                       unwindData->location );

    *previousCallingContext = unwindData->previous_calling_context;

    UTILS_BUG_ON( unwindData->augmented_stack == NULL, "Leave event without instrumented regions." );

    /* wrapped regions dont have an instruction for the leave event */
    uint64_t ip = 0;
    if ( !unwindData->augmented_stack->surrogates->is_wrapped
         && unwindData->augmented_stack->surrogates->ip )
    {
        int ret = unw_getcontext( &unwindData->context );
        if ( ret < 0 )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not get libunwind context: %s", unw_strerror( ret ) );
        }
        ret = unw_init_local( &unwindData->cursor, &unwindData->context );
        if ( ret < 0 )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not get libunwind cursor: %s", unw_strerror( ret ) );
        }
        pop_skipped_frames( unwindData );

        scorep_unwinding_frame* current_stack = get_current_stack( unwindData );
        if ( !current_stack )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not unwind stack" );
        }

        /* Determine the IP where the leave happended */
        scorep_unwinding_augmented_frame* frame = unwindData->augmented_stack->prev;
        while ( current_stack )
        {
            if ( frame->region != current_stack->region )
            {
                break;
            }
            ip = current_stack->ip;

            scorep_unwinding_frame* top = current_stack;
            current_stack = current_stack->next;
            put_unused( unwindData, top );

            if ( frame == unwindData->augmented_stack )
            {
                break;
            }
            frame = frame->prev;
        }
        drop_stack( unwindData, current_stack );
    }
    unwindData->augmented_stack->ip = ip;
    SCOREP_RegionHandle region_handle =
        unwindData->augmented_stack->surrogates->region_handle;
    scorep_unwinding_calling_context_tree_node* unwind_context =
        unwindData->augmented_stack->surrogates->unwind_context;

    /* Now pop the instrumented region from the augmented stack */
    pop_surrogate( unwindData );

    /* Now create the calling context for the leave */
    *unwindDistance = 0;
    calling_context_descent( unwindData->location,
                             &unwind_context,
                             unwindDistance,
                             ip,
                             region_handle );
    *callingContext                      = unwind_context->handle;
    unwindData->previous_calling_context = SCOREP_CallingContextHandle_GetParent( *callingContext );

    return SCOREP_SUCCESS;
}

void
scorep_unwinding_cpu_deactivate( SCOREP_Unwinding_CpuLocationData* unwindData )
{
    if ( !unwindData )
    {
        UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
        return;
    }

    UTILS_DEBUG_ENTRY( "%p", unwindData->location );

    while ( unwindData->augmented_stack )
    {
        /* This is a real stack region, remove from augmented stack */
        scorep_unwinding_augmented_frame* frame = unwindData->augmented_stack;
        if ( frame == frame->prev )
        {
            /* The last one */
            unwindData->augmented_stack = NULL;
        }
        else
        {
            /* remove frame from double-linked list */
            frame->prev->next           = frame->next;
            frame->next->prev           = frame->prev;
            unwindData->augmented_stack = frame->next;
        }

        /* Now pop all instrumented region from the augmented stack */
        while ( frame->surrogates )
        {
            scorep_unwinding_surrogate* surrogate = frame->surrogates;
            frame->surrogates = surrogate->prev;
            put_unused( unwindData, surrogate );

            /* @todo trigger exit events of instrumented regions */
        }

        put_unused( unwindData, frame );
    }

    SCOREP_Location_DeactivateCpuSample( unwindData->location,
                                         unwindData->previous_calling_context );
    unwindData->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;
}

void
scorep_unwinding_cpu_push_wrapper( SCOREP_Unwinding_CpuLocationData* unwindData,
                                   SCOREP_RegionHandle               regionHandle,
                                   uint64_t                          wrapperIp,
                                   size_t                            framesToSkip )
{
    if ( !unwindData )
    {
        UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
        return;
    }

    UTILS_DEBUG_ENTRY( "%p regionHandle=%u[%s] %" PRIu64 " %zu",
                       unwindData->location, regionHandle,
                       SCOREP_RegionHandle_GetName( regionHandle ),
                       wrapperIp, framesToSkip );

    if ( wrapperIp == 0 )
    {
        int ret = unw_getcontext( &unwindData->context );
        if ( ret < 0 )
        {
            UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                         "Could not get libunwind context: %s", unw_strerror( ret ) );
            return;
        }
        ret = unw_init_local( &unwindData->cursor, &unwindData->context );
        if ( ret < 0 )
        {
            UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                         "Could not get libunwind cursor: %s", unw_strerror( ret ) );
            return;
        }
        pop_skipped_frames( unwindData );

        /*
         * Find the frame of the last wrapper (the one called
         * SCOREP_EnterWrappedRegion/SCOREP_EnterWrapper)
         */
        scorep_unwinding_region* region;
        ret = 1;
        for (; ret > 0; ret = unw_step( &unwindData->cursor ) )
        {
            region = NULL;

            /* the current instruction pointer */
            /* get the instruction pointer for the current instruction on the thread */
            wrapperIp = get_current_ip( unwindData );
            if ( 0 == wrapperIp )
            {
                break;
            }

            /* lock-up the region by the IP */
            region = get_region( unwindData, &unwindData->cursor, wrapperIp );

            /* if we could not recognize a region (because it has been in kernel space for example)
             * or we know we can skip the region (e.g., because its within Score-P), skip it */
            if ( !region || region->skip )
            {
                continue;
            }

            /* This is the first region after any ignored (i.e., inside Score-P) frames */
            break;
        }
        if ( ret < 0 )
        {
            UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( ret ) );
        }
        if ( 0 == ret )
        {
            UTILS_DEBUG( "unwinding: unw_step() returned 0" );
        }

        UTILS_BUG_ON( !wrapperIp, "Could not determine IP in wrapper region." );
    }

    scorep_unwinding_unhandled_wrapper* new_wrapper = get_unused( unwindData );
    new_wrapper->wrapper_ip        = wrapperIp;
    new_wrapper->n_wrapper_frames  = framesToSkip;
    new_wrapper->wrappee_handle    = regionHandle;
    new_wrapper->next              = unwindData->unhandled_wrappers;
    unwindData->unhandled_wrappers = new_wrapper;

    UTILS_DEBUG_EXIT( "%p regionHandle=%u[%s]",
                      unwindData->location, regionHandle,
                      SCOREP_RegionHandle_GetName( regionHandle ) );
}

void
scorep_unwinding_cpu_pop_wrapper( SCOREP_Unwinding_CpuLocationData* unwindData,
                                  SCOREP_RegionHandle               regionHandle )
{
    if ( !unwindData )
    {
        UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
        return;
    }

    UTILS_DEBUG_ENTRY( "%p regionHandle=%u[%s]",
                       unwindData->location, regionHandle,
                       SCOREP_RegionHandle_GetName( regionHandle ) );

    if ( unwindData->unhandled_wrappers == NULL )
    {
        /* The wrapper was handled (enter or sample) */
        UTILS_BUG_ON( unwindData->augmented_stack == NULL,
                      "Wrapper neither on the unhandled stack, nor on the augmented stack." );

        UTILS_BUG_ON( unwindData->augmented_stack->surrogates->region_handle != regionHandle,
                      "Wrong order of push/pop wrapper operations." );

        pop_surrogate( unwindData );

        return;
    }

    scorep_unwinding_unhandled_wrapper* top_wrapper = unwindData->unhandled_wrappers;
    unwindData->unhandled_wrappers = top_wrapper->next;
    UTILS_DEBUG( "%p top => %u[%s] %p",
                 unwindData->location, top_wrapper->wrappee_handle,
                 SCOREP_RegionHandle_GetName( top_wrapper->wrappee_handle ),
                 top_wrapper->wrapper_ip );

    UTILS_BUG_ON( top_wrapper->wrappee_handle != regionHandle,
                  "Wrong order of push/pop wrapper operations." );

    put_unused( unwindData, top_wrapper );
}
