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
 *  @file
 *
 *  Commonly used definitions and functionality in the Kokkos adapter.
 */

#ifndef SCOREP_KOKKOS_H
#define SCOREP_KOKKOS_H

#include <SCOREP_DefinitionHandles.h>

#include <UTILS_Error.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Kokkos features (to be enabled/disabled via environment variables)
 */
#define SCOREP_KOKKOS_FEATURE_REGIONS  ( 1 << 0 )
#define SCOREP_KOKKOS_FEATURE_USER     ( 1 << 1 )
#define SCOREP_KOKKOS_FEATURE_MALLOC   ( 1 << 2 )
#define SCOREP_KOKKOS_FEATURE_MEMCPY   ( 1 << 3 )
#define SCOREP_KOKKOS_FEATURES_DEFAULT SCOREP_KOKKOS_FEATURE_REGIONS | SCOREP_KOKKOS_FEATURE_USER | SCOREP_KOKKOS_FEATURE_MALLOC

/*
 * Specifies the Kokkos tracing mode with a bit mask.
 * See SCOREP_ConfigType_SetEntry of Kokkos adapter.
 */
extern uint64_t scorep_kokkos_features;

extern size_t scorep_kokkos_subsystem_id;

SCOREP_RmaWindowHandle
scorep_kokkos_define_rma_win( void );

typedef struct scorep_kokkos_cpu_location_data
{
    bool active_deep_copy;
} scorep_kokkos_cpu_location_data;

typedef struct scorep_kokkos_gpu_location_data
{
    uint32_t rma_win_rank;
    bool     rma_win_created;
} scorep_kokkos_gpu_location_data;

struct SCOREP_Location;

struct SCOREP_Location*
scorep_kokkos_get_device_location( void );

#endif  /* SCOREP_KOKKOS_H */
