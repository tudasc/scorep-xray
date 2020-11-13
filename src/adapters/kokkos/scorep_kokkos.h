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

#include <UTILS_Error.h>

#include <stdint.h>

/*
 * Kokkos features (to be enabled/disabled via environment variables)
 */
#define SCOREP_KOKKOS_FEATURE_REGIONS  ( 1 << 0 )
#define SCOREP_KOKKOS_FEATURES_DEFAULT SCOREP_KOKKOS_FEATURE_REGIONS

/*
 * Specifies the Kokkos tracing mode with a bit mask.
 * See SCOREP_ConfigType_SetEntry of Kokkos adapter.
 */
extern uint64_t scorep_kokkos_features;

#endif  /* SCOREP_KOKKOS_H */
