/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#include <config.h>

#include "SCOREP_User.h"

#include "user_test_baz.c.inc"

SCOREP_USER_METRIC_GROUP_GLOBAL( globalGroup )
SCOREP_USER_METRIC_GLOBAL( globalMetric )

extern void
foo();

void
test()
{
    SCOREP_USER_FUNC_BEGIN
    SCOREP_USER_METRIC_INT64( globalMetric,
                              2 );

    SCOREP_USER_PARAMETER_INT64( "int_param", 2 )
    SCOREP_USER_PARAMETER_STRING( "string_param", "test" )
    SCOREP_USER_FUNC_END
}

int
main( int   argc,
      char* argv[] )
{
    SCOREP_USER_METRIC_GROUP_LOCAL( localGroup );
    SCOREP_USER_METRIC_LOCAL( localMetric );
    SCOREP_USER_FUNC_BEGIN;
    SCOREP_USER_METRIC_GROUP_INIT( localGroup, "localGroup" );
    SCOREP_USER_METRIC_GROUP_INIT( globalGroup, "globalGroup" );
    SCOREP_USER_METRIC_INIT( localMetric, "localMetric", "s", SCOREP_USER_METRIC_TYPE_DOUBLE,
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, SCOREP_USER_METRIC_GROUP_DEFAULT );
    SCOREP_USER_METRIC_INIT( globalMetric, "globalMetric", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH, localGroup );
    foo();
    SCOREP_USER_METRIC_DOUBLE( localMetric, 3.0 );
    baz();
    SCOREP_USER_METRIC_INT64( globalMetric, 1 );
    test();
    foo();
    SCOREP_USER_FUNC_END
    foo();

    SCOREP_RECORDING_OFF
    if ( !SCOREP_RECORDING_IS_ON )
    {
        SCOREP_RECORDING_ON
        return 0;
    }
}
