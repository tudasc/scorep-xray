/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2016,
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
 *  This file provides the feature configuration of the OpenACC adapter.
 */

#ifndef SCOREP_OPENACC_CONFVARS_H
#define SCOREP_OPENACC_CONFVARS_H

#include <stdint.h>
#include <stdbool.h>


/*
 * OpenACC features (to be enabled/disabled via environment variables)
 */
#define SCOREP_OPENACC_FEATURE_REGIONS             ( 1 << 0 )
#define SCOREP_OPENACC_FEATURE_ENQUEUE             ( 1 << 1 )
#define SCOREP_OPENACC_FEATURE_WAIT                ( 1 << 2 )
#define SCOREP_OPENACC_FEATURE_KERNEL_PROPERTIES   ( 1 << 3 )
#define SCOREP_OPENACC_FEATURE_DEVICE_ALLOC        ( 1 << 4 )
#define SCOREP_OPENACC_FEATURE_VARNAMES            ( 1 << 5 )
#define SCOREP_OPENACC_FEATURES_DEFAULT \
    ( SCOREP_OPENACC_FEATURE_REGIONS | SCOREP_OPENACC_FEATURE_WAIT | \
      SCOREP_OPENACC_FEATURE_ENQUEUE )

/*
 * Specifies the OpenACC tracing mode with a bit mask.
 * See SCOREP_ConfigType_SetEntry of OpenACC adapter.
 */
extern uint64_t scorep_openacc_features;

/*
 * Enable/Disable recording of OpenACC regions calls.
 */
extern bool scorep_openacc_record_regions;


/*
 * Write additional kernel properties?
 * (kernel name, gang size, worker size, vector length)
 */
extern bool scorep_openacc_record_kernel_props;

/*
 * flag: Are OpenACC memory copies recorded?
 */
extern bool scorep_openacc_record_enqueue;

/*
 * flag: Is OpenACC wait recorded?
 */
extern bool scorep_openacc_record_wait;

/*
 * flag: Is OpenACC memory usage recorded?
 */
extern bool scorep_openacc_record_device_alloc;

/*
 * flag: Shall variable names be recorded?
 */
extern bool scorep_openacc_record_varnames;

/**
 * Setup user-specified features.
 */
void
scorep_openacc_setup_features( void );

#endif  /* SCOREP_OPENACC_CONFVARS_H */
