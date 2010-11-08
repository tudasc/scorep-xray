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
 * @file       test/measurement/config/number.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>


#include <CuTest.h>


#include <SCOREP_Config.h>


#include "error_callback.h"


static void
test_number( CuTest* tc )
{
    uint64_t              number_variable;
    SCOREP_ConfigVariable config_variables[] = {
        {
            "number",
            SCOREP_CONFIG_TYPE_NUMBER,
            &number_variable,
            NULL,
            "0",
            "",
            ""
        },
        SCOREP_CONFIG_TERMINATOR
    };

    SCOREP_Error_Code     ret;
    ret = SCOREP_ConfigRegister( NULL, config_variables );
    CuAssert( tc, "SCOREP_ConfigRegister", ret == SCOREP_SUCCESS );

    printf( "%" PRIu64 "\n", number_variable );
}


int
main()
{
    SCOREP_Error_CallbackPointer old_error_callback =
        SCOREP_Error_RegisterCallback( cutest_scorep_error_callback );

    the_test = CuTestNew( "test", test_number );
    CuTestRun( the_test );
    if ( the_test->message )
    {
        fprintf( stderr, "%s\n", the_test->message );
    }
    int failed = the_test->failed != 0;
    CuTestFree( the_test );

    SCOREP_Error_RegisterCallback( old_error_callback );

    return failed;
}
