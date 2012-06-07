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
 * @status     alpha
 * @file       scorep_compiler_empty.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Define empty compiler adapter.
 *
 * This file contains an empty compiler adapter to avoid unresolved symbols
 * in case the used compiler has no appropriate compiler adapter.
 */

#include <config.h>

#include <SCOREP_Error.h>
#include <SCOREP_Debug.h>

#include <SCOREP_Location.h>

#include <SCOREP_Compiler_Init.h>

SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                         "No compiler adapter available" );
    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_compiler_init_location( SCOREP_Location* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "No compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_finalize_location( SCOREP_Location* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "No compiler adapter finalize location!" );
}

void
scorep_compiler_finalize()
{
}
