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
 * @status       alpha
 * @maintainer   Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Test for the vector.
 */

#include <config.h>
#include <SCOREP_Vector.h>
#include <assert.h>

int8_t
comp_int( const void* value,
          const void* item )
{
    if ( ( *( int* )value - *( int* )item ) == 0 )
    {
        return 0;
    }
    return 1;
}

int value[ 11 ];

int
main
(
    void
)
{
    size_t         index = 0;
    SCOREP_Vector* vec   = SCOREP_Vector_CreateSize( 10 );
    assert( SCOREP_Vector_Size( vec ) == 0 );
    assert( SCOREP_Vector_Capacity( vec ) == 10 );

    value[ 0 ] = 0;
    SCOREP_Vector_PushBack( vec, &value[ 0 ] );
    assert( SCOREP_Vector_Size( vec ) == 1 );

    for ( int i = 1; i < 11; ++i )
    {
        value[ i ] = i;
        SCOREP_Vector_PushBack( vec, &value[ i ] );
    }
    assert( SCOREP_Vector_Size( vec ) == 11 );

    assert( SCOREP_Vector_Find( vec, &value[ 3 ], comp_int, &index ) != 0 );
    assert( index == 3 );

    SCOREP_Vector_Free( vec );
    return 0;
}
