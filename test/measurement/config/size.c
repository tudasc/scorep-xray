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



/**
 * @file       test/measurement/config/size.c
 *
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>


#include <UTILS_Error.h>


#include <CuTest.h>


#include <SCOREP_Config.h>


#include "error_callback.h"


static void
test_size( CuTest* tc )
{
    SCOREP_ErrorCode            ret;
    uint64_t                    size_variable;
    const SCOREP_ConfigVariable confvars[] = {
        {
            "size",
            SCOREP_CONFIG_TYPE_SIZE,
            &size_variable,
            NULL,
            "1",
            "",
            ""
        },
        SCOREP_CONFIG_TERMINATOR
    };

    ret = SCOREP_ConfigInit();
    CuAssert( tc, "SCOREP_ConfigInit", ret == SCOREP_SUCCESS );

    ret = SCOREP_ConfigRegister( "", confvars );
    CuAssert( tc, "SCOREP_ConfigRegister", ret == SCOREP_SUCCESS );

    ret = SCOREP_ConfigApplyEnv();
    CuAssert( tc, "SCOREP_ConfigApplyEnv", ret == SCOREP_SUCCESS );

    SCOREP_ConfigFini();

    printf( "%" PRIu64 "\n", size_variable );
}


int
main( int argc, char* argv[] )
{
    CuTest* the_test = CuTestNew( "test", test_size );
    CuTestRun( the_test );
    if ( the_test->message )
    {
        fprintf( stderr, "%s\n", the_test->message );
    }
    int failed = the_test->failed != 0;
    CuTestFree( the_test );

    return failed;
}
