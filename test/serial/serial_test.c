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
 * @file       serial_test.c
 *
 *
 */


#include <config.h>

#include <stdio.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


void
foo()
{
    printf( "in foo\n" );
}

int
main()
{
    printf( "in main\n" );
    foo();
    return 0;
}
