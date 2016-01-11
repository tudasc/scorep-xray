/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * Implementation of all __wrap_* functions used by the pthread library wrapper
 */

#include <config.h>

#include "scorep_pthread.h"
#include "scorep_pthread_event_functions.h"
#include "scorep_pthread_mutex.h"

#include <SCOREP_Events.h>
#include <SCOREP_Types.h>
#include <SCOREP_ThreadCreateWait_Event.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Libwrap_Macros.h>
#include <SCOREP_Task.h>

#define SCOREP_DEBUG_MODULE_NAME PTHREAD
#include <UTILS_Debug.h>

#include <UTILS_Error.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>

/* *INDENT-OFF* */
static void* wrapped_start_routine( void* wrappedArg );
static void cleanup_handler( void* wrappedArg );
static inline void record_acquire_lock_event( scorep_pthread_mutex* scorepMutex );
static void issue_process_shared_mutex_warning( void );
/* *INDENT-ON* */

typedef struct scorep_pthread_wrapped_arg scorep_pthread_wrapped_arg;
struct scorep_pthread_wrapped_arg
{
    void*                              ( * orig_start_routine )( void* );
    void*                              orig_arg;
    void*                              orig_ret_val;
    struct scorep_thread_private_data* parent_tpd;
    struct scorep_pthread_wrapped_arg* next;
    uint32_t                           sequence_count;
    int                                detached;
    bool                               cancelled;
    bool                               exited;
};

static size_t
get_reuse_key( scorep_pthread_wrapped_arg* wrappedArg );


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_create )( pthread_t*            thread,
                                            const pthread_attr_t* attr,
                                            void* ( *start_routine )( void* ),
                                            void*                 arg )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_create( thread,
                                      attr,
                                      start_routine,
                                      arg );
    }

    UTILS_DEBUG_ENTRY();

    int detach_state = PTHREAD_CREATE_JOINABLE; /* pthread default */
    if ( attr )
    {
        int result = pthread_attr_getdetachstate( attr, &detach_state );

        if ( detach_state == PTHREAD_CREATE_DETACHED )
        {
            UTILS_WARN_ONCE( "The current thread is in detached state. "
                             "The usage of pthread_detach is considered dangerous in the "
                             "context of Score-P. If the detached thread is still running "
                             "at the end of main, the measurement will fail." );
        }
    }

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_CREATE ],
                               ( intptr_t )__real_pthread_create );
    SCOREP_Location*              location = SCOREP_Location_GetCurrentCPULocation();
    scorep_pthread_location_data* data     =
        SCOREP_Location_GetSubsystemData( location, scorep_pthread_subsystem_id );
    scorep_pthread_wrapped_arg* wrapped_arg = data->free_list;
    if ( wrapped_arg )
    {
        data->free_list   = wrapped_arg->next;
        wrapped_arg->next = NULL;
    }
    else
    {
        wrapped_arg = SCOREP_Location_AllocForMisc( location,
                                                    sizeof( *wrapped_arg ) );
    }
    memset( wrapped_arg, 0, sizeof( *wrapped_arg ) );
    wrapped_arg->cancelled = true;
    wrapped_arg->detached  = detach_state;

    wrapped_arg->orig_start_routine = start_routine;
    wrapped_arg->orig_arg           = arg;
    SCOREP_ThreadCreateWait_Create( SCOREP_PARADIGM_PTHREAD,
                                    &( wrapped_arg->parent_tpd ),
                                    &( wrapped_arg->sequence_count ) );

    /* wrap __real_pthread_create by SCOREP_Enter|Exit? Could be used to calculate
     * Pthread creation overhead. POMP2 does not have SCOREP_Enter|Exit for fork
     * and join.
     */
    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_create( thread,
                                        attr,
                                        &wrapped_start_routine,
                                        ( void* )wrapped_arg );
    SCOREP_EXIT_WRAPPED_REGION();
    UTILS_BUG_ON( result != 0 );
    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_CREATE ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


static void*
wrapped_start_routine( void* wrappedArg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY();

    scorep_pthread_wrapped_arg* wrapped_arg = wrappedArg;
    UTILS_DEBUG( "SequenceCount:%" PRIu32 "", wrapped_arg->sequence_count );

    SCOREP_Location* location;
    SCOREP_ThreadCreateWait_Begin( SCOREP_PARADIGM_PTHREAD,
                                   wrapped_arg->parent_tpd,
                                   wrapped_arg->sequence_count,
                                   get_reuse_key( wrapped_arg ),
                                   &location );

    scorep_pthread_location_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_pthread_subsystem_id );
    data->wrapped_arg = wrapped_arg;

    pthread_cleanup_push( cleanup_handler, location );

    /* Call original start_routine. It might call pthread_exit or the thread
     * might get cancelled, i.e. we might not return from here and have to
     * rely on the cleanup_handler(). */
    SCOREP_IN_MEASUREMENT_DECREMENT();
    wrapped_arg->orig_ret_val = wrapped_arg->orig_start_routine( wrapped_arg->orig_arg );
    SCOREP_IN_MEASUREMENT_INCREMENT();

    /* The cleanup_handler will be called, but we must notify him, that we were
     * not cancelled in time (i.e. before normal thread execution finished) and
     * still use the wrapper_arg for the return value. Note that we cannot
     * maintain scorep_pthread_wrapped_arg.cancelled in the pthread_cancel
     * wrapper because there we just issue a cancellation request that might be
     * acted upon. If we reach this point, it hasn't been acted upon. */
    wrapped_arg->cancelled = false;
    pthread_cleanup_pop( 1 /* execute cleanup handler */ );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();

    return wrapped_arg;
}


static void
cleanup_handler( void* arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IN_WRAPPED_REGION() )
    {
        SCOREP_EXIT_WRAPPED_REGION();
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_Location*              location = arg;
    scorep_pthread_location_data* data     =
        SCOREP_Location_GetSubsystemData( location, scorep_pthread_subsystem_id );
    scorep_pthread_wrapped_arg* wrapped_arg = data->wrapped_arg;

    if ( wrapped_arg->exited )
    {
        SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_EXIT ] );
    }

    if ( wrapped_arg->exited || wrapped_arg->cancelled )
    {
        /*
         * @todo Move to task subsystems deactivate_cpu_location.
         */

        /* A call to pthread_exit will prevent entered regions to be exited
         * the normal way. Therefore, exit remaining regions explicitly. */

        /* A call to pthread_cancel might prevent entered regions to be exited
         * the normal way. If the runtime acts upon a cancellation request,
         * exit remaining regions explicitly. */

        SCOREP_Task_ExitAllRegions( location,
                                    SCOREP_Task_GetCurrentTask( location ) );
    }
    SCOREP_ThreadCreateWait_End( SCOREP_PARADIGM_PTHREAD,
                                 wrapped_arg->parent_tpd,
                                 wrapped_arg->sequence_count );

    if ( wrapped_arg->cancelled ||  wrapped_arg->detached == PTHREAD_CREATE_DETACHED )
    {
        /* wrapped_arg will not be used as return value wrapper, put it in our
         * own free list for later reuse
         */
        wrapped_arg->next = data->free_list;
        data->free_list   = wrapped_arg;
        data->wrapped_arg = NULL;
    }

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_join )( pthread_t thread,
                                          void**    valuePtr )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_join( thread, valuePtr );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_JOIN ],
                               ( intptr_t )__real_pthread_join );

    void* result;
    SCOREP_ENTER_WRAPPED_REGION();
    int status = __real_pthread_join( thread, &result );
    SCOREP_EXIT_WRAPPED_REGION();
    UTILS_BUG_ON( status != 0, "__real_pthread_join failed." );

    if ( result == PTHREAD_CANCELED )
    {
        UTILS_DEBUG( "Thread was cancelled." );
        // Can we provide sequence count to pthread_exit?
        // Do we want to see
        // SCOREP_ThreadCreateWait_Wait( SCOREP_PARADIGM_PTHREAD, sequence_count );???
    }
    else
    {
        UTILS_DEBUG( "result :%p", result );
        scorep_pthread_wrapped_arg* wrapped_arg = result;
        if ( valuePtr )
        {
            *valuePtr = wrapped_arg->orig_ret_val;
        }
        UTILS_DEBUG( "sequence_count :%" PRIu32 "", wrapped_arg->sequence_count );

        SCOREP_ThreadCreateWait_Wait( SCOREP_PARADIGM_PTHREAD,
                                      wrapped_arg->sequence_count );

        SCOREP_Location*              location = SCOREP_Location_GetCurrentCPULocation();
        scorep_pthread_location_data* data     =
            SCOREP_Location_GetSubsystemData( location, scorep_pthread_subsystem_id );
        wrapped_arg->next = data->free_list;
        data->free_list   = wrapped_arg;
    }

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_JOIN ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return status;
}


void
SCOREP_LIBWRAP_FUNC_NAME( pthread_exit )( void* valuePtr )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        __real_pthread_exit( valuePtr );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_EXIT ],
                               ( intptr_t )__real_pthread_exit );
    /* Matching exit will be triggered from cleanup_handler(); */

    SCOREP_Location*              location = SCOREP_Location_GetCurrentCPULocation();
    scorep_pthread_location_data* data     =
        SCOREP_Location_GetSubsystemData( location, scorep_pthread_subsystem_id );
    scorep_pthread_wrapped_arg* wrapped_arg = data->wrapped_arg;
    UTILS_BUG_ON( wrapped_arg == 0 );
    UTILS_BUG_ON( wrapped_arg->orig_ret_val != 0 );

    /* The cleanup_handler will be called, but we must notify him, that
     * we were not cancelled and still use the wrapper_arg for the return value.
     */
    wrapped_arg->cancelled = false;
    wrapped_arg->exited    = true;

    UTILS_DEBUG( "sequence_count :%" PRIu32 "", wrapped_arg->sequence_count );

    wrapped_arg->orig_ret_val = valuePtr;

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    SCOREP_ENTER_WRAPPED_REGION();
    __real_pthread_exit( wrapped_arg );
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cancel )( pthread_t thread )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cancel( thread );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_CANCEL ],
                               ( intptr_t )__real_pthread_cancel );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cancel( thread );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_CANCEL ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_detach )( pthread_t thread )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_detach( thread );
    }

    UTILS_DEBUG_ENTRY();

    UTILS_WARN_ONCE( "The usage of pthread_detach is considered dangerous in the "
                     "context of Score-P. If the detached thread is still running "
                     "at the end of main, the measurement will fail." );

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_DETACH ],
                               ( intptr_t )__real_pthread_detach );

    // If detached thread still runs at finalization time, finalization will
    // fail because it requires serial execution.
    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_detach( thread );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_DETACH ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_mutex_init )( pthread_mutex_t*           pthreadMutex,
                                                const pthread_mutexattr_t* pthreadAttr )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_mutex_init( pthreadMutex, pthreadAttr );
    }

    UTILS_DEBUG_ENTRY();

    // check if the mutex is process shared one
    int process_shared = PTHREAD_PROCESS_PRIVATE;
    if ( pthreadAttr )
    {
        int ignore = pthread_mutexattr_getpshared( pthreadAttr, &process_shared );
    }

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_INIT ],
                               ( intptr_t )__real_pthread_mutex_init );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_mutex_init( pthreadMutex, pthreadAttr );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( 0 == result )
    {
        scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
        if ( !scorep_mutex )
        {
            scorep_mutex = scorep_pthread_mutex_hash_put( pthreadMutex );

            if ( process_shared == PTHREAD_PROCESS_SHARED )
            {
                issue_process_shared_mutex_warning();
                scorep_mutex->process_shared = true;
            }
        }
    }

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_INIT ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


static void
issue_process_shared_mutex_warning( void )
{
    UTILS_WARN_ONCE( "The current mutex is a process shared mutex "
                     "which is currently not supported. "
                     "No events will be recorded." );
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_mutex_destroy )( pthread_mutex_t* pthreadMutex )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_mutex_destroy( pthreadMutex );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_DESTROY ],
                               ( intptr_t )__real_pthread_mutex_destroy );

    scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
    if ( scorep_mutex )
    {
        scorep_pthread_mutex_hash_remove( pthreadMutex );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_mutex_destroy( pthreadMutex );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_DESTROY ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_mutex_lock )( pthread_mutex_t* pthreadMutex )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_mutex_lock( pthreadMutex );
    }

    UTILS_DEBUG_ENTRY();

    scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
    if ( !scorep_mutex  )
    {
        /* Mutex initialized statically via PTHREAD_MUTEX_INITIALIZER and friends. */
        scorep_mutex = scorep_pthread_mutex_hash_put( pthreadMutex );
    }

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_LOCK ],
                               ( intptr_t )__real_pthread_mutex_lock );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_mutex_lock( pthreadMutex );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( result == 0 )
    {
        record_acquire_lock_event( scorep_mutex );
    }

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_LOCK ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


static inline void
record_acquire_lock_event( scorep_pthread_mutex* scorepMutex )
{
    if ( scorepMutex->process_shared == false )
    {
        /* Inside the lock, save to modify scorepMutex object. */
        if ( scorepMutex->nesting_level == 0 )
        {
            scorepMutex->acquisition_order++;
        }
        scorepMutex->nesting_level++;
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_PTHREAD,
                                  scorepMutex->id,
                                  scorepMutex->acquisition_order );
    }
    else
    {
        issue_process_shared_mutex_warning();
    }
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_mutex_unlock )( pthread_mutex_t* pthreadMutex )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_mutex_unlock( pthreadMutex );
    }

    UTILS_DEBUG_ENTRY();

    scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
    UTILS_BUG_ON( scorep_mutex == 0,
                  "Undefined behavior: unlocking unlocked mutex %p", pthreadMutex );
    if ( scorep_mutex->nesting_level == 0 )
    {
        UTILS_WARNING( "Undefined behavior: unlocking unlocked mutex %p", pthreadMutex );
    }

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_UNLOCK ],
                               ( intptr_t )__real_pthread_mutex_unlock );

    if ( scorep_mutex->process_shared == false )
    {
        /* Still inside the lock, save to modify scorep_mutex object. */
        if ( scorep_mutex->nesting_level != 0 )
        {
            scorep_mutex->nesting_level--;
        }
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_PTHREAD,
                                  scorep_mutex->id,
                                  scorep_mutex->acquisition_order );
    }
    else
    {
        issue_process_shared_mutex_warning();
    }

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_mutex_unlock( pthreadMutex );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_UNLOCK ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_mutex_trylock )( pthread_mutex_t* pthreadMutex )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_mutex_trylock( pthreadMutex );
    }

    UTILS_DEBUG_ENTRY();

    scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
    if ( !scorep_mutex  )
    {
        /* Mutex initialized statically via PTHREAD_MUTEX_INITIALIZER and friends. */
        scorep_mutex = scorep_pthread_mutex_hash_put( pthreadMutex );
    }

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_TRYLOCK ],
                               ( intptr_t )__real_pthread_mutex_trylock );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_mutex_trylock( pthreadMutex );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( result == 0 )
    {
        record_acquire_lock_event( scorep_mutex );
    }

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_MUTEX_TRYLOCK ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cond_init )( pthread_cond_t*           cond,
                                               const pthread_condattr_t* attr )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cond_init( cond, attr );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_INIT ],
                               ( intptr_t )__real_pthread_cond_init );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cond_init( cond, attr );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_INIT ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cond_signal )( pthread_cond_t* cond )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cond_signal( cond );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_SIGNAL ],
                               ( intptr_t )__real_pthread_cond_signal );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cond_signal( cond );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_SIGNAL ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cond_broadcast )( pthread_cond_t* cond )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cond_broadcast( cond );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_BROADCAST ],
                               ( intptr_t )__real_pthread_cond_broadcast );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cond_broadcast( cond );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_BROADCAST ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cond_wait )( pthread_cond_t*  cond,
                                               pthread_mutex_t* pthreadMutex )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cond_wait( cond, pthreadMutex );
    }

    UTILS_DEBUG_ENTRY();

    // do we want to see enter/exit for implicit pthread_mutex_unlock here?

    scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
    UTILS_BUG_ON( scorep_mutex == 0,
                  "Pthread mutex %p is required to be locked", pthreadMutex );
    UTILS_BUG_ON( scorep_mutex->nesting_level == 0,
                  "Pthread mutex %p is required to be locked", pthreadMutex );

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_WAIT ],
                               ( intptr_t )__real_pthread_cond_wait );

    if ( scorep_mutex->process_shared == false )
    {
        scorep_mutex->nesting_level--;
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_PTHREAD,
                                  scorep_mutex->id,
                                  scorep_mutex->acquisition_order );
    }
    else
    {
        issue_process_shared_mutex_warning();
    }

    /* Will unlock mutex, wait for cond, then lock mutex again.
       Therefore, don't change the nesting level. */
    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cond_wait( cond, pthreadMutex );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_mutex->process_shared == false )
    {
        scorep_mutex->acquisition_order++;
        scorep_mutex->nesting_level++;
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_PTHREAD,
                                  scorep_mutex->id,
                                  scorep_mutex->acquisition_order );
    }
    else
    {
        issue_process_shared_mutex_warning();
    }

    // do we want to see enter/exit for implicit pthread_mutex_lock here?

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_WAIT ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cond_timedwait )( pthread_cond_t*        cond,
                                                    pthread_mutex_t*       pthreadMutex,
                                                    const struct timespec* time )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cond_timedwait( cond, pthreadMutex, time );
    }

    UTILS_DEBUG_ENTRY();

    // do we want to see enter/exit for implicit pthread_mutex_unlock here?

    scorep_pthread_mutex* scorep_mutex = scorep_pthread_mutex_hash_get( pthreadMutex );
    UTILS_BUG_ON( scorep_mutex == 0,
                  "Pthread mutex %p is required to be locked", pthreadMutex );
    UTILS_BUG_ON( scorep_mutex->nesting_level == 0,
                  "Pthread mutex %p is required to be locked", pthreadMutex );

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_TIMEDWAIT ],
                               ( intptr_t )__real_pthread_cond_timedwait );

    if ( scorep_mutex->process_shared == false )
    {
        scorep_mutex->nesting_level--;
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_PTHREAD,
                                  scorep_mutex->id,
                                  scorep_mutex->acquisition_order );
    }
    else
    {
        issue_process_shared_mutex_warning();
    }

    /* Will unlock mutex, wait for cond, then lock mutex again.
       Therefore, don't change the nesting level. */
    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cond_timedwait( cond, pthreadMutex, time );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_mutex->process_shared == false )
    {
        scorep_mutex->acquisition_order++;
        scorep_mutex->nesting_level++;
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_PTHREAD,
                                  scorep_mutex->id,
                                  scorep_mutex->acquisition_order );
    }
    else
    {
        issue_process_shared_mutex_warning();
    }

    // do we want to see enter/exit for implicit pthread_mutex_lock here?

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_TIMEDWAIT ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}


int
SCOREP_LIBWRAP_FUNC_NAME( pthread_cond_destroy )( pthread_cond_t* cond )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger || !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_pthread_cond_destroy( cond );
    }

    UTILS_DEBUG_ENTRY();

    SCOREP_EnterWrappedRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_DESTROY ],
                               ( intptr_t )__real_pthread_cond_destroy );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_pthread_cond_destroy( cond );
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion( scorep_pthread_regions[ SCOREP_PTHREAD_COND_DESTROY ] );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

size_t
get_reuse_key( scorep_pthread_wrapped_arg* wrappedArg )
{
    switch ( scorep_pthread_reuse_policy )
    {
        case SCOREP_PTHREAD_REUSE_POLICY_NEVER:
            /* A key of 0 indicates in the threading component 'no reuse' at all*/
            return 0;
        case SCOREP_PTHREAD_REUSE_POLICY_SAME_START_ROUTINE:
            /* When we use the address of the start routine as the key,
             * only threads with the same start routine may share the same
             * locations.
             */
            return ( size_t )wrappedArg->orig_start_routine;
        case SCOREP_PTHREAD_REUSE_POLICY_ALWAYS:
            /* Using an arbitrary key different than 0 will reuse locations
             * also for threads which have different start routine.
             */
            return 1;
        default:
            UTILS_FATAL( "Invalid reuse-policy." );
            return 0;
    }
}
