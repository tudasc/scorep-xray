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
 * @ file      selective_test.c
 *
 * @brief Tests the selective tracing
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <scorep/SCOREP_User.h>

static int c = 0;

void
foo()
{
    SCOREP_USER_FUNC_BEGIN();
    c++;
    SCOREP_USER_FUNC_END();
}

int
main()
{
    SCOREP_USER_FUNC_BEGIN();
    for ( int i = 0; i < 20; i++ )
    {
        SCOREP_USER_REGION_DEFINE( loop );
        SCOREP_USER_REGION_BEGIN( loop, "loop", SCOREP_USER_REGION_TYPE_DYNAMIC );
        foo();
        SCOREP_USER_REGION_END( loop );
    }
    SCOREP_USER_FUNC_END();

    return 0;
}
