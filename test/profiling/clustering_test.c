/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @ file      clustering_test.c
 *
 * @brief Tests clustering.
 */

#include <config.h>
#include <scorep/SCOREP_User.h>
#include <stdio.h>

void
foo( void )
{
    SCOREP_USER_FUNC_BEGIN();
    SCOREP_USER_FUNC_END();
}

void
bar( void )
{
    SCOREP_USER_REGION_DEFINE( bar );
    SCOREP_USER_REGION_BEGIN( bar, "bar", SCOREP_USER_REGION_TYPE_DYNAMIC );
    SCOREP_USER_REGION_END( bar );
}

int
main()
{
    SCOREP_USER_REGION_DEFINE( cluster );
    for ( int i = 0; i < 400; i++ )
    {
        SCOREP_USER_REGION_BEGIN( cluster, "cluster", SCOREP_USER_REGION_TYPE_DYNAMIC );
        #pragma omp parallel
        {
            foo();
        }
        SCOREP_USER_REGION_END( cluster );
    }

    SCOREP_USER_REGION_DEFINE( no_cluster );
    for ( int i = 0; i < 400; i++ )
    {
        SCOREP_USER_REGION_BEGIN( no_cluster, "no_cluster",
                                  SCOREP_USER_REGION_TYPE_DYNAMIC );
        #pragma omp parallel
        {
            bar();
        }
        SCOREP_USER_REGION_END( no_cluster );
    }
    return 0;
}
