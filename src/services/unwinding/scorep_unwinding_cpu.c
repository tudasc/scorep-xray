/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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

SCOREP_ErrorCode
scorep_unwinding_cpu_init_location( SCOREP_Location* location )
{
    /* Create per-location unwinding management data */
    SCOREP_Unwinding_CpuLocationData* cpu_unwind_data =
        SCOREP_Location_AllocForMisc( location,
                                      sizeof( *cpu_unwind_data ) );

    /* Initialize the object */
    memset( cpu_unwind_data, 0, sizeof( *cpu_unwind_data ) );
    cpu_unwind_data->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;

    SCOREP_Location_SetSubsystemData( location,
                                      scorep_unwinding_subsystem_id,
                                      cpu_unwind_data );

    return SCOREP_SUCCESS;
}

/**
 * Translate current stack to a calling context tree representation.
 *
 * @param[inout] unwindContext  The unwind context where to start
 * @param instructionStack      Current stack of this location
 * @param instructionStackDepth Depth of current stack
 *
 * @return calling context tree representation of current stack
 */
static void
update_calling_context( scorep_unwinding_calling_context_tree_node** unwindContext,
                        uint64_t                                     ip,
                        SCOREP_RegionHandle                          region )
{
    /*
     * Look in the children array of current parent for an entry
     * which already represents a region with current IP
     */
    scorep_unwinding_calling_context_tree_node* child = ( *unwindContext )->children;
    while ( child )
    {
        if ( child->ip == ip && child->region == region )
        {
            /*
             * If found, use this child as new parent and go to
             * next element in the instruction stack
             */
            *unwindContext = child;
            return;
        }
        child = child->next_sibling;
    }

    /* Allocate memory for a new child */
    child         = SCOREP_Memory_AllocForMisc( sizeof( *child ) );
    child->handle =
        SCOREP_Definitions_NewCallingContext( ip,
                                              region,
                                              SCOREP_INVALID_SOURCE_CODE_LOCATION,
                                              ( *unwindContext )->handle );
    child->ip       = ip;
    child->region   = region;
    child->children = NULL;

    /* Enqueue new child to parent's list of children */
    child->next_sibling          = ( *unwindContext )->children;
    ( *unwindContext )->children = child;

    /* The new allocated child is our new node now */
    *unwindContext = child;
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
 * be skiped in the backtrac egeneration.
 *
 * @param regionName Region name to check
 *
 * @return True if this regin should be ignored in the backtrace.
 */
static bool
region_to_skip( const char* regionName )
{
    if ( 0 == strncmp( "pomp_", regionName, 5 )  ||
         0 == strncmp( "POMP_", regionName, 5 )  ||
         0 == strncmp( "pomp2_", regionName, 6 ) ||
         0 == strncmp( "POMP2_", regionName, 6 ) ||
         0 == strncmp( "scorep_", regionName, 7 ) ||
         0 == strncmp( "SCOREP_", regionName, 7 ) ||
         /* generated by the papi sampling source */
         0 == strncmp( "_papi_", regionName, 6 ) ||
         0 == strncmp( "Tau_", regionName, 4 ) ||
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
        UTILS_DEBUG( "unw_get_proc_info() failed for IP %tx: %s", ip, unw_strerror( -ret ) );
        return NULL;
    }

    /* FIXME: libunwind bug workaround */
    /* This has been introduced by Zoltan, We use it because it might fix something */
    if ( proc_info.end_ip == 0 || proc_info.end_ip == ip )
    {
        UTILS_DEBUG( "workaround active: proc_info.end_ip == ip: %tx, start=%tx", ip, proc_info.start_ip );
        return NULL;
    }

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
        UTILS_DEBUG( "error while retrieving function name for IP %tx through libunwind: %s",
                     proc_info.start_ip, unw_strerror( -ret ) );
        snprintf( unwindData->region_name_buffer, MAX_FUNC_NAME_LENGTH,
                  "UNKNOWN@%tx", proc_info.start_ip );
        // ??? return NULL;
    }

    return create_region( unwindData,
                          proc_info.start_ip,
                          proc_info.end_ip,
                          unwindData->region_name_buffer );
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
    scorep_unwinding_frame* frame = unwindData->unused_frames;
    if ( frame )
    {
        unwindData->unused_frames = frame->next;
    }
    else
    {
        frame = SCOREP_Memory_AllocForMisc( sizeof( *frame ) );
    }
    memset( frame, 0, sizeof( *frame ) );

    frame->ip     = ip;
    frame->region = region;

    frame->next = *stack;
    *stack      = frame;
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
        stack                     = top->next;
        top->next                 = unwindData->unused_frames;
        unwindData->unused_frames = top;
    }
}

/** Creates the current stack out of the unwind cursor
 *
 *  @param unwindData             Unwinding data of the location
 *  @param wrappedRegionIsOnStack True, if the wrapped region is on the stack.
 *                                This is false, for the enter event of the wrapped region
 *  @param[out] wrappedRegionIp   The IP of the wrapped region frame.
 *
 *  @return the stack
 */
static scorep_unwinding_frame*
get_current_stack( SCOREP_Unwinding_CpuLocationData* unwindData,
                   bool                              wrappedRegionIsOnStack,
                   uint64_t*                         wrappedRegionIp )
{
    scorep_unwinding_frame* current_stack = NULL;

    bool   in_signal_context = SCOREP_IN_SIGNAL_CONTEXT() > 0;
    bool   in_wrapped_region = unwindData->wrapped_region != 0 && wrappedRegionIsOnStack;
    size_t frames_to_skip    = unwindData->wrapped_region != 0 ? unwindData->frames_to_skip : 0;

    UTILS_DEBUG( "unwinding %s %s %zu",
                 in_signal_context ? "true" : "false",
                 in_wrapped_region ? "true" : "false",
                 frames_to_skip );

    while ( true )
    {
        int ret = unw_step( &unwindData->cursor );
        if ( ret < 0 )
        {
            UTILS_DEBUG( "Breaking after unw_step() returned %s", unw_strerror( -ret ) );
            break;
        }
        if ( 0 == ret )
        {
            UTILS_DEBUG( "unwinding %s %s %zu: unw_step() returned 0",
                         in_signal_context ? "true" : "false",
                         in_wrapped_region ? "true" : "false",
                         frames_to_skip );
            break;
        }

        int use_prev_instr = 1;
        if ( unw_is_signal_frame( &unwindData->cursor ) )
        {
            in_signal_context = false;
            use_prev_instr    = 0;
        }

        /* If we were called from a signal handler, than we ignore stack frames
           until we found the signal frame */
        if ( in_signal_context )
        {
            continue;
        }

        /* the current instruction pointer */
        unw_word_t ip;
        /* get the instruction pointer for the current instruction on the thread */
        ret = unw_get_reg( &unwindData->cursor, UNW_REG_IP, &ip );
        if ( ret < 0 )
        {
            UTILS_DEBUG( "Could not get IP register (unw_get_reg() returned %s", unw_strerror( -ret ) );
            continue;
        }
        UTILS_DEBUG( "unwinding %s %s %zu: IP %p",
                     in_signal_context ? "true" : "false",
                     in_wrapped_region ? "true" : "false",
                     frames_to_skip,
                     ( void* )ip );
        if ( 0 == ip )
        {
            break;
        }

        /* check if this frame is the wrapped region */
        if ( in_wrapped_region
             && unwindData->wrapped_region->start <= ip
             && ip < unwindData->wrapped_region->end )
        {
            /* This is the frame of the wrapped region, we still skip it
               as we have the wrapped region as an instrumented on the
               augmented stack already */
            in_wrapped_region = false;
            /* Remember the IP of this frame, we will use it later when
               updating the calling context */
            *wrappedRegionIp = ip - use_prev_instr;
            continue;
        }

        /* We ignore frames until we found the wrapped region frame. */
        if ( in_wrapped_region )
        {
            continue;
        }

        /* lock-up the region by the IP */
        scorep_unwinding_region* region = get_region( unwindData, &unwindData->cursor, ip );

        /* if we could not recognize a region (because it has been in kernel space for example)
         * or we know we can skip the region (e.g., because its within Score-P), skip it */
        if ( !region || region->skip )
        {
            continue;
        }

        /* If we got this far, we may need to skip further frames based on frames_to_skip */
        if ( frames_to_skip > 0 )
        {
            frames_to_skip--;
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

    return current_stack;
}

/** Resolve PLT entries, works only if linked/executed with bind-now semantic.
 *  Implemented only for x86-64.
 *
 *  @param functionAddress The function address to resolve.
 *
 *  @return the resolved function address
 */
static inline intptr_t
resolve_plt( intptr_t functionAddress )
{
    intptr_t v0, v1;

    v0 = *( intptr_t* )functionAddress;
    v1 = *( intptr_t* )( functionAddress + 8 );

    /* Is this a plt entry? */
    if ( ( v0 & 0xffff ) == 0x25ff
         && ( ( ( v0 >> 48 ) & 0xff ) == 0x68 )
         && ( ( ( v1 >> 24 ) & 0xff ) == 0xe9 ) )
    {
        /* it is, is it already resolved? */
        intptr_t got = ( v0 >> 16 ) & 0xffffffff;
        got += functionAddress + 6;
        intptr_t v2 = *( intptr_t* )got;

        if ( v2 == functionAddress + 6 )
        {
            UTILS_FATAL( "Executable or Score-P libraries not linked with bind-now semantics." );
        }

        functionAddress = v2;
    }

    return functionAddress;
}

static scorep_unwinding_region*
get_wrapped_region( SCOREP_Unwinding_CpuLocationData* unwindData,
                    intptr_t                          wrappedRegion )
{
    /* Resolve PLT entries */
    wrappedRegion = resolve_plt( wrappedRegion );

    /* Look for the region belonging to ip */
    scorep_unwinding_region* region =
        scorep_unwinding_region_find( unwindData, wrappedRegion );

    if ( !region )
    {
        /* Get address range of function from libunwind */
        unw_proc_info_t proc_info;
        int             ret = unw_get_proc_info_by_ip( unw_local_addr_space,
                                                       ( unw_word_t )wrappedRegion,
                                                       &proc_info, 0 );
        if ( ret < 0 )
        {
            UTILS_DEBUG( "unw_get_proc_info_by_ip() failed for IP %tx: %s", wrappedRegion, unw_strerror( -ret ) );
            proc_info.start_ip = wrappedRegion;
            proc_info.end_ip   = wrappedRegion + 1;
        }

        /* FIXME: libunwind bug workaround */
        /* This has been introduced by Zoltan, We use it because it might fix something */
        if ( proc_info.end_ip == wrappedRegion )
        {
            UTILS_DEBUG( "workaround active: proc_info.end_ip == ip: %tx, start=%tx", wrappedRegion, proc_info.start_ip );
            proc_info.end_ip++;
        }

        region = create_region( unwindData,
                                proc_info.start_ip,
                                proc_info.end_ip,
                                "" /* name is not important */ );

        /* Don't set a handle, the region may be wrapped under different names */
        region->handle = SCOREP_INVALID_REGION;
    }

    return region;
}

SCOREP_ErrorCode
scorep_unwinding_cpu_handle_enter( SCOREP_Location*             location,
                                   SCOREP_RegionHandle          instrumentedRegionHandle,
                                   intptr_t                     wrappedRegion,
                                   size_t                       framesToSkip,
                                   SCOREP_CallingContextHandle* callingContext,
                                   uint32_t*                    unwindDistance,
                                   SCOREP_CallingContextHandle* previousCallingContext )
{
    UTILS_DEBUG_ENTRY( "instrumentedRegionHandle=%u[%s]",
                       instrumentedRegionHandle,
                       instrumentedRegionHandle
                       ? SCOREP_RegionHandle_GetName( instrumentedRegionHandle )
                       : "" );

    SCOREP_Unwinding_CpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    if ( !unwind_data )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    /* export the previous calling context, but do not reset our previous yet,
       as we may fail to get an backtrace */
    *previousCallingContext = unwind_data->previous_calling_context;

    int ret = unw_getcontext( &unwind_data->context );
    if ( ret < 0 )
    {
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Could not get libunwind context: %s", unw_strerror( -ret ) );
    }
    ret = unw_init_local( &unwind_data->cursor, &unwind_data->context );
    if ( ret < 0 )
    {
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Could not get libunwind cursor: %s", unw_strerror( -ret ) );
    }

    bool     wrapped_region_is_on_stack = false;
    uint64_t wrapped_region_ip          = 0;
    if ( unwind_data->wrapped_region )
    {
        wrapped_region_is_on_stack = true;
    }
    if ( wrappedRegion )
    {
        UTILS_BUG_ON( unwind_data->wrapped_region, "Entering a wrapped region again!" );

        /* If we just enter the wrapped region, the wrapped region wont be on
           the stack yet */
        wrapped_region_is_on_stack = false;

        unwind_data->wrapped_region =
            get_wrapped_region( unwind_data, wrappedRegion );
        if ( !unwind_data->wrapped_region )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not determine function for wrapped region." );
        }
        /* For the enter event in the wrapped region, we use the start address,
           wont be changed by get_current_stack */
        wrapped_region_ip           = unwind_data->wrapped_region->start;
        unwind_data->frames_to_skip = framesToSkip;
    }

    scorep_unwinding_frame* current_stack =
        get_current_stack( unwind_data, wrapped_region_is_on_stack, &wrapped_region_ip );
    if ( !current_stack )
    {
        UTILS_BUG_ON( instrumentedRegionHandle != 0, "Empty stack for enter" );
        /* Just ignore this sample */
        return SCOREP_SUCCESS;
    }

    /* As this is the virtual root, we need to assume that it made progress,
       thus start with an unwind distance of 1 */
    *unwindDistance = 1;
    scorep_unwinding_calling_context_tree_node* unwind_context = &unwind_data->calling_context_root;

    /* If this is a sample inside a wrapped region, just take the unwind context from the
       instrumented region and the wrapped_region_ip to create the calling context and return */
    if ( !instrumentedRegionHandle && unwind_data->wrapped_region )
    {
        UTILS_BUG_ON( unwind_data->augmented_stack == NULL,
                      "Sample in wrapped region without instrumented region on the stack." );
        UTILS_BUG_ON( scorep_in_wrapped_region == 0,
                      "Sample inside a wrapped-region triggered without being told so." );
        unwind_context                   = unwind_data->augmented_stack->surrogates->unwind_context;
        unwind_data->augmented_stack->ip = wrapped_region_ip;
        /* Decent into the instrumented region */
        *unwindDistance = 1;
        update_calling_context( &unwind_context,
                                wrapped_region_ip,
                                unwind_data->augmented_stack->surrogates->region_handle );

        *callingContext                       = unwind_context->handle;
        unwind_data->previous_calling_context = *callingContext;

        drop_stack( unwind_data, current_stack );

        return SCOREP_SUCCESS;
    }

    /* If we have instrumented regions on the stack, determine the unwind context
     * and the the tail of the current unwind stack. */
    if ( unwind_data->augmented_stack )
    {
        uint64_t                          previous_ip = unwind_data->augmented_stack->ip;
        scorep_unwinding_augmented_frame* frame       = unwind_data->augmented_stack->prev;
        while ( current_stack )
        {
            if ( frame->region != current_stack->region )
            {
                break;
            }
            previous_ip = current_stack->ip;

            scorep_unwinding_frame* top = current_stack;
            current_stack              = current_stack->next;
            top->next                  = unwind_data->unused_frames;
            unwind_data->unused_frames = top;

            if ( frame == unwind_data->augmented_stack )
            {
                break;
            }
            frame = frame->prev;
        }

        /* Use the unwind context from the last instrumented region, as this
           is a real node, we start the unwind distance with 0 again */
        unwind_context                   = unwind_data->augmented_stack->surrogates->unwind_context;
        *unwindDistance                  = 0;
        unwind_data->augmented_stack->ip = previous_ip;

        /* Decent into the instrumented region */
        ( *unwindDistance )++;
        update_calling_context( &unwind_context,
                                unwind_data->augmented_stack->ip,
                                unwind_data->augmented_stack->surrogates->region_handle );
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
        ( *unwindDistance )++;
        update_calling_context( &unwind_context,
                                current_stack->ip,
                                current_stack->region->handle );

        /* We want to enter an instrumented region, thus we need to create
           the augmented stack, thus convert the current frame to an augmented
           one */
        if ( instrumentedRegionHandle != SCOREP_INVALID_REGION )
        {
            scorep_unwinding_augmented_frame* frame = unwind_data->unused_augmented_frames;
            if ( frame )
            {
                unwind_data->unused_augmented_frames = frame->next;
            }
            else
            {
                frame = SCOREP_Memory_AllocForMisc( sizeof( *frame ) );
            }
            memset( frame, 0, sizeof( *frame ) );

            if ( unwind_data->augmented_stack == NULL )
            {
                /* First frame */
                frame->next = frame;
                frame->prev = frame;
            }
            else
            {
                frame->next       = unwind_data->augmented_stack;
                frame->prev       = unwind_data->augmented_stack->prev;
                frame->prev->next = frame;
                frame->next->prev = frame;
            }

            frame->ip                    = current_stack->ip;
            frame->region                = current_stack->region;
            unwind_data->augmented_stack = frame;
        }

        /* Move stack frame to the unused list */
        scorep_unwinding_frame* frame = current_stack;
        current_stack              = current_stack->next;
        frame->next                = unwind_data->unused_frames;
        unwind_data->unused_frames = frame;
    }

    /* We now have the calling context for the current CPU stack, now enter
       the provided instrumented region */
    if ( instrumentedRegionHandle != SCOREP_INVALID_REGION )
    {
        scorep_unwinding_surrogate* surrogate = unwind_data->unused_surrogates;
        if ( surrogate )
        {
            unwind_data->unused_surrogates = surrogate->prev;
        }
        else
        {
            surrogate = SCOREP_Memory_AllocForMisc( sizeof( *surrogate ) );
        }
        memset( surrogate, 0, sizeof( *surrogate ) );

        surrogate->prev = unwind_data->augmented_stack->surrogates;
        if ( wrappedRegion )
        {
            surrogate->ip = unwind_data->wrapped_region->start;
        }
        else
        {
            surrogate->ip = unwind_data->augmented_stack->ip;
        }
        surrogate->region_handle                 = instrumentedRegionHandle;
        surrogate->unwind_context                = unwind_context;
        unwind_data->augmented_stack->surrogates = surrogate;

        /* Now descent into the instrumented region */
        ( *unwindDistance )++;
        update_calling_context( &unwind_context,
                                surrogate->ip,
                                instrumentedRegionHandle );
    }

    *callingContext                       = unwind_context->handle;
    unwind_data->previous_calling_context = *callingContext;

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_unwinding_cpu_handle_exit( SCOREP_Location*             location,
                                  SCOREP_CallingContextHandle* callingContext,
                                  uint32_t*                    unwindDistance,
                                  SCOREP_CallingContextHandle* previousCallingContext )
{
    UTILS_DEBUG_ENTRY( "%p", location );

    SCOREP_Unwinding_CpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    if ( !unwind_data )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    *previousCallingContext = unwind_data->previous_calling_context;

    int ret = unw_getcontext( &unwind_data->context );
    if ( ret < 0 )
    {
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Could not get libunwind context: %s", unw_strerror( -ret ) );
    }
    ret = unw_init_local( &unwind_data->cursor, &unwind_data->context );
    if ( ret < 0 )
    {
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Could not get libunwind cursor: %s", unw_strerror( -ret ) );
    }

    UTILS_BUG_ON( unwind_data->augmented_stack == NULL, "Leave event without instrumented regions." );

    uint64_t ip = unwind_data->augmented_stack->ip;
    if ( unwind_data->wrapped_region )
    {
        /* If we are leaving the wrapped region, we wont have a address on the stack,
           take end-1 as the ip */
        ip                          = unwind_data->wrapped_region->end - 1;
        unwind_data->wrapped_region = NULL;
        unwind_data->frames_to_skip = 0;
    }
    else
    {
        uint64_t                wrapped_region_ip;
        scorep_unwinding_frame* current_stack =
            get_current_stack( unwind_data, false, &wrapped_region_ip );
        if ( !current_stack )
        {
            return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                "Could not unwind stack" );
        }

        /* Determine the IP where the leave happended */
        scorep_unwinding_augmented_frame* frame = unwind_data->augmented_stack->prev;
        while ( current_stack )
        {
            if ( frame->region != current_stack->region )
            {
                break;
            }
            ip = current_stack->ip;

            scorep_unwinding_frame* top = current_stack;
            current_stack              = current_stack->next;
            top->next                  = unwind_data->unused_frames;
            unwind_data->unused_frames = top;

            if ( frame == unwind_data->augmented_stack )
            {
                break;
            }
            frame = frame->prev;
        }
        drop_stack( unwind_data, current_stack );
    }
    unwind_data->augmented_stack->ip = ip;
    SCOREP_RegionHandle region_handle =
        unwind_data->augmented_stack->surrogates->region_handle;
    scorep_unwinding_calling_context_tree_node* unwind_context =
        unwind_data->augmented_stack->surrogates->unwind_context;

    /* Now pop the instrumented region from the augmented stack */
    scorep_unwinding_augmented_frame* frame     = unwind_data->augmented_stack;
    scorep_unwinding_surrogate*       surrogate = frame->surrogates;
    frame->surrogates              = surrogate->prev;
    surrogate->prev                = unwind_data->unused_surrogates;
    unwind_data->unused_surrogates = surrogate;

    /*
     * pop also all non-surrogate frames from the augmented stack until the next
     * instrumented region or drop the whole argumented stack if this was the
     * last instrumented region we left
     */
    while ( unwind_data->augmented_stack && unwind_data->augmented_stack->surrogates == NULL )
    {
        /* This is a real stack region, remove from augmented stack */
        frame = unwind_data->augmented_stack;
        if ( frame == frame->prev )
        {
            /* The last one */
            unwind_data->augmented_stack = NULL;
        }
        else
        {
            /* remove frame from double-linked list */
            frame->prev->next            = frame->next;
            frame->next->prev            = frame->prev;
            unwind_data->augmented_stack = frame->next;
        }
        frame->next                          = unwind_data->unused_augmented_frames;
        frame->prev                          = NULL;
        unwind_data->unused_augmented_frames = frame;
    }

    /* Now create the calling context for the leave */
    update_calling_context( &unwind_context,
                            ip,
                            region_handle );
    *unwindDistance                       = 1;
    *callingContext                       = unwind_context->handle;
    unwind_data->previous_calling_context = SCOREP_CallingContextHandle_GetParent( *callingContext );

    return SCOREP_SUCCESS;
}

void
scorep_unwinding_cpu_deactivate( SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY( "%p", location );

    SCOREP_Unwinding_CpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    if ( !unwind_data )
    {
        UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
        return;
    }

    while ( unwind_data->augmented_stack )
    {
        /* This is a real stack region, remove from augmented stack */
        scorep_unwinding_augmented_frame* frame = unwind_data->augmented_stack;
        if ( frame == frame->prev )
        {
            /* The last one */
            unwind_data->augmented_stack = NULL;
        }
        else
        {
            /* remove frame from double-linked list */
            frame->prev->next            = frame->next;
            frame->next->prev            = frame->prev;
            unwind_data->augmented_stack = frame->next;
        }

        /* Now pop all instrumented region from the augmented stack */
        while ( frame->surrogates )
        {
            scorep_unwinding_surrogate* surrogate = frame->surrogates;
            frame->surrogates = surrogate->prev;

            /* @todo trigger exit events of instrumented regions */

            surrogate->prev                = unwind_data->unused_surrogates;
            unwind_data->unused_surrogates = surrogate;
        }


        frame->next                          = unwind_data->unused_augmented_frames;
        frame->prev                          = NULL;
        unwind_data->unused_augmented_frames = frame;
    }

    SCOREP_Location_DeactivateCpuSample( location,
                                         unwind_data->previous_calling_context );
    unwind_data->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;
}
