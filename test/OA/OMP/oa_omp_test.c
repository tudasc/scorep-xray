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

/* *INDENT-OFF* */
#include <config.h>
#include <stdio.h>
#include <omp.h>
#include "SCOREP_User.h"

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
main( int    argc,
      char** argv )
{
    int	retVal = 0; /* return value */
    int k,i;

    for(k=0;k<30;k++)
    {
        #pragma omp parallel
        {
                printf("Here is the first parallel section outside of the OA...\n");
        }

        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_REGION_DEFINE( region1 );
        SCOREP_USER_REGION_DEFINE( region2 );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);

        #pragma omp parallel private(i)
        {

                for(i=0;i<5;i++){
                SCOREP_USER_REGION_BEGIN( region1, "region1", SCOREP_USER_REGION_TYPE_COMMON);
                printf("OA_TEST_C:thread %d: Inside Online Access phase, iteration %d\n", omp_get_thread_num() ,i);
                SCOREP_USER_REGION_BEGIN( region2, "region2", SCOREP_USER_REGION_TYPE_COMMON);
                SCOREP_USER_REGION_END(region2);
                SCOREP_USER_REGION_END(region1);
                }
                #pragma omp single
                        foo( 10 );

        }

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

        SCOREP_USER_OA_PHASE_END( mainRegion );

    }

    return retVal;
}
