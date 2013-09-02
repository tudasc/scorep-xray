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
 * @file       test/rewind/rewind_test.c
 *
 *
 * @brief      testing the functionality of the rewind feature
 *
 */


#include <config.h>
#include <scorep/SCOREP_User.h>
#include <stdio.h>


int
main( int argc, char* argv[] )
{
    /* rewind check true */

    SCOREP_USER_REWIND_DEFINE( rewind1 )

    SCOREP_USER_REGION_DEFINE( foo )

    SCOREP_USER_REWIND_POINT( rewind1, "rewind1" )

    SCOREP_USER_REGION_BEGIN( foo, "foo", SCOREP_USER_REGION_TYPE_COMMON )

    SCOREP_USER_REGION_END( foo )

    SCOREP_USER_REWIND_CHECK( rewind1, 1 )

    /* rewind check false */

    SCOREP_USER_REWIND_DEFINE( rewind2 )

    SCOREP_USER_REGION_DEFINE( bar )

    SCOREP_USER_REWIND_POINT( rewind2, "rewind2" )

    SCOREP_USER_REGION_BEGIN( bar, "bar", SCOREP_USER_REGION_TYPE_COMMON )

    SCOREP_USER_REGION_END( bar )

    SCOREP_USER_REWIND_CHECK( rewind2, 0 )

    /* wrong nested rewind regions */

    SCOREP_USER_REWIND_DEFINE( rewind3 )

    SCOREP_USER_REWIND_DEFINE( rewind4 )

    SCOREP_USER_REWIND_POINT( rewind3, "rewind3" )

    SCOREP_USER_REWIND_POINT( rewind4, "rewind4" )

    SCOREP_USER_REWIND_CHECK( rewind3, 1 )

    SCOREP_USER_REWIND_CHECK( rewind4, 1 )

    /* flush test */

    SCOREP_USER_REWIND_DEFINE( rewind5 )

    SCOREP_USER_REWIND_POINT( rewind5, "rewind5" )

    SCOREP_USER_REGION_DEFINE( blub )
    SCOREP_USER_REGION_INIT( blub, "blub", SCOREP_USER_REGION_TYPE_COMMON )
    int i;
    FILE* f;
    do
    {
        for ( i = 0; i < 10000; i++ )
        {
            SCOREP_USER_REGION_ENTER( blub )
            SCOREP_USER_REGION_END( blub )
        }
        f = fopen( "rewind-test-dir/traces/0.evt", "r" );
    }
    while ( f == 0 );
    fclose( f );

    SCOREP_USER_REWIND_CHECK( rewind5, 1 )

    return 0;
}
