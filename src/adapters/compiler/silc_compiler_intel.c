/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @ file      SILC_compiler_intel.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-fcollect' flag of the intel
 * compiler.
 */

#include <stdio.h>
#include <unistd.h>

#include <SILC_Types.h>
#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <silc_compiler_data.h>
#include <silc_compiler_symbol_table.h>

/**
    @def SILC_COMPILER_FILTER_ID defines the id for an filtered region.
 */
#define SILC_COMPILER_FILTER_ID -1

/**
 * static variable to control initialize status of the compiler adapter.
 * If it is 0 it is initialized.
 */
static int silc_compiler_initialize = 1;

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
    silc_compiler_hash_node* hash_node;

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call at function enter." );

    /*
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses if measurement was not initialized
     */

    if ( silc_compiler_initialize )
    {
        /* not initialized so far */
        SILC_InitMeasurement();
    }

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " function id: %d ", *id );

    /* Register new region if unknown */
    if ( *id == 0 )
    {
        if ( hash_node = silc_compiler_hash_get( *id ) )
        {
            /* -- region entered the first time, register region -- */
            silc_compiler_register_region( hash_node );

            /* Set exit id */
            *id2 = hash_node->key;
        }
        else
        {
            *id  = SILC_COMPILER_FILTER_ID;
            *id2 = SILC_COMPILER_FILTER_ID;
        }
    }

    /* Enter event */
    if ( *id != SILC_COMPILER_FILTER_ID )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                           "enter the region with handle %i ",
                           hash_node->region_handle );
        SILC_EnterRegion( hash_node->region_handle );
    }
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
    silc_compiler_hash_node* hash_node;

    /* Check if function is filtered */
    if ( *id2 == SILC_COMPILER_FILTER_ID )
    {
        return;
    }

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call function exit." );

    if ( hash_node = silc_compiler_hash_get( *id2 ) )
    {
        SILC_ExitRegion( hash_node->region_handle );
    }
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
    silc_compiler_hash_node* hash_node;

    /* Check if function is filtered */
    if ( *id2 == SILC_COMPILER_FILTER_ID )
    {
        return;
    }

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "Catch in region." );

    if ( hash_node = silc_compiler_hash_get( *id2 ) )
    {
        SILC_ExitRegion( hash_node->region_handle );
    }
}

void
VT_IntelCatch( uint32_t* id2 )
{
    __VT_IntelCatch( id2 );
}

void
__VT_IntelCheck( uint32_t* id2 )
{
    silc_compiler_hash_node* hash_node;

    /* Check if function is filtered */
    if ( *id2 == SILC_COMPILER_FILTER_ID )
    {
        return;
    }

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "Check in region. Try " );

    if ( hash_node = silc_compiler_hash_get( *id2 ) )
    {
        SILC_ExitRegion( hash_node->region_handle );
    }
}

void
VT_IntelCheck( uint32_t* id2 )
{
    __VT_IntelCheck( id2 );
}


/* ***************************************************************************************
   Adapter management
*****************************************************************************************/

SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " inititialize intel compiler adapter." );

        /* Initialize hash tables */
        silc_compiler_hash_init();

        /* call function to calculate symbol table */
        silc_compiler_get_sym_tab();

        /* Sez flag */
        silc_compiler_initialize = 0;

        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                           " inititialization of intel compiler adapter done." );
    }

    return SILC_SUCCESS;
}

/* Adapter finalization */
void
silc_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !silc_compiler_initialize )
    {
        /* Delete hash table */
        silc_compiler_hash_free();

        /* Set initilaization flag */
        silc_compiler_initialize = 1;
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize intel compiler adapter." );
    }
}
