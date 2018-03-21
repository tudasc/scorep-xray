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

static SCOREP_RegionHandle user_region;

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

int
main( int ac, char* av[] )
{
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    user_region = SCOREP_Definitions_NewRegion( "USER",
                                                "USER",
                                                SCOREP_INVALID_SOURCE_FILE,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_PARADIGM_USER,
                                                SCOREP_REGION_FUNCTION );

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

    SCOREP_EnterRegion( user_region );

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

    SCOREP_ExitRegion( user_region );

    raise( SIGUSR1 );

    printf( "%u %u %u\n", a, b, c );

    return 0;
}

/*
 * expected:
 *
 * PR   MEASUREMENT OFF:MEASUREMENT OFF	   ( id=0, -1, -1, paradigm=user, role=artificial, url=, descr=, mod=)
 * PR   TRACE BUFFER FLUSH:TRACE BUFFER FLUSH	   ( id=1, -1, -1, paradigm=measurement, role=artificial, url=, descr=, mod=)
 * PR   THREADS:THREADS	   ( id=2, -1, -1, paradigm=measurement, role=artificial, url=, descr=, mod=THREADS)
 * PR   USER:USER	   ( id=3, -1, -1, paradigm=user, role=function, url=, descr=, mod=)
 * PR   main:main	   ( id=4, 37, 104, paradigm=SAMPLING, role=function, url=, descr=, mod=.../test/services/unwinding/test_cct_4.c)
 *
 * PC       Visits (E) | Diff-Calltree
 * PC                3 |  * main
 * PC                1 |  |  * USER
 *
 * 1 calling context node for "main" as region
 * 7 calling context nodes with "USER" as region, but different SCLs, parent is always "main"
 *
 * TD REGION                               0  Name: "USER" <> (Aka. "USER" <>), Descr.: "" <>, Role: FUNCTION, Paradigm: USER, Flags: NONE, File: UNDEFINED, Begin: 0, End: 0
 * TD REGION                               1  Name: "main" <> (Aka. "main" <>), Descr.: "" <>, Role: FUNCTION, Paradigm: SAMPLING, Flags: NONE, File: ".../test/services/unwinding/test_cct_4.c" <>, Begin: 37, End: 95
 * TD CALLING_CONTEXT                      0  Region: "main" <1>, Source code location: ".../test/services/unwinding/test_cct_4.c:58" <0>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      1  Region: "main" <1>, Source code location: ".../test/services/unwinding/test_cct_4.c:60" <1>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      2  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:60" <1>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      3  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:67" <2>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      4  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:74" <3>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      5  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:81" <4>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      6  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:88" <5>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      7  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:95" <6>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      8  Region: "USER" <0>, Source code location: ".../test/services/unwinding/test_cct_4.c:97" <7>, Parent: "main@test_cct_4.c:60" <1>
 * TD CALLING_CONTEXT                      9  Region: "main" <1>, Source code location: ".../test/services/unwinding/test_cct_4.c:99" <8>, Parent: UNDEFINED
 * TD INTERRUPT_GENERATOR                  0  Name: "manually" <>, Mode: COUNT, Base: DECIMAL, Exponent: 0, Period: 1
 *
 * 1 sample event with "main"
 * 1 enter event with "USER"
 * 5 sample events, each references another "USER" node, unwind distance is always 1
 * 1 leave event with "USER"
 * 1 sample event with "main"
 *
 * TE CALLING_CONTEXT_SAMPLE                     0     1  Calling Context: "main@test_cct_4.c:58" <0>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_4.c:58" <0>
 * TE CALLING_CONTEXT_ENTER                      0     2  Calling Context: "USER@test_cct_4.c:60" <2>, Unwind Distance: 3
 * TE                                                     +"USER@test_cct_4.c:60" <2>
 * TE                                                     +"main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     3  Calling Context: "USER@test_cct_4.c:67" <3>, Unwind Distance: 1, Interrupt Generator: "manually" <0>
 * TE                                                     *"USER@test_cct_4.c:67" <3>
 * TE                                                      "main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     4  Calling Context: "USER@test_cct_4.c:74" <4>, Unwind Distance: 1, Interrupt Generator: "manually" <0>
 * TE                                                     *"USER@test_cct_4.c:74" <4>
 * TE                                                      "main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     5  Calling Context: "USER@test_cct_4.c:81" <5>, Unwind Distance: 1, Interrupt Generator: "manually" <0>
 * TE                                                     *"USER@test_cct_4.c:81" <5>
 * TE                                                      "main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     6  Calling Context: "USER@test_cct_4.c:88" <6>, Unwind Distance: 1, Interrupt Generator: "manually" <0>
 * TE                                                     *"USER@test_cct_4.c:88" <6>
 * TE                                                      "main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     7  Calling Context: "USER@test_cct_4.c:95" <7>, Unwind Distance: 1, Interrupt Generator: "manually" <0>
 * TE                                                     *"USER@test_cct_4.c:95" <7>
 * TE                                                      "main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_LEAVE                      0     8  Calling Context: "USER@test_cct_4.c:97" <8>
 * TE                                                     *"USER@test_cct_4.c:97" <8>
 * TE                                                      "main@test_cct_4.c:60" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     9  Calling Context: "main@test_cct_4.c:99" <9>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_4.c:99" <9>
 */
