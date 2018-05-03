/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>

#define rot( x, k ) ( ( ( x ) << ( k ) ) | ( ( x ) >> ( 32 - ( k ) ) ) )
#define mix( a, b, c ) \
    { \
        a -= c;  a ^= rot( c, 4 );  c += b; \
        b -= a;  b ^= rot( a, 6 );  a += c; \
        c -= b;  c ^= rot( b, 8 );  b += a; \
        a -= c;  a ^= rot( c, 16 );  c += b; \
        b -= a;  b ^= rot( a, 19 );  a += c; \
        c -= b;  c ^= rot( b, 4 );  b += a; \
    }

static SCOREP_RegionHandle wrapped0_region;
static SCOREP_RegionHandle wrapped1_region;
static SCOREP_RegionHandle user0_region;
static SCOREP_RegionHandle user1_region;
static unsigned            active_wrapped;

#if HAVE( SAMPLING_SUPPORT )

static SCOREP_InterruptGeneratorHandle raise_irq_handle;

static void
handler_SIGUSR1( int        signalNumber,
                 siginfo_t* signalInfo,
                 void*      contextPtr )
{
    ( void )signalNumber;
    ( void )signalInfo;
    bool outside = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    SCOREP_ENTER_SIGNAL_CONTEXT();

    if ( outside )
    {
        SCOREP_Sample( raise_irq_handle, contextPtr );
    }

    SCOREP_EXIT_SIGNAL_CONTEXT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#else

#define raise( signal ) do { } while ( 0 )

#endif

size_t
wrapped0_internal( const char* arg,
                   size_t      len,
                   uint32_t*   a,
                   uint32_t*   b,
                   uint32_t*   c,
                   size_t      i )
{
    *a += arg[ i++ % len ];
    *b += arg[ i++ % len ];
    *c += arg[ i++ % len ];

    raise( SIGUSR1 );

    mix( ( *a ), ( *b ), ( *c ) );

    return i;
}

size_t
pwrapped0( const char* arg,
           size_t      len,
           uint32_t*   a,
           uint32_t*   b,
           uint32_t*   c,
           size_t      i )
{
    *a += arg[ i++ % len ];
    *b += arg[ i++ % len ];
    *c += arg[ i++ % len ];

    raise( SIGUSR1 );

    mix( ( *a ), ( *b ), ( *c ) );

    i = active_wrapped & 4 ? wrapped0_internal( arg, len, a, b, c, i ) : i;

    raise( SIGUSR1 );

    return i;
}

size_t
wrapped0( const char* arg,
          size_t      len,
          uint32_t*   a,
          uint32_t*   b,
          uint32_t*   c,
          size_t      i )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( active_wrapped & 1 )
    {
        SCOREP_EnterWrappedRegion( wrapped0_region );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( wrapped0_region );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    i = pwrapped0( arg, len, a, b, c, i );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( active_wrapped & 1 )
    {
        SCOREP_ExitRegion( wrapped0_region );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( wrapped0_region );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return i;
}

// simulates a MPI fortran wrapper
size_t
wrapped0_( const char* arg,
           size_t      len,
           uint32_t*   a,
           uint32_t*   b,
           uint32_t*   c,
           size_t      i )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    i = wrapped0( arg, len, a, b, c, i );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return i;
}

size_t
wrapped1_internal( const char* arg,
                   size_t      len,
                   uint32_t*   a,
                   uint32_t*   b,
                   uint32_t*   c,
                   size_t      i )
{
    *a += arg[ i++ % len ];
    *b += arg[ i++ % len ];
    *c += arg[ i++ % len ];

    raise( SIGUSR1 );

    mix( ( *a ), ( *b ), ( *c ) );

    i = ( active_wrapped & 2 ? wrapped0_ : wrapped0 )( arg, len, a, b, c, i );

    raise( SIGUSR1 );

    return i;
}

size_t
wrapped1( const char* arg,
          size_t      len,
          uint32_t*   a,
          uint32_t*   b,
          uint32_t*   c,
          size_t      i )
{
    *a += arg[ i++ % len ];
    *b += arg[ i++ % len ];
    *c += arg[ i++ % len ];

    raise( SIGUSR1 );

    mix( ( *a ), ( *b ), ( *c ) );

    i = ( active_wrapped & 32 ? wrapped1_internal : active_wrapped & 2 ? wrapped0_ : wrapped0 )( arg, len, a, b, c, i );

    raise( SIGUSR1 );

    return i;
}

size_t
__wrap_wrapped1( const char* arg,
                 size_t      len,
                 uint32_t*   a,
                 uint32_t*   b,
                 uint32_t*   c,
                 size_t      i )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( active_wrapped & 8 )
    {
        SCOREP_EnterWrappedRegion( wrapped1_region );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( wrapped1_region );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    i = wrapped1( arg, len, a, b, c, i );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( active_wrapped & 8 )
    {
        SCOREP_ExitRegion( wrapped1_region );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( wrapped1_region );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return i;
}

// simulates a MPI fortran wrapper
size_t
__wrap_wrapped1_( const char* arg,
                  size_t      len,
                  uint32_t*   a,
                  uint32_t*   b,
                  uint32_t*   c,
                  size_t      i )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    i = __wrap_wrapped1( arg, len, a, b, c, i );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return i;
}

int
main( int ac, char* av[] )
{
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    wrapped0_region = SCOREP_Definitions_NewRegion( "wrapped0",
                                                    NULL,
                                                    SCOREP_INVALID_SOURCE_FILE,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_PARADIGM_MEASUREMENT,
                                                    SCOREP_REGION_WRAPPER );
    wrapped1_region = SCOREP_Definitions_NewRegion( "wrapped1",
                                                    NULL,
                                                    SCOREP_INVALID_SOURCE_FILE,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_PARADIGM_MEASUREMENT,
                                                    SCOREP_REGION_WRAPPER );
    user0_region = SCOREP_Definitions_NewRegion( "user0",
                                                 NULL,
                                                 SCOREP_INVALID_SOURCE_FILE,
                                                 SCOREP_INVALID_LINE_NO,
                                                 SCOREP_INVALID_LINE_NO,
                                                 SCOREP_PARADIGM_USER,
                                                 SCOREP_REGION_USER );
    user1_region = SCOREP_Definitions_NewRegion( "user1",
                                                 NULL,
                                                 SCOREP_INVALID_SOURCE_FILE,
                                                 SCOREP_INVALID_LINE_NO,
                                                 SCOREP_INVALID_LINE_NO,
                                                 SCOREP_PARADIGM_USER,
                                                 SCOREP_REGION_USER );

#if HAVE( SAMPLING_SUPPORT )

    raise_irq_handle = SCOREP_Definitions_NewInterruptGenerator(
        "manually",
        SCOREP_INTERRUPT_GENERATOR_MODE_COUNT,
        SCOREP_METRIC_BASE_DECIMAL,
        0,
        1 );

    struct sigaction signal_action;
    memset( &signal_action, 0, sizeof( signal_action ) );
    signal_action.sa_sigaction = handler_SIGUSR1;
    signal_action.sa_flags     = SA_SIGINFO | SA_RESTART;
    sigfillset( &signal_action.sa_mask );
    if ( 0 != sigaction( SIGUSR1, &signal_action, NULL ) )
    {
        UTILS_WARNING( "Failed to install signal handler for sampling." );
    }

#endif

    uint32_t a = 0, b = 0, c = 0;

    const char* arg = ac > 2 ? av[ 1 ] : "abc";
    size_t      len = strlen( arg );
    size_t      i   = 0;

    raise( SIGUSR1 );

    for ( int active_regions = 0; active_regions < 3; active_regions++ )
    {
        if ( active_regions > 0 )
        {
            SCOREP_EnterRegion( user0_region );
        }

        raise( SIGUSR1 );

        if ( active_regions > 1 )
        {
            SCOREP_EnterRegion( user1_region );
        }

        for ( active_wrapped = 0; active_wrapped < 8; active_wrapped++ )
        {
            raise( SIGUSR1 );

            i = ( active_wrapped & 2 ? wrapped0_ : wrapped0 )( arg, len, &a, &b, &c, i );

            raise( SIGUSR1 );
        }

        for ( active_wrapped = 0; active_wrapped < 64; active_wrapped++ )
        {
            raise( SIGUSR1 );

            i = ( active_wrapped & 16 ? __wrap_wrapped1_ : __wrap_wrapped1 )( arg, len, &a, &b, &c, i );

            raise( SIGUSR1 );
        }

        if ( active_regions > 1 )
        {
            SCOREP_ExitRegion( user1_region );
        }

        raise( SIGUSR1 );

        if ( active_regions > 0 )
        {
            SCOREP_ExitRegion( user0_region );
        }
    }

    raise( SIGUSR1 );

    printf( "%u %u %u\n", a, b, c );

    return 0;
}

/*
 * expected:
 */
