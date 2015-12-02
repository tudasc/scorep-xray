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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>

static SCOREP_InterruptGeneratorHandle irq_handle;

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

int
main( int ac, char* av[] )
{
    irq_handle = SCOREP_Definitions_NewInterruptGenerator(
        "manually",
        SCOREP_INTERRUPT_GENERATOR_MODE_COUNT,
        SCOREP_METRIC_BASE_DECIMAL,
        0,
        1 );

    uint32_t a = 0, b = 0, c = 0;

    const char* arg = ac > 2 ? av[ 1 ] : "abc";
    size_t      len = strlen( arg );
    size_t      i   = 0;

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    printf( "%u %u %u\n", a, b, c );

    return 0;
}

/*
 * expected:
 *
 * PR   MEASUREMENT OFF:MEASUREMENT OFF	   ( id=0, -1, -1, paradigm=user, role=artificial, url=, descr=, mod=)
 * PR   TRACE BUFFER FLUSH:TRACE BUFFER FLUSH	   ( id=1, -1, -1, paradigm=measurement, role=artificial, url=, descr=, mod=)
 * PR   THREADS:THREADS	   ( id=2, -1, -1, paradigm=measurement, role=artificial, url=, descr=, mod=THREADS)
 * PR   main:main	   ( id=3, 36, 88, paradigm=SAMPLING, role=function, url=, descr=, mod=.../test/services/unwinding/test_cct_1.c)
 *
 * PC       Visits (E) | Diff-Calltree
 * PC                5 |  * main
 *
 * 5 calling context nodes with "main" as region, but different SCLs
 * TD REGION                               0  Name: "main" <> (Aka. "main" <>), Descr.: "" <0>, Role: FUNCTION, Paradigm: SAMPLING, Flags: NONE, File: ".../test/services/unwinding/test_cct_1.c" <>, Begin: 36, End: 83
 * TD CALLING_CONTEXT                      0  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_1.c:55" <0>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      1  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_1.c:62" <1>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      2  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_1.c:69" <2>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      3  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_1.c:76" <3>, Parent: UNDEFINED
 * TD CALLING_CONTEXT                      4  Region: "main" <0>, Source code location: ".../test/services/unwinding/test_cct_1.c:83" <4>, Parent: UNDEFINED
 * TD INTERRUPT_GENERATOR                  0  Name: "manually" <>, Mode: COUNT, Base: DECIMAL, Exponent: 0, Period: 1
 *
 * 5 sample events, each references another node, i.e., each node is referenced once
 * TE CALLING_CONTEXT_SAMPLE                     0     1  Calling Context: "main@test_cct_1.c:55" <0>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_1.c:55" <0>
 * TE CALLING_CONTEXT_SAMPLE                     0     2  Calling Context: "main@test_cct_1.c:62" <1>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_1.c:62" <1>
 * TE CALLING_CONTEXT_SAMPLE                     0     3  Calling Context: "main@test_cct_1.c:69" <2>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_1.c:69" <2>
 * TE CALLING_CONTEXT_SAMPLE                     0     4  Calling Context: "main@test_cct_1.c:76" <3>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_1.c:76" <3>
 * TE CALLING_CONTEXT_SAMPLE                     0     5  Calling Context: "main@test_cct_1.c:83" <4>, Unwind Distance: 2, Interrupt Generator: "manually" <0>
 * TE                                                     +"main@test_cct_1.c:83" <4>
 */
