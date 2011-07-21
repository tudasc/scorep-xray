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
 * @file       filter_test.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <stdio.h>

void
filtered2();
void
filter_not2();

void
filtered1()
{
    printf( "1" );
}

void
filter_not1()
{
    printf( "3" );
}

int
main()
{
    printf( "Run filter test: " );
    filtered1();
    filtered2();
    filter_not1();
    filter_not2();
}
