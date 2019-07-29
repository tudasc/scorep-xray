/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @ file      user_test.c
 *
 * @brief Tests user instrumentation.
 */

#include <config.h>

#include <scorep/SCOREP_User.h>

static void
test( int          a,
      unsigned int b,
      const char*  c );

#include "user_test_baz.c.inc"

SCOREP_USER_METRIC_GLOBAL( globalMetric )

extern void
foo();

static void
test( int a, unsigned int b, const char* c )
{
    SCOREP_USER_FUNC_BEGIN();
    SCOREP_USER_PARAMETER_INT64( "a", a )
    SCOREP_USER_PARAMETER_UINT64( "b", b )
    SCOREP_USER_PARAMETER_STRING( "c", c )

    SCOREP_USER_METRIC_INT64( globalMetric, 2 );

    SCOREP_USER_FUNC_END();
}

int
main( int   argc,
      char* argv[] )
{
    SCOREP_USER_METRIC_LOCAL( localMetric );
    SCOREP_USER_FUNC_BEGIN();
    SCOREP_USER_METRIC_INIT( localMetric, "localMetric", "s", SCOREP_USER_METRIC_TYPE_DOUBLE,
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL );
    SCOREP_USER_METRIC_INIT( globalMetric, "globalMetric", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );
    foo();
    SCOREP_USER_METRIC_DOUBLE( localMetric, 3.0 );
    test( -1, 1, "test1" );
    baz();
    SCOREP_USER_METRIC_INT64( globalMetric, 1 );
    test( -1, 1, "test1" );
    foo();
    SCOREP_USER_FUNC_END();
    foo();

    SCOREP_RECORDING_OFF();
    if ( !SCOREP_RECORDING_IS_ON() )
    {
        SCOREP_RECORDING_ON();
    }

    SCOREP_USER_REGION_DEFINE( my_region );
    SCOREP_USER_REGION_INIT( my_region, "my region", SCOREP_USER_REGION_TYPE_COMMON );
    SCOREP_USER_REGION_ENTER(  my_region );
    SCOREP_USER_REGION_END( my_region );

    /* BY_NAME Regions*/
    SCOREP_USER_REGION_BY_NAME_BEGIN( "byname_region", SCOREP_USER_REGION_TYPE_COMMON );
    SCOREP_USER_REGION_BY_NAME_END( "byname_region" );
}
