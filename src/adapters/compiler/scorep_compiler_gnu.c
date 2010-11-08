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
 * @status     alpha
 * @file       scorep_compiler_gnu.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-finstrument-functions' flag of the GNU
 * compiler.
 */

#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include <SCOREP_Types.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Events.h>
#include <SCOREP_RuntimeManagement.h>

#include <SCOREP_Compiler_Init.h>
#include <scorep_compiler_data.h>
#include <scorep_compiler_symbol_table.h>


/**
 * static variable to control initialize status of GNU
 */
static int scorep_compiler_initialize = 1;

/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/

/**
 * @brief This function is called just after the entry of a function
 * generated by the GNU compiler.
 * @param func      The address of the start of the current function.
 * @param callsice  The call site of the current function.
 */
void
__cyg_profile_func_enter( void* func,
                          void* callsite )
{
    scorep_compiler_hash_node* hash_node;

    /*
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses if measurement was not initialized
     */

    if ( scorep_compiler_initialize )
    {
        /* not initialized so far */
        SCOREP_InitMeasurement();
    }

    if ( ( hash_node = scorep_compiler_hash_get( ( long )func ) ) )
    {
        if ( hash_node->region_handle == SCOREP_INVALID_REGION )
        {
            /* -- region entered the first time, register region -- */
            scorep_compiler_register_region( hash_node );
        }
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                             "enter the region with handle %i ",
                             hash_node->region_handle );
        SCOREP_EnterRegion( hash_node->region_handle );
    }
}

/**
 * @brief This function is called just before the exit of a function
 * generated by the GNU compiler.
 * @param func      The address of the end of the current function.
 * @param callsice  The call site of the current function.
 */
void
__cyg_profile_func_exit( void* func,
                         void* callsite )
{
    scorep_compiler_hash_node* hash_node;
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call function exit." );
    if ( hash_node = scorep_compiler_hash_get( ( long )func ) )
    {
        SCOREP_ExitRegion( hash_node->region_handle );
    }
}

/* ***************************************************************************************
   Adapter management
*****************************************************************************************/

SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    if ( scorep_compiler_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " inititialize GNU compiler adapter." );

        /* Initialize hash tables */
        scorep_compiler_hash_init();

        /* call function to calculate symbol table */
        scorep_compiler_get_sym_tab();

        /* Sez flag */
        scorep_compiler_initialize = 0;

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                             " inititialization of GNU compiler adapter done." );
    }

    return SCOREP_SUCCESS;
}

/* Adapter finalization */
void
scorep_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !scorep_compiler_initialize )
    {
        /* Delete hash table */
        scorep_compiler_hash_free();

        /* Set initilaization flag */
        scorep_compiler_initialize = 1;
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize GNU compiler adapter." );
    }
}
