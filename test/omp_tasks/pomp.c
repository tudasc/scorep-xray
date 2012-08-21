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
 * @file       pomp.c
 * @maintainer Dirk Schmidl <schmidl@rz.rwth-aachen.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
POMP2_Init_reg_1318332431780061_4();

void
POMP2_Init_regions()
{
    POMP2_Init_reg_1318332431780061_4();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

const char*
POMP2_Get_opari2_version()
{
    return "2.x";
}

#ifdef __cplusplus
}
#endif
