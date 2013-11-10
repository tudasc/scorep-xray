/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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

#include <config.h>

#include <stdio.h>

int
foo_area( int length,
          int width )
{
    printf( " in function foo_area \n" );
    return length * width;
}

int
foo_square( int number )
{
    printf( " in function foo_square \n" );
    return foo_area( number, number );
}

int
foo_volume( int length,
            int width,
            int height )
{
    printf( " in function foo_volume \n" );
    return foo_area( length, foo_area( width, height ) );
}

int
main( int   argc,
      char* argv[] )
{
    //MPI_Init( &argc, &argv );

    printf( "hello, world\n" );

    printf( " the square of 5       : %i \n", foo_square( 5 ) );
    printf( " the area of 5 x 4      : %i \n", foo_area( 5, 4 ) );
    printf( " the volume of 5 x 4 x 3: %i \n", foo_volume( 5, 4, 3 ) );

    //MPI_Finalize();

    return 0;
}
