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
static SCOREP_RegionHandle             compiler_region;

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

static void
scorep_enter_region( void )
{
    SCOREP_EnterRegion( compiler_region );
}

static void
scorep_exit_region( void )
{
    SCOREP_ExitRegion( compiler_region );
}

size_t
bar( const char* arg,
     size_t      len,
     uint32_t*   a,
     uint32_t*   b,
     uint32_t*   c,
     size_t      i )
{
    scorep_enter_region();

    *a += arg[ i++ % len ];
    *b += arg[ i++ % len ];
    *c += arg[ i++ % len ];

    SCOREP_Sample( irq_handle );

    mix( ( *a ), ( *b ), ( *c ) );

    scorep_exit_region();

    return i;
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
    compiler_region = SCOREP_Definitions_NewRegion( "bar",
                                                    NULL,
                                                    SCOREP_INVALID_SOURCE_FILE,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_PARADIGM_COMPILER,
                                                    SCOREP_REGION_FUNCTION );

    uint32_t a = 0, b = 0, c = 0;

    const char* arg = ac > 2 ? av[ 1 ] : "abc";
    size_t      len = strlen( arg );
    size_t      i   = 0;

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    printf( "%u %u %u\n", a, b, c );

    return 0;
}

/*
 * expected:
 */
