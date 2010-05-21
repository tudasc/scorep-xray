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
 * @file       test/measurement/config/bool.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <CuTest.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include <SILC_Config.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


// CuTest's assert functions:
// void CuAssert(CuTest* tc, char* message, int condition);
// void CuAssertTrue(CuTest* tc, int condition);
// void CuAssertStrEquals(CuTest* tc, char* expected, char* actual);
// void CuAssertIntEquals(CuTest* tc, int expected, int actual);
// void CuAssertPtrEquals(CuTest* tc, void* expected, void* actual);
// void CuAssertPtrNotNull(CuTest* tc, void* pointer);

static void
test_number_default_0( CuTest* tc )
{
    uint64_t            number_variable;
    SILC_ConfigVariable config_variables[] = {
        {
            "number_default_0",
            SILC_CONFIG_TYPE_NUMBER,
            &number_variable,
            NULL,
            "0",
            "",
            ""
        },
        SILC_CONFIG_TERMINATOR
    };

    SILC_ConfigRegister( NULL, config_variables );

    CuAssert( tc, "default \"0\"", number_variable == 0 );
}


static void
test_number_default_1( CuTest* tc )
{
    uint64_t            number_variable;
    SILC_ConfigVariable config_variables[] = {
        {
            "number_default_1",
            SILC_CONFIG_TYPE_NUMBER,
            &number_variable,
            NULL,
            "1",
            "",
            ""
        },
        SILC_CONFIG_TERMINATOR
    };

    SILC_ConfigRegister( NULL, config_variables );

    CuAssert( tc, "default \"1\"", number_variable == 1 );
}

static void
test_number_default_INT_MAX( CuTest* tc )
{
    uint64_t            number_variable;
    SILC_ConfigVariable config_variables[] = {
        {
            "number_default_INT_MAX",
            SILC_CONFIG_TYPE_NUMBER,
            &number_variable,
            NULL,
            "2147483647",
            "",
            ""
        },
        SILC_CONFIG_TERMINATOR
    };

    SILC_ConfigRegister( NULL, config_variables );

    CuAssert( tc, "default \"INT_MAX\"", number_variable == INT_MAX );
}


static void
test_number_default_UINT64_MAX( CuTest* tc )
{
    uint64_t            number_variable;
    SILC_ConfigVariable config_variables[] = {
        {
            "number_default_UINT64_MAX",
            SILC_CONFIG_TYPE_NUMBER,
            &number_variable,
            NULL,
            "18446744073709551615",
            "",
            ""
        },
        SILC_CONFIG_TERMINATOR
    };

    SILC_ConfigRegister( NULL, config_variables );

    CuAssert( tc, "default \"UINT64_MAX\"", number_variable == UINT64_MAX );
}


int
main()
{
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "default values for config type number" );

    SUITE_ADD_TEST_NAME( suite, "number with default \"0\"",
                         test_number_default_0 );
    SUITE_ADD_TEST_NAME( suite, "number with default \"1\"",
                         test_number_default_1 );
    SUITE_ADD_TEST_NAME( suite, "number with default \"INT_MAX\"",
                         test_number_default_INT_MAX );
    SUITE_ADD_TEST_NAME( suite, "number with default \"UINT64_MAX\"",
                         test_number_default_UINT64_MAX );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );
    printf( "%s", output->buffer );

    return suite->failCount;
}
