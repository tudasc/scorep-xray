/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Interface to detect if we are already inside the measurement system.
 *
 * Every adapter event function needs to have SCOREP_IN_MEASUREMENT_INCREMENT
 * as first and SCOREP_IN_MEASUREMENT_DECREMENT as last statement
 * (i.e., before every return).
 */

#include <SCOREP_Types.h>

#if HAVE( THREAD_LOCAL_STORAGE )

#include <signal.h>

#endif /* HAVE( THREAD_LOCAL_STORAGE ) */


UTILS_BEGIN_C_DECLS

extern volatile SCOREP_MeasurementPhase scorep_measurement_phase;

/**
 *  Evaluates to true if Score-P is in the measurement phase `PHASE`.
 *
 *  `PHASE` may be one of:
 *   - `PRE`:    before and during initialization
 *   - `WITHIN`: after initialization but before finalization
 *   - `POST`:   during and after finalization
 */
#define SCOREP_IS_MEASUREMENT_PHASE( PHASE ) \
    ( scorep_measurement_phase == SCOREP_MEASUREMENT_PHASE_ ## PHASE )


#if HAVE( THREAD_LOCAL_STORAGE )

#ifdef __cplusplus

volatile sig_atomic_t*
scorep_get_in_measurement( void );

#define scorep_in_measurement ( *scorep_get_in_measurement() )

#else /* !__cplusplus */

extern SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile sig_atomic_t scorep_in_measurement SCOREP_THREAD_LOCAL_STORAGE_MODEL( "initial-exec" );

#if HAVE( SAMPLING_SUPPORT )

extern SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile sig_atomic_t scorep_in_signal_context SCOREP_THREAD_LOCAL_STORAGE_MODEL( "initial-exec" );

#endif /* HAVE( SAMPLING_SUPPORT ) */

#endif /* !__cplusplus */

/** Enter the measurement system */
#define SCOREP_IN_MEASUREMENT_INCREMENT() ( scorep_in_measurement++ )

/** Enter the measurement system and return true if we just entered it */
#define SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT() ( 0 == scorep_in_measurement++ )

/** Leave the measurement system */
#if defined( __ICC ) || defined( __ECC ) || defined( __INTEL_COMPILER )
/* The Intel compiler sometimes produces wrong code, it clobbers registers to
 * access the TLS scorep_in_measurement variable. The empty asm prevent this. */
#define SCOREP_IN_MEASUREMENT_DECREMENT() ( { asm ( "" ); --scorep_in_measurement; } )
#else
#define SCOREP_IN_MEASUREMENT_DECREMENT() ( --scorep_in_measurement )
#endif

/** Value of the in-measurement counter */
#define SCOREP_IN_MEASUREMENT() ( scorep_in_measurement )

/**
 * @def SCOREP_ENTER_WRAPPED_REGION
 * Enter a wrapped region.
 *
 * This is only eligible if the corresponding enter event was also triggered.
 */
#define SCOREP_ENTER_WRAPPED_REGION() \
    sig_atomic_t scorep_in_measurement_save = scorep_in_measurement; \
    scorep_in_measurement                   = 0

/**
 * @def SCOREP_ENTER_WRAPPED_REGION
 * Leave a wrapped region
 */
#define SCOREP_EXIT_WRAPPED_REGION() \
    scorep_in_measurement = scorep_in_measurement_save

#if HAVE( SAMPLING_SUPPORT )

/** Enter the signal context */
#define SCOREP_ENTER_SIGNAL_CONTEXT() ( scorep_in_signal_context++ )

/** Leave the signal-context */
#define SCOREP_EXIT_SIGNAL_CONTEXT() ( --scorep_in_signal_context )

/** Value of the in-signal-context counter */
#define SCOREP_IN_SIGNAL_CONTEXT() ( scorep_in_signal_context )

#endif /* HAVE( SAMPLING_SUPPORT ) */

#else /* !HAVE( THREAD_LOCAL_STORAGE ) */

/* Following defines are documented above.*/

#define SCOREP_IN_MEASUREMENT_INCREMENT() 0

#define SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT() 1

#define SCOREP_IN_MEASUREMENT_DECREMENT() do {} while ( 0 )

#define SCOREP_IN_MEASUREMENT() 0

#define SCOREP_ENTER_WRAPPED_REGION() do {} while ( 0 )

#define SCOREP_EXIT_WRAPPED_REGION() do {} while ( 0 )

#endif /* !HAVE( THREAD_LOCAL_STORAGE ) */

#if !HAVE( THREAD_LOCAL_STORAGE ) || !HAVE( SAMPLING_SUPPORT )

#define SCOREP_ENTER_SIGNAL_CONTEXT() do {} while ( 0 )

#define SCOREP_EXIT_SIGNAL_CONTEXT() do {} while ( 0 )

#define SCOREP_IN_SIGNAL_CONTEXT() 0

#endif /* !HAVE( THREAD_LOCAL_STORAGE ) || !HAVE( SAMPLING_SUPPORT ) */

UTILS_END_C_DECLS
