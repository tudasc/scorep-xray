/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2019,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2013
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file        region_types_consistency.c
 *
 * @brief Checks whether all Score-P region types are usable.
 *
 */

#include <config.h>
#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <scorep_type_utils.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main()
{
    /* The studio compiler does not instrument C functions. Thus, we must initialize the
       measurement manually. */
    SCOREP_InitMeasurement();

    for ( SCOREP_RegionType t = 1; t < SCOREP_INVALID_REGION_TYPE; t++ )
    {
        const char* name = scorep_region_type_to_string( t );
        if ( strcmp( name, "unknown" ) == 0 )
        {
            fprintf( stderr,
                     "Region type to string conversion failed for type %d. Missing entry in"
                     "scorep_region_type_to_string.\n", t );
            abort();
        }
        SCOREP_Definitions_NewRegion( name,
                                      NULL,
                                      SCOREP_INVALID_SOURCE_FILE,
                                      0, 0,
                                      SCOREP_PARADIGM_USER,
                                      t );
    }
    return 0;
}
