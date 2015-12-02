/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#include <config.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_InMeasurement.h>
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


static SCOREP_InterruptGeneratorHandle raise_irq_handle;

static void
handler_SIGUSR1( int sig )
{
    ( void )sig;
    SCOREP_ENTER_SIGNAL_CONTEXT();
    SCOREP_Sample( raise_irq_handle );
    SCOREP_EXIT_SIGNAL_CONTEXT();
}

int
main( int ac, char* av[] )
{
    SCOREP_InterruptGeneratorHandle manually_irq_handle =
        SCOREP_Definitions_NewInterruptGenerator(
            "manually",
            SCOREP_INTERRUPT_GENERATOR_MODE_COUNT,
            SCOREP_METRIC_BASE_DECIMAL,
            0,
            1 );
    raise_irq_handle = SCOREP_Definitions_NewInterruptGenerator(
        "raise",
        SCOREP_INTERRUPT_GENERATOR_MODE_COUNT,
        SCOREP_METRIC_BASE_DECIMAL,
        0,
        1 );

    signal( SIGUSR1, handler_SIGUSR1 );

    uint32_t a = 0, b = 0, c = 0;

    const char* arg = ac > 2 ? av[ 1 ] : "abc";
    size_t      len = strlen( arg );
    size_t      i   = 0;

    SCOREP_Sample( manually_irq_handle );

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

    SCOREP_Sample( manually_irq_handle );

    return 0;
}

/*
 * expected:
 * TD REGION                               0  Name: "main" <> (Aka. "main" <>), Descr.: "" <>, Role: FUNCTION, Paradigm: SAMPLING, Flags: NONE, File: ".../test/services/unwinding/test_cct_8.c" <>, Begin: 50, End: 106
 * TD REGION                               1  Name: "raise" <> (Aka. "raise" <>), Descr.: "" <>, Role: FUNCTION, Paradigm: SAMPLING, Flags: NONE, File: UNDEFINED, Begin: 0, End: 0
 * TD CALLING_CONTEXT                      0  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_8.c:73" <0>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      1  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_8.c:75" <1>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      2  Region: "raise" <1>, Source code location: UNDEFINED, Parent: "main@test_cct_8.c:75" <1>
 * TD CALLING_CONTEXT                      3  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_8.c:82" <2>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      4  Region: "raise" <1>, Source code location: UNDEFINED, Parent: "main@test_cct_8.c:82" <3>
 * TD CALLING_CONTEXT                      5  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_8.c:89" <3>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      6  Region: "raise" <1>, Source code location: UNDEFINED, Parent: "main@test_cct_8.c:89" <5>
 * TD CALLING_CONTEXT                      7  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_8.c:96" <4>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      8  Region: "raise" <1>, Source code location: UNDEFINED, Parent: "main@test_cct_8.c:96" <7>
 * TD CALLING_CONTEXT                      9  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_8.c:103" <5>, Parent: UNDEFINED
 * TD INTERRUPT_GENERATOR                  0  Name: "manually" <>, Mode: COUNT, Base: DECIMAL, Exponent: 0, Period: 1
 * TD INTERRUPT_GENERATOR                  1  Name: "raise" <>, Mode: COUNT, Base: DECIMAL, Exponent: 0, Period: 1
 *
 * TE CALLING_CONTEXT_SAMPLE                     0     0  Calling Context: "main@test_cct_8.c:73" <0>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_8.c:73" <0>
 * TE CALLING_CONTEXT_SAMPLE                     0     1  Calling Context: "raise" <2>, Unwind Distance: 3, Interrupt Generator: "raise" <1>
 * TE                                                     +"raise" <2>
 * TE                                                     +"main@test_cct_8.c:75" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     2  Calling Context: "raise" <4>, Unwind Distance: 3, Interrupt Generator: "raise" <1>
 * TE                                                     +"raise" <4>
 * TE                                                     +"main@test_cct_8.c:82" <3>
 * TE CALLING_CONTEXT_SAMPLE                     0     3  Calling Context: "raise" <6>, Unwind Distance: 3, Interrupt Generator: "raise" <1>
 * TE                                                     +"raise" <6>
 * TE                                                     +"main@test_cct_8.c:89" <5>
 * TE CALLING_CONTEXT_SAMPLE                     0     4  Calling Context: "raise" <8>, Unwind Distance: 3, Interrupt Generator: "raise" <1>
 * TE                                                     +"raise" <8>
 * TE                                                     +"main@test_cct_8.c:96" <7>
 * TE CALLING_CONTEXT_SAMPLE                     0     5  Calling Context: "main@test_cct_8.c:103" <9>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_8.c:103" <9>
 */
