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
 * @status     alpha
 * @file       silc_compiler_ibm.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Support for XL IBM-Compiler
 * Will be triggered by the frunctrion trace option by the xl
 * compiler.
 */

#include <config.h>
#include <stdio.h>
#include <string.h>

#include <silc_utility/SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <silc_compiler_data.h>


/**
 * static variable to control initialize status of compiler adapter.
 */
static int silc_compiler_initialize = 1;


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
    silc_compiler_hash_node* hash_node;

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                       " function name: %s %s", region_name, file_name );

    if ( silc_compiler_initialize )
    {
        SILC_InitMeasurement();
    }

    /* put function to list */
    if ( ( hash_node = silc_compiler_hash_get( ( long )region_name ) ) == 0 )
    {
        SILC_LockRegionDefinition();
        if ( ( hash_node = silc_compiler_hash_get( ( long )region_name ) ) == 0 )
        {
            hash_node = silc_compiler_hash_put( ( long )region_name,
                                                region_name,
                                                file_name, line_no );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                               " number %ld and put name -- %s -- to list",
                               ( long )region_name, region_name );
        }
        SILC_UnlockRegionDefinition();

        /* Check for filters:
           1. In case OpenMP is used, the XL compiler creates some functions
              like <func_name>:<func_name>$OL$OL.1 which cause the measurement
              system to crash. Thus, filter functions which names contain a
              '$' symbol.
           2. POMP and POMP2 functions.
         */
        bool is_filtered = false;
        if ( ( strchr( region_name, '$' ) != NULL ) ||
             ( strncmp( region_name, "POMP", 4 ) == 0 ) )
        {
            is_filtered = true;
        }

        /* If no $ found in name register region */
        if ( !is_filtered )
        {
            silc_compiler_register_region( hash_node );
        }
    }

    /* Invalid handle marks filtered regions */
    if ( ( hash_node->region_handle != SILC_INVALID_REGION ) )
    {
        SILC_EnterRegion( hash_node->region_handle );
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
    silc_compiler_hash_node* hash_node;
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call function exit!!!" );
    if ( hash_node = silc_compiler_hash_get( ( long )region_name ) )
    {
        /* Invalid handle marks filtered regions */
        if ( ( hash_node->region_handle != SILC_INVALID_REGION ) )
        {
            SILC_ExitRegion( hash_node->region_handle );
        }
    }
}

/* Initialize adapter */
SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                           " inititialize IBM xl compiler adapter!" );

        /* Initialize hash table */
        silc_compiler_hash_init();

        /* Sez flag */
        silc_compiler_initialize = 0;
    }
    return SILC_SUCCESS;
}

/* Finalize adapter */
void
silc_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize IBM xl compiler adapter!" );

        /* Delete hash table */
        silc_compiler_hash_free();

        /* Set flag to not initialized */
        silc_compiler_initialize = 1;
    }
}
