/*
 * This file is part of the SILC project (http://www.silc.de)
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


#include <stddef.h>

extern void
POMP_Init_regions_1269874420956379_3();
extern void
POMP_Init_regions_1269874414236437_1();

void
POMP_Init_regions()
{
    POMP_Init_regions_1269874420956379_3();
    POMP_Init_regions_1269874414236437_1();
}

size_t
POMP_Get_num_regions()
{
    return 4;
}
