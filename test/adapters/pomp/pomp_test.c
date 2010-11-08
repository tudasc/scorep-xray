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

#include <config.h>

#include <omp.h>

int
main( int   argc,
      char* argv[] )
{
    int a;
  #pragma omp parallel
    {
        int i;
    #pragma omp for
        for ( i = 0; i < 1000; i++ )
        {
            a++;
        }

    #pragma omp master
        {
            a++;
        }

    #pragma omp barrier

    #pragma omp critical(test1)
        {
            a++;
        }

    #pragma omp atomic
        a++;

    #pragma omp sections
        {
      #pragma omp section
            {
                a++;
            }
      #pragma omp section
            {
                a += 2;
            }
        }
    }
    return 0;
}
