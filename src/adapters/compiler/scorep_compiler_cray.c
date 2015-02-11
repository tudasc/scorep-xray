/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Support for CCE-Compiler.
 * Because the CCE compiler calls functions with the same signature like
 * GNU, we simply wrap the GNU functions with Cray functions.
 */

#include <config.h>

#include <stdlib.h>
#include <stdbool.h>

#include <SCOREP_RuntimeManagement.h>

static void* scorep_first_func        = NULL;
static void* scorep_first_callsite    = NULL;
static bool  scorep_exited_first_func = false;

void
__cyg_profile_func_enter( void* func,
                          void* callsite );

void
__cyg_profile_func_exit( void* func,
                         void* callsite );


/* Checks whether the main function was exited. The Cray Fortran compiler instrumentation
 * does not issue the exit */
static int
scorep_compiler_cray_final_exit( void )
{
    if ( !scorep_exited_first_func )
    {
        scorep_exited_first_func = true;
        __cyg_profile_func_exit( scorep_first_func, scorep_first_callsite );
    }
    return 0;
}

/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/

/**
 * @brief This function is called just after the entry of a function
 * generated by the CCE compiler.
 * @param func      The address of the start of the current function.
 * @param callsice  The call site of the current function.
 */
void
__pat_tp_func_entry( void* func,
                     void* callsite )
{
    if ( scorep_first_func == NULL )
    {
        scorep_first_func     = func;
        scorep_first_callsite = callsite;
        SCOREP_RegisterExitCallback( &scorep_compiler_cray_final_exit );
    }
    __cyg_profile_func_enter( func, callsite );
}

/**
 * @brief This function is called just before the exit of a function
 * generated by the CCE compiler.
 * @param func      The address of the end of the current function.
 * @param callsice  The call site of the current function.
 */
void
__pat_tp_func_return( void* func,
                      void* callsite )
{
    __cyg_profile_func_exit( func, callsite );
    if ( func == scorep_first_func )
    {
        scorep_exited_first_func = true;
    }
}
