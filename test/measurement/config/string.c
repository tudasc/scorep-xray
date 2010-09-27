/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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



/**
 * @file       test/measurement/config/string.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>


#include <CuTest.h>


#include <SCOREP_Config.h>


static void
test_string_default_empty( CuTest* tc )
{
    char*                 string_variable    = NULL;
    SCOREP_ConfigVariable config_variables[] = {
        {
            "string_default_empty",
            SCOREP_CONFIG_TYPE_STRING,
            &string_variable,
            NULL,
            "",
            "",
            ""
        },
        SCOREP_CONFIG_TERMINATOR
    };

    SCOREP_ConfigRegister( NULL, config_variables );

    CuAssertPtrNotNull( tc, string_variable );
    CuAssertStrEquals( tc, "", string_variable );

    free( string_variable );
}


static void
test_string_default_foo( CuTest* tc )
{
    char*                 string_variable    = NULL;
    SCOREP_ConfigVariable config_variables[] = {
        {
            "string_default_empty",
            SCOREP_CONFIG_TYPE_STRING,
            &string_variable,
            NULL,
            "foo",
            "",
            ""
        },
        SCOREP_CONFIG_TERMINATOR
    };

    SCOREP_ConfigRegister( NULL, config_variables );

    CuAssertPtrNotNull( tc, string_variable );
    CuAssertStrEquals( tc, "foo", string_variable );

    free( string_variable );
}


int
main()
{
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "default values for config type string" );

    SUITE_ADD_TEST_NAME( suite, "string with default \"\"",
                         test_string_default_empty );
    SUITE_ADD_TEST_NAME( suite, "string with default \"foo\"",
                         test_string_default_foo );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );
    printf( "%s", output->buffer );

    int failCount = suite->failCount;
    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount;
}
