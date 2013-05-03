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
 * @file       SCOREP_compiler_intel.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-fcollect' flag of the intel
 * compiler.
 */

#include <config.h>
#include <stdio.h>
#include <unistd.h>

#include <SCOREP_Types.h>
#include <SCOREP_Location.h>
#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Mutex.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_data_intel.h"
#include "scorep_compiler_symbol_table.h"

/**
 * static variable to control initialize status of the compiler adapter.
 * If it is 0 it is initialized.
 */
static int scorep_compiler_initialize = 1;

/**
 * flag that indicates whether the intel compiler adapter is finalized.
 */
static int scorep_compiler_finalized = 0;

/**
 * Mutex for exclusive access to the region hash table.
 */
static SCOREP_Mutex scorep_compiler_region_mutex;

/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/
/*
 *  This function is called at the entry of each function
 */

void
__VT_IntelEntry( char*     str,
                 uint32_t* id,
                 uint32_t* id2 )
{
    scorep_compiler_hash_node* hash_node = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call at function enter." );

    /*
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses if measurement was not initialized
     */

    if ( scorep_compiler_initialize )
    {
        if ( scorep_compiler_finalized )
        {
            return;
        }

        /* not initialized so far */
        SCOREP_InitMeasurement();
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " function id: %u ", *id );

    /* Register new region if unknown */
    if ( *id == 0 )
    {
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( *id == 0 )
        {
            hash_node = scorep_compiler_hash_get( str );
            if ( hash_node )
            {
                if ( hash_node->region_handle == SCOREP_INVALID_REGION )
                {
                    /* -- region entered the first time, register region -- */
                    scorep_compiler_register_region( hash_node );
                }
                *id = hash_node->region_handle;
            }
            else
            {
                *id = SCOREP_COMPILER_FILTER_HANDLE;
            }
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }

    /* Enter event */
    if ( *id != SCOREP_COMPILER_FILTER_HANDLE )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                            "enter the region with id %u ", *id );
        SCOREP_EnterRegion( ( SCOREP_RegionHandle ) * id );
    }

    /* Set exit id */
    *id2 = *id;
}

void
VT_IntelEntry( char*     str,
               uint32_t* id,
               uint32_t* id2 )
{
    __VT_IntelEntry( str, id, id2 );
}


/*
 * This function is called at the exit of each function
 */

void
__VT_IntelExit( uint32_t* id2 )
{
    if ( scorep_compiler_finalized )
    {
        return;
    }

    /* Check if function is filtered */
    if ( *id2 == SCOREP_COMPILER_FILTER_HANDLE )
    {
        return;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call function exit." );
    SCOREP_ExitRegion( ( SCOREP_RegionHandle ) * id2 );
}

void
VT_IntelExit( uint32_t* id2 )
{
    __VT_IntelExit( id2 );
}

/*
 * This function is called when an exception is caught
 */

void
__VT_IntelCatch( uint32_t* id2 )
{
    if ( scorep_compiler_finalized )
    {
        return;
    }

    /* Check if function is filtered */
    if ( *id2 == SCOREP_COMPILER_FILTER_HANDLE )
    {
        return;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "Catch in region." );
    SCOREP_ExitRegion( ( SCOREP_RegionHandle ) * id2 );
}

void
VT_IntelCatch( uint32_t* id2 )
{
    __VT_IntelCatch( id2 );
}

void
__VT_IntelCheck( uint32_t* id2 )
{
    if ( scorep_compiler_finalized )
    {
        return;
    }

    /* Check if function is filtered */
    if ( *id2 == SCOREP_COMPILER_FILTER_HANDLE )
    {
        return;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "Check in region. Try " );
    SCOREP_ExitRegion( ( SCOREP_RegionHandle ) * id2 );
}

void
VT_IntelCheck( uint32_t* id2 )
{
    __VT_IntelCheck( id2 );
}


/* ***************************************************************************************
   Adapter management
*****************************************************************************************/

SCOREP_ErrorCode
scorep_compiler_init_adapter( void )
{
    if ( scorep_compiler_initialize )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " initialize intel compiler adapter." );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize hash tables */
        scorep_compiler_hash_init();

        /* call function to calculate symbol table */
        scorep_compiler_get_sym_tab();

        /* Set flag */
        scorep_compiler_initialize = 0;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                            " initialization of intel compiler adapter done." );
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_compiler_init_location( SCOREP_Location* location )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "intel compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_finalize_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "intel compiler adapter finalize location!" );
}

/* Adapter finalization */
void
scorep_compiler_finalize( void )
{
    /* call only, if previously initialized */
    if ( !scorep_compiler_initialize )
    {
        /* Delete hash table */
        scorep_compiler_hash_free();

        /* Set initialization flag */
        scorep_compiler_initialize = 1;
        scorep_compiler_finalized  = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize intel compiler adapter." );

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
