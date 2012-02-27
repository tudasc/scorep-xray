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
 * @file       scorep_compiler_ftrace.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Compiler adapter version for NEC SX compiler
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Thread_Types.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Mutex.h>

#include <SCOREP_Compiler_Init.h>
#include <scorep_compiler_data.h>


extern void*
scorep_ftrace_getname( void );
extern int
scorep_ftrace_getname_len( void );

#define SCOREP_FILTERED_REGION ( SCOREP_INVALID_REGION - 1 )

static uint32_t scorep_compiler_initialize = 1;

static uint32_t scorep_compiler_finalized = 0;

/**
 * Mutex for exclusive access to the region hash table.
 */
static SCOREP_Mutex scorep_compiler_region_mutex;


void
_ftrace_enter2_( void );
void
_ftrace_exit2_( void );
void
_ftrace_stop2_( void );

/*
 * This function is called at the entry of each function
 * The call is generated by the NEC SX compilers
 */

void
_ftrace_enter2_()
{
    char* region_name = scorep_ftrace_getname();
    int   name_len    = scorep_ftrace_getname_len();

    scorep_compiler_hash_node* hash_node = scorep_compiler_hash_get( ( long )region_name );

    if ( scorep_compiler_initialize )
    {
        if ( scorep_compiler_finalized )
        {
            return;
        }

        /* not initialized so far */
        SCOREP_InitMeasurement();
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "function name: %s", region_name );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "function length: %i", name_len );

    if ( !hash_node )
    {
        scorep_compiler_hash_put( ( long )region_name, region_name, "", name_len );
    }

    if ( ( hash_node = scorep_compiler_hash_get( ( long )region_name ) ) )
    {
        if ( hash_node->reghandle == SCOREP_INVALID_REGION )
        {
            SCOREP_MutexLock( scorep_compiler_region_mutex );
            if ( hash_node->reghandle == SCOREP_INVALID_REGION )
            {
                /* Check for filtered regions */
                if ( ( strncmp( region_name, "POMP", 4 ) == 0 ) ||
                     ( strncmp( region_name, "Pomp", 4 ) == 0 ) ||
                     ( strncmp( region_name, "pomp", 4 ) == 0 ) ||
                     SCOREP_Filter_Match( NULL, region_name, true ) )
                {
                    hash_node->reghandle = SCOREP_FILTERED_REGION;
                }

                /* Region entered the first time, register region */
                else
                {
                    scorep_compiler_register_region( hash_node );
                    assert( hash_node->reghandle != SCOREP_FILTERED_REGION );
                    assert( hash_node->reghandle != SCOREP_INVALID_REGION );
                }
            }
            SCOREP_MutexUnlock( scorep_compiler_region_mutex );
        }
        if ( hash_node->reghandle != SCOREP_FILTERED_REGION )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                                 "enter the region with handle %i",
                                 hash_node->region_handle );
            SCOREP_EnterRegion( hash_node->region_handle );
        }
    }
}


/*
 * This function is called at the exit of each function
 * The call is generated by the NEC SX compilers
 */

void
_ftrace_exit2_()
{
    if ( scorep_compiler_finalized )
    {
        return;
    }

    char*                      region_name = scorep_ftrace_getname();
    long                       key         = ( long )region_name;
    scorep_compiler_hash_node* hash_node;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call function exit!!!" );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " ftrace exit 2 %i", key );
    if ( hash_node = scorep_compiler_hash_get( key ) )
    {
        if ( hash_node->region_handle != SCOREP_FILTERED_REGION )
        {
            SCOREP_ExitRegion( hash_node->region_handle );
        }
    }
}

/*
 * This function is called at the exit of the program
 * The call is generated by the NEC SX compilers
 */
void
_ftrace_stop2_()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " ftrace stop 2" );
}

SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    if ( scorep_compiler_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                             " inititialize ftrace compiler adapter!" );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize hash table */
        scorep_compiler_hash_init();

        /* Set flag */
        scorep_compiler_initialize = 0;
    }

    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_compiler_init_location()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "ftrace Compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_final_location( SCOREP_Thread_LocationData* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "ftrace compiler adapter final location!" );
}

void
scorep_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !scorep_compiler_initialize )
    {
        /* Delete hash table */
        scorep_compiler_hash_free();

        scorep_compiler_initialize = 1;
        scorep_compiler_finalized  = 1;
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize ftrace compiler adapter!" );

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
