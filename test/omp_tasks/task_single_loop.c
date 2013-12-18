/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <scorep/SCOREP_User.h>

int
main( int argc, char** argv )
{
    SCOREP_USER_FUNC_BEGIN()
    uint64_t N;
    uint64_t result = 0;

    if ( argc < 2 )
    {
        printf( "Creates N non-sense tasks in a single loop.\nUsage: %s N\n", argv[ 0 ] );
        SCOREP_USER_FUNC_END()
        exit( EXIT_FAILURE );
    }
    N = atoi( argv[ 1 ] );

  #pragma omp parallel
    {
  #pragma omp single nowait
        {
            for ( uint32_t i = 0; i < N; i++ )
            {
      #pragma omp task
                {
                    if ( i > N )
                    {
                        exit( EXIT_FAILURE );
                    }
                }
            }
        }
    }
    SCOREP_USER_FUNC_END()
    return 0;
}
