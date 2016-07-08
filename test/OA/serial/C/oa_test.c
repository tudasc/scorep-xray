/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/* *INDENT-OFF* */
#include <stdio.h>

#include <scorep/SCOREP_User.h>

int
main( int    argc,
      char** argv )
{
    int retVal = 0; /* return value */
    int k,i;

    for(k=0;k<30;k++)
    {
        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_REGION_DEFINE( region1 );
        SCOREP_USER_REGION_DEFINE( region2 );
        SCOREP_USER_METRIC_LOCAL( METRIC1 );
        SCOREP_USER_METRIC_INIT( METRIC1, "METRIC1", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);
        for(i=0;i<5;i++){
                SCOREP_USER_REGION_BEGIN( region1, "region1", SCOREP_USER_REGION_TYPE_COMMON);
                printf("OA_TEST_C: Inside Online Access phase, iteration %d\n",i);
                SCOREP_USER_REGION_BEGIN( region2, "region2", SCOREP_USER_REGION_TYPE_COMMON);
                SCOREP_USER_METRIC_INT64( METRIC1, 1001 );
                SCOREP_USER_REGION_END(region2);
                SCOREP_USER_REGION_END(region1);
        }
        SCOREP_USER_OA_PHASE_END( mainRegion );

    }

    return retVal;
}
