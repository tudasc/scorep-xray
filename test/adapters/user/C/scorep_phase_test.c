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
 * @ file      scorep_phase_test.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Tests the phase instrumentation and postprocessing.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <SCOREP_User.h>

int i = 0;

void
foo()
{
    SCOREP_USER_FUNC_BEGIN
    SCOREP_USER_REGION_DEFINE( phase4 );

    SCOREP_USER_REGION_BEGIN( phase4, "phase4", SCOREP_USER_REGION_TYPE_PHASE );
    i++;
    SCOREP_USER_REGION_END( phase4 );
    i++;
    SCOREP_USER_FUNC_END
}

void
bar()
{
    SCOREP_USER_FUNC_BEGIN
    SCOREP_USER_REGION_DEFINE( phase3 );

    SCOREP_USER_REGION_BEGIN( phase3, "phase3", SCOREP_USER_REGION_TYPE_PHASE );
    foo();
    SCOREP_USER_REGION_END( phase3 );
    foo();
    SCOREP_USER_FUNC_END
}

void
baz()
{
    SCOREP_USER_FUNC_BEGIN
    SCOREP_USER_REGION_DEFINE( phase2 );

    SCOREP_USER_REGION_BEGIN( phase2, "phase2", SCOREP_USER_REGION_TYPE_PHASE );
    bar();
    foo();
    SCOREP_USER_REGION_END( phase2 );
    bar();
    foo();
    SCOREP_USER_FUNC_END
}

int
main()
{
  #pragma omp parallel
    {
        SCOREP_USER_FUNC_BEGIN
        SCOREP_USER_REGION_DEFINE( phase1 );

        SCOREP_USER_REGION_BEGIN( phase1, "phase1", SCOREP_USER_REGION_TYPE_PHASE );
        baz();
        bar();
        foo();
        SCOREP_USER_REGION_END( phase1 );
        baz();
        bar();
        foo();
        SCOREP_USER_FUNC_END
    }
    printf( "%d\n", i );
    return EXIT_SUCCESS;
}
