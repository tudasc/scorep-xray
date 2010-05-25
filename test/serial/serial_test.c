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
 * @file       serial_test.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


//#include <CuTest.h>
#include <stdio.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


// CuTest's assert functions:
// void CuAssert(CuTest* tc, char* message, int condition);
// void CuAssertTrue(CuTest* tc, int condition);
// void CuAssertStrEquals(CuTest* tc, char* expected, char* actual);
// void CuAssertIntEquals(CuTest* tc, int expected, int actual);
// void CuAssertPtrEquals(CuTest* tc, void* expected, void* actual);
// void CuAssertPtrNotNull(CuTest* tc, void* pointer);


//void
//test_1( CuTest* tc )
//{
//    CuAssert( tc, "I will fail", 0 == 1 );
//}


void
foo()
{
    printf( "in foo\n" );
}

int
main()
{
    printf( "in main\n" );
    foo();
    return 0;
//    CuString* output = CuStringNew();
//    CuSuite*  suite  = CuSuiteNew();
//
//    SUITE_ADD_TEST( suite, test_1 );
//
//    CuSuiteRun( suite );
//    CuSuiteSummary( suite, output );
//    CuSuiteDetails( suite, output );
//    printf( "%s\n", output->buffer );
//
//    return suite->failCount;
}
