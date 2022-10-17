/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * See 'Score-P and OMPT: Navigating the Perils of Callback-Driven Parallel
 *      Runtime Introspection'
 *      https://hdl.handle.net/2128/23232
 *      https://dx.doi.org/10.1007/978-3-030-28596-8_2
 */

#include "scorep_ompt_callbacks_host.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <SCOREP_InMeasurement.h>
#include <UTILS_Atomic.h>
#include <SCOREP_Location.h>
#include <SCOREP_Thread_Mgmt.h>
#include <SCOREP_Events.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Timer_Ticks.h>


/* Objects corresponding to implicit and explicit tasks. Will be passed from
   callback to callback via argument 'ompt_data_t* task_data'. */
typedef struct task_t
{
    /* for implicit and explicit tasks */
    ompt_task_flag_t type;               /* for consistency checks and
                                          * potentially union. */
    SCOREP_Location* scorep_location;    /* to ensure that
                                          * ompt_finalize_tool events
                                          * are triggered on the
                                          * correct location. */

    struct parallel_t* parallel_region;  /* to which parallel region the
                                          * task belongs. */

    /* Retrieve barrier handle in sync_region_begin, usually contended. Use it
       there for EnterRegion and 'pass' it to sync_region_end for ExitRegion.
       Used for all sync kinds except ibarrier of parallel_region where the
       handle is assigned uncontended in parallel_begin.
       Unused if task does not take part in synchronization. */
    SCOREP_RegionHandle barrier_handle;

    /* for implicit tasks only */
    struct scorep_thread_private_data* tpd; /* Use the tpd stored at itask_begin
                                             * also for itask_end, as runtime
                                             * thread might have changed. */

    uint32_t team_size;                     /* for implicit only, in implicit_task_end,
                                             * triggered from ompt_finalize_tool,
                                             * runtime provides == 0. It is unclear
                                             * what the runtime is supposed to
                                             * provide. */
    uint32_t index;                         /* for implicit only, to be on the safe
                                             * side for implicit_task_end (as it is
                                             * unclear what the runtime needs to
                                             * provide). */
    bool in_overdue_use;                    /* Signal that a thread different than the
                                             * one that triggered itask_begin is
                                             * processing ibarrier_end and itask_end. */
    bool belongs_to_league;                 /* Tasks object belongs to a league. Such
                                             * tasks wont create events in the first
                                             * installment. */

    struct task_t* next;                    /* free list handling */
} task_t;



/* Objects corresponding to parallel regions. Will be passed from callback to
   callback via argument 'ompt_data_t* parallel_data'. */
typedef struct parallel_t
{
    struct scorep_thread_private_data* parent;
    SCOREP_RegionHandle                region;
    uint32_t                           team_size;  /* Might change from requested
                                                    * team size to real team size. */
    uintptr_t                          codeptr_ra; /* Return-address taken from
                                                    * parallel_begin, for cases
                                                    * where no return address is
                                                    * given at parallel_end. Also
                                                    * used for the parallel's ibarrier
                                                    * begin. */

    /* Timestamps used to trigger the overdue events ibarrier_end and itask_end.
       Set by master, used by non-master if already set. Otherwise, current
       timestamp ( < timestamp_i*_end ) is used. */
    uint64_t timestamp_ibarrier_end;
    uint64_t timestamp_itask_end;

    /* Init refcount with actual_paralelism + 1 in itask_begin, then decrement
       in itask_end/ibarrier_end and parallel_end until 0. We cannot simply
       release in parallel_end due to overdueimplicit_barrier/task_end events.
       Don't decrement before initialized (<0). */
    int32_t ref_count;

    bool    belongs_to_league; /* Parallel object belongs to a league.
                                * Such parallel objects wont create
                                * events in the first installment. */

    struct parallel_t* next; /* free list */
} parallel_t;


/* *INDENT-OFF* */
static void on_first_parallel_begin( ompt_data_t* encounteringTaskData );
static void on_initial_task( int flags );
static void implicit_task_end_impl( task_t* task, char* utilsDebugcaller );
static void barrier_implicit_parallel_end( ompt_data_t* taskData );
static void barrier_implicit_parallel_end_finalize_tool( ompt_data_t* taskData );
static void barrier_implicit_parallel_end_impl( task_t* task, char* utilsDebugCaller );
/* *INDENT-ON* */


/* Unique, adapter-local thread id. Initialized in scorep_ompt_cb_thread_begin().
   primary thread's tid == 1 to distinguish from the not-initialized state.
   In debug output, named 'atid' to distinguish from other thread ids. */
static THREAD_LOCAL_STORAGE_SPECIFIER uint32_t adapter_tid;


/* Used for smoother itask_begin error checking and debug output. */
static task_t initial_task;


/* We need to track and pass scorep_thread_private_data parent information from
   parallel_begin to implicit_task_begin. Use this thread local variable to do so. */
static THREAD_LOCAL_STORAGE_SPECIFIER struct scorep_thread_private_data* tpd;


/* free list of parallel_t objects */
static UTILS_Mutex parallel_regions_free_list_mutex = UTILS_MUTEX_INIT;
static parallel_t* parallel_regions_free_list;


/* thread-local free list of implicit and explicit task_t objects. */
static THREAD_LOCAL_STORAGE_SPECIFIER task_t* tasks_free_list;


/* convenience */
#define GET_SUBSYSTEM_DATA( LOCATION, STORED_DATA, STORED_TASK ) \
    task_t* STORED_TASK                        = NULL; \
    scorep_ompt_cpu_location_data* STORED_DATA = \
        SCOREP_Location_GetSubsystemData( LOCATION, scorep_ompt_get_subsystem_id() );


/* convenience */
#define RELEASE_AT_TEAM_END( TASK, OMPT_TASK_DATA ) \
    UTILS_MutexWait(& ( TASK )->in_overdue_use, UTILS_ATOMIC_RELAXED ); \
    release_parallel_region( ( TASK )->parallel_region ); \
    release_task_to_pool( TASK ); \
    ( OMPT_TASK_DATA )->ptr = NULL;


static inline parallel_t*
get_parallel_region_from_pool( void )
{
    UTILS_MutexLock( &parallel_regions_free_list_mutex );
    parallel_t* parallel_region;
    if ( parallel_regions_free_list != NULL )
    {
        parallel_region            = parallel_regions_free_list;
        parallel_regions_free_list = parallel_regions_free_list->next;
    }
    else
    {
        /* For league parallel regions we might have no location.*/
        parallel_region = SCOREP_Memory_AlignedMalloc( SCOREP_CACHELINESIZE,
                                                       sizeof( *parallel_region ) );
        UTILS_BUG_ON( parallel_region == NULL );
    }
    memset( parallel_region, 0,  sizeof( *parallel_region ) );
    UTILS_MutexUnlock( &parallel_regions_free_list_mutex );
    return parallel_region;
}


static inline void
release_parallel_region( parallel_t* parallelRegion )
{
    /* Wait for refcount to become initialized */
    while ( UTILS_Atomic_LoadN_int32( &( parallelRegion->ref_count ),
                                      UTILS_ATOMIC_RELAXED ) < 0 )
    {
        SCOREP_CPU_RELAX;
    }
    if ( UTILS_Atomic_SubFetch_int32( &( parallelRegion->ref_count ), 1,
                                      UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ) == 0 )
    {
        /* return to pool */
        UTILS_MutexLock( &parallel_regions_free_list_mutex );
        parallelRegion->next       = parallel_regions_free_list;
        parallel_regions_free_list = parallelRegion;
        UTILS_MutexUnlock( &parallel_regions_free_list_mutex );
    }
}


static inline task_t*
get_task_from_pool( void )
{
    task_t* data;
    if ( tasks_free_list != NULL )
    {
        data            = tasks_free_list;
        tasks_free_list = tasks_free_list->next;
    }
    else
    {
        /* For league parallel regions we might have no location.*/
        data = SCOREP_Memory_AlignedMalloc( SCOREP_CACHELINESIZE,
                                            sizeof( *data ) );
        UTILS_BUG_ON( data == NULL );
    }
    memset( data, 0,  sizeof( *data ) );
    return data;
}


static void
release_task_to_pool( task_t* task )
{
    task->next      = tasks_free_list;
    tasks_free_list = task;
}


void
scorep_ompt_cb_host_thread_begin( ompt_thread_t thread_type,
                                  ompt_data_t*  thread_data )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_BUG_ON( adapter_tid != 0 );
    static uint32_t thread_counter = 0;
    adapter_tid = UTILS_Atomic_AddFetch_uint32(
        &thread_counter, 1, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    thread_data->value = adapter_tid;

    if ( thread_type & ompt_thread_initial )
    {
        tpd = SCOREP_Thread_GetInitialTpd();
        init_region_fallbacks();
    }

    UTILS_DEBUG( "[%s] atid %" PRIu32 " | thread_type %s",
                 UTILS_FUNCTION_NAME, adapter_tid, thread2string( thread_type ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_thread_end( ompt_data_t* thread_data )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG( "[%s] atid %" PRIu32,
                 UTILS_FUNCTION_NAME, thread_data->value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_parallel_begin( ompt_data_t*        encountering_task_data,
                                    const ompt_frame_t* encountering_task_frame,
                                    ompt_data_t*        parallel_data,
                                    unsigned int        requested_parallelism,
                                    int                 flags,
                                    const void*         codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | encountering_task_data->ptr %p | "
                       "parallel_data->ptr %p | requested_parallelism %d | "
                       "flags %s | codeptr_ra %p ",
                       adapter_tid, encountering_task_data->ptr, parallel_data->ptr,
                       requested_parallelism, parallel_flag2string( flags ),
                       codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    UTILS_BUG_ON( requested_parallelism == 0 );
    UTILS_BUG_ON( parallel_data->ptr != NULL,
                  "Expected no ompt_data_t object for a new parallel region." );

    /* First parallel region runs in serial context. No need for synchronization. */
    static bool first_parallel_encountered = false;
    if ( !first_parallel_encountered
         /* For now, ignore league events. */
         && !( flags & ompt_parallel_league ) && !( ( ( task_t* )encountering_task_data->ptr )->belongs_to_league ) )
    {
        first_parallel_encountered = true;
        on_first_parallel_begin( encountering_task_data );
    }

    /* init a parallel_t object that will be passed around as
       parallel_data->ptr */
    parallel_t* parallel_region = get_parallel_region_from_pool();
    parallel_region->ref_count = -1;

    parallel_data->ptr = parallel_region;

    /* For now, prevent league events. */
    if ( ( ( task_t* )encountering_task_data->ptr )->belongs_to_league ||
         ( flags & ompt_parallel_league ) )
    {
        parallel_region->belongs_to_league = true;
        UTILS_WARN_ONCE( "OpenMP league implicit-task-begin event detected. "
                         "Not handled yet. Score-P might crash on child events." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | parallel_data->ptr %p | "
                          "encountering_task_data->ptr %p | belongs_to_league",
                          adapter_tid, parallel_data->ptr, encountering_task_data->ptr );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    parallel_region->parent     = tpd;
    parallel_region->region     = get_region( codeptr_ra, OMPT_PARALLEL );
    parallel_region->team_size  = requested_parallelism;
    parallel_region->codeptr_ra = ( uintptr_t )codeptr_ra;

    UTILS_BUG_ON( !( flags & ompt_parallel_team ) );

    /* codeptr_ra is also used for the (contended) ibarrier begin later on.
       Thus, create and insert the barrier region handle uncontended into
       the hash table already here. */
    get_region( codeptr_ra, OMPT_IMPLICIT_BARRIER );

    SCOREP_ThreadForkJoin_Fork( SCOREP_PARADIGM_OPENMP, requested_parallelism );

    /* Set subsystem_data's task for this location to NULL as this location will
     * be reused as worker thread 0 within the parallel region. subsystem_data
     * will be restored in parallel_end. */
    GET_SUBSYSTEM_DATA( SCOREP_Location_GetCurrentCPULocation(), stored_data, stored_task );
    UTILS_MutexLock( &stored_data->protect_task_exchange );
    stored_task       = stored_data->task;
    stored_data->task = NULL;
    UTILS_MutexUnlock( &stored_data->protect_task_exchange );
    UTILS_BUG_ON( stored_task == NULL );

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | parallel_data->ptr %p | "
                      "encountering_task_data->ptr %p | parallel_region->region %d",
                      adapter_tid, parallel_data->ptr, encountering_task_data->ptr,
                      SCOREP_RegionHandle_GetId( parallel_region->region ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static void
on_first_parallel_begin( ompt_data_t* encounteringTaskData )
{
    UTILS_BUG_ON( encounteringTaskData->ptr == NULL );
    UTILS_BUG_ON( tpd == NULL );

    GET_SUBSYSTEM_DATA( initial_task.scorep_location, stored_data, stored_task );
    UTILS_MutexLock( &( stored_data->protect_task_exchange ) );
    stored_data->task = encounteringTaskData->ptr;
    UTILS_MutexUnlock( &( stored_data->protect_task_exchange ) );
    //UTILS_BUG_ON( stored_task != NULL );

    UTILS_DEBUG( "[%s] atid %" PRIu32 " | loc %" PRIu32 " | stored_task( %p->%p ) ",
                 UTILS_FUNCTION_NAME, adapter_tid,
                 SCOREP_Location_GetId( initial_task.scorep_location ),
                 stored_task, encounteringTaskData->ptr );
}


void
scorep_ompt_cb_host_parallel_end( ompt_data_t* parallel_data,
                                  ompt_data_t* encountering_task_data,
                                  int          flags,
                                  const void*  codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | parallel_data->ptr %p | "
                       "encountering_task_data->ptr %p | flags %s | codeptr_ra %p",
                       adapter_tid, parallel_data->ptr,
                       encountering_task_data->ptr, parallel_flag2string( flags ),
                       codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    parallel_t* parallel_region = parallel_data->ptr;

    /* For now, prevent league events */
    if ( parallel_region->belongs_to_league )
    {
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | parallel_data->ptr %p | "
                          "encountering_task_data->ptr %p | belongs_to_league" );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    struct scorep_thread_private_data* tpd_from_now_on = NULL;
    SCOREP_ThreadForkJoin_Join( SCOREP_PARADIGM_OPENMP, &tpd_from_now_on );

    /* Reset subsystem data after it was used in previous parallel region. */
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    GET_SUBSYSTEM_DATA( location, stored_data, stored_task );
    UTILS_MutexLock( &stored_data->protect_task_exchange );
    stored_task       = stored_data->task;
    stored_data->task = encountering_task_data->ptr;
    UTILS_MutexUnlock( &stored_data->protect_task_exchange );

    /* For team_size == 1 we don't see ibarrier events, and therefore
       subsystem_data->task is not set to NULL. */
    UTILS_BUG_ON( parallel_region->team_size > 1 && stored_task != NULL );

    #if HAVE( UTILS_DEBUG )
    int index;
    scorep_ompt_get_task_info( 0, NULL, NULL, NULL, NULL, &index );
    SCOREP_Location* loc = SCOREP_Location_GetCurrentCPULocation();
    #endif
    UTILS_DEBUG( "[%s] atid %" PRIu32 " | last_timestamp %" PRIu64 " | loc %" PRIu32
                 " | parallel->ptr %p | task->ptr %p | stored_task( %p->%p ) | "
                 "index %" PRIu32 " | tpd(%p->%p)",
                 UTILS_FUNCTION_NAME, adapter_tid,
                 SCOREP_Location_GetLastTimestamp( loc ),
                 SCOREP_Location_GetId( loc ),
                 parallel_data->ptr,
                 encountering_task_data->ptr,
                 stored_task,
                 encountering_task_data->ptr,
                 index,
                 tpd,
                 tpd_from_now_on );

    tpd = tpd_from_now_on;
    release_parallel_region( parallel_data->ptr );

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static task_t*
new_league_task( ompt_scope_endpoint_t endpoint,
                 ompt_data_t*          parallelData,
                 unsigned int          actualParallelism,
                 unsigned int          index,
                 bool                  initial )
{
    task_t* task = get_task_from_pool();
    if ( initial )
    {
        /* rocm 5.2.0 provides no parallel_data, although it should */
        UTILS_BUG_ON( parallelData->ptr != NULL );
        task->parallel_region   = NULL;
        task->index             = index;
        task->belongs_to_league = true;
    }
    else
    {
        UTILS_BUG_ON( parallelData->ptr == NULL );
        parallel_t* parallel_region = parallelData->ptr;
        task->parallel_region   = parallel_region;
        task->index             = index;
        task->belongs_to_league = true;
        if ( index == 0 )
        {
            UTILS_Atomic_StoreN_int32( &( parallel_region->ref_count ),
                                       ( int32_t )( actualParallelism + 1 ),
                                       UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        }
    }
    UTILS_WARN_ONCE( "OpenMP league implicit-task-begin event detected. "
                     "Not handled yet. Score-P might crash on child events." );
    return task;
}


/*
    typedef enum ompt_task_flag_t {
    ompt_task_initial
    ompt_task_implicit
    ompt_task_explicit
    ompt_task_target
    ompt_task_taskwait
    ompt_task_undeferred
    ompt_task_untied
    ompt_task_final
    ompt_task_mergeable
    ompt_task_merged
    } ompt_task_flag_t
 */

void
scorep_ompt_cb_host_implicit_task( ompt_scope_endpoint_t endpoint,
                                   ompt_data_t*          parallel_data,
                                   ompt_data_t*          task_data,
                                   unsigned int          actual_parallelism,
                                   unsigned int          index, /* thread or team num */
                                   int                   flags )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | endpoint %s | parallel_data->ptr %p | "
                       "task_data->ptr %p | actual_parallelism %d | index %d | "
                       "flags %s", adapter_tid, scope_endpoint2string( endpoint ),
                       parallel_data == NULL ? NULL : parallel_data->ptr,
                       task_data->ptr, actual_parallelism, index,
                       task_flag2string( flags ) );

    /* Special handling for initial thread's initial task (outside of any
       OpenMP construct) and initial tasks of leagues. */
    if ( flags & ompt_task_initial )
    {
        switch ( endpoint )
        {
            case ompt_scope_begin:
            {
                /* Initial thread's initial task */
                if ( initial_task.tpd == NULL )
                {
                    on_initial_task( flags );
                    task_data->ptr = &initial_task;
                    /* If there is a need for parallel_data of the implicit, program-wide
                       parallel region, this is the place to set it. */
                    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | initial_task: task_data->ptr %p | location %p",
                                      adapter_tid, task_data->ptr, initial_task.scorep_location );
                    break;
                }

                /* For now, prevent league events: initial task of league. */
                /* TODO why is parallel_data->ptr == NULL in rocm 5.2.0? parallel was created
                   and binding is the current teams region */
                UTILS_BUG_ON( parallel_data->ptr != NULL );

                task_data->ptr = new_league_task( endpoint, parallel_data, actual_parallelism, index, true );
                UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | index = %d | "
                                  "parallel_data->ptr %p | task_data->ptr %p | "
                                  "belongs_to_league (initial)",
                                  adapter_tid, scope_endpoint2string( endpoint ),
                                  index, parallel_data->ptr, task_data->ptr );
                break;
            }
            case ompt_scope_end:
            {
                task_t* task = task_data->ptr;
                /* Initial thread's initial task */
                if ( task == &initial_task )
                {
                    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | initial_task: task_data->ptr %p | location %p",
                                      adapter_tid, task_data->ptr, initial_task.scorep_location );
                    break;
                }

                /* For now, prevent league events: initial task of league. */
                UTILS_BUG_ON( !task->belongs_to_league );
                /* TODO why is parallel_data->ptr == NULL in rocm 5.2.0? parallel was created
                   and binding is the current teams region */
                UTILS_BUG_ON( task->parallel_region != NULL );
                /* Will crash as relying on valid parallel_data->ptr */
                /* RELEASE_AT_TEAM_END( task, task_data ); */
                UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | "
                                  "parallel_data->ptr %p | task_data->ptr %p "
                                  "| index %d | flags %s | belongs_to_league",
                                  adapter_tid, scope_endpoint2string( endpoint ),
                                  parallel_data == NULL ? NULL : parallel_data->ptr,
                                  task_data->ptr, index, task_flag2string( flags ) );
                break;
            }
            #if HAVE( DECL_OMPT_SCOPE_BEGINEND )
            case ompt_scope_beginend:
                UTILS_BUG( "ompt_scope_beginend not allowed in implicit_task callback" );
                break;
            #endif  /* DECL_OMPT_SCOPE_BEGINEND */
        }
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    switch ( endpoint )
    {
        case ompt_scope_begin:
        {
            /* For now, prevent league events. */
            UTILS_BUG_ON( parallel_data->ptr == NULL );
            if ( ( ( parallel_t* )parallel_data->ptr )->belongs_to_league == true )
            {
                UTILS_BUG_ON( parallel_data->ptr == NULL );
                task_data->ptr = new_league_task( endpoint, parallel_data, actual_parallelism, index, false );
                UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | index = %d | "
                                  "parallel_data->ptr %p | task_data->ptr %p | "
                                  "belongs_to_league",
                                  adapter_tid, scope_endpoint2string( endpoint ),
                                  index, parallel_data->ptr, task_data->ptr );
                break;
            }

            parallel_t*                        parallel_region = parallel_data->ptr;
            struct scorep_thread_private_data* parent          = parallel_region->parent;
            UTILS_BUG_ON( parent == NULL, "Valid parent required." );

            if ( index == 0 ) /* primary thread/team */
            {
                if ( parallel_region->team_size != actual_parallelism )
                {
                    UTILS_WARNING( "Requested team size larger than actual size (%d > %d)",
                                   parallel_region->team_size, actual_parallelism );
                    parallel_region->team_size = actual_parallelism;
                }
                UTILS_Atomic_StoreN_int32( &( parallel_region->ref_count ),
                                           ( int32_t )( actual_parallelism + 1 ),
                                           UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
            }

            struct scorep_thread_private_data* new_tpd = NULL;
            struct SCOREP_Task*                scorep_task;

            /* Triggers overdue handling via subsystem_cb before substrates
             * gets informed and location activated. */
            SCOREP_ThreadForkJoin_TeamBegin(
                SCOREP_PARADIGM_OPENMP,
                ( uint32_t )index,
                ( uint32_t )actual_parallelism,
                0,               /* use ancesterInfo instead of nesting level */
                ( void* )parent, /* ancestorInfo */
                &new_tpd,
                &scorep_task );
            SCOREP_EnterRegion( parallel_region->region );

            SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
            task_t*          task     = get_task_from_pool();

            task->team_size       = actual_parallelism;
            task->index           = index;
            task->type            = ompt_task_implicit;
            task->parallel_region = parallel_region;
            task->tpd             = new_tpd;
            task->scorep_location = location;

            task_data->ptr = task;

            /* store task_data->ptr as subsystem data to be able to trigger
            * ibarrier_end and itask_end from a different thread that gets
            * attached to this location in a subsequent parallel region. */
            GET_SUBSYSTEM_DATA( task->scorep_location, stored_data, stored_task );
            UTILS_Atomic_StoreN_bool( &( stored_data->is_ompt_location ), true,
                                      UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
            UTILS_MutexLock( &stored_data->protect_task_exchange );
            stored_task       = stored_data->task;
            stored_data->task = task_data->ptr;
            UTILS_MutexUnlock( &stored_data->protect_task_exchange );
            UTILS_BUG_ON( stored_task != NULL );

            UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | index = %d | "
                              "parallel_data->ptr %p | task_data->ptr %p | atid %"
                              PRIu64 " | loc %" PRIu32 " | stored_task (%p->%p) "
                              "| tpd (%p->%p) | parent %p",
                              adapter_tid, scope_endpoint2string( endpoint ),
                              index, parallel_data->ptr,
                              task_data->ptr, adapter_tid,
                              SCOREP_Location_GetId( task->scorep_location ),
                              stored_task, task_data->ptr, tpd, new_tpd, parent );

            tpd = new_tpd;
            break;
        }
        case ompt_scope_end:
        {
            task_t* task = task_data->ptr;
            if ( task != NULL )
            {
                /* For now, prevent league events. */
                if ( task->belongs_to_league )
                {
                    RELEASE_AT_TEAM_END( task, task_data );
                    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | "
                                      "parallel_data->ptr %p | task_data->ptr %p "
                                      "| index %d | flags %s | belongs_to_league",
                                      adapter_tid, scope_endpoint2string( endpoint ),
                                      parallel_data == NULL ? NULL : parallel_data->ptr,
                                      task_data->ptr, index, task_flag2string( flags ) );
                    break;
                }

                implicit_task_end_impl( task, " (itask_exit)" );

                /* Release location as this thread wont write
                 * any events to this location. */
                GET_SUBSYSTEM_DATA( task->scorep_location, stored_data, unused );
                UTILS_MutexUnlock( &stored_data->preserve_order );

                RELEASE_AT_TEAM_END( task, task_data );

                UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | parallel_data->ptr"
                                  " %p | task_data->ptr %p | actual_parallelism %d |"
                                  " index %d | flags %s | loc %" PRIu32 " | tpd %p",
                                  adapter_tid, scope_endpoint2string( endpoint ),
                                  parallel_data == NULL ? NULL : parallel_data->ptr,
                                  task_data->ptr, actual_parallelism, index,
                                  task_flag2string( flags ),
                                  SCOREP_Location_GetId( task->scorep_location ), tpd );
            }
            else
            {
                /* Already handled via either, overdue_exit, sync_region_exit,
                   or finalizing_tool.*/
                UTILS_DEBUG_EXIT( "(noop) atid %" PRIu32 " | endpoint %s | "
                                  "parallel_data->ptr %p | task_data->ptr %p "
                                  "| actual_parallelism %d | index %d | flags %s",
                                  adapter_tid, scope_endpoint2string( endpoint ),
                                  parallel_data == NULL ? NULL : parallel_data->ptr,
                                  task_data->ptr, actual_parallelism, index,
                                  task_flag2string( flags ) );
            }

            if ( index != 0 ) /* non-primary */
            {
                tpd = NULL;
            }
            break;
        }
        #if HAVE( DECL_OMPT_SCOPE_BEGINEND )
        case ompt_scope_beginend:
            UTILS_BUG( "ompt_scope_beginend not allowed in implicit_task callback" );
            break;
        #endif /* DECL_OMPT_SCOPE_BEGINEND */
    }

    /* empty asm block to avoid segfaults when configuring --disable-debug (seen
       with Intel compiler and 2018 LLVM RT) */
    asm ( "" );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static void
on_initial_task( int flags )
{
    initial_task.type            = flags;
    initial_task.scorep_location = SCOREP_Location_GetCurrentCPULocation();
    initial_task.tpd             = tpd;
    UTILS_BUG_ON( initial_task.tpd == NULL );
}


SCOREP_ErrorCode
scorep_ompt_subsystem_trigger_overdue_events( struct SCOREP_Location* location )
{
    if ( !scorep_ompt_record_event() )
    {
        UTILS_DEBUG( "[%s] atid %" PRIu32, UTILS_FUNCTION_NAME, adapter_tid );
        return SCOREP_SUCCESS;
    }

    /* called during implicit_task_begin before location activation and
     * substrate events. */

    GET_SUBSYSTEM_DATA( location, stored_data, stored_task );
    /* ignore non-OMPT locations in overdue-processing. Note that a newly created
       OMPT location (itask_begin) is not-yet a OMPT location and there are no
       overdue events. This location will become a OMPT location on itask_begin
       completion. */
    if ( !UTILS_Atomic_LoadN_bool( &( stored_data->is_ompt_location ),
                                   UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ) )
    {
        UTILS_DEBUG( "[%s] atid %" PRIu32 " | loc %" PRIu32 " not a OMPT location (yet)",
                     UTILS_FUNCTION_NAME, adapter_tid, SCOREP_Location_GetId( location ) );
        return SCOREP_SUCCESS;
    }

    /* subsystem data's task is either NULL or task_data->ptr from task that
     * previously wrote into location but whose ibarrier_end and itask_end
     * events were not triggered yet by the runtime (overdue events). Before
     * we write a new itask_begin into location (from a different thread) we
     * need to write the overdue ibarrier_end and itask_end events, otherwise
     * the event order is garbage (timestamp order violation is the symptom).
     * If we write the overdue event here, we also need to prevent the two
     * remaining runtime events to be written once they are triggered by
     * the (hopefully) original thread. We also need to take care that the
     * task object isn't released until both parties (this function and the
     * itask_end callback) are done with their work (see in_overdue_use). */

    UTILS_MutexLock( &stored_data->protect_task_exchange );
    stored_task = stored_data->task;

    if ( stored_task != NULL )
    {
        /* process overdue events */
        UTILS_Atomic_StoreN_bool( &( stored_task->in_overdue_use ),
                                  true, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        stored_data->task = NULL;

        UTILS_DEBUG( "[%s] process_overdue: atid %" PRIu32 " | loc (new) %" PRIu32
                     " | stored_task->loc (overdue) %" PRIu32 " | "
                     "stored_task->parallel_region %p | stored_task->index %" PRIu32
                     " | stored_task (%p->%p) | tpd %p | stored_task->tpd %p",
                     UTILS_FUNCTION_NAME, adapter_tid, SCOREP_Location_GetId( location ),
                     SCOREP_Location_GetId( stored_task->scorep_location ),
                     stored_task->parallel_region, stored_task->index, stored_task,
                     UTILS_Atomic_LoadN_void_ptr( &( stored_data->task ),
                                                  UTILS_ATOMIC_RELAXED ),
                     tpd, stored_task->tpd );

        UTILS_MutexUnlock( &stored_data->protect_task_exchange );

        /* trigger postponed implicit_barrier_end and implicit_task_end events */
        barrier_implicit_parallel_end_impl( stored_task, " (overdue_exit)" );
        implicit_task_end_impl( stored_task, " (overdue_exit)" );

        UTILS_Atomic_StoreN_bool( &( stored_task->in_overdue_use ),
                                  false, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }
    else /* stored_task == NULL */
    {
        /* ignore overdue events, processed by another thread */
        UTILS_DEBUG( "[%s] ignore_overdue: atid %" PRIu32 " | loc %" PRIu32 " | "
                     "stored_task (%p->%p) | tpd %p",
                     UTILS_FUNCTION_NAME, adapter_tid,
                     SCOREP_Location_GetId( location ), stored_task,
                     UTILS_Atomic_LoadN_void_ptr( &( stored_data->task ),
                                                  UTILS_ATOMIC_RELAXED ),
                     tpd );

        UTILS_MutexUnlock( &stored_data->protect_task_exchange );

        /* spin: other thread writes/wrote ibrarrier_end and itask_end on
         * same location. Let other thread finish before proceeding to
         * itask_begin, which will write the next event to this location. */
        UTILS_MutexWait( &stored_data->preserve_order,
                         UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }

    /* proceed to itask_begin */
    return SCOREP_SUCCESS;
}


static void
implicit_task_end_impl( task_t* task, char* utilsDebugCaller )
{
    /* Can be called by a thread different to the one that executed the
     * corresponding itask_begin. */
    UTILS_BUG_ON( task == NULL );

    parallel_t* parallel_region = task->parallel_region;

    /* As non-master itask_barrier/end can be delayed, use the timestamp of
       master for non-master threads, if already available ( !=0 ). Otherwise,
       get the current timestamp. */
    uint64_t timestamp = 0;
    if ( task->index == 0 )
    {
        timestamp = SCOREP_Timer_GetClockTicks();
        UTILS_Atomic_StoreN_uint64( &( parallel_region->timestamp_itask_end ),
                                    timestamp, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }
    else
    {
        timestamp =
            UTILS_Atomic_LoadN_uint64( &( parallel_region->timestamp_itask_end ),
                                       UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        if ( timestamp == 0 )
        {
            timestamp = SCOREP_Timer_GetClockTicks();
        }
    }

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    if ( task->scorep_location != location )
    {
        UTILS_WARNING( "itask_end triggered from location %" PRIu32 " but "
                       "corresponding itask_begin triggered from location %" PRIu32
                       " (finalizing_tool = %s).",
                       SCOREP_Location_GetId( location ),
                       SCOREP_Location_GetId( task->scorep_location ),
                       scorep_ompt_finalizing_tool_in_progress() ? "yes" : "no" );
    }

    UTILS_DEBUG( "[%s%s] atid %" PRIu32 " | loc %" PRIu32 " | task->loc %"
                 PRIu32 " | task->parallel_data %p | task %p | task->index %"
                 PRIu32 " | tpd %p |  task->tpd %p  | timestamp_itask_end %" PRIu64,
                 UTILS_FUNCTION_NAME,
                 utilsDebugCaller, adapter_tid, SCOREP_Location_GetId( location ),
                 SCOREP_Location_GetId( task->scorep_location ), parallel_region,
                 task, task->index, tpd, task->tpd, timestamp );

    /* event might be triggered from location different from the one that
       executed itask_begin; so far, seen in finalize_tool only. */
    SCOREP_Location_ExitRegion( task->scorep_location,
                                timestamp,
                                parallel_region->region );
    /* itask_end might be triggered by a thread different from the one that
       triggered itask_begin. */
    SCOREP_ThreadForkJoin_Tpd_TeamEnd( SCOREP_PARADIGM_OPENMP,
                                       task->tpd,
                                       timestamp,
                                       task->index,
                                       task->team_size );
}


/*
   typedef enum ompt_sync_region_t {
   ompt_sync_region_barrier = 1,          // deprecated since 5.1
   ompt_sync_region_barrier_implicit = 2, // deprecated since 5.1
   ompt_sync_region_barrier_explicit = 3,
   ompt_sync_region_barrier_implementation = 4,
   ompt_sync_region_taskwait = 5,
   ompt_sync_region_taskgroup = 6,
   ompt_sync_region_reduction = 7,
   ompt_sync_region_barrier_implicit_workshare = 8,
   ompt_sync_region_barrier_implicit_parallel = 9,
   ompt_sync_region_barrier_teams = 10
   } ompt_sync_region_t;
 */

void
scorep_ompt_cb_host_sync_region( ompt_sync_region_t    kind,
                                 ompt_scope_endpoint_t endpoint,
                                 ompt_data_t*          parallel_data,
                                 ompt_data_t*          task_data,
                                 const void*           codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | kind %s | endpoint %s | "
                       "parallel_data->ptr %p | task_data->ptr %p | codeptr_ra %p ",
                       adapter_tid, sync_region2string( kind ),
                       scope_endpoint2string( endpoint ),
                       parallel_data == NULL ? NULL : parallel_data->ptr,
                       task_data->ptr, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = task_data->ptr;

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | endpoint %s | "
                          "parallel_data->ptr %p | task_data->ptr %p | codeptr_ra %p | "
                          "belongs_to_league",
                          adapter_tid, sync_region2string( kind ),
                          scope_endpoint2string( endpoint ),
                          parallel_data == NULL ? NULL : parallel_data->ptr,
                          task_data->ptr, codeptr_ra );

        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( endpoint )
    {
        case ompt_scope_begin:
            switch ( kind )
            {
                case ompt_sync_region_barrier:
                {
                    UTILS_FATAL( "Deprecated enum ompt_sync_region_barrier encountered." );
                    break;
                }
                /* rocm-5.2.0 and 5.3.0 declare both, ompt_sync_region_barrier_implicit
                   and ompt_sync_region_barrier_implicit_parallel, but use he former. */
                case ompt_sync_region_barrier_implicit:
                {
                    UTILS_WARN_ONCE( "Deprecated enum ompt_sync_region_barrier_implicit encountered." );
                } /* fall-through into ompt_sync_region_barrier_implicit_parallel intended */
                case ompt_sync_region_barrier_implicit_parallel:
                {
                    /* TODO: distinguish between implicit-barrier-begin and
                       implicit-barrier-wait-begin. IMO, between the two events,
                       explicit task get executed. This gives us more information
                       than opari2 can provide */

                    /* oneAPI 2022.1.2, 2018 LLVM RT:
                       - codeptr_ra == NULL for non-master
                       - ibarrier codeptr_ra == parallel_region->codeptr_ra
                       Due to spec (5.2 p488:27ff) codeptr_ra may be NULL. Thus,
                       Be pragmatic and always use the parallel_region's
                       codeptr_ra. */
                    const void* parallel_codeptr_ra =
                        ( const void* )task->parallel_region->codeptr_ra;
                    UTILS_BUG_ON( task->barrier_handle != SCOREP_INVALID_REGION );
                    task->barrier_handle = get_region( parallel_codeptr_ra,
                                                       OMPT_IMPLICIT_BARRIER );
                    SCOREP_EnterRegion( task->barrier_handle );

                    #if HAVE( UTILS_DEBUG )
                    SCOREP_Location* loc = SCOREP_Location_GetCurrentCPULocation();
                    #endif
                    UTILS_DEBUG( "[%s] begin: atid %" PRIu32 " | loc %" PRIu32 " | "
                                 "task->loc %" PRIu32 " | parallel_data->ptr %p | "
                                 "task %p | task->index %" PRIu32 " | tpd %p | "
                                 "task->tpd %p | barrier %" PRIu32,
                                 UTILS_FUNCTION_NAME, adapter_tid,
                                 SCOREP_Location_GetId( loc ),
                                 SCOREP_Location_GetId( task->scorep_location ),
                                 task->parallel_region, task, task->index,
                                 tpd, task->tpd,
                                 SCOREP_RegionHandle_GetId( task->barrier_handle ) );

                    break;
                }
                default:
                {
                    UTILS_WARNING( "ompt_sync_region_t %s not implemented yet ",
                                   sync_region2string( kind ) );
                }
            }
            break;
        case ompt_scope_end:
            switch ( kind )
            {
                case ompt_sync_region_barrier:
                {
                    UTILS_FATAL( "Deprecated enum ompt_sync_region_barrier encountered." );
                    break;
                }
                /* rocm-5.2.0 and 5.3.0 declare both, ompt_sync_region_barrier_implicit
                   and ompt_sync_region_barrier_implicit_parallel, but use he former. */
                case ompt_sync_region_barrier_implicit:
                {
                    UTILS_WARN_ONCE( "Deprecated enum ompt_sync_region_barrier_implicit encountered." );
                } /* fall-through into ompt_sync_region_barrier_implicit_parallel intended */
                case ompt_sync_region_barrier_implicit_parallel:
                {
                    /* parallel_data == NULL for ompt_sync_region_barrier_implicit_parallel
                       according to spec (5.2 p488:22 */
                    UTILS_BUG_ON( parallel_data != NULL );

                    #if HAVE( UTILS_DEBUG )
                    SCOREP_Location* loc = SCOREP_Location_GetCurrentCPULocation();
                    UTILS_BUG_ON( task->scorep_location == NULL );
                    const char* postfix = "";
                    if ( task->scorep_location != loc )
                    {
                        postfix = " (loc mismatch)";
                    }
                    #endif
                    UTILS_DEBUG( "[%s] end%s: atid %" PRIu32 " | loc %" PRIu32 " | "
                                 "task->loc %" PRIu32 " | parallel_data->ptr %p | "
                                 "task %p | task->index %" PRIu32 " | tpd %p | "
                                 "task->tpd %p ",
                                 UTILS_FUNCTION_NAME, postfix, adapter_tid,
                                 SCOREP_Location_GetId( loc ),
                                 SCOREP_Location_GetId( task->scorep_location ),
                                 task->parallel_region, task, task->index,
                                 tpd, task->tpd );

                    if ( scorep_ompt_finalizing_tool_in_progress() )
                    {
                        barrier_implicit_parallel_end_finalize_tool( task_data );
                    }
                    else
                    {
                        barrier_implicit_parallel_end( task_data );
                    }
                    break;
                }
                default:
                {
                    UTILS_WARNING( "ompt_sync_region_t %s not implemented yet ",
                                   sync_region2string( kind ) );
                }
            }
            break;
        #if HAVE( DECL_OMPT_SCOPE_BEGINEND )
        case ompt_scope_beginend:
            UTILS_BUG( "ompt_scope_beginend not allowed in sync_region callback" );
            break;
        #endif /* DECL_OMPT_SCOPE_BEGINEND */
    }

    /* empty asm block to avoid segfaults when configuring --disable-debug (seen
       with Intel compiler and 2018 LLVM RT) */
    asm ( " " );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static void
barrier_implicit_parallel_end( ompt_data_t* taskData )
{
    /* subsystem data is either NULL, task_data->ptr from this implicit task,
     * or task_data->ptr from another implicit task. Trigger the ibarrier_exit
     * only if subsystem data equals this task's task_data->ptr. In this case,
     * set subsystem data to NULL, leave unchanged otherwise. */

    /*
     * Two actors will compete to write events into task->scorep_location during
     * runtime:
     * (1) this function/cb, always triggered by the runtime at unknown time. It
     *     will write ibarrier_end and potentially itask_end. The runtime thread
     *     executing this function/cb might differ from the one that executed
     *     itask_begin and ibarrier_begin.
     * (2) a different runtime thread starting a different parallel region. It
     *     will write to the same location, thus it needs to make sure that the
     *     overdue ibarrier_end and itask_end events of the previous parallel
     *     region on this location will be written before the itask_begin of
     *     the new region.
     * In case of runtime shutdown, some runtime thread(s?) will trigger overdue
     * events.
     */

    task_t*     task            = taskData->ptr;
    parallel_t* parallel_region = task->parallel_region;

    GET_SUBSYSTEM_DATA( task->scorep_location, stored_data, unused );
    UTILS_MutexLock( &stored_data->preserve_order );
    UTILS_MutexLock( &stored_data->protect_task_exchange );

    if ( stored_data->task == task )
    {
        /* ibarrier_end and itask_end handled from here/via runtime. */
        stored_data->task = NULL;
        UTILS_MutexUnlock( &stored_data->protect_task_exchange );

        UTILS_DEBUG( "[%s] (trigger) atid %" PRIu64 " | task->loc %" PRIu32 " | "
                     "task->parallel_region %p | task %p | task->index %" PRIu32
                     " | tpd %p | task->tpd %p | stored_task( %p->%p ) ",
                     UTILS_FUNCTION_NAME, adapter_tid,
                     SCOREP_Location_GetId( task->scorep_location ),
                     parallel_region, task, task->index, tpd, task->tpd,
                     task, UTILS_Atomic_LoadN_void_ptr( &( stored_data->task ),
                                                        UTILS_ATOMIC_RELAXED ) );

        barrier_implicit_parallel_end_impl( task, " (sync_region_exit)" );

        /* If itask_end timestamp already known, trigger
         * itask_end from here and prevent itask_end
         * callback to do any work. */
        uint64_t timestamp =
            UTILS_Atomic_LoadN_uint64( &( parallel_region->timestamp_itask_end ),
                                       UTILS_ATOMIC_RELAXED );
        if ( timestamp != 0 )
        {
            implicit_task_end_impl( task, " (sync_region_exit)" );
            /* Release location as this thread wont write
             * any additional events to this location. */
            UTILS_MutexUnlock( &stored_data->preserve_order );

            RELEASE_AT_TEAM_END( task, taskData );
        }
        else
        {
            /* Let runtime trigger itask_end: will unlock location_mutex
             * and RELEASE_AT_TEAM_END. */
        }
    }
    else /* stored_data->task != task */
    {
        /* ibarrier_end and itask_end handled via trigger_overdue. */
        UTILS_MutexUnlock( &stored_data->protect_task_exchange );
        /* Release location as this thread wont write
         * any additional events to this location. */
        UTILS_MutexUnlock( &stored_data->preserve_order );

        RELEASE_AT_TEAM_END( task, taskData );

        UTILS_DEBUG( "[%s] (ignore) atid %" PRIu32 " | task->loc %" PRIu32 " | "
                     "task->parallel_region %p | task %p | task->index %" PRIu32
                     " | tpd %p | task->tpd %p | stored_task %p ",
                     UTILS_FUNCTION_NAME, adapter_tid,
                     SCOREP_Location_GetId( task->scorep_location ),
                     parallel_region, task, task->index, tpd, task->tpd,
                     stored_data->task );
    }
}


static void
barrier_implicit_parallel_end_finalize_tool( ompt_data_t* taskData )
{
    /* serialize overdue event processing as runtime threads (and
     * therefore locations) can change between ibarrier_begin and
     * ibarrier_end. */
    static UTILS_Mutex finalize_tool = UTILS_MUTEX_INIT;
    UTILS_MutexLock( &finalize_tool );

    task_t*     task            = taskData->ptr;
    parallel_t* parallel_region = task->parallel_region;

    GET_SUBSYSTEM_DATA( task->scorep_location, stored_data, unused );

    if ( stored_data->task == task )
    {
        stored_data->task = NULL;

        UTILS_DEBUG( "[%s] atid %" PRIu64 " | task->loc %" PRIu32 " | "
                     "task->parallel_region %p | task %p | task->index %" PRIu32
                     " | tpd %p | task->tpd %p | stored_task( %p->%p ) ",
                     UTILS_FUNCTION_NAME, adapter_tid,
                     SCOREP_Location_GetId( task->scorep_location ),
                     parallel_region, task, task->index, tpd, task->tpd, task,
                     UTILS_Atomic_LoadN_void_ptr( &( stored_data->task ),
                                                  UTILS_ATOMIC_RELAXED ) );

        barrier_implicit_parallel_end_impl( task, " (finalizing_tool)" );

        /* itask_end timestamp must be already known, trigger
         * itask_end from here. */
        uint64_t timestamp =
            UTILS_Atomic_LoadN_uint64( &( parallel_region->timestamp_itask_end ),
                                       UTILS_ATOMIC_RELAXED );
        UTILS_BUG_ON( timestamp == 0 );
        implicit_task_end_impl( task, " (finalizing_tool)" );
    }

    RELEASE_AT_TEAM_END( task, taskData );

    UTILS_MutexUnlock( &finalize_tool );
}


static void
barrier_implicit_parallel_end_impl( task_t* task, char* utilsDebugCaller )
{
    parallel_t* parallel_region = task->parallel_region;

    /* Use the timestamp of master, if already set (!=0). Otherwise get the
       current timestamp. */
    uint64_t timestamp = 0;
    if ( task->index == 0 )
    {
        timestamp = SCOREP_Timer_GetClockTicks();
        UTILS_Atomic_StoreN_uint64( &( parallel_region->timestamp_ibarrier_end ),
                                    timestamp, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }
    else
    {
        timestamp =
            UTILS_Atomic_LoadN_uint64( &( parallel_region->timestamp_ibarrier_end ),
                                       UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        if ( timestamp == 0 )
        {
            timestamp = SCOREP_Timer_GetClockTicks();
        }
    }

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    if ( task->scorep_location != location )
    {
        UTILS_WARNING( "itask_end triggered from location %" PRIu32 " but "
                       "corresponding itask_begin triggered from location %" PRIu32
                       " (finalizing_tool = %s).",
                       SCOREP_Location_GetId( location ),
                       SCOREP_Location_GetId( task->scorep_location ),
                       scorep_ompt_finalizing_tool_in_progress() ? "yes " : "no " );
    }

    UTILS_DEBUG( "[%s%s] atid %" PRIu32 " | loc %" PRIu32 " | task->loc %" PRIu32
                 " | task->parallel_region %p | task %p | task->index %" PRIu32
                 "tpd %p | task->tpd %p | barrier %" PRIu32
                 " | timestamp_ibarrier_end %" PRIu64,
                 UTILS_FUNCTION_NAME, utilsDebugCaller, adapter_tid,
                 SCOREP_Location_GetId( location ),
                 SCOREP_Location_GetId( task->scorep_location ),
                 parallel_region, task, task->index, tpd, task->tpd,
                 SCOREP_RegionHandle_GetId( task->barrier_handle ), timestamp );

    UTILS_BUG_ON( task->barrier_handle == SCOREP_INVALID_REGION,
                  "ibarrier_end %s : loc %" PRIu32 " | task %p ",
                  utilsDebugCaller, SCOREP_Location_GetId( task->scorep_location ),
                  task );
    SCOREP_Location_ExitRegion( task->scorep_location,
                                timestamp,
                                task->barrier_handle );
    task->barrier_handle = SCOREP_INVALID_REGION;
}
