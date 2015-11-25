/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015,
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
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>


#include <CuTest.h>


#include <SCOREP_Config.h>


static uint64_t optionset_variable;


static const SCOREP_ConfigType_SetEntry optionset_entries[] = {
    { "alpha",       1,          ""            },
    { "beta",        2,          ""            },
    { "gamma/DELTA", 3,          ""            },
    { "mu/Nu/xI",    4,          ""            },
    { "no",          0,          ""            },
    { NULL,          0,          NULL          }
};

static const SCOREP_ConfigVariable optionset_confvars[] = {
    {
        "optionset",
        SCOREP_CONFIG_TYPE_OPTIONSET,
        &optionset_variable,
        ( void* )optionset_entries,
        "alpha",
        "",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};


struct optionset_test
{
    const char* value;
    bool        success;
    uint64_t    expected;
};


static void
test_register( CuTest* tc )
{
    SCOREP_ErrorCode ret = SCOREP_ConfigRegister( "test", optionset_confvars );
    CuAssert( tc, "SCOREP_ConfigRegister", ret == SCOREP_SUCCESS );
}


static void
test_success( CuTest* tc )
{
    struct optionset_test* test = tc->userArg;
    SCOREP_ErrorCode       ret  = SCOREP_ConfigSetValue( "test", "optionset", test->value );
    CuAssert( tc, "SCOREP_ConfigSetValue", ret == SCOREP_SUCCESS );
    CuAssertIntEquals( tc, test->expected, optionset_variable );
}


static void
test_xfail( CuTest* tc )
{
    struct optionset_test* test = tc->userArg;
    SCOREP_ErrorCode       ret  = SCOREP_ConfigSetValue( "test", "optionset", test->value );
    CuAssert( tc, "SCOREP_ConfigSetValue", ret != SCOREP_SUCCESS );
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "optionset config type" );

    SUITE_ADD_TEST_NAME( suite, test_register,
                         "register variable" );

    struct optionset_test tests[] = {
        { "",            true,            0            },
        { "no",          true,            0            },
        { "No",          true,            0            },
        { "nO",          true,            0            },
        { "NO",          true,            0            },
        { "none",        false,           0            },
        { "alpha",       true,            1            },
        { "ALPHA",       true,            1            },
        { "beta",        true,            2            },
        { "gamma",       true,            3            },
        { "delta",       true,            3            },
        { "mu",          true,            4            },
        { "nu",          true,            4            },
        { "xi",          true,            4            },
        { "alpha,",      false,           0            },
        { ",alpha",      false,           0            },
        { ",alpha,",     false,           0            },
        { "alpha,beta",  false,           0            },
        { "gamma/delta", false,           0            },
        { NULL,          true,            0            }
    };

    struct optionset_test* test_it = tests;

    CuString name;
    CuStringInit( &name );
    while ( test_it->value )
    {
        if ( test_it->success )
        {
            CuStringAppendFormat( &name, "accepts '%s' and results in %" PRId64,
                                  test_it->value,
                                  test_it->expected );
        }
        else
        {
            CuStringAppendFormat( &name, "rejects '%s'",
                                  test_it->value );
        }

        CuTest* the_test = CuTestNew( name.buffer,
                                      test_it->success
                                      ? test_success
                                      : test_xfail );
        the_test->userArg = test_it;
        CuSuiteAdd( suite, the_test );

        CuStringReset( &name );
        test_it++;
    }

    SCOREP_ConfigInit();

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
