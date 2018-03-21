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
 */

#include <config.h>

#include "SCOREP_Sampling.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>

#define SCOREP_DEBUG_MODULE_NAME SAMPLING
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <string.h>
#include <sys/time.h>


/** Definition handle if this interrupt generator */
static SCOREP_InterruptGeneratorHandle timer_interrupt_generator = SCOREP_INVALID_INTERRUPT_GENERATOR;

/* *********************************************************************
 * Helper functions
 **********************************************************************/

/**
 * Set up the timer
 *
 * @param timer         itimer object
 * @param timeInUsec    Interval in micro seconds
 *
 */
static void
set_timer_value( struct itimerval* timer,
                 uint64_t          timeInUsec )
{
    /* Set the timer values */
    timer->it_interval.tv_sec  = timeInUsec / 1000000;
    timer->it_interval.tv_usec = timeInUsec % 1000000;
    timer->it_value.tv_sec     = timeInUsec / 1000000;
    timer->it_value.tv_usec    = timeInUsec % 1000000;
}

/**
 * The signal handler. This is the only function triggered by a sample.
 *
 * @param signalNumber          Signal number
 * @param signalInfo            Signal information
 * @param contextPtr            Context
 */
static void
timer_signal_handler( int        signalNumber,
                      siginfo_t* signalInfo,
                      void*      contextPtr )
{
    bool outside = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    SCOREP_ENTER_SIGNAL_CONTEXT();

    if ( outside && scorep_sampling_is_enabled() )
    {
        SCOREP_Sample( timer_interrupt_generator, contextPtr );
    }

    SCOREP_EXIT_SIGNAL_CONTEXT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/* *********************************************************************
 * Signal handler functions
 **********************************************************************/

static void
initialize_interrupt_generator( scorep_sampling_interrupt_generator_definition definition )
{
    timer_interrupt_generator =
        SCOREP_Definitions_NewInterruptGenerator( definition.event,
                                                  SCOREP_INTERRUPT_GENERATOR_MODE_TIME,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  -6 /* micro seconds */,
                                                  definition.period );
}

static void
create_interrupt_generator( scorep_sampling_single_location_data*          samplingData,
                            scorep_sampling_interrupt_generator_definition definition )
{
    UTILS_DEBUG( "Installing signal handler (frequency %d)", definition.period );

    /* Set up the signal handler */
    struct sigaction signal_action;
    memset( &signal_action, 0, sizeof( signal_action ) );
    signal_action.sa_sigaction = timer_signal_handler;
    /*
     * SA_SIGINFO:  we want the 3 argument version of the signal handler
     * SA_NODEFER:  make it possible to receive the signal when in its
     *              own signal handler
     * SA_RESTART:  restart interrupted system calls
     */
    signal_action.sa_flags = SA_SIGINFO | /* SA_NODEFER | */ SA_RESTART;
    /* A full set means no signal can arrive while the signal handler is
       running. All signals are blocked and queued */
    sigfillset( &signal_action.sa_mask );
    /* Allow this signal to arrive even when inside the signal handler.
     * We want to deal with it ourselves, queueing is not a good idea
     * here */

    /* Install the signal handler */
    if ( 0 != sigaction( SIGPROF, &signal_action, NULL ) )
    {
        UTILS_WARNING( "Failed to install signal handler for sampling." );
    }

    // Set up the signal timer
    struct itimerval signal_timer_value;
    set_timer_value( &signal_timer_value, definition.period );

    sigset_t signal_block_set;

    sigemptyset( &signal_block_set );
    sigaddset( &signal_block_set, SIGPROF );

    /* Start the timer */
    int retval = setitimer( ITIMER_PROF, &signal_timer_value, NULL );
    if ( retval != 0 )
    {
        UTILS_WARNING( "Failed to enable sampling timer: %s (ret=%i).", strerror( errno ), retval );
    }
}

static void
enable_interrupt_generator( scorep_sampling_single_location_data* samplingData,
                            uint64_t                              samplingInterval )
{
    UTILS_DEBUG( "Installing signal handler (frequency %d)", samplingInterval );

    /* Set up the signal handler */
    struct sigaction signal_action;
    memset( &signal_action, 0, sizeof( signal_action ) );
    signal_action.sa_sigaction = timer_signal_handler;
    /*
     * SA_SIGINFO:  we want the 3 argument version of the signal handler
     * SA_NODEFER:  make it possible to receive the signal when in its
     *              own signal handler
     * SA_RESTART:  restart interrupted system calls
     */
    signal_action.sa_flags = SA_SIGINFO | /* SA_NODEFER | */ SA_RESTART;
    /* A full set means no signal can arrive while the signal handler is
       running. All signals are blocked and queued */
    sigfillset( &signal_action.sa_mask );
    /* Allow this signal to arrive even when inside the signal handler.
       We want to deal with it ourselves, queuing is not a good idea here */
    /*sigdelset(&signal_action.sa_mask, SIGPROF);*/

    /* Install the signal handler */
    if ( 0 != sigaction( SIGPROF, &signal_action, NULL ) )
    {
        UTILS_WARNING( "Failed to install signal handler for sampling." );
    }

    // Set up the signal timer
    struct itimerval signal_timer_value;
    set_timer_value( &signal_timer_value, samplingInterval );

    sigset_t signal_block_set;

    sigemptyset( &signal_block_set );
    sigaddset( &signal_block_set, SIGPROF );


    /* Start the timer */
    if ( 0 != setitimer( ITIMER_PROF, &signal_timer_value, NULL ) )
    {
        UTILS_WARNING( "Failed to enable sampling timer." );
    }
}

static void
disable_interrupt_generator( scorep_sampling_single_location_data* samplingData )
{
    struct sigaction signal_action;
    memset( &signal_action, 0, sizeof( signal_action ) );
    signal_action.sa_handler = SIG_DFL;

    /* Install the signal handler */
    if ( 0 != sigaction( SIGPROF, &signal_action, NULL ) )
    {
        UTILS_WARNING( "Failed to uninstall signal handler for sampling." );
    }

    /* Stop the timer */
    if ( 0 != setitimer( ITIMER_PROF, NULL, NULL ) )
    {
        UTILS_WARNING( "Failed to deinitialize sampling timer." );
    }
}


const scorep_sampling_interrupt_generator scorep_sampling_interrupt_generator_itimer =
{
    .initialize_interrupt_generator = initialize_interrupt_generator,
    .create_interrupt_generator     = create_interrupt_generator,
    .enable_interrupt_generator     = enable_interrupt_generator,
    .disable_interrupt_generator    = disable_interrupt_generator,
    .finalize_interrupt_generator   = NULL
};
