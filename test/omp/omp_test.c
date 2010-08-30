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
 * @file       omp_test.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <stdio.h>
#include <omp.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


void
foo()
{
    printf( "thread %d in foo\n", omp_get_thread_num() );
}


int
main()
{
    printf( "in main\n" );
#pragma omp parallel
    {
        foo();
    }

    return 0;
}
