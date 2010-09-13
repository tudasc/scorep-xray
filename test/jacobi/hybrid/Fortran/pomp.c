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


#include <config.h>

#include <stddef.h>

extern void
pomp2_init_regions_1276000468650595_3_();
extern void
pomp2_init_regions_1276000461487383_1_();

void
POMP2_Init_regions()
{
    pomp2_init_regions_1276000468650595_3_();
    pomp2_init_regions_1276000461487383_1_();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}
