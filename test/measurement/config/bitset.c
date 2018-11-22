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


static uint64_t bitset_variable;


static const SCOREP_ConfigType_SetEntry bitset_entries[] = {
    { "all",         15,         ""            },
    { "alpha",       1,          ""            },
    { "beta",        2,          ""            },
    { "gamma/DELTA", 4,          ""            },
    { "mu/Nu/xI",    8,          ""            },
    { NULL,          0,          NULL          }
};

static const SCOREP_ConfigVariable bitset_confvars[] = {
    {
        "bitset",
        SCOREP_CONFIG_TYPE_BITSET,
        &bitset_variable,
        ( void* )bitset_entries,
        "none",
        "",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};


struct bitset_test
{
    const char* value;
    bool        success;
    uint64_t    expected;
};


static void
test_register( CuTest* tc )
{
    SCOREP_ErrorCode ret = SCOREP_ConfigRegister( "test", bitset_confvars );
    CuAssert( tc, "SCOREP_ConfigRegister", ret == SCOREP_SUCCESS );
}


static void
test_success( CuTest* tc )
{
    struct bitset_test* test = tc->userArg;
    SCOREP_ErrorCode    ret  = SCOREP_ConfigSetValue( "test", "bitset", test->value );
    CuAssert( tc, "SCOREP_ConfigSetValue", ret == SCOREP_SUCCESS );
    CuAssertIntEquals( tc, test->expected, bitset_variable );
}


static void
test_xfail( CuTest* tc )
{
    struct bitset_test* test = tc->userArg;
    SCOREP_ErrorCode    ret  = SCOREP_ConfigSetValue( "test", "bitset", test->value );
    CuAssert( tc, "SCOREP_ConfigSetValue", ret != SCOREP_SUCCESS );
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "bitset config type" );

    SUITE_ADD_TEST_NAME( suite, test_register,
                         "register variable" );

    struct bitset_test tests[] = {
        { "",                    true,                    0                     },
        { "no",                  true,                    0                     },
        { "No",                  true,                    0                     },
        { "nO",                  true,                    0                     },
        { "NO",                  true,                    0                     },
        { "none",                true,                    0                     },
        { "nein",                false,                   0                     },
        { "alpha",               true,                    1                     },
        { "ALPHA",               true,                    1                     },
        { "beta",                true,                    2                     },
        { "alpha,beta",          true,                    3                     },
        { "alpha;beta",          true,                    3                     },
        { "alpha:beta",          true,                    3                     },
        { "alpha beta",          true,                    3                     },
        { "beta,alpha",          true,                    3                     },
        { "alpha,",              true,                    1                     },
        { ",alpha",              true,                    1                     },
        { ",alpha,",             true,                    1                     },
        { "gamma",               true,                    4                     },
        { "delta",               true,                    4                     },
        { "gamma,delta",         true,                    4                     },
        { "gamma/delta",         false,                   0                     },
        { "mu",                  true,                    8                     },
        { "nu",                  true,                    8                     },
        { "xi",                  true,                    8                     },
        { "alpha,beta,gamma,mu", true,                    15                    },
        { "all",                 true,                    15                    },
        { "~all",                true,                    0                     },
        { "all,~alpha",          true,                    14                    },
        { "~alpha,all",          true,                    15                    },
        { "all,alpha,~alpha",    true,                    14                    },
        { "all,~alpha,alpha",    true,                    15                    },
        { "alpha,~alpha",        true,                    0                     },
        { "beta,~alpha",         true,                    2                     },
        { "~foo",                false,                   0                     },
        { "alpha~",              false,                   0                     },
        { "!alpha",              false,                   0                     },
        { NULL,                  true,                    0                     }
    };

    struct bitset_test* test_it = tests;

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
