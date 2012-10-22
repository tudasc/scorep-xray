/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       scorep_timer_cycle_counter_tsc.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * cycle_counter_tsc timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"

#include <UTILS_Error.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE( GETTIMEOFDAY ) && HAVE( USLEEP )
#include <sys/time.h>
#include <unistd.h>
#else
typedef long useconds_t;
#endif

#ifndef SCOREP_PROCDIR
#define SCOREP_PROCDIR "/proc/"
#endif

/** @todo investigate if we need scorep_ticks_per_sec on a per core basis if we
 * do frequency measurement instead of reading /proc/cpuinfo */
static uint64_t scorep_ticks_per_sec = 0;

static bool isInitialized = false;

static uint64_t
scorep_get_cylce_counter_frequency( useconds_t usleep_time );

static uint64_t
scorep_get_frequency_from_proc_cpuinfo( void );

static uint64_t
scorep_get_improved_frequency_from_timing_measurement( void );


void
SCOREP_Timer_Initialize( void )
{
    if ( isInitialized )
    {
        return;
    }

    scorep_ticks_per_sec = scorep_get_improved_frequency_from_timing_measurement();
    scorep_ticks_per_sec = scorep_get_frequency_from_proc_cpuinfo();
    assert( scorep_ticks_per_sec != 0 );

    isInitialized = true;
}


static uint64_t
scorep_get_frequency_from_proc_cpuinfo( void )
{
    FILE* cpuinfofp;
    if ( ( cpuinfofp = fopen( SCOREP_PROCDIR "cpuinfo", "r" ) ) == NULL )
    {
        UTILS_ERROR_POSIX();
        _Exit( EXIT_FAILURE );
    }

    char     line[ 1024 ];
    uint64_t ticks_per_sec = scorep_ticks_per_sec;
    while ( fgets( line, sizeof( line ), cpuinfofp ) )
    {
        if ( !strncmp( "cpu MHz", line, 7 ) )
        {
            strtok( line, ":" );
            ticks_per_sec = ( uint64_t )( strtod( ( char* )strtok( NULL, " \n" ), ( char** )NULL ) * 1e6 );
            break;
        }
        if ( !strncmp( "timebase", line, 8 ) )
        {
            strtok( line, ":" );
            ticks_per_sec = strtol( ( char* )strtok( NULL, " \n" ), ( char** )NULL, 0 );
            break;
        }
    }

    fclose( cpuinfofp );
    /* printf("updating cylce_counter freq to: %lu\n", ticks_per_sec); */
    return ticks_per_sec;
}


static uint64_t
scorep_get_improved_frequency_from_timing_measurement( void )
{
    /* try to something better than reading proc/cpuinfo on ia32 by doing
     * timing measurements on the TSC */

    bool     done             = false;
    int      num_measurements = 0;
    uint64_t ticks_per_sec    = scorep_ticks_per_sec;

    /** @todo seems unstable to me (roessel) because it does not reach
     * "ticks_per_sec = value" on every invocation */
    do /* ~100 milli sec sleeps until we have a stable value */
    {
        uint64_t value = scorep_get_cylce_counter_frequency( 100000 );
        /* printf("cylce_counter freq initial at: %llu\n", value); */
        /* at max two test against this value to see if it is stable; or to
         * see if we are able to read the get the same value (hopefully stable
         * means good) again
         */
        for ( int loop = 0; loop < 2; loop++ )
        {
            uint64_t test_value = scorep_get_cylce_counter_frequency( 100000 );
            /* printf("cylce_counter freq (t %d) at: %llu\n", loop, test_value); */
            uint64_t diff = ( test_value > value ) ? test_value - value : value - test_value;
            /* stable value is here defined as not more than 0.001% difference */
            if ( ( ( double )diff ) < ( ( double )0.00001 * value ) )
            {
                /* printf("updating cylce_counter freq to: %llu\n", value); */
                ticks_per_sec = value;
                done          = true;
                break;
            }
        }
        num_measurements++;
    }
    while ( !done && num_measurements < 3 );
    return ticks_per_sec;
}


#if HAVE( GETTIMEOFDAY ) && HAVE( USLEEP )
static uint64_t
scorep_get_cylce_counter_frequency( useconds_t usleep_time )
{
    struct timeval timestamp;

    /* start timestamp */
    uint64_t start1_cylce_counter = SCOREP_GetClockTicks();
    gettimeofday( &timestamp, NULL );
    uint64_t start2_cylce_counter = SCOREP_GetClockTicks();

    uint64_t start_time = timestamp.tv_sec * 1000000 + timestamp.tv_usec;

    usleep( usleep_time );

    /* end timestamp */
    uint64_t end1_cylce_counter = SCOREP_GetClockTicks();
    gettimeofday( &timestamp, NULL );
    uint64_t end2_cylce_counter = SCOREP_GetClockTicks();

    uint64_t end_time = timestamp.tv_sec * 1000000 + timestamp.tv_usec;

    uint64_t start_time_cylce_counter = ( start1_cylce_counter + start2_cylce_counter ) / 2;
    uint64_t end_time_cylce_counter   = ( end1_cylce_counter +  end2_cylce_counter ) / 2;

    /* freq is 1e6 * cylce_counter_time_diff/gettimeofday_time_diff */
    return ( uint64_t )( 1000000.0 * ( end_time_cylce_counter - start_time_cylce_counter ) /
                         ( double )( end_time - start_time ) );
}
#else
static uint64_t
scorep_get_cylce_counter_frequency( useconds_t usleep_time )
{
    return scorep_ticks_per_sec;
}
#endif /* HAVE( GETTIMEOFDAY ) && HAVE( USLEEP ) */


uint64_t
SCOREP_GetClockTicks( void )
{
    uint64_t clock_value;

# ifdef __powerpc64__

    asm volatile ( "mftb %0" : "=r" ( clock_value ) );

# elif defined( __powerpc__ ) || defined( __POWERPC__ )

    uint32_t low   = 0;
    uint32_t higha = 0;
    uint32_t highb = 0;

    do
    {
        asm volatile ( "mftbu %0" : "=r" ( highb ) );
        asm volatile ( "mftb %0" : "=r" ( low ) );
        asm volatile ( "mftbu %0" : "=r" ( higha ) );
    }
    while ( highb != higha );

    clock_value = ( ( uint64_t )higha << 32 ) | ( uint64_t )low;

# else

    uint32_t low  = 0;
    uint32_t high = 0;

    __asm__ volatile ( "rdtsc" : "=a" ( low ), "=d" ( high ) );

    clock_value = ( ( uint64_t )high << 32 ) | ( uint64_t )low;

# endif

    return clock_value;
}


uint64_t
SCOREP_GetClockResolution( void )
{
    return scorep_ticks_per_sec;
}


bool
SCOREP_ClockIsGlobal( void )
{
    return false;
}
