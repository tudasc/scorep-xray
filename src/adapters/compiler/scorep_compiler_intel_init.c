/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       scorep_compiler_intel_init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Initialization of the Intel compiler adapter.
 */

#include <config.h>

#include <UTILS_Debug.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_data_intel.h"
#include "scorep_compiler_symbol_table.h"


/* ***************************************************************************************
   Adapter management
*****************************************************************************************/

SCOREP_ErrorCode
scorep_compiler_init_adapter( void )
{
    if ( !scorep_compiler_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " initialize intel compiler adapter." );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize hash tables */
        scorep_compiler_hash_init();

        /* call function to calculate symbol table */
        scorep_compiler_get_sym_tab();

        /* Set flag */
        scorep_compiler_initialized = true;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                            " initialization of intel compiler adapter done." );
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_compiler_init_location( struct SCOREP_Location* location )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "intel compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Adapter finalization */
void
scorep_compiler_finalize( void )
{
    /* call only, if previously initialized */
    if ( scorep_compiler_initialized )
    {
        /* Delete hash table */
        scorep_compiler_hash_free();

        /* Set initialization flag */
        scorep_compiler_initialized = false;
        scorep_compiler_finalized   = true;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize intel compiler adapter." );

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
