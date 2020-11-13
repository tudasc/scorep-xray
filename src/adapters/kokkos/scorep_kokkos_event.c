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

#include <SCOREP_InMeasurement.h>

#define SCOREP_DEBUG_MODULE_NAME KOKKOS
#include <UTILS_Debug.h>

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

    UTILS_DEBUG( "[Kokkos] Initialize library." );

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

    SCOREP_IN_MEASUREMENT_DECREMENT();
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

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * @brief Pop the top user region off the stack.
 */
void
kokkosp_pop_profile_region( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

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

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
