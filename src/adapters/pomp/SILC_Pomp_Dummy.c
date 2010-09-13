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
 * @file       SILC_Pomp_Dummy.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * Dummy implementation of two functions which are generated by the awk script. In case
 * the Opari instrumentation is nor used, the instrumentater should link these dummy
 * functions to the binary, else the initialization of the pomp adapter contains
 * unresolved links.
 *
 */

#include <config.h>
#include <stddef.h>

void
POMP2_Init_regions()
{
}

size_t
POMP2_Get_num_regions()
{
    return 0;
}
