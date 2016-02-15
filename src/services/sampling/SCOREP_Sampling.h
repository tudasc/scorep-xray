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

#ifndef SCOREP_SAMPLING_H
#define SCOREP_SAMPLING_H

/**
 * @file
 *
 */

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>


/* *********************************************************************
 * Data types
 * ********************************************************************/

/** Sources that can be used to generate interrupts for sampling */
typedef enum scorep_sampling_interrupt_generator_type
{
    /** Interval timer */
    SCOREP_SAMPLING_TRIGGER_ITIMER = 0,
    /** PAPI event */
    SCOREP_SAMPLING_TRIGGER_PAPI,
    /** Linux perf event */
    SCOREP_SAMPLING_TRIGGER_PERF,

    /** For internal use only. */
    SCOREP_SAMPLING_TRIGGER_MAX
} scorep_sampling_interrupt_generator_type;


/** Thread local data for sampling */
typedef struct scorep_sampling_single_location_data
{
#if HAVE( PAPI )
    int papi_eventset;
    int papi_code;
#endif

#if HAVE( METRIC_PERF )
    int   perf_fd;
    void* perf_mmap_buffer;
#endif

#if !HAVE( PAPI ) && !HAVE( METRIC_PERF )
    /** Dummy value to avoid empty structure if neither PAPI nor PERF is supported */
    bool dummy;
#endif
} scorep_sampling_single_location_data;


/** Thread local data for the profiling system */
typedef struct SCOREP_Sampling_LocationData
{
    scorep_sampling_single_location_data* data;
    size_t                                nr_data;
} SCOREP_Sampling_LocationData;



/** Sampling source datatype */
typedef struct scorep_sampling_interrupt_generator_definition
{
    /** Interrupt generator: TIMER, PERF, or a PAPI event */
    scorep_sampling_interrupt_generator_type type;
    /** Name of the interrupt event, used only by PAPI, e.g. PAPI_L1_DCM */
    char*                                    event;
    /** Interrupt period */
    uint64_t                                 period;
} scorep_sampling_interrupt_generator_definition;


/**
 * Callbacks of a signal handler used by sampling.
 * They are collected in this structure for easy handling.
 */
typedef struct scorep_sampling_interrupt_generator
{
    void ( * initialize_interrupt_generator )( scorep_sampling_interrupt_generator_definition definition );

    void ( * create_interrupt_generator )( scorep_sampling_single_location_data*          samplingData,
                                           scorep_sampling_interrupt_generator_definition definition );

    void ( * enable_interrupt_generator )( scorep_sampling_single_location_data* samplingData,
                                           uint64_t                              samplingInterval );

    void ( * disable_interrupt_generator )( scorep_sampling_single_location_data* samplingData );

    void ( * finalize_interrupt_generator )( scorep_sampling_single_location_data* samplingData );
} scorep_sampling_interrupt_generator;


/* *********************************************************************
 * Functions
 * ********************************************************************/

/**
 * Variable to check whether this thread is enabled and current sample
 * should be processed.
 */
extern SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile sig_atomic_t scorep_sampling_is_known_pthread;

/** Values for scorep_sampling_is_known_pthread */
enum
{
    SCOREP_SAMPLING_UNKNOWN_THREAD  = 0,
    SCOREP_SAMPLING_ENABLED_THREAD  = 1,
    SCOREP_SAMPLING_DISABLED_THREAD = 2
};

/**
 * Check whether this thread is enabled and current sample
 * should be processed.
 *
 * @return True if this thread is enabled and sample
 *         should be processed, otherwise false
 */
#define scorep_sampling_is_enabled() \
    ( scorep_sampling_is_known_pthread == SCOREP_SAMPLING_ENABLED_THREAD )

/**
 * General initialization of the interrupt sources (e.g., allocate memory).
 *
 * @param definition        Interrupt generator definition (type, event, period)
 */
void
scorep_initialize_interrupt_sources( scorep_sampling_interrupt_generator_definition definition );

/**
 * Set up interrupt sources (e.g., register them).
 *
 * @param samplingData      Location specific sampling data
 * @param definitions       List of interrupt generator definitions
 * @param numDefinitions    Number of elements in @ definitions
 */
void
scorep_create_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                 scorep_sampling_interrupt_generator_definition* definitions,
                                 size_t                                          numDefinitions );

/**
 * Enable interrupt sources. After calling this function the interrupt
 * sources will run and produce interrupts.
 *
 * @param samplingData      Location specific sampling data
 * @param definitions       List of interrupt generator definitions
 * @param numDefinitions    Number of elements in @ definitions
 */
void
scorep_enable_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                 scorep_sampling_interrupt_generator_definition* definitions,
                                 size_t                                          numDefinitions );

/**
 * Temporarily disable interrupt sources. After calling this function
 * the interrupt sources won't produce further interrupts.
 *
 * @param samplingData      Location specific sampling data
 * @param definitions       List of interrupt generator definitions
 * @param numDefinitions    Number of elements in @ definitions
 */
void
scorep_disable_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                  scorep_sampling_interrupt_generator_definition* definitions,
                                  size_t                                          numDefinitions );

/**
 * Finalize interrupt sources.
 *
 * @param samplingData      Location specific sampling data
 * @param definitions       List of interrupt generator definitions
 * @param numDefinitions    Number of elements in @ definitions
 */
void
scorep_finalize_interrupt_sources( SCOREP_Sampling_LocationData*                   samplingData,
                                   scorep_sampling_interrupt_generator_definition* definitions,
                                   size_t                                          numDefinitions );

#endif /* SCOREP_SAMPLING_H */
