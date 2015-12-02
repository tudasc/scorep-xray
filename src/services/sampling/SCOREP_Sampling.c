/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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
 */
#include <config.h>

#include "SCOREP_Sampling.h"

#include <SCOREP_InMeasurement.h>
#include <UTILS_Error.h>

#include <signal.h>

/** Variable for thread-specific data key */
SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile sig_atomic_t scorep_sampling_is_known_pthread = SCOREP_SAMPLING_UNKNOWN_THREAD;

/** Signal handler for itimer interrupts */
extern const scorep_sampling_interrupt_generator scorep_sampling_interrupt_generator_itimer;
#if HAVE( PAPI )
/** Signal handler for PAPI interrupts */
extern const scorep_sampling_interrupt_generator scorep_sampling_interrupt_generator_papi;
#endif
#if HAVE( METRIC_PERF )
/** Signal handler for PERF interrupts */
extern const scorep_sampling_interrupt_generator scorep_sampling_interrupt_generator_perf;
#endif

/* Insert signal handlers according to the order of
 * SCOREP_Sampling_InterruptGeneratorType. The type will be
 * used for indexing in the array. */
static const scorep_sampling_interrupt_generator* scorep_sampling_interrupt_generators[] = {
#if HAVE( SAMPLING_SIGACTION )
    &scorep_sampling_interrupt_generator_itimer,
#else
    NULL,
#endif
#if HAVE( PAPI )
    &scorep_sampling_interrupt_generator_papi,
#else
    NULL,
#endif
#if HAVE( METRIC_PERF )
    &scorep_sampling_interrupt_generator_perf
#else
    NULL
#endif
};

/* *********************************************************************
 * Function implementation
 * ********************************************************************/

void
scorep_initialize_interrupt_sources( scorep_sampling_interrupt_generator_definition samplingDef )
{
    if ( scorep_sampling_interrupt_generators[ samplingDef.type ] &&
         scorep_sampling_interrupt_generators[ samplingDef.type ]->initialize_interrupt_generator )
    {
        scorep_sampling_interrupt_generators[ samplingDef.type ]->initialize_interrupt_generator( samplingDef );
    }
}

void
scorep_create_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                 scorep_sampling_interrupt_generator_definition* samplingDef,
                                 size_t                                          nrSamplingDef )
{
    int    retval;
    size_t i;
    if ( scorep_sampling_is_known_pthread != SCOREP_SAMPLING_UNKNOWN_THREAD )
    {
        return;
    }

    for ( i = 0; i < nrSamplingDef; i++ )
    {
        if ( scorep_sampling_interrupt_generators[ samplingDef[ i ].type ] &&
             scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->create_interrupt_generator )
        {
            scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->create_interrupt_generator( &( samplingData->data[ i ] ),
                                                                                                       samplingDef[ i ] );
        }
        else
        {
            UTILS_WARNING( "The sub-system for sampling event %s is not supported", samplingDef[ i ].event );
        }
    }
}

void
scorep_enable_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                 scorep_sampling_interrupt_generator_definition* samplingDef,
                                 size_t                                          nrSamplingDef )
{
    size_t i;
    if ( scorep_sampling_is_known_pthread == SCOREP_SAMPLING_ENABLED_THREAD )
    {
        return;
    }
    scorep_sampling_is_known_pthread = SCOREP_SAMPLING_ENABLED_THREAD;

    return;

    for ( i = 0; i < nrSamplingDef; i++ )
    {
        if ( scorep_sampling_interrupt_generators[ samplingDef[ i ].type ] &&
             scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->enable_interrupt_generator )
        {
            scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->enable_interrupt_generator( &( samplingData->data[ i ] ),
                                                                                                       samplingDef[ i ].period );
        }
    }
}

void
scorep_disable_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                  scorep_sampling_interrupt_generator_definition* samplingDef,
                                  size_t                                          nrSamplingDef )
{
    /* only stop if enabled */
    if ( scorep_sampling_is_known_pthread != SCOREP_SAMPLING_ENABLED_THREAD )
    {
        return;
    }
    scorep_sampling_is_known_pthread = SCOREP_SAMPLING_DISABLED_THREAD;
    return;

    for ( size_t i = 0; i < nrSamplingDef; i++ )
    {
        /* Stop counting */
        if ( scorep_sampling_interrupt_generators[ samplingDef[ i ].type ] &&
             scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->disable_interrupt_generator )
        {
            scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->disable_interrupt_generator( &( samplingData->data[ i ] ) );
        }
    }
}

void
scorep_finalize_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                   scorep_sampling_interrupt_generator_definition* samplingDef,
                                   size_t                                          nrSamplingDef )
{
    /* only stop if enabled */
    if ( scorep_sampling_is_known_pthread != SCOREP_SAMPLING_ENABLED_THREAD )
    {
        return;
    }

    for ( size_t i = 0; i < nrSamplingDef; i++ )
    {
        /* Call finalization routine of interrupt sources */
        if ( scorep_sampling_interrupt_generators[ samplingDef[ i ].type ] &&
             scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->finalize_interrupt_generator )
        {
            scorep_sampling_interrupt_generators[ samplingDef[ i ].type ]->finalize_interrupt_generator( &( samplingData->data[ i ] ) );
        }
    }
}
