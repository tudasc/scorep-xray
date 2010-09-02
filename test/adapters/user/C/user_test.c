/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#include <config.h>

#include "SILC_User.h"

#include "user_test_baz.c.inc"

SILC_USER_METRIC_GROUP_GLOBAL( globalGroup )
SILC_USER_METRIC_GLOBAL( globalMetric )

extern void
foo();

void
test()
{
    SILC_USER_FUNC_BEGIN
    SILC_USER_METRIC_INT64( globalMetric,
                            2 );

    SILC_USER_FUNC_END
}

int
main( int   argc,
      char* argv[] )
{
    SILC_USER_METRIC_GROUP_LOCAL( localGroup );
    SILC_USER_METRIC_LOCAL( localMetric );
    SILC_USER_FUNC_BEGIN;
    SILC_USER_METRIC_GROUP_INIT( localGroup, "localGroup" );
    SILC_USER_METRIC_GROUP_INIT( globalGroup, "globalGroup" );
    SILC_USER_METRIC_INIT( localMetric, "localMetric", "s", SILC_USER_METRIC_TYPE_DOUBLE,
                           SILC_USER_METRIC_CONTEXT_GLOBAL, SILC_USER_METRIC_GROUP_DEFAULT );
    SILC_USER_METRIC_INIT( globalMetric, "globalMetric", "s", SILC_USER_METRIC_TYPE_INT64,
                           SILC_USER_METRIC_CONTEXT_CALLPATH, localGroup );
    foo();
    SILC_USER_METRIC_DOUBLE( localMetric, 3.0 );
    baz();
    SILC_USER_METRIC_INT64( globalMetric, 1 );
    test();
    foo();
    SILC_USER_FUNC_END
    foo();

    SILC_RECORDING_OFF
    if ( !SILC_RECORDING_IS_ON )
    {
        SILC_RECORDING_ON
        return 0;
    }
}
