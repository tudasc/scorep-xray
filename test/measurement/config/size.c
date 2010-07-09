/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       test/measurement/config/size.c
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


#include <SILC_Config.h>


#include "error_callback.h"


static void
test_size( CuTest* tc )
{
    uint64_t            size_variable;
    SILC_ConfigVariable config_variables[] = {
        {
            "size",
            SILC_CONFIG_TYPE_SIZE,
            &size_variable,
            NULL,
            "1",
            "",
            ""
        },
        SILC_CONFIG_TERMINATOR
    };

    SILC_Error_Code     ret;
    ret = SILC_ConfigRegister( NULL, config_variables );
    CuAssert( tc, "SILC_ConfigRegister", ret == SILC_SUCCESS );

    printf( "%" PRIu64 "\n", size_variable );
}


int
main()
{
    SILC_Error_CallbackPointer old_error_callback =
        SILC_Error_RegisterCallback( cutest_silc_error_callback );

    the_test = CuTestNew( "test", test_size );
    CuTestRun( the_test );
    if ( the_test->message )
    {
        fprintf( stderr, "%s\n", the_test->message );
    }
    int failed = the_test->failed != 0;
    CuTestFree( the_test );

    SILC_Error_RegisterCallback( old_error_callback );

    return failed;
}
