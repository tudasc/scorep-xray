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
 * @file       omp_tasks.c
 *
 *
 */


#include <config.h>

#include <stdio.h>
#include <omp.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


void
foo( int depth )
{
    printf( "in foo; depth=%d\n", depth );
    if ( depth == 1 )
    {
        return;
    }
    else
    {
#pragma omp task
        {
            foo( depth - 1 );
        }
    }
#pragma omp taskwait
    return;
}


int
main()
{
    printf( "in main\n" );
#pragma omp parallel
    {
#pragma omp single
        foo( 10 );
    }

    return 0;
}
