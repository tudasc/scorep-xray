/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
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
 *  This file provides the feature configuration of the HIP adapter.
 */

#ifndef SCOREP_HIP_CONFVARS_H
#define SCOREP_HIP_CONFVARS_H

#include <stdint.h>


/*
 * HIP features (to be enabled/disabled via environment variables)
 */
#define SCOREP_HIP_FEATURE_API             ( 1 << 0 )
#define SCOREP_HIP_FEATURE_KERNEL          ( 1 << 1 )
#define SCOREP_HIP_FEATURE_KERNEL_CALLSITE ( 1 << 2 )
#define SCOREP_HIP_FEATURE_MALLOC          ( 1 << 3 )
#define SCOREP_HIP_FEATURE_MEMCPY          ( 1 << 4 )
#define SCOREP_HIP_FEATURE_SYNC            ( 1 << 5 )
#define SCOREP_HIP_FEATURE_USER            ( 1 << 6 )
#define SCOREP_HIP_FEATURES_DEFAULT \
    ( SCOREP_HIP_FEATURE_API | \
      SCOREP_HIP_FEATURE_KERNEL | \
      SCOREP_HIP_FEATURE_KERNEL_CALLSITE | \
      SCOREP_HIP_FEATURE_MALLOC | \
      SCOREP_HIP_FEATURE_MEMCPY | \
      SCOREP_HIP_FEATURE_SYNC )


/*
 * Specifies the HIP tracing mode with a bit mask.
 * See SCOREP_ConfigType_SetEntry of HIP adapter.
 */
extern uint64_t scorep_hip_features;

/*
 * Size of the device activity buffer
 */
extern uint64_t scorep_hip_activity_buffer_size;

#endif  /* SCOREP_HIP_CONFVARS_H */
