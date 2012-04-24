/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file        region_types_consistency.c
 * @status       alpha
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Checks whether all Score-P region types are usable.
 *
 */

#include <config.h>
#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <scorep_types.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main()
{
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
        SCOREP_DefineRegion( name,
                             SCOREP_INVALID_SOURCE_FILE,
                             0, 0,
                             SCOREP_ADAPTER_USER,
                             t );
    }
    return 0;
}
