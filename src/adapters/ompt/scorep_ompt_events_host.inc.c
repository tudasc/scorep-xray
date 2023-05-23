/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022-2023,
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


/* Squeeze explicit task creation data into 64 bits, see also parallel_t's
   (shift|mask)_requested_parallelism/task_generation_number. task creation
   data is encoded into 64-bit like this:
   [region|requested-parallelism|task-generation-number|new-task-bit] */
static uint8_t  nbits_region;
static uint8_t  shift_region;
static uint8_t  shift_new_task;
static uint64_t mask_region;
static uint64_t mask_new_task;


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

    /* Workshare regions are assigned in <workshare>-begin and passed via task_t
       to <workshare>-end. As workshare constructs might be nested, we keep a
       cacheline-size aligned task-local stack of region handles. For omp single
       we already need two regions, although no nesting of constructs. */
    SCOREP_RegionHandle* workshare_regions; /* aligned */
    uint8_t              workshare_regions_capacity;
    uint8_t              workshare_regions_current;

    /* For dispatch events, we see the begin only. Here, we store data needed for
       a corresponding end event. This data gets used at the next dispatch-begin
       or the corresponding workshare-end. */
    union
    {
        SCOREP_RegionHandle section;
    } dispatch;

    struct SCOREP_Task* scorep_task; /* Score-P task object, mainly for explicit
                                        tasks, see also implicit-task-begin. */

    /* If task is involved in mutex-like synchronization, pass data from acquire
       to acquired callbacks (acquire will then create a mutex objects that is
       passed to released). */
    uint64_t    mutex_acquire_timestamp;
    const void* mutex_acquire_codeptr_ra;

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

    /* for explicit tasks only */
    bool                is_undeferred; /* Undeferred tasks don't create SCOREP_ events. */
    SCOREP_RegionHandle region;

    struct task_t*      next;               /* free list handling */
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

    /* For explicit tasking: */
    /* Squeeze explicit task creation data into 64 bits, see also
     * (shift|mask)_region/new_task */
    uint8_t   shift_requested_parallelism;
    uint8_t   shift_task_generation_number;
    uint32_t  max_explicit_tasks; /* check for overflow */
    uint64_t  mask_requested_parallelism;
    uint64_t  mask_task_generation_number;

    uint32_t* task_generation_numbers; /* implicit-task-local task generation number,
                                          array of size requested_parallelism, indexed
                                          by thread_num in task-create. */

    /* Have a single undeferred task object per parallel region to be able to
       ignore undeferred tasks in task-schedule. Will be passed via
       task_data->ptr. */
    struct task_t      undeferred_task;

    struct parallel_t* next; /* free list */
} parallel_t;


/* *INDENT-OFF* */
static void on_first_parallel_begin( ompt_data_t* encounteringTaskData );
static void init_parallel_obj( parallel_t* parallel, struct scorep_thread_private_data* parent, uint32_t requestedParallelism, const void* codeptrRa, uint32_t refCount );
static void on_initial_task( int flags );
static void implicit_task_end_impl( task_t* task, char* utilsDebugcaller );
static void barrier_implicit_parallel_end( ompt_data_t* taskData );
static void barrier_implicit_parallel_end_finalize_tool( ompt_data_t* taskData );
static void barrier_implicit_parallel_end_impl( task_t* task, char* utilsDebugCaller );
static inline SCOREP_RegionHandle work_begin( task_t* task, const void* codeptrRa, tool_event_t regionType );
static inline void enlarge_region_array( uint8_t* capacity, SCOREP_RegionHandle** regions );
static inline SCOREP_RegionHandle work_end( task_t* task );
static inline uint64_t get_mask( uint32_t width, uint32_t shift );
static inline task_t* get_current_task( void );
static inline void construct_mutex_acquire( task_t* task, const void* codeptrRa );
static inline void construct_mutex_acquired( task_t* task, tool_event_t regionType, tool_event_t regionTypeSblock, ompt_mutex_t kind, ompt_wait_id_t waitId );
static inline void construct_mutex_released( ompt_mutex_t kind, ompt_wait_id_t waitId );
/* *INDENT-ON* */


/* Unique, adapter-local thread id. Initialized in scorep_ompt_cb_thread_begin().
   primary thread's tid == 1 to distinguish from the not-initialized state.
   In debug output, named 'atid' to distinguish from other thread ids. */
static THREAD_LOCAL_STORAGE_SPECIFIER uint32_t adapter_tid;


/* Spec: implicit parallel region:
   An inactive parallel region that is not generated from a parallel construct.
   Implicit parallel regions surround the whole OpenMP program, all target
   regions, and all teams regions.

   We need this as the initial task may take part in schedule events where we
   need to access a (deferred) task's parallel_region. */
static parallel_t implicit_parallel;


/* Spec: initial task:
   An implicit task associated with an implicit parallel region.
   Used for smoother itask_begin error checking and debug output. */
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
    uint32_t*   task_generation_numbers = NULL;
    if ( parallel_regions_free_list != NULL )
    {
        parallel_region            = parallel_regions_free_list;
        parallel_regions_free_list = parallel_regions_free_list->next;
        task_generation_numbers    = parallel_region->task_generation_numbers;
    }
    else
    {
        /* For league parallel regions we might have no location.*/
        parallel_region = SCOREP_Memory_AlignedMalloc( SCOREP_CACHELINESIZE,
                                                       sizeof( *parallel_region ) );
        UTILS_BUG_ON( parallel_region == NULL );
    }
    memset( parallel_region, 0,  sizeof( *parallel_region ) );
    parallel_region->task_generation_numbers = task_generation_numbers;
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
        UTILS_CPU_RELAX;
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
    task_t*              data;
    SCOREP_RegionHandle* workshare_regions          = NULL;
    uint8_t              workshare_regions_capacity = 0;
    if ( tasks_free_list != NULL )
    {
        data                       = tasks_free_list;
        workshare_regions          = data->workshare_regions;
        workshare_regions_capacity = data->workshare_regions_capacity;
        tasks_free_list            = tasks_free_list->next;
    }
    else
    {
        /* For league parallel regions we might have no location.*/
        data = SCOREP_Memory_AlignedMalloc( SCOREP_CACHELINESIZE,
                                            sizeof( *data ) );
        UTILS_BUG_ON( data == NULL );
    }
    memset( data, 0,  sizeof( *data ) );
    data->workshare_regions          = workshare_regions;
    data->workshare_regions_capacity = workshare_regions_capacity;
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

        /* Init the per process shift/mask for explicit task creation. */
        nbits_region = SCOREP_Memory_GetDefinitionHandlesBitWidth();
        shift_region = 64 - nbits_region;
        mask_region  = get_mask( nbits_region, shift_region );
        /* new task bit, makes task_data->value odd. Used in first task-schedule
            to identify 'new task'. From there on, task is even. */
        shift_new_task = 0;
        mask_new_task  = get_mask( 1, shift_new_task );
    }

    UTILS_DEBUG( "[%s] atid %" PRIu32 " | thread_type %s",
                 UTILS_FUNCTION_NAME, adapter_tid, thread2string( thread_type ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static inline uint64_t
get_mask( uint32_t width /* [1,64] */, uint32_t shift /* [0,63] */ )
{
    UTILS_BUG_ON( width - 1 > 63, "width = %" PRIu32 "", width );
    UTILS_BUG_ON( shift > 63, "shift =%" PRIu32 "", shift );
    UTILS_BUG_ON( width + shift > 64, "width = %" PRIu32 " | shift =%" PRIu32 "", width, shift );

    uint64_t mask = UINT64_MAX;
    mask <<= 64 - width;
    mask >>= ( 64 - width - shift );

#if HAVE( UTILS_DEBUG )
    char msg[ 128 ];
    int  len = snprintf( msg, 128, "mask(%" PRIu32 ",%" PRIu32 ")\t", width, shift );
    for ( int i = 64; i >= 1; i-- )
    {
        uint64_t bit = mask << ( 64 - i );
        bit >>= 63;
        snprintf( &msg[ len++ ], 2, "%" PRIu64, bit );
    }
    UTILS_DEBUG( "%s", msg );
#endif /* HAVE( UTILS_DEBUG ) */

    return mask;
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
         /* For now, prevent league events. */
         && !( ( flags & ompt_parallel_league )
               || ( ( task_t* )encountering_task_data->ptr )->belongs_to_league ) )
    {
        first_parallel_encountered = true;
        on_first_parallel_begin( encountering_task_data );
    }

    /* init a parallel_t object that will be passed around as
       parallel_data->ptr */
    parallel_t* parallel_region = get_parallel_region_from_pool();

    /* For now, prevent league events. */
    if ( ( flags & ompt_parallel_league )
         || ( ( task_t* )encountering_task_data->ptr )->belongs_to_league )
    {
        parallel_region->belongs_to_league = true;
        UTILS_WARN_ONCE( "OpenMP league implicit-task-begin event detected. "
                         "Not handled yet. Score-P might crash on child events." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | parallel_data->ptr %p | "
                          "encountering_task_data->ptr %p | belongs_to_league",
                          adapter_tid, parallel_data->ptr, encountering_task_data->ptr );
        parallel_data->ptr = parallel_region;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    init_parallel_obj( parallel_region, tpd, requested_parallelism, codeptr_ra, -1 );
    /* parallel_t object ready */
    parallel_data->ptr = parallel_region;

    UTILS_BUG_ON( !( flags & ompt_parallel_team ) );

    /* codeptr_ra is also used for the (contended) ibarrier begin later on.
       Thus, create and insert the barrier region handle uncontended into
       the hash table already here. */
    get_region( codeptr_ra, TOOL_EVENT_IMPLICIT_BARRIER );

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
                      "encountering_task_data->ptr %p | parallel_region->region %d | "
                      "max_explicit_tasks %" PRIu32,
                      adapter_tid, parallel_data->ptr, encountering_task_data->ptr,
                      SCOREP_RegionHandle_GetId( parallel_region->region ),
                      parallel_region->max_explicit_tasks );
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


static void
init_parallel_obj( parallel_t*                        parallel,
                   struct scorep_thread_private_data* parent,
                   uint32_t                           requestedParallelism,
                   const void*                        codeptrRa,
                   uint32_t                           refCount )
{
    UTILS_BUG_ON( parallel == NULL );
    UTILS_BUG_ON( requestedParallelism == 0 );

    parallel->parent     = parent;
    parallel->team_size  = requestedParallelism;
    parallel->codeptr_ra = ( uintptr_t )codeptrRa;
    parallel->region     = get_region( codeptrRa, TOOL_EVENT_PARALLEL );
    parallel->ref_count  = refCount;

    /* The remainder is for explicit tasking only */

    /* shift/mask for requested_parallelism and task_creation_number are per
       parallel-region. requested_parallelism is out of [1,N], thus encode
       thread_num values [0,N-1]. Use remaining bits for task_creation_number,
       but no more than 32 bits. */

    uint8_t nbits_req_parallelism = 1;
    while ( ( parallel->team_size - 1 ) >> nbits_req_parallelism )
    {
        nbits_req_parallelism++;
    }
    UTILS_BUG_ON( 64 - nbits_region - nbits_req_parallelism - 1 <= 0,
                  "Not enough space to encode task-creation-numbers" );
    uint8_t nbits_remaining       = 64 - nbits_region - nbits_req_parallelism - 1 /* new task bit */;
    uint8_t nbits_task_gen_number = ( nbits_remaining > 32 ) ? 32 : nbits_remaining;

    parallel->shift_requested_parallelism = shift_region - nbits_req_parallelism;
    parallel->mask_requested_parallelism  = get_mask( nbits_req_parallelism,
                                                      parallel->shift_requested_parallelism );
    parallel->max_explicit_tasks           = ( ( uint64_t )1 << nbits_task_gen_number ) - 1;
    parallel->shift_task_generation_number = parallel->shift_requested_parallelism - nbits_task_gen_number;
    parallel->mask_task_generation_number  = get_mask( nbits_task_gen_number,
                                                       parallel->shift_task_generation_number );

    /* Reserve memory for task-generation-number per implicit-task */
    size_t mem = sizeof( uint32_t ) * parallel->team_size;
    parallel->task_generation_numbers = realloc( parallel->task_generation_numbers, mem );
    memset( parallel->task_generation_numbers, 0, mem );

    /* Init task_t object that gets passed around for undeferred tasks. */
    parallel->undeferred_task.is_undeferred   = true;
    parallel->undeferred_task.parallel_region = parallel;
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
                    task_data->ptr     = &initial_task;
                    parallel_data->ptr = &implicit_parallel;
                    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | initial_task: task_data->ptr %p "
                                      "| implicit_parallel: parallel_data->ptr %p "
                                      "| location %p",
                                      adapter_tid, task_data->ptr, parallel_data->ptr,
                                      initial_task.scorep_location );
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
            task->scorep_task     = scorep_task;

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
    init_parallel_obj( &implicit_parallel, NULL, 1, NULL, 0 );

    initial_task.type            = flags;
    initial_task.scorep_location = SCOREP_Location_GetCurrentCPULocation();
    initial_task.tpd             = tpd;
    UTILS_BUG_ON( initial_task.tpd == NULL );
    /* The initial task has no SCOREP_ThreadForkJoin_TaskBegin and TaskEnd
       events. Prevent TaskSwitch events by marking it 'undeferred'. */
    initial_task.is_undeferred   = true;
    initial_task.parallel_region = &implicit_parallel;
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
        {
            const void* ibarrier_codeptr_ra = NULL;
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
                    /* We need to create an ibarrier region handle for either the
                       barrier_implicit_parallel or barrier_implicit_workshare
                       case. In the former case, codeptr_ra might be NULL (as seen
                       in the oneAPI 2022.1.2, 2018 LLVM RT. But then,
                       barrier_implicit_parallel's codeptr_ra and
                       parallel_region->codeptr_ra were identical. */
                    if ( codeptr_ra == NULL )
                    {
                        ibarrier_codeptr_ra = ( const void* )task->parallel_region->codeptr_ra;
                    }
                    else
                    {
                        ibarrier_codeptr_ra = codeptr_ra;
                    }
                }  /* fall-through into ompt_sync_region_barrier_implicit_parallel intended */
                case ompt_sync_region_barrier_implicit_parallel:
                {
                    /* TODO: distinguish between implicit-barrier-begin and
                       implicit-barrier-wait-begin. IMO, between the two events,
                       explicit task get executed. This gives us more information
                       than opari2 can provide */

                    if ( ibarrier_codeptr_ra == NULL )
                    {
                        ibarrier_codeptr_ra = ( const void* )task->parallel_region->codeptr_ra;
                    }
                    UTILS_BUG_ON( task->barrier_handle != SCOREP_INVALID_REGION );
                    task->barrier_handle = get_region( ibarrier_codeptr_ra,
                                                       TOOL_EVENT_IMPLICIT_BARRIER );
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
                case ompt_sync_region_barrier_explicit:
                {
                    UTILS_BUG_ON( task->barrier_handle != SCOREP_INVALID_REGION );
                    task->barrier_handle = get_region( codeptr_ra, TOOL_EVENT_BARRIER );
                    SCOREP_EnterRegion( task->barrier_handle );
                    break;
                }
                case ompt_sync_region_taskwait:
                    UTILS_BUG_ON( task->barrier_handle != SCOREP_INVALID_REGION );
                    task->barrier_handle = get_region( codeptr_ra, TOOL_EVENT_TASKWAIT );
                    SCOREP_EnterRegion( task->barrier_handle );
                    break;
                case ompt_sync_region_taskgroup:
                    UTILS_BUG_ON( task->barrier_handle != SCOREP_INVALID_REGION );
                    task->barrier_handle = get_region( codeptr_ra, TOOL_EVENT_TASKGROUP );
                    SCOREP_EnterRegion( task->barrier_handle );
                    break;
                case ompt_sync_region_barrier_implicit_workshare:
                {
                    UTILS_BUG_ON( task->barrier_handle != SCOREP_INVALID_REGION );
                    task->barrier_handle = get_region( codeptr_ra,
                                                       TOOL_EVENT_IMPLICIT_BARRIER );
                    SCOREP_EnterRegion( task->barrier_handle );
                    break;
                }
                default:
                {
                    UTILS_WARNING( "ompt_sync_region_t %s not implemented yet ",
                                   sync_region2string( kind ) );
                }
            }
            break;
        }
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
                    if ( parallel_data != NULL ) /* ibarrier inside parallel region */
                    {
                        task_t* task = task_data->ptr;
                        SCOREP_ExitRegion( task->barrier_handle );
                        task->barrier_handle = SCOREP_INVALID_REGION;
                        break;
                    }
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
                case ompt_sync_region_barrier_explicit:
                    UTILS_BUG_ON( task->barrier_handle == SCOREP_INVALID_REGION );
                    SCOREP_ExitRegion( task->barrier_handle );
                    task->barrier_handle = SCOREP_INVALID_REGION;
                    break;
                case ompt_sync_region_taskwait:
                    UTILS_BUG_ON( task->barrier_handle == SCOREP_INVALID_REGION );
                    SCOREP_ExitRegion( task->barrier_handle );
                    task->barrier_handle = SCOREP_INVALID_REGION;
                    break;
                case ompt_sync_region_taskgroup:
                    UTILS_BUG_ON( task->barrier_handle == SCOREP_INVALID_REGION );
                    SCOREP_ExitRegion( task->barrier_handle );
                    task->barrier_handle = SCOREP_INVALID_REGION;
                    break;
                case ompt_sync_region_barrier_implicit_workshare:
                {
                    task_t* task = task_data->ptr;
                    SCOREP_ExitRegion( task->barrier_handle );
                    task->barrier_handle = SCOREP_INVALID_REGION;
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


/*
   typedef enum ompt_work_t {
   ompt_work_loop = 1,
   ompt_work_sections = 2,
   ompt_work_single_executor = 3,
   ompt_work_single_other = 4,
   ompt_work_workshare = 5,
   ompt_work_distribute = 6,
   ompt_work_taskloop = 7,
   ompt_work_scope = 8,
   ompt_work_loop_static = 10,
   ompt_work_loop_dynamic = 11,
   ompt_work_loop_guided = 12,
   ompt_work_loop_other = 13
   } ompt_work_t;
 */

void
scorep_ompt_cb_host_work( ompt_work_t           work_type,
                          ompt_scope_endpoint_t endpoint,
                          ompt_data_t*          parallel_data,
                          ompt_data_t*          task_data,
                          uint64_t              count,
                          const void*           codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | endpoint %s | wstype %s | "
                       "parallel_data->ptr %p | task_data->ptr %p | count %"
                       PRIu64 " | codeptr_ra %p",
                       adapter_tid, scope_endpoint2string( endpoint ), work2string( work_type ),
                       parallel_data == NULL ? NULL : parallel_data->ptr, task_data->ptr,
                       count, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = task_data->ptr;

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league workshare event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | wstype %s | "
                          "parallel_data->ptr %p | task_data->ptr %p | count %"
                          PRIu64 " | codeptr_ra %p | belongs_to_league",
                          adapter_tid, work2string( work_type ), scope_endpoint2string( endpoint ),
                          parallel_data == NULL ? NULL : parallel_data->ptr, task_data->ptr,
                          count, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( endpoint )
    {
        case ompt_scope_begin:
            switch ( work_type )
            {
                case ompt_work_loop:
                    SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_LOOP ) );
                    break;
                case ompt_work_sections:
                    SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_SECTIONS ) );
                    task->dispatch.section = SCOREP_INVALID_REGION;
                    break;
                case ompt_work_single_executor:
                    SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_SINGLE ) );
                    SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_SINGLE_SBLOCK ) );
                    break;
                case ompt_work_single_other:
                    SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_SINGLE ) );
                    break;
                case ompt_work_workshare:
                    SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_WORKSHARE ) );
                    break;
                default:
                    UTILS_WARNING( "ompt_work_t %s not implemented yet ",
                                   work2string( work_type ) );
            }
            break;
        case ompt_scope_end:
            switch ( work_type )
            {
                case ompt_work_loop:
                    SCOREP_ExitRegion( work_end( task ) );
                    break;
                case ompt_work_sections:
                    /* Exit last section, it was entered in dispatch cb. */
                    if ( task->dispatch.section != SCOREP_INVALID_REGION )
                    {
                        SCOREP_ExitRegion( task->dispatch.section );
                        task->dispatch.section = SCOREP_INVALID_REGION;
                    }
                    SCOREP_ExitRegion( work_end( task ) );
                    break;
                case ompt_work_single_executor:
                    SCOREP_ExitRegion( work_end( task ) );
                    SCOREP_ExitRegion( work_end( task ) );
                    break;
                case ompt_work_single_other:
                    SCOREP_ExitRegion( work_end( task ) );
                    break;
                case ompt_work_workshare:
                    SCOREP_ExitRegion( work_end( task ) );
                    break;
                default:
                    UTILS_WARNING( "ompt_work_t %s not implemented yet ",
                                   work2string( work_type ) );
            }
            break;
        #if HAVE( DECL_OMPT_SCOPE_BEGINEND )
        case ompt_scope_beginend:
            UTILS_BUG( "ompt_scope_beginend not allowed in work callback" );
            break;
        #endif /* DECL_OMPT_SCOPE_BEGINEND */
    }

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static inline SCOREP_RegionHandle
work_begin( task_t*      task,
            const void*  codeptrRa,
            tool_event_t regionType )
{
    if ( task->workshare_regions_current == task->workshare_regions_capacity )
    {
        enlarge_region_array( &( task->workshare_regions_capacity ), &( task->workshare_regions ) );
    }

    UTILS_BUG_ON( task->workshare_regions[ task->workshare_regions_current ] != SCOREP_INVALID_REGION );
    SCOREP_RegionHandle region = get_region( codeptrRa, regionType );
    task->workshare_regions[ task->workshare_regions_current++ ] = region;
    return region;
}


/* Aligned-enlarge @a regions array by one SCOREP_CACHELINESIZE and update
   @a capacity. Contents gets copied. @a capacity up to UINT8_MAX is supported */
static inline void
enlarge_region_array( uint8_t* capacity, SCOREP_RegionHandle** regions )
{
    UTILS_BUG_ON( *capacity + SCOREP_CACHELINESIZE / sizeof( SCOREP_RegionHandle ) > UINT8_MAX,
                  "Cannot handle capacity of region array > " PRIu8 ".", UINT8_MAX );

    size_t size = *capacity == 0 ?
                  SCOREP_CACHELINESIZE :
                  *capacity * sizeof( SCOREP_RegionHandle ) + SCOREP_CACHELINESIZE;

    SCOREP_RegionHandle* new_regions = SCOREP_Memory_AlignedMalloc( SCOREP_CACHELINESIZE, size );
    UTILS_BUG_ON( new_regions == NULL );
    memcpy( new_regions, *regions, *capacity * sizeof( SCOREP_RegionHandle ) );
    SCOREP_Memory_AlignedFree( *regions );

    unsigned new_capacity = size / sizeof( SCOREP_RegionHandle );
    for ( unsigned i = *capacity; i < new_capacity; i++ )
    {
        new_regions[ i ] = SCOREP_INVALID_REGION;
    }

    *regions  = new_regions;
    *capacity = ( uint8_t )new_capacity;
}


static inline SCOREP_RegionHandle
work_end( task_t* task )
{
    UTILS_BUG_ON( task->workshare_regions_current == 0 );
    SCOREP_RegionHandle region = task->workshare_regions[ --task->workshare_regions_current ];
    task->workshare_regions[ task->workshare_regions_current ] = SCOREP_INVALID_REGION;
    UTILS_BUG_ON( region == SCOREP_INVALID_REGION );
    return region;
}


/*
   typedef enum ompt_task_flag_t {
    ompt_task_initial    = 0x00000001,
    ompt_task_implicit   = 0x00000002,
    ompt_task_explicit   = 0x00000004,
    ompt_task_target     = 0x00000008,
    ompt_task_taskwait   = 0x00000010,
    ompt_task_undeferred = 0x08000000,
    ompt_task_untied     = 0x10000000,
    ompt_task_final      = 0x20000000,
    ompt_task_mergeable  = 0x40000000,
    ompt_task_merged     = 0x80000000
   } ompt_task_flag_t;
 */


void
scorep_ompt_cb_host_task_create( ompt_data_t*        encountering_task_data,
                                 const ompt_frame_t* encountering_task_frame,
                                 ompt_data_t*        new_task_data,
                                 int                 flags,
                                 int                 has_dependences,
                                 const void*         codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu64 " | encountering_task_data->ptr %p | "
                       "new_task_data->ptr %p | flags %s", adapter_tid,
                       encountering_task_data == NULL ? NULL : encountering_task_data->ptr,
                       new_task_data == NULL ? NULL : new_task_data->ptr,
                       task_flag2string( flags ) );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    /* For now, prevent league events */
    task_t* task = encountering_task_data->ptr;
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league task-create event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu64 " | encountering_task_data->ptr %p | "
                          "new_task_data->ptr %p | flags %s | belongs_to_league", adapter_tid,
                          encountering_task_data == NULL ? NULL : encountering_task_data->ptr,
                          new_task_data == NULL ? NULL : new_task_data->ptr,
                          task_flag2string( flags ) );
        new_task_data->value = 0;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( !( flags & ompt_task_explicit ), "Expected explicit task only." );

    UTILS_BUG_ON( flags & ompt_task_taskwait, "taskwait-init not supported yet." );

    /* No scheduling events occur when switching to or from a merged task ... */
    if ( flags & ompt_task_merged )
    {
        new_task_data->ptr = encountering_task_data->ptr;
        UTILS_DEBUG_EXIT( "ompt_task_merged: atid %" PRIu32, adapter_tid );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* ...; for the others, assume they trigger task-schedule events. */

    /* We need to transfer the following to task-schedule:
       - for SCOREP_ThreadForkJoin_TaskBegin:
         - this thread's thread_num,
         - a thread-local and parallel-region-local task generation number,
         - a region handle generated from codeptr_ra,
       - communicate 'undeferred' (no creation of Score-P events), and
       - a flag that this task is new, i.e., created but waiting to be resumed.
       We use the 64 bit available in new_task_data->value. This way we prevent
       an additional allocation on thread A that might be released on thread B,
       which would lead to draining one thread's memory pool faster than others. */

    /* Undeferred tasks:
       Execute immediately, thus don't generate
       SCOREP_ThreadForkJoin_TaskCreate|Switch|End events. Communicate
       'undeferred' to task-schedule by passing SCOREP_INVALID_REGION. */
    if ( flags & ompt_task_undeferred )
    {
        uint64_t new_task = 0;
        new_task = ( uint64_t )SCOREP_INVALID_REGION << shift_region |
                   ( uint64_t )1 << shift_new_task;
        new_task_data->value = new_task;
        UTILS_DEBUG_EXIT( "(creating) undeferred task: atid %" PRIu32, adapter_tid );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* Other tasks:
       Assign region names as known from OPARI2. Finer grained naming possible,
       though. */
    SCOREP_RegionHandle task_region;
    if ( flags & ompt_task_untied )
    {
        task_region = get_region( codeptr_ra, TOOL_EVENT_TASK_UNTIED );
    }
    else
    {
        task_region = get_region( codeptr_ra, TOOL_EVENT_TASK );
    }
    UTILS_BUG_ON( task_region == SCOREP_INVALID_REGION );

    /* Collect and encode data into new_task */

    /* Note: creation of non-undeferred tasks from the implicit-parallel-region
       has not been observed yet but should work as we maintain
       'implicit_parallel'. */

    int thread_num;
    scorep_ompt_get_task_info( 0, NULL, NULL, NULL, NULL, &thread_num );
    parallel_t* parallel = ( ( task_t* )encountering_task_data->ptr )->parallel_region;
    /* TODO: What todo on overflow? */
    UTILS_BUG_ON( parallel->max_explicit_tasks == parallel->task_generation_numbers[ thread_num ],
                  "Cannot handle more than %" PRIu32 " task creations.",
                  parallel->max_explicit_tasks );
    uint32_t task_generation_number = ++( parallel->task_generation_numbers[ thread_num ] );

    uint64_t new_task = 0;
    new_task = ( uint64_t )task_region << shift_region |
               ( uint64_t )thread_num << parallel->shift_requested_parallelism |
               ( uint64_t )task_generation_number << parallel->shift_task_generation_number |
               ( uint64_t )1 << shift_new_task;

    new_task_data->value = new_task;

    /* Corresponding OPARI2 instrumentation and
       POMP2_Task_create_begin/POMP2_Task_create_end create a different event
       sequence: the exit appears after the structured block of the task. With
       OMPT, we cannot intercept this point.
       The duration of 'task_create' here is pure scorep time, thus
       provide same timestamp to enter/exit(task_create). */
    SCOREP_RegionHandle task_create = get_region( codeptr_ra, TOOL_EVENT_TASK_CREATE );
    SCOREP_Location*    location    = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            timestamp   = SCOREP_Timer_GetClockTicks();

    SCOREP_Location_EnterRegion( location, timestamp, task_create );
    SCOREP_ThreadForkJoin_TaskCreate( SCOREP_PARADIGM_OPENMP,
                                      thread_num,
                                      task_generation_number );
    SCOREP_Location_ExitRegion( location, timestamp, task_create );

    UTILS_DEBUG_EXIT( "(creating) task: atid %" PRIu32 " | thread_num % d | "
                      "task_generation_number % " PRIu32 " | task % " PRIu64,
                      adapter_tid, thread_num, task_generation_number, new_task );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/*
   typedef enum ompt_task_status_t {
     ompt_task_complete      = 1,
     ompt_task_yield         = 2,
     ompt_task_cancel        = 3,
     ompt_task_detach        = 4,
     ompt_task_early_fulfill = 5,
     ompt_task_late_fulfill  = 6,
     ompt_task_switch        = 7,
     ompt_taskwait_complete  = 8
   } ompt_task_status_t;
 */


void
scorep_ompt_cb_host_task_schedule( ompt_data_t*       prior_task_data,
                                   ompt_task_status_t prior_task_status,
                                   ompt_data_t*       next_task_data )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | prior_task_data->ptr %p | prior_task_status %s | "
                       "next_task_data->ptr %p | next_task_data->value %" PRIu64,
                       adapter_tid, prior_task_data == NULL ? NULL : prior_task_data->ptr,
                       task_status2string( prior_task_status ),
                       next_task_data == NULL ? NULL : next_task_data->ptr,
                       next_task_data == NULL ? 0 : next_task_data->value );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    UTILS_BUG_ON( !( prior_task_status == ompt_task_switch )
                  && !( prior_task_status == ompt_task_complete ),
                  "Only prior_task_status complete and switch supported." );

    task_t* prior_task = prior_task_data->ptr;

    /* For now, prevent league events. We need to take prior and next task
       into account. This is asking for trouble. */
    if ( prior_task->belongs_to_league || next_task_data->value == 0 )
    {
        UTILS_WARN_ONCE( "OpenMP league task-schedule event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | prior_task_data->ptr %p | prior_task_status %s | "
                          "next_task_data->ptr %p | next_task_data->value %" PRIu64 " | "
                          "belongs_to_league",
                          adapter_tid, prior_task_data == NULL ? NULL : prior_task_data->ptr,
                          task_status2string( prior_task_status ),
                          next_task_data == NULL ? NULL : next_task_data->ptr,
                          next_task_data == NULL ? 0 : next_task_data->value );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( prior_task_status == ompt_taskwait_complete,
                  "taskwait-complete not supported yet" );

    /* Handle prior_task */
    if ( prior_task_status == ompt_task_complete )
    {
        if ( !prior_task->is_undeferred )
        {
            UTILS_BUG_ON( !( prior_task->type & ompt_task_explicit ),
                          "Expected only explicit tasks to show up with status "
                          "ompt_task_complete." );
            SCOREP_ThreadForkJoin_TaskEnd( SCOREP_PARADIGM_OPENMP,
                                           prior_task->region,
                                           prior_task->scorep_task );
            release_task_to_pool( prior_task );
            UTILS_DEBUG( "(completing) task %p | atid %" PRIu32,
                         prior_task, adapter_tid );
        }
        else
        {
            UTILS_DEBUG( "(completing) undeferred task %p | atid %" PRIu32,
                         prior_task, adapter_tid );
        }
        prior_task_data->ptr = NULL;
    }
    else
    {
        UTILS_DEBUG( "(suspending) task %p | atid %" PRIu32,
                     prior_task, adapter_tid );
    }


    /* Handle next_task. */
    UTILS_BUG_ON( next_task_data->ptr == NULL,
                  "task_data not initialized, should not happen." );

    if ( next_task_data->value & mask_new_task )
    {
        /* First task-schedule for next_task. Extract data collected in
           task-create which was passed via next_task_data->value and create
           real task object. */
        uint64_t            task_create_data = next_task_data->value;
        SCOREP_RegionHandle region           = ( task_create_data & mask_region ) >> shift_region;
        parallel_t*         parallel_region  = prior_task->parallel_region;

        if ( region == SCOREP_INVALID_REGION )
        {
            /* Undeferred task, don't trigger SCOREP_ThreadForkJoin_TaskBegin.
               Pass parallel-region's undeferred_task around. */
            next_task_data->ptr = &( parallel_region->undeferred_task );
            UTILS_DEBUG( "(starting) undeferred task %p | atid %" PRIu32,
                         next_task_data->ptr, adapter_tid );
        }
        else
        {
            uint32_t thread_num = ( task_create_data
                                    & parallel_region->mask_requested_parallelism )
                                  >> parallel_region->shift_requested_parallelism;
            uint32_t task_generation_number = ( task_create_data
                                                & parallel_region->mask_task_generation_number )
                                              >> parallel_region->shift_task_generation_number;

            task_t* next_task = get_task_from_pool();
            next_task->type            = ompt_task_explicit;
            next_task->parallel_region = parallel_region;
            next_task->region          = region;
            next_task->scorep_task     = SCOREP_ThreadForkJoin_TaskBegin( SCOREP_PARADIGM_OPENMP,
                                                                          region,
                                                                          thread_num,
                                                                          task_generation_number );

            next_task_data->ptr = next_task;

            UTILS_DEBUG( "(starting) task %p | atid %" PRIu32 " | thread_num %d | "
                         "task_generation_number %" PRIu32 " | task % " PRIu64,
                         next_task, adapter_tid, thread_num, task_generation_number,
                         next_task_data->value );
        }
    }
    else
    {
        task_t* next_task = next_task_data->ptr;
        if ( !next_task->is_undeferred )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP, next_task->scorep_task );
            UTILS_DEBUG( "(resuming) task %p | atid %" PRIu32,
                         next_task, adapter_tid );
        }
        else
        {
            UTILS_DEBUG( "(resuming) undeferred task %p | atid %" PRIu32,
                         next_task, adapter_tid );
        }
    }

    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_masked( ompt_scope_endpoint_t endpoint,
                            ompt_data_t*          parallel_data,
                            ompt_data_t*          task_data,
                            const void*           codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | endpoint %s | parallel_data->ptr %p | "
                       "task_data->ptr %p | codeptr_ra %p",
                       adapter_tid, scope_endpoint2string( endpoint ),
                       parallel_data == NULL ? NULL : parallel_data->ptr, task_data->ptr,
                       codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = task_data->ptr;

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league masked event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | parallel_data->ptr %p | "
                          "task_data->ptr %p | codeptr_ra %p",
                          adapter_tid, scope_endpoint2string( endpoint ),
                          parallel_data == NULL ? NULL : parallel_data->ptr, task_data->ptr,
                          codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( endpoint )
    {
        case ompt_scope_begin:
            SCOREP_EnterRegion( work_begin( task, codeptr_ra, TOOL_EVENT_MASKED ) );
            break;
        case ompt_scope_end:
            SCOREP_ExitRegion( work_end( task ) );
            break;
        #if HAVE( DECL_OMPT_SCOPE_BEGINEND )
        case ompt_scope_beginend:
            UTILS_BUG( "ompt_scope_beginend not allowed in masked callback" );
            break;
        #endif /* DECL_OMPT_SCOPE_BEGINEND */
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | parallel_data->ptr %p | "
                      "task_data->ptr %p | codeptr_ra %p",
                      adapter_tid, scope_endpoint2string( endpoint ),
                      parallel_data == NULL ? NULL : parallel_data->ptr, task_data->ptr,
                      codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/*
   typedef enum ompt_mutex_t {
   ompt_mutex_lock = 1,
   ompt_mutex_test_lock = 2,
   ompt_mutex_nest_lock = 3,
   ompt_mutex_test_nest_lock = 4,
   ompt_mutex_critical = 5,
   ompt_mutex_atomic = 6,
   ompt_mutex_ordered = 7
   } ompt_mutex_t;
 */

void
scorep_ompt_cb_host_mutex_acquire( ompt_mutex_t   kind,
                                   unsigned int   hint,
                                   unsigned int   impl,
                                   ompt_wait_id_t wait_id,
                                   const void*    codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                       adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = get_current_task();

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league mutex_acquire event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                          adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( kind )
    {
        case ompt_mutex_lock:
            SCOREP_EnterRegion( lock_regions[ TOOL_LOCK_EVENT_SET ] );
            break;
        case ompt_mutex_nest_lock:
            /* nest-lock-acquire event. Followed by either nest-lock-acquired
               in scorep_ompt_cb_host_mutex_acquired() or nest-lock-owned in
               scorep_ompt_cb_host_nest_lock(). */
            SCOREP_EnterRegion( lock_regions[ TOOL_LOCK_EVENT_SET_NEST ] );
            break;
        case ompt_mutex_critical:
            construct_mutex_acquire( task, codeptr_ra );
            break;
        case ompt_mutex_atomic:
            /* Intentionally ignore atomic events. */
            break;
        case ompt_mutex_ordered:
            construct_mutex_acquire( task, codeptr_ra );
            break;
        default:
            UTILS_WARNING( "mutex kind %s not implemented yet.", mutex2string( kind ) );
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                      adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static inline task_t*
get_current_task( void )
{
    ompt_data_t* task_data_rt = NULL;
    /* Runtime entry point returns 2 if a task region exists at the specified
       ancestor level and the information is available. */
    int ret = scorep_ompt_get_task_info( 0, NULL, &task_data_rt, NULL, NULL, NULL );
    UTILS_BUG_ON( ret != 2, "ompt_get_task_info() returned %d", ret );
    UTILS_BUG_ON( task_data_rt->ptr == NULL );
    return task_data_rt->ptr;
}


static inline void
construct_mutex_acquire( task_t* task, const void* codeptrRa )
{
    /* This data will be accessed in mutex_acquired. There will be no
       event between acquire and acquired, so this is safe. There is
       no guarantee that the return addresses in acquire, acquired,
       and released are identical, so just use the one from acquire
       to create the Score-P region handles. */
    task->mutex_acquire_timestamp  = SCOREP_Timer_GetClockTicks();
    task->mutex_acquire_codeptr_ra = codeptrRa;
}


void
scorep_ompt_cb_host_mutex_acquired( ompt_mutex_t   kind,
                                    ompt_wait_id_t wait_id,
                                    const void*    codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                       adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = get_current_task();

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league mutex_acquired event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                          adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( kind )
    {
        case ompt_mutex_lock:
        {
            mutex_obj_t* mutex = mutex_get( wait_id, kind );
            UTILS_MutexLock( &( mutex->in_release_operation ) );
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, mutex->id, ++( mutex->acquisition_order ) );
            SCOREP_ExitRegion( lock_regions[ TOOL_LOCK_EVENT_SET ] );
        }
        break;
        case ompt_mutex_nest_lock:
        {
            /* nest-lock-acquired event. Followed by nest-lock-acquire (i.e.
               nesting) in scorep_ompt_cb_host_mutex_acquire() or
               nest-lock-release in scorep_ompt_cb_host_nest_lock(). */
            mutex_obj_t* mutex = mutex_get( wait_id, kind );
            UTILS_MutexLock( &( mutex->in_release_operation ) );
            if ( mutex->optional.nest_level == 0 )
            {
                mutex->acquisition_order++;
            }
            mutex->optional.nest_level++;
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, mutex->id, mutex->acquisition_order );
            SCOREP_ExitRegion( lock_regions[ TOOL_LOCK_EVENT_SET_NEST ] );
        }
        break;
        case ompt_mutex_critical:
            construct_mutex_acquired( task, TOOL_EVENT_CRITICAL, TOOL_EVENT_CRITICAL_SBLOCK, kind, wait_id );
            break;
        case ompt_mutex_atomic:
            /* Intentionally ignore atomic events. */
            break;
        case ompt_mutex_ordered:
            construct_mutex_acquired( task, TOOL_EVENT_ORDERED, TOOL_EVENT_ORDERED_SBLOCK, kind, wait_id );
            break;
        default:
            UTILS_WARNING( "mutex kind %s not implemented yet.", mutex2string( kind ) );
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                      adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static inline void
construct_mutex_acquired( task_t*        task,
                          tool_event_t   regionType,
                          tool_event_t   regionTypeSblock,
                          ompt_mutex_t   kind,
                          ompt_wait_id_t waitId )
{
    /* We are in the OpenMP synchronization, no one else will access the
       mutext_obj_t handle. */
    mutex_obj_t* mutex = mutex_get_and_insert( waitId, kind );

    /* Getting the regions in acquired as compared to in acquire potentially
       causes less contention. */
    mutex->optional.outer_region = get_region( task->mutex_acquire_codeptr_ra,
                                               regionType );
    mutex->optional.sblock_region = get_region( task->mutex_acquire_codeptr_ra,
                                                regionTypeSblock );

    SCOREP_Location_EnterRegion( NULL, task->mutex_acquire_timestamp, mutex->optional.outer_region );
    /* We need to use outer_region and sblock_region in mutex_released. As this
       task can take part in nested synchronizations between this acquire and
       the corresponding release, we can't transfer the regions via the task
       object. As the mutex handle is unique to this synchronization it is
       the natural candidate to transfer the regions. But when we access the
       regions in mutex_released, the mutex handle could already be re-acquired,
       i.e., we need to postpone this new assignment here until the regions
       have been used in mutex_released.
       In addition, we need to preserve acquisition_order from the acquire till
       the release event. The latter is written when the lock is already
       released and potentially re-acquired.
       Therefore, the explicit Lock/Unlock of in_release_operation. */
    UTILS_MutexLock( &( mutex->in_release_operation ) );
    mutex->acquisition_order++;
    SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP,
                              mutex->id,
                              mutex->acquisition_order );
    SCOREP_EnterRegion( mutex->optional.sblock_region );

    task->mutex_acquire_codeptr_ra = NULL;
    task->mutex_acquire_timestamp  = 0;
}


void
scorep_ompt_cb_host_mutex_released( ompt_mutex_t   kind,
                                    ompt_wait_id_t wait_id,
                                    const void*    codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                       adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = get_current_task();

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league mutex_released event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                          adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( kind )
    {
        case ompt_mutex_lock:
        {
            mutex_obj_t* mutex = mutex_get( wait_id, kind );
            SCOREP_EnterRegion( lock_regions[ TOOL_LOCK_EVENT_UNSET ] );
            SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, mutex->id, mutex->acquisition_order );
            UTILS_MutexUnlock( &( mutex->in_release_operation ) );
            mutex = NULL; /* Don't use after unlock */
            SCOREP_ExitRegion( lock_regions[ TOOL_LOCK_EVENT_UNSET ] );
        }
        break;
        case ompt_mutex_nest_lock:
        {
            /* nest-lock-release event. See also nest-lock-held in
               scorep_ompt_cb_host_nest_lock(). */
            mutex_obj_t* mutex = mutex_get( wait_id, kind );
            SCOREP_EnterRegion( lock_regions[ TOOL_LOCK_EVENT_UNSET_NEST ] );
            SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, mutex->id, mutex->acquisition_order );
            mutex->optional.nest_level--;
            UTILS_BUG_ON( mutex->optional.nest_level != 0 );
            UTILS_MutexUnlock( &( mutex->in_release_operation ) );
            mutex = NULL; /* Don't use after unlock */
            SCOREP_ExitRegion( lock_regions[ TOOL_LOCK_EVENT_UNSET_NEST ] );
        }
        break;
        case ompt_mutex_critical:
            construct_mutex_released( kind, wait_id );
            break;
        case ompt_mutex_atomic:
            /* Intentionally ignore atomic events. */
            break;
        case ompt_mutex_ordered:
            construct_mutex_released( kind, wait_id );
            break;
        default:
            UTILS_WARNING( "mutex kind %s not implemented yet.", mutex2string( kind ) );
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                      adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static inline void
construct_mutex_released( ompt_mutex_t kind, ompt_wait_id_t waitId )
{
    mutex_obj_t*        mutex        = mutex_get( waitId, kind );
    SCOREP_RegionHandle outer_region = mutex->optional.outer_region;
    SCOREP_ExitRegion( mutex->optional.sblock_region );
    SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, mutex->id, mutex->acquisition_order );
    UTILS_MutexUnlock( &( mutex->in_release_operation ) );
    mutex = NULL; /* Don't use mutex after unlock */
    SCOREP_ExitRegion( outer_region );
}


void
scorep_ompt_cb_host_lock_init( ompt_mutex_t   kind,
                               unsigned int   hint,
                               unsigned int   impl,
                               ompt_wait_id_t wait_id,
                               const void*    codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                       adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = get_current_task();

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league lock_init event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                          adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* There is no duration available for lock-init. */
    SCOREP_Location*    location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            timestamp = SCOREP_Timer_GetClockTicks();
    SCOREP_RegionHandle region    = SCOREP_INVALID_REGION;

    switch ( kind )
    {
        case ompt_mutex_lock:
            mutex_get_and_insert( wait_id, kind );
            region = lock_regions[ TOOL_LOCK_EVENT_INIT ];
            if ( hint != 0 /* omp_sync_hint_none */ )
            {
                region = lock_regions[ TOOL_LOCK_EVENT_INIT_WITH_HINT ];
            }
            SCOREP_Location_EnterRegion( location, timestamp, region );
            SCOREP_Location_ExitRegion( location, timestamp, region );
            break;
        case ompt_mutex_nest_lock:
            mutex_get_and_insert( wait_id, kind );
            region = lock_regions[ TOOL_LOCK_EVENT_INIT_NEST ];
            if ( hint != 0 /* omp_sync_hint_none */ )
            {
                region = lock_regions[ TOOL_LOCK_EVENT_INIT_NEST_WITH_HINT ];
            }
            SCOREP_Location_EnterRegion( location, timestamp, region );
            SCOREP_Location_ExitRegion( location, timestamp, region );
            break;
        default:
            UTILS_WARNING( "mutex kind %s not implemented yet.", mutex2string( kind ) );
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                      adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_lock_destroy( ompt_mutex_t   kind,
                                  ompt_wait_id_t wait_id,
                                  const void*    codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                       adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = get_current_task();

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league mutex_acquired event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                          adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* There is no duration available for lock-destroy. */
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = SCOREP_Timer_GetClockTicks();

    switch ( kind )
    {
        case ompt_mutex_lock:
            SCOREP_Location_EnterRegion( location, timestamp, lock_regions[ TOOL_LOCK_EVENT_DESTROY ] );
            SCOREP_Location_ExitRegion( location, timestamp, lock_regions[ TOOL_LOCK_EVENT_DESTROY ] );
            break;
        case ompt_mutex_nest_lock:
            SCOREP_Location_EnterRegion( location, timestamp, lock_regions[ TOOL_LOCK_EVENT_DESTROY_NEST ] );
            SCOREP_Location_ExitRegion( location, timestamp, lock_regions[ TOOL_LOCK_EVENT_DESTROY_NEST ] );
            break;
        default:
            UTILS_WARNING( "mutex kind %s not implemented yet.", mutex2string( kind ) );
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | kind %s | wait_id %ld | codeptr_ra %p",
                      adapter_tid, mutex2string( kind ), wait_id, codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_nest_lock( ompt_scope_endpoint_t endpoint,
                               ompt_wait_id_t        wait_id,
                               const void*           codeptr_ra )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | endpoint %s | wait_id %ld | codeptr_ra %p",
                       adapter_tid, scope_endpoint2string( endpoint ), wait_id, codeptr_ra );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = get_current_task();

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league nest-lock event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | wait_id %ld | codeptr_ra %p",
                          adapter_tid, scope_endpoint2string( endpoint ), wait_id, codeptr_ra );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    mutex_obj_t* mutex = mutex_get( wait_id, ompt_mutex_nest_lock );
    switch ( endpoint )
    {
        case ompt_scope_begin:
            /* nest-lock-owned event: See corresponding nest-lock-acquire in
               scorep_ompt_cb_host_mutex_acquire() for SCOREP_EnterRegion(). */
            mutex->optional.nest_level++;
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, mutex->id, mutex->acquisition_order );
            SCOREP_ExitRegion( lock_regions[ TOOL_LOCK_EVENT_SET_NEST ] );
            break;
        case ompt_scope_end:
            /* nest-lock-held event. See nest-lock-release in scorep_ompt_cb_host_mutex_released()
               for final release. */
            SCOREP_EnterRegion( lock_regions[ TOOL_LOCK_EVENT_UNSET_NEST ] );
            SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, mutex->id, mutex->acquisition_order );
            mutex->optional.nest_level--;
            SCOREP_ExitRegion( lock_regions[ TOOL_LOCK_EVENT_UNSET_NEST ] );
            break;
#if HAVE( DECL_OMPT_SCOPE_BEGINEND )
        case ompt_scope_beginend:
            UTILS_BUG( "ompt_scope_beginend not allowed in nest_lock callback" );
            break;
#endif  /* DECL_OMPT_SCOPE_BEGINEND */
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | endpoint %s | wait_id %ld | codeptr_ra %p",
                      adapter_tid, scope_endpoint2string( endpoint ), wait_id, codeptr_ra );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/*
   typedef enum ompt_dispatch_t {
   ompt_dispatch_iteration = 1,
   ompt_dispatch_section = 2,
   ompt_dispatch_ws_loop_chunk = 3,
   ompt_dispatch_taskloop_chunk = 4,
   ompt_dispatch_distribute_chunk = 5
   } ompt_dispatch_t;
 */

void
scorep_ompt_cb_host_dispatch( ompt_data_t*    parallel_data,
                              ompt_data_t*    task_data,
                              ompt_dispatch_t kind,
                              ompt_data_t     instance )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY( "atid %" PRIu32 " | parallel_data->ptr %p | task_data->ptr %p | "
                       "dispatch %s | instance.value %" PRIu64 " | instance.ptr %p",
                       adapter_tid, parallel_data->ptr, task_data->ptr,
                       dispatch2string( kind ), instance.value, instance.ptr );
    SCOREP_OMPT_RETURN_ON_INVALID_EVENT();

    task_t* task = task_data->ptr;

    /* For now, prevent league events */
    if ( task->belongs_to_league )
    {
        UTILS_WARN_ONCE( "OpenMP league dispatch event detected. "
                         "Not handled yet. Score-P might crash." );
        UTILS_DEBUG_EXIT( "atid %" PRIu32 " | parallel_data->ptr %p | task_data->ptr %p | "
                          "dispatch %s | instance.value %" PRIu64 " | instance.ptr %p",
                          adapter_tid, parallel_data->ptr, task_data->ptr,
                          dispatch2string( kind ), instance.value, instance.ptr );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    switch ( kind )
    {
        case ompt_dispatch_section:
            /* Exit previous section, if any. */
            if ( task->dispatch.section != SCOREP_INVALID_REGION )
            {
                SCOREP_ExitRegion( task->dispatch.section );
            }
            /* Enter new section. To be exited when this cb is triggered again
               or when the enclosing sections-end event is encountered. */
            task->dispatch.section = get_region( instance.ptr, TOOL_EVENT_SECTION );
            SCOREP_EnterRegion( task->dispatch.section );
            break;
        default:
            UTILS_WARNING( "dispatch kind %s not implemented yet.", dispatch2string( kind ) );
    }

    UTILS_DEBUG_EXIT( "atid %" PRIu32 " | parallel_data->ptr %p | task_data->ptr %p | "
                      "dispatch %s | instance.value %" PRIu64 " | instance.ptr %p",
                      adapter_tid, parallel_data->ptr, task_data->ptr,
                      dispatch2string( kind ), instance.value, instance.ptr );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_flush( ompt_data_t* thread_data,
                           const void*  codeptr_ra )
{
    SCOREP_RegionHandle region    = get_region( codeptr_ra, TOOL_EVENT_FLUSH );
    SCOREP_Location*    location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            timestamp = SCOREP_Timer_GetClockTicks();
    /* No duration available for flush. Use identical timestamp. */
    SCOREP_Location_EnterRegion( location, timestamp, region );
    SCOREP_Location_ExitRegion( location, timestamp, region );
}
