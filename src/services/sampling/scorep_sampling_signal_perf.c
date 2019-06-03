/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017, 2019,
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
#include <UTILS_Error.h>

#if defined( __PGI )
// work around missing __builtin_constant_p used in swab.h.
// https://www.pgroup.com/userforum/viewtopic.php?t=6100
#define __builtin_constant_p( x ) ( 0 )
#endif
#include <linux/perf_event.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/mman.h>

/** Length of the memory mapping
 *  mmap size should be 1+2^n pages,
 *  first page is a metadata page */
#define MMAP_LENGTH     ( 3 ) * sysconf( _SC_PAGESIZE )

/** Definition handle if this interrupt generator */
static SCOREP_InterruptGeneratorHandle perf_interrupt_generator = SCOREP_INVALID_INTERRUPT_GENERATOR;


/* *********************************************************************
 * Helper functions
 **********************************************************************/

/**
 * Returns a PERF metric. If metric's type is PERF_TYPE_MAX
 * the metric is invalid.
 *
 * @param name              Metric name
 * @param returnMetric      Returned PERF metric
 *
 * @return PERF metric representing user specifications
 */
static int
perf_parse_event( char*                   name,
                  struct perf_event_attr* returnMetric )
{
    returnMetric->type = PERF_TYPE_MAX;

    /* hardware events */
    if ( ( strstr( name, "cpu-cycles" ) == name ) || ( strstr( name, "cycles" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_CPU_CYCLES;
        return 0;
    }
#if HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_FRONTEND )
    else
    if ( ( strstr( name, "stalled-cycles-frontend" ) == name ) || ( strstr( name, "idle-cycles-frontend" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
        return 0;
    }
#endif /* HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_FRONTEND ) */
#if HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_BACKEND )
    else
    if ( ( strstr( name, "stalled-cycles-backend" ) == name ) || ( strstr( name, "idle-cycles-backend" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
        return 0;
    }
#endif /* HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_BACKEND ) */
#if HAVE( DECL_PERF_COUNT_HW_REF_CPU_CYCLES )
    else
    if ( strstr( name, "ref-cycles" ) == name )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_REF_CPU_CYCLES;
        return 0;
    }
#endif /* HAVE( DECL_PERF_COUNT_HW_REF_CPU_CYCLES ) */
    else
    if ( strstr( name, "instructions" ) == name )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_INSTRUCTIONS;
        return 0;
    }
    if ( strstr( name, "cache-references" ) == name )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_CACHE_REFERENCES;
        return 0;
    }
    if ( strstr( name, "cache-misses" ) == name )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_CACHE_MISSES;
        return 0;
    }
    if ( ( strstr( name, "branch-instructions" ) == name ) || ( strstr( name, "branches" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
        return 0;
    }
    if ( strstr( name, "branch-misses" ) == name )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_BRANCH_MISSES;
        return 0;
    }
    if ( strstr( name, "bus-cycles" ) == name )
    {
        returnMetric->type   = PERF_TYPE_HARDWARE;
        returnMetric->config = PERF_COUNT_HW_BUS_CYCLES;
        return 0;
    }

    /* software events */
    if ( strstr( name, "cpu-clock" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_CPU_CLOCK;
    }
    if ( strstr( name, "task-clock" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_TASK_CLOCK;
    }
    if ( ( strstr( name, "page-faults" ) == name ) || ( strstr( name, "faults" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_PAGE_FAULTS;
    }
    if ( strstr( name, "minor-faults" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_PAGE_FAULTS_MIN;
    }
    if ( strstr( name, "major-faults" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_PAGE_FAULTS_MAJ;
    }
    if ( strstr( name, "major-faults" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_PAGE_FAULTS_MAJ;
    }
    if ( ( strstr( name, "context-switches" ) == name ) || ( strstr( name, "cs" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_CONTEXT_SWITCHES;
    }
    if ( ( strstr( name, "cpu-migrations" ) == name ) || ( strstr( name, "migrations" ) == name ) )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_CPU_MIGRATIONS;
    }
#if HAVE( DECL_PERF_COUNT_SW_ALIGNMENT_FAULTS )
    if ( strstr( name, "alignment-faults" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_ALIGNMENT_FAULTS;
    }
#endif /* HAVE( DECL_PERF_COUNT_SW_ALIGNMENT_FAULTS ) */
#if HAVE( DECL_PERF_COUNT_SW_EMULATION_FAULTS )
    if ( strstr( name, "emulation-faults" ) == name )
    {
        returnMetric->type   = PERF_TYPE_SOFTWARE;
        returnMetric->config = PERF_COUNT_SW_EMULATION_FAULTS;
    }
#endif /* HAVE( DECL_PERF_COUNT_SW_EMULATION_FAULTS ) */

    if ( returnMetric->type == PERF_TYPE_SOFTWARE )
    {
        return 0;
    }

    /* cache events */
    char  buffer[ 128 ];
    int   num_chars    = 0;
    char* caches[ 7 ]  = { "L1-dcache-", "L1-icache-", "LLC-", "dTLB-", "iTLB-", "branch-", "node-" };
    int   num_caches   = 7;
    char* actions[ 3 ] = { "load", "store", "prefetch" };
    int   num_actions  = 2;
    char* results[ 3 ] = { "s", "-misses", "es" };
    int   num_results  = 3;
    for ( int i = 0; i < num_caches; i++ )
    {
        if ( strstr( name, caches[ i ] ) == name )
        {
            for ( int j = 0; j < num_actions; j++ )
            {
                for ( int k = 0; k < num_results; k++ )
                {
                    num_chars = snprintf( buffer, 127, "%s%s%s", caches[ i ], actions[ j ], results[ k ] );
                    /* TODO assert that num_chars is < 127 */
                    if ( strstr( name, buffer ) == name )
                    {
                        returnMetric->type   = PERF_TYPE_HW_CACHE;
                        returnMetric->config = i << 16 | j << 8;
                        if ( k == 1 )
                        {
                            returnMetric->config |= 1;
                        }
                        return 0;
                    }
                }
            }
        }
    }
    /* raw events */
    if ( name[ 0 ] == 'r' )
    {
        /* get event */
        returnMetric->type   = PERF_TYPE_RAW;
        returnMetric->config = strtoll( &( name[ 1 ] ), NULL, 16 );
        return 0;
    }
    return 1;
}

/**
 * The signal handler. This is the only function triggered by a sample.
 *
 * @param signalNumber          Signal number
 * @param signalInfo            Signal information
 * @param contextPtr            Context
 */
static void
perf_signal_handler( int        signalNumber,
                     siginfo_t* signalInfo,
                     void*      contextPtr )
{
    bool outside = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    SCOREP_ENTER_SIGNAL_CONTEXT();

    if ( outside && scorep_sampling_is_enabled() )
    {
        SCOREP_Sample( perf_interrupt_generator, contextPtr );
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
    perf_interrupt_generator =
        SCOREP_Definitions_NewInterruptGenerator( definition.event,
                                                  SCOREP_INTERRUPT_GENERATOR_MODE_COUNT,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  0,
                                                  definition.period );
}

static void
create_interrupt_generator( scorep_sampling_single_location_data*          samplingData,
                            scorep_sampling_interrupt_generator_definition definition )
{
    if ( !samplingData->perf_fd )
    {
        struct perf_event_attr perf_attr;
        struct sigaction       action;
        struct f_owner_ex      fown_ex;

        memset( &perf_attr, 0, sizeof( struct perf_event_attr ) );
        perf_attr.disabled = 1;
        perf_attr.type     = PERF_TYPE_HARDWARE;

        /* parse samplingEvent for name and interval */
        if ( strstr( definition.event, "perf_" ) != definition.event )
        {
            UTILS_WARNING( "%s is not a valid perf event\n", definition.event );
            return;
        }
        if ( perf_parse_event( &( definition.event[ 5 ] ), &perf_attr ) )
        {
            UTILS_WARNING( "%s is not a valid perf event\n", definition.event );
            return;
        }
        perf_attr.sample_type    = PERF_SAMPLE_IP;
        perf_attr.mmap           = 1;
        perf_attr.wakeup_events  = 1;
        perf_attr.exclude_kernel = 1;   /* don't count kernel */
        perf_attr.exclude_hv     = 1;   /* don't count hypervisor */
        perf_attr.size           = sizeof( struct perf_event_attr );
        perf_attr.sample_period  = definition.period;

        samplingData->perf_fd = syscall( __NR_perf_event_open, &perf_attr, 0, -1, -1, 0 ); /* allocate memory for mmap */
        if ( samplingData->perf_fd < 1 )
        {
            UTILS_WARNING( "Error: perf_event_open failed." );
            return;
        }

        if ( fcntl( samplingData->perf_fd, F_SETFL, O_ASYNC | O_NONBLOCK ) )
        {
            UTILS_WARNING( "perf event sampling: Error changing fd" );
            return;
        }

        fown_ex.type = F_OWNER_TID;
        fown_ex.pid  = syscall( SYS_gettid );
        if ( fcntl( samplingData->perf_fd, F_SETOWN_EX, ( unsigned long )&fown_ex ) < 0 )
        {
            UTILS_WARNING( "perf event sampling: Error owning fd" );
            return;
        }
        if ( fcntl( samplingData->perf_fd, F_SETFD, FD_CLOEXEC ) )
        {
            UTILS_WARNING( "perf event sampling: Error while setting close-on-exec flag" );
            return;
        }

        /*
         * Call mmap to work around Linux kernel bug:
         *
         * Before kernel 2.6.39, there is a bug that means you must
         * allocate a mmap ring buffer when sampling even if you
         * do not plan to access it. This ring-buffer is created
         * and accessed through mmap.
         */
        samplingData->perf_mmap_buffer = mmap( NULL, MMAP_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, samplingData->perf_fd, 0 );
        if ( samplingData->perf_mmap_buffer == MAP_FAILED )
        {
            UTILS_WARNING( "perf event sampling: Error: mmap failed (%i).\n", errno );
            return;
        }

        if ( fcntl( samplingData->perf_fd, F_SETSIG, SIGPROF ) )
        {
            UTILS_WARNING( "perf event sampling: Unable to create signal" );
            return;
        }
        memset( &action, 0, sizeof( struct sigaction ) );
        action.sa_flags     = SA_RESTART;
        action.sa_flags    |= SA_SIGINFO;
        action.sa_sigaction =
            ( void ( * )( int, siginfo_t*, void* ) )perf_signal_handler;
        if ( sigaction( SIGPROF, &action, NULL ) < 0 )
        {
            UTILS_WARNING( "perf event sampling: Unable to install signal handler" );
            return;
        }
    }

    if ( ioctl( samplingData->perf_fd, PERF_EVENT_IOC_ENABLE ) < 0 )
    {
        UTILS_WARNING( "perf event sampling: Error while enabling event " );
        return;
    }
}

static void
enable_interrupt_generator( scorep_sampling_single_location_data* samplingData,
                            uint64_t                              samplingInterval )
{
    if ( ioctl( samplingData->perf_fd, PERF_EVENT_IOC_ENABLE ) < 0 )
    {
        UTILS_WARNING( "Error while enabling perf event sampling" );
        return;
    }
}

static void
disable_interrupt_generator( scorep_sampling_single_location_data* samplingData )
{
    if ( ioctl( samplingData->perf_fd, PERF_EVENT_IOC_DISABLE ) < 0 )
    {
        UTILS_WARNING( "Error while disabling event " );
        return;
    }
}

static void
finalize_interrupt_generator( scorep_sampling_single_location_data* samplingData )
{
    if ( ioctl( samplingData->perf_fd, PERF_EVENT_IOC_DISABLE ) < 0 )
    {
        UTILS_WARNING( "Error while finalizing perf interrupt generator: disabling event failed." );
    }
    if ( close( samplingData->perf_fd ) < 0 )
    {
        UTILS_WARNING( "Error while finalizing perf interrupt generator: closing file descriptor failed." );
    }
}


const scorep_sampling_interrupt_generator scorep_sampling_interrupt_generator_perf =
{
    .initialize_interrupt_generator = initialize_interrupt_generator,
    .create_interrupt_generator     = create_interrupt_generator,
    .enable_interrupt_generator     = enable_interrupt_generator,
    .disable_interrupt_generator    = disable_interrupt_generator,
    .finalize_interrupt_generator   = finalize_interrupt_generator
};
