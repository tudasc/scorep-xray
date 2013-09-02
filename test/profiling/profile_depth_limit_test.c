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
 * @ file      profile_depth_limit_test.c
 *
 * @brief Tests profile depth limitations.
 */

#include <config.h>

void
foo( int depth )
{
    if ( depth < 20 )
    {
        foo( depth + 1 );
    }
}

int
main()
{
    foo( 0 );
    return 0;
}
