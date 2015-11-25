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
 * @file       test/measurement/config/string.c
 *
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>


#include <CuTest.h>


#include <SCOREP_Config.h>


static char* string_default_empty;
static char* string_default_foo;


static const SCOREP_ConfigVariable string_confvars[] =
{
    {
        "string_default_empty",
        SCOREP_CONFIG_TYPE_STRING,
        &string_default_empty,
        NULL,
        "",
        "",
        ""
    },
    {
        "string_default_foo",
        SCOREP_CONFIG_TYPE_STRING,
        &string_default_foo,
        NULL,
        "foo",
        "",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};

static void
test_string_default_empty( CuTest* tc )
{
    CuAssertPtrNotNull( tc, string_default_empty );
    CuAssertStrEquals( tc, "", string_default_empty );

    free( string_default_empty );
}


static void
test_string_default_foo( CuTest* tc )
{
    CuAssertPtrNotNull( tc, string_default_foo );
    CuAssertStrEquals( tc, "foo", string_default_foo );

    free( string_default_foo );
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "string config type" );

    SUITE_ADD_TEST_NAME( suite, test_string_default_empty,
                         "string with default \"\"" );
    SUITE_ADD_TEST_NAME( suite, test_string_default_foo,
                         "string with default \"foo\"" );

    SCOREP_ConfigInit();

    SCOREP_ConfigRegister( "", string_confvars );

    CuSuiteRun( suite );

    SCOREP_ConfigFini();

    CuSuiteSummary( suite, output );
    int failCount = suite->failCount;
    if ( failCount )
    {
        printf( "%s", output->buffer );
    }

    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount ? EXIT_FAILURE : EXIT_SUCCESS;
}
