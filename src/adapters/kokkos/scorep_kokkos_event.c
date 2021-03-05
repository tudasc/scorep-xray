/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  Implementation of Kokkos Tools Interface routines.
 */

#include <config.h>

#include "scorep_kokkos.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Task.h>
#include <SCOREP_AllocMetric.h>
#include <SCOREP_Timer_Ticks.h>

#define SCOREP_DEBUG_MODULE_NAME KOKKOS
#include <UTILS_Debug.h>

#include <UTILS_CStr.h>

#include <jenkins_hash.h>

#include <stdbool.h>
#include <string.h>

/* Kokkos Tools Interface definitions */

/**
 * Kokkosp definitions copied in here
 *
 * This is the official way to access the
 * contents of the opaque SpaceHandle type
 * that remains backwards-compatible through
 * essentially all tools-supporting versions
 * of Kokkos.
 *
 */
#define KOKKOSP_SPACE_NAME_LENGTH 64

typedef struct SpaceHandle
{
    char name[ KOKKOSP_SPACE_NAME_LENGTH ];
} SpaceHandle;

/* Kokkos adapter definitions */

static bool kokkos_initialized;
static bool kokkos_record_parallel_region;
static bool kokkos_record_user_region;
static bool kokkos_record_malloc;
static bool kokkos_record_memcpy;

/* source file handle for all Kokkos regions */
static SCOREP_SourceFileHandle kokkos_file_handle = SCOREP_INVALID_SOURCE_FILE;

static SCOREP_RegionHandle kokkos_deep_copy_region = SCOREP_INVALID_REGION;

// declarations for event lib
static SCOREP_RmaWindowHandle kokkos_rma_window;
#define KOKKOS_RMA_MAGIC 0xDEADBEEF

/*
 * Enum for region groups
 *
 */
typedef enum
{
    scorep_kokkos_parallel_for,
    scorep_kokkos_parallel_scan,
    scorep_kokkos_parallel_reduce,
    scorep_kokkos_profile_region,
    scorep_kokkos_profile_section
} scorep_kokkos_group;

/*
 * The key of the region node is the name of the region and its Kokkos group,
 * the value is the corresponding region handle.
 */
typedef struct scorep_kokkos_region_node
{
    struct scorep_kokkos_region_node* next;   /**< bucket for collision */
    SCOREP_RegionHandle               region; /**< associated region handle */
    uint32_t                          hash;   /**< hash of string for faster comparison */
    scorep_kokkos_group               group;  /**< ID of group for the region, also part of comparsion */
} scorep_kokkos_region_node;

#define KOKKOS_REGION_HASH_SHIFT 10
#define KOKKOS_REGION_HASH_MASK  hashmask( KOKKOS_REGION_HASH_SHIFT )
#define KOKKOS_REGION_HASH_SIZE  hashsize( KOKKOS_REGION_HASH_SHIFT )

static scorep_kokkos_region_node* kokkos_regions_hashtab[ KOKKOS_REGION_HASH_SIZE ];
static SCOREP_Mutex               kokkos_regions_hashtab_mutex;

static const char*
scorep_kokkos_group_name( scorep_kokkos_group group )
{
    switch ( group )
    {
        case scorep_kokkos_parallel_for:
            return "Kokkos parallel_for";
        case scorep_kokkos_parallel_scan:
            return "Kokkos parallel_scan";
        case scorep_kokkos_parallel_reduce:
            return "Kokkos parallel_reduce";
        case scorep_kokkos_profile_region:
            return "Kokkos profile regions";
        case scorep_kokkos_profile_section:
            return "Kokkos profile sections";
        default:
            return "UNKNOWN KOKKOS GROUP";
    }
}

static SCOREP_RegionType
scorep_kokkos_group_region_type( scorep_kokkos_group group )
{
    switch ( group )
    {
        case scorep_kokkos_parallel_for:
        case scorep_kokkos_parallel_scan:
        case scorep_kokkos_parallel_reduce:
            return SCOREP_REGION_PARALLEL;
        case scorep_kokkos_profile_region:
            return SCOREP_REGION_USER;
        case scorep_kokkos_profile_section:
            return SCOREP_REGION_PHASE;
        default:
            return SCOREP_REGION_UNKNOWN;
    }
}

static SCOREP_RegionHandle
get_region( scorep_kokkos_group group,
            const char*         name,
            const char*         mangledName )
{
    SCOREP_MutexLock( kokkos_regions_hashtab_mutex );

    uint32_t hash = jenkins_hash( name, strlen( name ), 0 );
    uint32_t id   = hash & KOKKOS_REGION_HASH_MASK;

    scorep_kokkos_region_node* node = kokkos_regions_hashtab[ id ];
    while ( node )
    {
        if ( hash == node->hash
             && group == node->group
             && strcmp( SCOREP_RegionHandle_GetName( node->region ), name ) == 0 )
        {
            break;
        }

        node = node->next;
    }

    if ( !node )
    {
        SCOREP_RegionHandle region_handle =
            SCOREP_Definitions_NewRegion( name,
                                          mangledName,
                                          kokkos_file_handle,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_KOKKOS,
                                          scorep_kokkos_group_region_type( group ) );
        SCOREP_RegionHandle_SetGroup( region_handle, scorep_kokkos_group_name( group ) );

        node                         = SCOREP_Memory_AllocForMisc( sizeof( *node ) );
        node->region                 = region_handle;
        node->hash                   = hash;
        node->next                   = kokkos_regions_hashtab[ id ];
        kokkos_regions_hashtab[ id ] = node;
    }
    SCOREP_RegionHandle region = node->region;

    SCOREP_MutexUnlock( kokkos_regions_hashtab_mutex );

    return region;
}

/**
 * Keep an AllocMetric per SpaceHandle (see above).
 *
 */
typedef struct scorep_kokkos_metric_entry
{
    char                               kokkos_space[ KOKKOSP_SPACE_NAME_LENGTH ];
    SCOREP_AllocMetric*                metric;
    struct scorep_kokkos_metric_entry* next;
} scorep_kokkos_metric_entry;

static scorep_kokkos_metric_entry* kokkos_alloc_metrics;
static SCOREP_Mutex                kokkos_alloc_metrics_mutex;

static SCOREP_AllocMetric*
get_metric( const char* name )
{
    SCOREP_MutexLock( kokkos_alloc_metrics_mutex );

    scorep_kokkos_metric_entry* current = kokkos_alloc_metrics;
    while ( current && strcmp( current->kokkos_space, name ) )
    {
        current = current->next;
    }
    if ( current )
    {
        SCOREP_MutexUnlock( kokkos_alloc_metrics_mutex );
        return current->metric;
    }

    scorep_kokkos_metric_entry* new_entry = SCOREP_Memory_AllocForMisc( sizeof( *new_entry ) );
    new_entry->next      = kokkos_alloc_metrics;
    kokkos_alloc_metrics = new_entry;
    memcpy( new_entry->kokkos_space, name, KOKKOSP_SPACE_NAME_LENGTH );
    struct SCOREP_AllocMetric* allocMetric = NULL;
    SCOREP_AllocMetric_New( name, &allocMetric );
    new_entry->metric = allocMetric;

    SCOREP_MutexUnlock( kokkos_alloc_metrics_mutex );

    return allocMetric;
}

#if HAVE( DEMANGLE )
extern char*
cplus_demangle( const char* mangled,
                int         options );

    #define kokkos_demangle( mangled ) cplus_demangle( mangled, \
                                                       ( 1 << 0 ) /* include function arguments */ | \
                                                       ( 1 << 1 ) /* include const, volatile, etc. */ | \
                                                       ( 1 << 3 ) /* include implementation details */ | \
                                                       ( 1 << 4 ) /* include type encodings */ )

#else

    #define kokkos_demangle( mangled ) NULL

#endif

/*
 * The name provided to parallel_{for,scan,reduce} may be either the label
 * the user provided to the Kokkos call, or if that was empty, the combination
 * of two typeid().name() strings combined with a '/'. These two type-id-names
 * may be mangled for GCC and Clang. Thus if we find a `/`, we split it, try
 * to demangle the two parts, and combine these again.
 *
 * The return value should be used as the region name, @a *name should be for
 * mangled name. Call `free` on the return value, if @a *name is not the NULL
 * pointer.
 */
static const char*
decode_parallel_region_name( const char** name )
{
    if ( NULL == strchr( *name, '/' ) )
    {
        /* No `/`, keep it as it, no mangled */
        const char* ret = *name;
        *name = NULL;
        return ret;
    }

    char*       copy  = UTILS_CStr_dup( *name );
    char*       slash = strchr( copy, '/' );
    const char* part0 = copy;
    *slash = '\0';
    const char* part1 = slash + 1;

    char* demangled0 = kokkos_demangle( part0 );
    if ( NULL == demangled0 )
    {
        /* demangling failed, keep it as it, no mangled */
        const char* ret = *name;
        *name = NULL;
        free( copy );
        return ret;
    }

    char* demangled1 = kokkos_demangle( part1 );
    if ( NULL == demangled1 )
    {
        /* demangling failed, keep it as it, no mangled */
        const char* ret = *name;
        *name = NULL;
        free( copy );
        free( demangled0 );
        return ret;
    }

    /* combine the two parts again */
    size_t total_length = strlen( demangled0 ) + 1 + strlen( demangled1 ) + 1;
    char*  result       = calloc( 1, total_length );
    strcat( result, demangled0 );
    strcat( result, "/" );
    strcat( result, demangled1 );

    free( copy );
    free( demangled0 );
    free( demangled1 );

    return result;
}

static void
recording_setup( void )
{
    kokkos_record_parallel_region = ( scorep_kokkos_features & SCOREP_KOKKOS_FEATURE_REGIONS );
    kokkos_record_user_region     = ( scorep_kokkos_features & SCOREP_KOKKOS_FEATURE_USER    );
    kokkos_record_malloc          = ( scorep_kokkos_features & SCOREP_KOKKOS_FEATURE_MALLOC  );
    kokkos_record_memcpy          = ( scorep_kokkos_features & SCOREP_KOKKOS_FEATURE_MEMCPY  );
    UTILS_DEBUG( "Record parallel region is %s", kokkos_record_parallel_region ? "on" : "off" );
    UTILS_DEBUG( "Record user region is %s",     kokkos_record_user_region     ? "on" : "off" );
    UTILS_DEBUG( "Record malloc is %s",          kokkos_record_malloc          ? "on" : "off" );
    UTILS_DEBUG( "Record memcpy is %s",          kokkos_record_memcpy          ? "on" : "off" );
}

static void
init_kokkos( void )
{
    if ( false == kokkos_initialized )
    {
        recording_setup();

        kokkos_file_handle = SCOREP_Definitions_NewSourceFile( "KOKKOS" );

        SCOREP_MutexCreate( &kokkos_regions_hashtab_mutex );
        SCOREP_MutexCreate( &kokkos_alloc_metrics_mutex );

        if ( kokkos_record_memcpy )
        {
            kokkos_deep_copy_region = SCOREP_Definitions_NewRegion(
                "kokkos_deep_copy", NULL,
                kokkos_file_handle,
                SCOREP_INVALID_LINE_NO,
                SCOREP_INVALID_LINE_NO,
                SCOREP_PARADIGM_KOKKOS,
                SCOREP_REGION_RMA );
            SCOREP_RegionHandle_SetGroup( kokkos_deep_copy_region, "Kokkos deep copy" );

            kokkos_rma_window = scorep_kokkos_define_rma_win();

            UTILS_DEBUG( "Creating host-side RMA window" );
            SCOREP_RmaWinCreate( kokkos_rma_window );
        }
        kokkos_initialized = true;
    }
}

/*
 * should be called at first use, to ensure that the GPU device was already
 * created by the lower adapter
 */
static uint32_t
get_device_location_rank( void )
{
    SCOREP_Location*                 location = scorep_kokkos_get_device_location();
    scorep_kokkos_gpu_location_data* data     =
        SCOREP_Location_GetSubsystemData( location, scorep_kokkos_subsystem_id );
    if ( !data->rma_win_created )
    {
        UTILS_DEBUG( "Creating device-side RMA window [rank %u]", data->rma_win_rank );
        SCOREP_Location_RmaWinCreate( location,
                                      SCOREP_Timer_GetClockTicks(),
                                      kokkos_rma_window );
        data->rma_win_created = true;
    }
    return data->rma_win_rank;
}

/* Kokkos Tools Interface implementation */

/**
 * @brief Called at Kokkos::initialize time.
 *
 * @param loadSeq       Where we are in the tools stack.
 *                      Unused, as we don't care.
 * @param interfaceVer  Version of the kokkosp interface.
 * @param devInfoCount  Number of devices in @a deviceInfo
 * @param deviceInfo    Information about the devices present.
 *                      Currently a struct wrapping a size_t containing
 *                      the Kokkos device id.
 *
 * Called for each profiling library, as specified by
 * semicolon-separated environment variable, at the
 * time of Kokkos::initialize().
 * Proper Kokkos multi-device support needs to start
 * here with extracting the number and IDs of Kokkos
 * devices.
 */
void
kokkosp_init_library( const int      loadSeq,
                      const uint64_t interfaceVer,
                      const uint32_t devInfoCount,
                      void*          deviceInfo )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    UTILS_DEBUG( "[Kokkos] Initialize library." );
    init_kokkos();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Called at Kokkos::finalize time.
 *
 * This is not guaranteed to be the end of measurement,
 * but it is guaranteed to be the end of Kokkos measurement.
 */
void
kokkosp_finalize_library( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG( "[Kokkos] Finalize library" );

    SCOREP_MutexDestroy( &kokkos_regions_hashtab_mutex );
    SCOREP_MutexDestroy( &kokkos_alloc_metrics_mutex );

    if ( kokkos_record_memcpy )
    {
        SCOREP_Location*                 location = scorep_kokkos_get_device_location();
        scorep_kokkos_gpu_location_data* data     =
            SCOREP_Location_GetSubsystemData( location, scorep_kokkos_subsystem_id );
        if ( data->rma_win_created )
        {
            UTILS_DEBUG( "Destroying device-side RMA window [rank %u]", data->rma_win_rank );
            SCOREP_Location_RmaWinDestroy( location,
                                           SCOREP_Timer_GetClockTicks(),
                                           kokkos_rma_window );
        }
        UTILS_DEBUG( "Destroying host-side RMA window" );
        SCOREP_RmaWinDestroy( kokkos_rma_window );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Begin a parallel-for.
 *
 * @param name     The name of the parallel region.
 *                 May be user-provided or derived from the C++ typename
 *                  of the functor being invoked (and thus some form of
 *                  mangled name).
 * @param devID     Kokkos ID of the device. Currently we
 *                  assume Kokkos does not properly support multiple devices
 *                  and ignore this parameter. Needs fixing, but ideally as
 *                  part of more general multi-device support in Score-P.
 * @param[out] kID  Kernel ID to use for the matching end_parallel_for.
 *                  Scope of this variable inside Kokkos can only be
 *                  assumed to be from begin to a matching end.
 *                  We use a region handle to make the logic at end()
 *                  as simple as possible; however, this assumes
 *                  (correctly at present) that Kokkos delivers begin()
 *                  and the matching end() on the same thread.
 */
void
kokkosp_begin_parallel_for( const char*    name,
                            const uint32_t devID,
                            uint64_t*      kID )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_parallel_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    const char* region_name = decode_parallel_region_name( &name );
    if ( SCOREP_Filtering_MatchFunction( region_name, name ) )
    {
        if ( name )
        {
            free( ( char* )region_name );
        }
        UTILS_DEBUG( "[Kokkos] parallel_for %s filtered\n", name );
        *kID = SCOREP_FILTERED_REGION;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = get_region( scorep_kokkos_parallel_for,
                                             region_name, name );
    if ( name )
    {
        free( ( char* )region_name );
    }

    *kID = ( uint64_t )region;
    SCOREP_EnterRegion( region );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief End a parallel-work.
 *
 * @param kID  Kernel to end.
 * Region handle set by the associated begin_parallel_for().
 *
 */
void
kokkosp_end_parallel_for( const uint64_t kID )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_parallel_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = ( SCOREP_RegionHandle )kID;
    if ( region != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Begin a parallel-scan.
 *
 * @see kokkosp_begin_parallel_for
 * No difference from our perspective except for naming.
 */
void
kokkosp_begin_parallel_scan( const char*    name,
                             const uint32_t devID,
                             uint64_t*      kID )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_parallel_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    const char* region_name = decode_parallel_region_name( &name );
    if ( SCOREP_Filtering_MatchFunction( region_name, name ) )
    {
        if ( name )
        {
            free( ( char* )region_name );
        }
        UTILS_DEBUG( "[Kokkos] parallel_scan %s filtered\n", name );
        *kID = SCOREP_FILTERED_REGION;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = get_region( scorep_kokkos_parallel_scan,
                                             region_name, name );
    if ( name )
    {
        free( ( char* )region_name );
    }

    *kID = ( uint64_t )region;
    SCOREP_EnterRegion( region );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief End a parallel-scan.
 *
 * @see kokkosp_end_parallel_for
 * No difference from our perspective except for naming.
 */
void
kokkosp_end_parallel_scan( const uint64_t kID )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_parallel_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = ( SCOREP_RegionHandle )kID;
    if ( region != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Begin a parallel-reduce.
 *
 * @see kokkosp_begin_parallel_for
 * No difference from our perspective except for naming.
 */
void
kokkosp_begin_parallel_reduce( const char*    name,
                               const uint32_t devID,
                               uint64_t*      kID )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_parallel_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    const char* region_name = decode_parallel_region_name( &name );
    if ( SCOREP_Filtering_MatchFunction( region_name, name ) )
    {
        if ( name )
        {
            free( ( char* )region_name );
        }
        UTILS_DEBUG( "[Kokkos] parallel_reduce %s filtered\n", name );
        *kID = SCOREP_FILTERED_REGION;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = get_region( scorep_kokkos_parallel_reduce,
                                             region_name, name );
    if ( name )
    {
        free( ( char* )region_name );
    }

    *kID = ( uint64_t )region;
    SCOREP_EnterRegion( region );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief End a parallel-reduce.
 *
 * @see kokkosp_end_parallel_for
 * No difference from our perspective except for naming.
 */
void
kokkosp_end_parallel_reduce( const uint64_t kID )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_parallel_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = ( SCOREP_RegionHandle )kID;
    if ( region != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * Relies on Kokkos naming conventions
 * to determine whether a SpaceHandle refers to
 * host or device memory. Note that CudaUVM could
 * refer to both, and may need to be broken out
 * for special handling.
 */
static bool
is_spacehandle_device( const char* name )
{
    if ( strcmp( name, "Cuda" ) == 0 ||
         strcmp( name, "CudaUVM" ) == 0 ||
         strcmp( name, "HIP" ) == 0 )
    {
        return true;
    }
    return false;
}

/**
 * @bried Start a deep-copy operation.
 *
 * @param dstHandle  Destination of the deep copy
 * @param dstName    Name of the destination variable. Currently unused.
 * @param dstPtr     Address of the destination variable. Currently unused
 * @param srcHandle  Source of the deep copy
 * @param srcName    Name of the source variable. Currently unused.
 * @param srcPtr     Address of the source variable. Currently unused
 * @param size       Number of bytes transferred.
 */
void
kokkosp_begin_deep_copy( SpaceHandle dstHandle,
                         const char* dstName,
                         const void* dstPtr,
                         SpaceHandle srcHandle,
                         const char* srcName,
                         const void* srcPtr,
                         uint64_t    size )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_memcpy )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_Location*                 location = SCOREP_Location_GetCurrentCPULocation();
    scorep_kokkos_cpu_location_data* data     =
        SCOREP_Location_GetSubsystemData( location, scorep_kokkos_subsystem_id );

    if ( data->active_deep_copy )
    {
        UTILS_WARNING( "[Kokkos] Nested deep-copy detected" );
    }

    SCOREP_EnterRegion( kokkos_deep_copy_region );

    // Host to host deep copy: no RMA
    if ( !is_spacehandle_device( dstHandle.name ) && !is_spacehandle_device( srcHandle.name ) )
    {
        UTILS_DEBUG( "Skipping host to host deep copy: %s to %s", srcHandle.name, dstHandle.name );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    // if device to device, then src/target are both device and it's a get
    if ( is_spacehandle_device( dstHandle.name ) && is_spacehandle_device( srcHandle.name ) )
    {
        UTILS_DEBUG( "Skipping deep copy, device to device must rely on offloading adapter" );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    // Do the RMA
    uint32_t target_rank = get_device_location_rank();

    if ( !is_spacehandle_device( dstHandle.name ) )
    {
        // dest is host, so a host-perspective get is forced
        SCOREP_RmaGet( kokkos_rma_window,
                       target_rank,
                       size,
                       KOKKOS_RMA_MAGIC );
    }
    else
    {
        SCOREP_RmaPut( kokkos_rma_window,
                       target_rank,
                       size,
                       KOKKOS_RMA_MAGIC );
    }
    data->active_deep_copy = true;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @bried End a deep-copy operation.
 *
 * Kokkos promises that deep copy operations
 * are single-threaded and there is therefore no need
 * to match begin and end operations.
 */
void
kokkosp_end_deep_copy( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_memcpy )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_Location*                 location = SCOREP_Location_GetCurrentCPULocation();
    scorep_kokkos_cpu_location_data* data     =
        SCOREP_Location_GetSubsystemData( location, scorep_kokkos_subsystem_id );

    /* We ignore host->host or device->device transfers */
    if ( data->active_deep_copy )
    {
        SCOREP_RmaOpCompleteBlocking( kokkos_rma_window,
                                      KOKKOS_RMA_MAGIC );
        data->active_deep_copy = false;
    }
    SCOREP_ExitRegion( kokkos_deep_copy_region );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Allocate memory.
 *
 * @param handle  SpaceHandle for where the data will be allocated
 * @param name    Name of the allocated variable. Not currently used.
 * @param ptr     Address of the allocation.
 * @param size    Size of the allocation
 */
void
kokkosp_allocate_data( SpaceHandle handle,
                       const char* name,
                       void*       ptr,
                       uint64_t    size )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_malloc )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_AllocMetric* metric = get_metric( handle.name );
    SCOREP_AllocMetric_HandleAlloc( metric, ( uint64_t )( ptr ), size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Deallocate memory.
 *
 * @param handle  SpaceHandle for where the data was allocated
 * @param name    Name of the allocated variable. Not currently used.
 * @param ptr     Address of the allocation.
 * @param size    Size of the allocation
 */
void
kokkosp_deallocate_data( SpaceHandle handle,
                         const char* name,
                         void*       ptr,
                         uint64_t    size )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_malloc )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_AllocMetric* metric     = get_metric( handle.name );
    void*               allocation = NULL;
    SCOREP_AllocMetric_AcquireAlloc( metric, ( uint64_t )( ptr ), &allocation );
    SCOREP_AllocMetric_HandleFree( metric, allocation, NULL );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Push a user region onto the stack.
 *
 * @param name  User-provided region name
 */
void
kokkosp_push_profile_region( const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_user_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_Filtering_MatchFunction( name, NULL ) )
    {
        SCOREP_Task_Enter( SCOREP_Location_GetCurrentCPULocation(), SCOREP_FILTERED_REGION );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region = get_region( scorep_kokkos_profile_region,
                                             name, NULL );

    SCOREP_EnterRegion( region );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Pop the top user region off the stack.
 */
void
kokkosp_pop_profile_region( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_user_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( location != NULL );

    SCOREP_RegionHandle region_handle =
        SCOREP_Task_GetTopRegion( SCOREP_Task_GetCurrentTask( location ) );
    UTILS_ASSERT( region_handle != SCOREP_INVALID_REGION );

    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region_handle );
    }
    else
    {
        SCOREP_Task_Exit( location );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Named user event.
 *
 * Not presently supported, but see RocTX/NVTX
 * for a plausible implementation of such point events.
 */
void
kokkosp_profile_event( const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Create a persistent profiling section.
 *
 * @param name            Name to give the section
 * @param[out] sectionID  Associate a uint32 with the provided name.
 *                        We use a region ID here.
 *
 * The profile section interface was created because
 * once upon a time, it was measurably faster than
 * the user region stack interface.
 * It behaves like the RocTX/NVTX begin/end user
 * region (as opposed to push/pop user region)
 * interfaces, but users conventionally
 * treat it as push/pop most of the time.
 *
 * Profile sections are treated as "phase" regions.
 */
void
kokkosp_create_profile_section( const char* name,
                                uint32_t*   sectionId )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_user_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_Filtering_MatchFunction( name, NULL ) )
    {
        *sectionId = SCOREP_FILTERED_REGION;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    *sectionId = get_region( scorep_kokkos_profile_section, name, NULL );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Desctroy a persistent profile section.
 *
 * @param sectionId  Section ID to invalidate.
 */
void
kokkosp_destroy_profile_section( uint32_t sectionId )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    /* we can reuse section IDs, thus no need to dispose anything here */

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Enter a persistent profile section.
 *
 * @param sectionID  Section to enter.
 */
void
kokkosp_start_profile_section( uint32_t sectionId )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_user_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( sectionId != SCOREP_FILTERED_REGION )
    {
        SCOREP_EnterRegion( sectionId );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Exit a persistent profile section.
 *
 * @param sectionID  Section to exit.
 */
void
kokkosp_stop_profile_section( uint32_t sectionId )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !kokkos_record_user_region )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( sectionId != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( sectionId );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
