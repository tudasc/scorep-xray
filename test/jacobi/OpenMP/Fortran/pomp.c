/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       pomp.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <stddef.h>

extern void
FORTRAN_MANGLED( pomp2_init_regions_1287563317750978_3 ) ();
extern void
FORTRAN_MANGLED( pomp2_init_regions_1287563324447323_1 ) ();

void
POMP2_Init_regions()
{
    FORTRAN_MANGLED( pomp2_init_regions_1287563317750978_3 ) ();
    FORTRAN_MANGLED( pomp2_init_regions_1287563324447323_1 ) ();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}
