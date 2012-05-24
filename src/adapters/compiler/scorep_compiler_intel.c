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
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Mutex.h>

#include <SCOREP_Compiler_Init.h>
#include <scorep_compiler_data_intel.h>
#include <scorep_compiler_symbol_table.h>

/**
    @def SCOREP_COMPILER_FILTER_ID defines the id for an filtered region.
 */
#define SCOREP_COMPILER_FILTER_ID -1

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

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call at function enter." );

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

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " function id: %d ", *id );

    /* Register new region if unknown */
    if ( *id == 0 )
    {
        SCOREP_MutexLock( scorep_compiler_region_mutex );

        if ( hash_node = scorep_compiler_hash_get( str ) )
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
            *id = SCOREP_COMPILER_FILTER_ID;
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }

    /* Enter event */
    if ( *id != SCOREP_COMPILER_FILTER_ID )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                             "enter the region with handle %i ",
                             hash_node->region_handle );
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
    scorep_compiler_hash_node* hash_node;

    if ( scorep_compiler_finalized )
    {
        return;
    }

    /* Check if function is filtered */
    if ( *id2 == SCOREP_COMPILER_FILTER_ID )
    {
        return;
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call function exit." );
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
    scorep_compiler_hash_node* hash_node;

    if ( scorep_compiler_finalized )
    {
        return;
    }

    /* Check if function is filtered */
    if ( *id2 == SCOREP_COMPILER_FILTER_ID )
    {
        return;
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "Catch in region." );
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
    scorep_compiler_hash_node* hash_node;

    if ( scorep_compiler_finalized )
    {
        return;
    }

    /* Check if function is filtered */
    if ( *id2 == SCOREP_COMPILER_FILTER_ID )
    {
        return;
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "Check in region. Try " );
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

SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    if ( scorep_compiler_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " initialize intel compiler adapter." );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize hash tables */
        scorep_compiler_hash_init();

        /* call function to calculate symbol table */
        scorep_compiler_get_sym_tab();

        /* Set flag */
        scorep_compiler_initialize = 0;

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                             " initialization of intel compiler adapter done." );
    }

    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_compiler_init_location( SCOREP_Location* location )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "intel compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_final_location( SCOREP_Location* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "intel compiler adapter final location!" );
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

        /* Set initialization flag */
        scorep_compiler_initialize = 1;
        scorep_compiler_finalized  = 1;
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize intel compiler adapter." );

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
