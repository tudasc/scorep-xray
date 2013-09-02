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
 * @file       serial_ack_test.c
 *
 *
 */


#include <config.h>

#include <stdio.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


int
ack( int m, int n )
{
    if ( m == 0 )
    {
        return n + 1;
    }
    if ( n == 0 )
    {
        return ack( m - 1, 1 );
    }
    return ack( m - 1, ack( m, n - 1 ) );
}

int
main()
{
    for ( int i = 1; i < 9; i++ )
    {
        printf( "%d: %d\n", i, ack( 3, i ) );
    }
    return 0;
}
