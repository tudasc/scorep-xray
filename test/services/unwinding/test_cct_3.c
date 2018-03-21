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

void
bar( const char* arg )
{
    size_t len = strlen( arg );
    size_t i   = 0;

    uint32_t a = 0, b = 0, c = 0;

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    raise( SIGUSR1 );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    raise( SIGUSR1 );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    raise( SIGUSR1 );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    raise( SIGUSR1 );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    raise( SIGUSR1 );

    printf( "%u %u %u\n", a, b, c );
}

int
main( int ac, char* av[] )
{
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

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

    const char* arg = ac > 2 ? av[ 1 ] : "abc";

    raise( SIGUSR1 );

    bar( arg );

    raise( SIGUSR1 );

    return 0;
}

/*
 * expected:
 *
 * PR   MEASUREMENT OFF:MEASUREMENT OFF	   ( id=0, -1, -1, paradigm=user, role=artificial, url=, descr=, mod=)
 * PR   TRACE BUFFER FLUSH:TRACE BUFFER FLUSH	   ( id=1, -1, -1, paradigm=measurement, role=artificial, url=, descr=, mod=)
 * PR   THREADS:THREADS	   ( id=2, -1, -1, paradigm=measurement, role=artificial, url=, descr=, mod=THREADS)
 * PR   main:main	   ( id=3, 82, 99, paradigm=SAMPLING, role=function, url=, descr=, mod=.../test/services/unwinding/test_cct_3.c)
 * PR   bar:bar	   ( id=4, 36, 78, paradigm=SAMPLING, role=function, url=, descr=, mod=.../test/services/unwinding/test_cct_3.c)
 *
 * PC       Visits (E) | Diff-Calltree
 * PC                7 |  * main
 * PC                5 |  |  * bar
 *
 * 1 calling context node with "main" as region
 * 5 calling context nodes with "bar" as region, but different SCLs, and all have "main" as parent
 * TD REGION                               0  Name: "bar" <> (Aka. "bar" <>), Descr.: "" <>, Role: FUNCTION, Paradigm: SAMPLING, Flags: NONE, File: ".../test/services/unwinding/test_cct_3.c" <>, Begin: 36, End: 78
 * TD REGION                               1  Name: "main" <> (Aka. "main" <>), Descr.: "" <>, Role: FUNCTION, Paradigm: SAMPLING, Flags: NONE, File: ".../test/services/unwinding/test_cct_3.c" <>, Begin: 82, End: 90
 * TD CALLING_CONTEXT                      0  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_3.c:92" <0>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      1  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_3.c:94" <1>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      2  Region: "bar" <1>, Source code location: ".../test/services/unwinding/test_cct_3.c:47" <2>, Parent: "main@test_cct_3.c:94" <1>
 * TD CALLING_CONTEXT                      3  Region: "bar" <1>, Source code location: ".../test/services/unwinding/test_cct_3.c:54" <3>, Parent: "main@test_cct_3.c:94" <1>
 * TD CALLING_CONTEXT                      4  Region: "bar" <1>, Source code location: ".../test/services/unwinding/test_cct_3.c:61" <4>, Parent: "main@test_cct_3.c:94" <1>
 * TD CALLING_CONTEXT                      5  Region: "bar" <1>, Source code location: ".../test/services/unwinding/test_cct_3.c:68" <5>, Parent: "main@test_cct_3.c:94" <1>
 * TD CALLING_CONTEXT                      6  Region: "bar" <1>, Source code location: ".../test/services/unwinding/test_cct_3.c:75" <6>, Parent: "main@test_cct_3.c:94" <1>
 * TD CALLING_CONTEXT                      7  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_3.c:96" <7>, Parent: UNDEFINED
 * TD INTERRUPT_GENERATOR                  0  Name: "manually" <>, Mode: COUNT, Base: DECIMAL, Exponent: 0, Period: 1
 *
 * 7 sample events, first and last references "main", the other references another "bar" node, i.e., each "bar" node is referenced once
 * TE CALLING_CONTEXT_SAMPLE                     0     1  Calling Context: "main@test_cct_3.c:92" <0>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_3.c:92" <0>
 * TE CALLING_CONTEXT_SAMPLE                     0     2  Calling Context: "bar@test_cct_3.c:47" <2>, Unwind Distance: 3, Interrupt Generator: "manually" <0>
 * TE                                                     +"bar@test_cct_3.c:47" <2>
 * TE                                                     +"main@test_cct_3.c:94" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     3  Calling Context: "bar@test_cct_3.c:54" <3>, Unwind Distance: 3, Interrupt Generator: "manually" <0>
 * TE                                                     +"bar@test_cct_3.c:54" <3>
 * TE                                                     +"main@test_cct_3.c:94" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     4  Calling Context: "bar@test_cct_3.c:61" <4>, Unwind Distance: 3, Interrupt Generator: "manually" <0>
 * TE                                                     +"bar@test_cct_3.c:61" <4>
 * TE                                                     +"main@test_cct_3.c:94" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     5  Calling Context: "bar@test_cct_3.c:68" <5>, Unwind Distance: 3, Interrupt Generator: "manually" <0>
 * TE                                                     +"bar@test_cct_3.c:68" <5>
 * TE                                                     +"main@test_cct_3.c:94" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     6  Calling Context: "bar@test_cct_3.c:75" <6>, Unwind Distance: 3, Interrupt Generator: "manually" <0>
 * TE                                                     +"bar@test_cct_3.c:75" <6>
 * TE                                                     +"main@test_cct_3.c:94" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     7  Calling Context: "main@test_cct_3.c:96" <7>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_3.c:96" <7>
 */
