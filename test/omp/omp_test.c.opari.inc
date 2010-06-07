/* 
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011, 
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       omp_test.c.opari.inc    
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *    
 */


#include "pomp_lib.h"

#define POMP_DLIST_00001 shared( pomp_region_1 )
static POMP_Region_handle pomp_region_1;

void POMP_Init_regions_1274996139600418_1()
{
    POMP_Assign_handle( &pomp_region_1, "66*regionType=parallel*sscl=omp_test.c:45:45*escl=omp_test.c:48:48**" );
}
