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
 * @file       scorep_compiler_ibm.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Support for XL IBM-Compiler
 * Will be triggered by the frunctrion trace option by the xl
 * compiler.
 */

#include <config.h>
#include <stdio.h>
#include <string.h>

#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Thread_Types.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Filter.h>

#include <SCOREP_Compiler_Init.h>
#include <scorep_compiler_data.h>


/**
 * static variable to control initialize status of compiler adapter.
 */
static int scorep_compiler_initialize = 1;

/**
 * flag that indicates whether the compiler adapter is finalized
 */
static int scorep_compiler_finalized = 0;

/**
 * Mutex for exclusive access to the region hash table.
 */
static SCOREP_Mutex scorep_compiler_region_mutex;

void
__func_trace_enter( char* region_name,
                    char* file_name,
                    int   line_no );
void
__func_trace_exit( char* region_name,
                   char* file_name,
                   int   line_no );

/**
 * @ brief This function is called at the entry of each function.
 * The call is generated by the IBM xl compilers
 *
 * @ param region_name function name
 * @ param file_name   file name
 * @ param line_no     line number
 */
void
__func_trace_enter( char* region_name,
                    char* file_name,
                    int   line_no )
{
    scorep_compiler_hash_node* hash_node;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                         " function name: %s %s", region_name, file_name );

    if ( scorep_compiler_initialize )
    {
        if ( scorep_compiler_finalized )
        {
            return;
        }
        SCOREP_InitMeasurement();
    }

    /* put function to list */
    if ( ( hash_node = scorep_compiler_hash_get( ( long )region_name ) ) == 0 )
    {
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( ( hash_node = scorep_compiler_hash_get( ( long )region_name ) ) == 0 )
        {
            char* file = SCOREP_CStr_dup( file_name );
            SCOREP_IO_SimplifyPath( file );
            hash_node = scorep_compiler_hash_put( ( long )region_name,
                                                  region_name,
                                                  file, line_no );
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                                 " number %ld and put name -- %s -- to list",
                                 ( long )region_name, region_name );

            /* Check for filters:
               1. In case OpenMP is used, the XL compiler creates some
                  functions like <func_name>:<func_name>$OL$OL.1 on BG/P or
                  <func_name>@OL@1 on AIX which cause the measurement system
                  to crash. Thus, filter functions which names contain a '$'
                  or '@' symbol.
               2. POMP and POMP2 functions.
             */
            bool is_filtered = false;
            if ( ( strchr( region_name, '$' ) != NULL ) ||
                 ( strchr( region_name, '@' ) != NULL ) ||
                 ( strncmp( region_name, "POMP", 4 ) == 0 ) ||
                 ( strncmp( region_name, "Pomp", 4 ) == 0 ) ||
                 ( strncmp( region_name, "pomp", 4 ) == 0 ) ||
                 SCOREP_Filter_Match( file, region_name, true ) )
            {
                is_filtered = true;
            }

            /* If no $ found in name register region */
            if ( !is_filtered )
            {
                scorep_compiler_register_region( hash_node );
            }
            free( file );
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }

    /* Invalid handle marks filtered regions */
    if ( ( hash_node->region_handle != SCOREP_INVALID_REGION ) )
    {
        SCOREP_EnterRegion( hash_node->region_handle );
    }
}

/**
 * @ brief This function is called at the exit of each function.
 * The call is generated by the IBM xl compilers
 *
 * @ param region_name function name
 * @ param file_name   file name
 * @ param line_no     line number
 */
void
__func_trace_exit( char* region_name,
                   char* file_name,
                   int   line_no )
{
    scorep_compiler_hash_node* hash_node;
    if ( scorep_compiler_finalized )
    {
        return;
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "call function exit!!!" );
    if ( hash_node = scorep_compiler_hash_get( ( long )region_name ) )
    {
        /* Invalid handle marks filtered regions */
        if ( ( hash_node->region_handle != SCOREP_INVALID_REGION ) )
        {
            SCOREP_ExitRegion( hash_node->region_handle );
        }
    }
}

/* Initialize adapter */
SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    if ( scorep_compiler_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                             " inititialize IBM xl compiler adapter!" );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize hash table */
        scorep_compiler_hash_init();

        /* Sez flag */
        scorep_compiler_initialize = 0;
    }
    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_compiler_init_location()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "IBM xl compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_final_location( SCOREP_Location* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "IBM xlcompiler adapter final location!" );
}

/* Finalize adapter */
void
scorep_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !scorep_compiler_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize IBM xl compiler adapter!" );

        /* Delete hash table */
        scorep_compiler_hash_free();

        /* Set flag to not initialized */
        scorep_compiler_initialize = 1;
        scorep_compiler_finalized  = 1;

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
