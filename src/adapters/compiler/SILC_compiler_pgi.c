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
 * @ file SILC_compiler_pgi.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief provided support for PGI-compiler profiling for 7.x and 8.x compilers.
 * Note: The PGI9.x compiler version uses GNU instrument function methods to provide
 * function enter and exit events.
 */

#include <stdio.h>
#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_DefinitionLocking.h>
#include <SILC_RuntimeManagement.h>
#include <SILC_Compiler_Data.h>

/* **************************************************************************************
 * Typedefs and global variables
 ***************************************************************************************/

/**
 * @brief Data structure to be used by the PGI compiler.
 * Container structure to map profiling informations, like function names
 * and region handles.
 */
struct s1
{
    uint64_t              l1;
    uint64_t              l2;
    double                d1;
    double                d2;
    uint32_t              isseen;
    char*                 c;
    void*                 p1;
    SILC_LineNo           lineno;
    void*                 p2;
    struct s1*            p3;
    SILC_SourceFileHandle file_handle;    /* file handle   */
    SILC_RegionHandle     region_handle;  /* region handle */
    char*                 file_name;      /* file name     */
    char*                 region_name;    /* routine name  */
};

/**
    Pointer to the current callstack position. Because it is needed for each thread,
    it is made thread private.
 */
SILC_RegionHandle* silc_compiler_callstack_top = NULL;
#pragma omp threadprivate(silc_compiler_callstack_top)

/**
    Pointer to the callstack starting position. Because it is needed for each thread,
    it is made thread private.
 */
SILC_RegionHandle* silc_compiler_callstack_base = NULL;
#pragma omp threadprivate(silc_compiler_callstack_base)

/**
    Counts the current level of nesting. Because it is needed for each thread,
    it is made thread private.
 */
uint32_t silc_compiler_callstack_count = 0;
#pragma omp threadprivate(silc_compiler_callstack_size)

/**
    Defines the maximum size of a callstack.
 */
static const uint32_t silc_compiler_callstack_max = 30;

/**
 * static variable to control initialize status of adapter
 */
static int silc_compiler_initialize = 1;

/* **************************************************************************************
 * Initialization / Finalization
 ***************************************************************************************/
/**
   Creates the callstack array for a new thread.
 */
void
silc_compiler_init_thread()
{
    /* Allocate memory for region handle stack */
    silc_compiler_callstack_base = ( SILC_RegionHandle* )
                                   malloc( silc_compiler_callstack_max * sizeof( SILC_RegionHandle ) );
    silc_compiler_callstack_top = silc_compiler_callstack_base;
}

/* Adapter initialization */
SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " inititialize PGI compiler adapter!" );

        /* Initialize default thread */
        silc_compiler_init_thread();

        /* Initialize file table only */
        silc_compiler_init_file_table();

        /* Set flag */
        silc_compiler_initialize = 0;
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
        /* Finalize file table */
        silc_compiler_final_file_table();

        silc_compiler_initialize = 1;
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize PGI compiler adapter!" );
    }
}

/* **************************************************************************************
 * Implementation of complier inserted functions
 ***************************************************************************************/

/**
 * called during program initialization
 */
#pragma save_all_regs
void
__rouinit
(
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "PGI init routine" );


    if ( silc_compiler_initialize )
    {
        SILC_InitMeasurement();
    }
}


/**
 * called during program termination
 */

#pragma save_all_regs
void
__rouexit
(
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                       "Termination routine from PGI compiler instrumentation called" );
}


/**
 * called at the beginning of each instrumented routine
 */
#pragma save_all_regs
void
___rouent2
(
    struct s1* p
)
{
    /* Ensure the compiler adapter is initialized */
    if ( silc_compiler_initialize )
    {
        SILC_InitMeasurement();
    }

    /* Ensure thread is initialized */
    if ( silc_compiler_callstack_top == NULL )
    {
        silc_compiler_init_thread();
    }

    /* Register new regions */
    if ( !p->isseen )
    {
        /* get the file name from instrumentation */

        /* get file id beloning to file name */
        SILC_LOCK( Region );
        if ( !p->isseen )
        {
            p->file_handle   = silc_compiler_get_file( p->file_name );
            p->region_handle = SILC_DefineRegion( p->region_name,
                                                  p->file_handle,
                                                  p->lineno,
                                                  SILC_INVALID_LINE_NO,
                                                  SILC_ADAPTER_COMPILER,
                                                  SILC_REGION_FUNCTION
                                                  );
        }
        p->isseen = 1;
        SILC_UNLOCK( Region );
    }

    /* Check callstack */
    silc_compiler_callstack_count++;
    if ( silc_compiler_callstack_count < silc_compiler_callstack_max )
    {
        /* Update callstack */
        *silc_compiler_callstack_top = p->region_handle;
        silc_compiler_callstack_top++;

        /* Enter event */
        if ( p->region_handle != SILC_INVALID_REGION )
        {
            SILC_EnterRegion( p->region_handle );
        }
    }
}

/**
 * called at the end of each instrumented routine
 */
#pragma save_all_regs
void
___rouret2
(
    void
)
{
    if ( silc_compiler_callstack_count < silc_compiler_callstack_max )
    {
        /* Exit event */
        SILC_ExitRegion( *silc_compiler_callstack_top );
        silc_compiler_callstack_top--;
    }

    silc_compiler_callstack_count--;
}

#pragma save_all_regs
void
___linent2
(
    void
)
{
}
