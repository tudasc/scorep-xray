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
 * @ file      compiler_test.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Test program for compiler instrumentation.
 *
 * The intention of the test is to check whether the compiler
 * adapter generates events (enter/exit). In order to archive this, we needed
 * to replace the original measurement system by an implementation which
 * allows to run a sequential compiler instrumented program. The dummy enter
 * sets a flag. The dummy exit tests the flag and exits with exit code success.
 * The instrumneted program exits with an error code when it reaches its end.
 * Thus, the test script will pass if enter and exit events are generated and
 * fail otherwise.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>

void
foo()
{
    printf( "In foo\n" );
}

int
main( int   argc,
      char* argv[] )
{
    foo();
    printf( "ERROR: No compiler instrumentation.\n" );
    abort();
}
