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
static SCOREP_RegionHandle             user_region;

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

void
enter_region_callback( void )
{
    SCOREP_EnterRegion( user_region );
}

void
exit_region_callback( void )
{
    SCOREP_ExitRegion( user_region );
}

void
trigger_callback( int type )
{
    switch ( type )
    {
        case 0:
            enter_region_callback();
            break;
        case 1:
            exit_region_callback();
            break;
    }
}

size_t
bar( const char* arg,
     size_t      len,
     uint32_t*   a,
     uint32_t*   b,
     uint32_t*   c,
     size_t      i )
{
    *a += arg[ i++ % len ];
    *b += arg[ i++ % len ];
    *c += arg[ i++ % len ];

    SCOREP_Sample( irq_handle );

    mix( ( *a ), ( *b ), ( *c ) );

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
    user_region = SCOREP_Definitions_NewRegion( "USER",
                                                "USER",
                                                SCOREP_INVALID_SOURCE_FILE,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_PARADIGM_USER,
                                                SCOREP_REGION_FUNCTION );

    uint32_t a = 0, b = 0, c = 0;

    const char* arg = ac > 2 ? av[ 1 ] : "abc";
    size_t      len = strlen( arg );
    size_t      i   = 0;

    SCOREP_Sample( irq_handle );

    trigger_callback( 0 );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    i = bar( arg, len, &a, &b, &c, i );

    SCOREP_Sample( irq_handle );

    a += arg[ i++ % len ];
    b += arg[ i++ % len ];
    c += arg[ i++ % len ];
    mix( a, b, c );

    SCOREP_Sample( irq_handle );

    trigger_callback( 1 );

    SCOREP_Sample( irq_handle );

    printf( "%u %u %u\n", a, b, c );

    return 0;
}

/*
 * expected:
 */
