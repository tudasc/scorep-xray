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
 * @file       test/measurement/string_duplicates.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#include <stdio.h>

#include <CuTest.h>

#include <SILC_Memory.h>

#include <silc_definitions.h>

static void
test_1( CuTest* tc )
{
    SILC_Memory_Initialize( 4 * 4096, 4096 );
    SILC_Definitions_Initialize();

    SILC_StringHandle handle1 = SILC_DefineString( "foo" );
    SILC_StringHandle handle2 = SILC_DefineString( "foo" );
    SILC_StringHandle handle3 = SILC_DefineString( "foo" );

    CuAssert( tc, "1st and 2nd call should return the same handle", handle1 == handle2 );
    CuAssert( tc, "2nd and 3rd call should return the same handle", handle2 == handle3 );

    SILC_Definitions_Finalize();
    SILC_Memory_Finalize();
}

static void
test_2( CuTest* tc )
{
    SILC_Memory_Initialize( 4 * 4096, 4096 );
    SILC_Definitions_Initialize();

    SILC_StringHandle foo_handle1 = SILC_DefineString( "foo" );
    SILC_StringHandle bar_handle1 = SILC_DefineString( "bar" );
    SILC_StringHandle foo_handle2 = SILC_DefineString( "foo" );
    SILC_StringHandle bar_handle2 = SILC_DefineString( "bar" );
    SILC_StringHandle foo_handle3 = SILC_DefineString( "foo" );
    SILC_StringHandle bar_handle3 = SILC_DefineString( "bar" );

    CuAssert( tc, "1st and 2nd call should return the same handle", foo_handle1 == foo_handle2 );
    CuAssert( tc, "2nd and 3rd call should return the same handle", foo_handle2 == foo_handle3 );

    CuAssert( tc, "1st and 2nd call should return the same handle", bar_handle1 == bar_handle2 );
    CuAssert( tc, "2nd and 3rd call should return the same handle", bar_handle2 == bar_handle3 );

    SILC_Definitions_Finalize();
    SILC_Memory_Finalize();
}

int
main()
{
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "no string duplications" );

    SUITE_ADD_TEST_NAME( suite,
                         "three times \"foo\" equals one",
                         test_1 );
    SUITE_ADD_TEST_NAME( suite,
                         "three times \"foo\" and \"bar\" equals two",
                         test_2 );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );
    printf( "%s", output->buffer );

    int failCount = suite->failCount;
    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount;
}
