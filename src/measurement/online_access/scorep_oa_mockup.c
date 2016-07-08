/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
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

/**
 * @file
 *
 * This file contains dummy implementation of Online Access Phase functions
 */

#include <config.h>
#include <UTILS_Debug.h>

#include "SCOREP_Types.h"


void
SCOREP_OA_PhaseBegin( const SCOREP_RegionHandle handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,  "Dummy %s\n", __func__ );
}

void
SCOREP_OA_PhaseEnd( const SCOREP_RegionHandle handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,  "Dummy %s\n", __func__ );
}

void
SCOREP_OA_Register( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,  "Dummy %s\n", __func__ );
}

void
SCOREP_OA_Finalize( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,  "Dummy %s\n", __func__ );
}
