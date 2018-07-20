/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 * @brief Support for PGI Compiler
 *
 * Provides support for PGI-compiler profiling for 7.x and 8.x compilers.
 * Note: The PGI9.x+ compiler versions can also use the GNU instrument function
 * methods to provide function enter and exit events.
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_IO.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Location.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Task.h>

#include "SCOREP_Compiler_Init.h"

/* **************************************************************************************
 * Typedefs and global variables
 ***************************************************************************************/

/**
 * @brief Data structures to be used by the PGI compiler.
 * Container structure to map profiling informations, like function names
 * and region handles.
 */

#if __i386__

/*
 * .LENT1:
 *      .4byte	0,0,0,0,0,0,0,0
 */
struct PGI_LENT_32
{
    uint32_t data[ 8 ];
};

/*
 * .LOOP1:
 *      .4byte	0,0,0,0,0,0,0,0
 */
struct PGI_LOOP_32
{
    uint32_t data[ 8 ];
};

/*
 * PGI_PROFENT1:
 *      .4byte	0,0,0,0,0,0,0
 *      .4byte	main,.LENT1,19,.LOOP1,0,0,0,.FLNM,.FCNM1
 */
struct PGI_PROFENT_32
{
    uint32_t            handle;
    uint32_t            data0[ 6 ];
    void                ( * func )();
    struct PGI_LENT_32* lent;
    uint32_t            lineno;
    struct PGI_LOOP_32* loop;
    uint32_t            data1[ 3 ];
    char*               flnm;
    char*               fcnm;
};

#elif __x86_64__

/*
 * .LENT1:
 *      .4byte	0,0
 *      .quad	0
 *      .4byte	0,0,0,0,0,0
 *      .quad	0
 */
struct PGI_LENT_64
{
    uint32_t data0[ 2 ];
    uint64_t data1;
    uint32_t data2[ 6 ];
    uint64_t data3;
};

/*
 * .LOOP1:
 *      .4byte	0,0,0,0
 *      .quad	0,0
 *      .4byte	0,0
 */
struct PGI_LOOP_64
{
    uint32_t data0[ 4 ];
    uint64_t data1[ 2 ];
    uint32_t data2[ 2 ];
};

/*
 * PGI_PROFENT1:
 *      .4byte	0,0
 *      .quad	0
 *      .4byte	0,0,0,0,0,0
 *      .quad	main,.LENT1
 *      .4byte	19,0
 *      .quad	.LOOP1,0
 *      .4byte	0,0
 *      .quad	.FLNM,.FCNM1
 */
struct PGI_PROFENT_64
{
    uint32_t            handle;
    uint32_t            data0;
    uint64_t            data1;
    uint32_t            data2[ 6 ];
    void                ( * func )();
    struct PGI_LENT_64* lent;
    uint32_t            lineno;
    uint32_t            data3;
    struct PGI_LOOP_64* loop;
    uint64_t            data4;
    uint32_t            data5[ 2 ];
    char*               flnm;
    char*               fcnm;
};

#else

#error "unsupported architecture"

#endif

/* **************************************************************************************
 * Implementation of complier inserted functions
 ***************************************************************************************/

/* *INDENT-OFF* */

#define SCOREP_COMPILER_ENTER_MINST()                                                   \
    SCOREP_IN_MEASUREMENT_INCREMENT();                                                  \
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )                                           \
    {                                                                                   \
        SCOREP_InitMeasurement();                                                       \
    }                                                                                   \
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )        \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    check_region( &profent->handle,                                                     \
                  profent->fcnm,                                                        \
                  profent->flnm,                                                        \
                  profent->lineno );                                                    \
                                                                                        \
    if ( profent->handle != SCOREP_FILTERED_REGION )                                    \
    {                                                                                   \
        SCOREP_EnterRegion( profent->handle );                                          \
    }                                                                                   \
                                                                                        \
    SCOREP_IN_MEASUREMENT_DECREMENT()

#define SCOREP_COMPILER_EXIT_MINST()                                                    \
    SCOREP_IN_MEASUREMENT_INCREMENT();                                                  \
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )        \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    if ( profent->handle != SCOREP_FILTERED_REGION )                                    \
    {                                                                                   \
        SCOREP_ExitRegion( profent->handle );                                           \
    }                                                                                   \
                                                                                        \
    SCOREP_IN_MEASUREMENT_DECREMENT();


#define SCOREP_COMPILER_ENTER_MPROF()                                                   \
    SCOREP_IN_MEASUREMENT_INCREMENT();                                                  \
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )                                           \
    {                                                                                   \
        SCOREP_InitMeasurement();                                                       \
    }                                                                                   \
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )        \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    check_region( &profent->handle,                                                     \
                  profent->fcnm,                                                        \
                  profent->flnm,                                                        \
                  profent->lineno );                                                    \
                                                                                        \
    if ( profent->handle != SCOREP_FILTERED_REGION )                                    \
    {                                                                                   \
        SCOREP_EnterRegion( profent->handle );                                          \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        SCOREP_Task_Enter( SCOREP_Location_GetCurrentCPULocation(), profent->handle );  \
    }                                                                                   \
                                                                                        \
    SCOREP_IN_MEASUREMENT_DECREMENT()


#define SCOREP_COMPILER_EXIT_MPROF()                                                    \
    SCOREP_IN_MEASUREMENT_INCREMENT();                                                  \
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )        \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();                \
    UTILS_ASSERT( location != NULL );                                                   \
                                                                                        \
    SCOREP_RegionHandle region_handle =                                                 \
        SCOREP_Task_GetTopRegion( SCOREP_Task_GetCurrentTask( location ) );             \
    UTILS_ASSERT( region_handle != SCOREP_INVALID_REGION );                             \
                                                                                        \
    /* Check whether the top element of the callstack has a valid region handle.*/      \
    /* If the region is filtered the top pointer is SCOREP_INVALID_REGION. */           \
    if ( region_handle != SCOREP_FILTERED_REGION )                                      \
    {                                                                                   \
        SCOREP_ExitRegion( region_handle );                                             \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        SCOREP_Task_Exit( location );                                                   \
    }                                                                                   \
    SCOREP_IN_MEASUREMENT_DECREMENT()


/* *INDENT-ON* */


/**
 * called during program initialization
 */
#pragma save_all_regs
void
__rouinit( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    UTILS_DEBUG_ENTRY();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/**
 * called during program termination
 */

#pragma save_all_regs
void
__rouexit( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static inline void
check_region( SCOREP_RegionHandle* region,
              char*                region_name,
              char*                file_name,
              int                  lineno )
{
    /* Register new regions */
    if ( !*region )
    {
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( !*region )
        {
            UTILS_IO_SimplifyPath( file_name );

            if ( ( strncmp( region_name, "POMP", 4 ) != 0 ) &&
                 ( strncmp( region_name, "Pomp", 4 ) != 0 ) &&
                 ( strncmp( region_name, "pomp", 4 ) != 0 ) &&
                 ( strstr( region_name, "SCOREP_User_RegionClass" ) == 0 ) &&
                 ( !SCOREP_Filtering_Match( file_name, region_name, NULL ) ) )
            {
                *region = SCOREP_Definitions_NewRegion( region_name,
                                                        NULL,
                                                        SCOREP_Definitions_NewSourceFile( file_name ),
                                                        lineno,
                                                        SCOREP_INVALID_LINE_NO,
                                                        SCOREP_PARADIGM_COMPILER,
                                                        SCOREP_REGION_FUNCTION );
            }
            else
            {
                *region = SCOREP_FILTERED_REGION;
            }
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }
}

#if __i386__

#pragma save_all_regs
void
___rouent( struct PGI_PROFENT_32* profent )
{
    SCOREP_COMPILER_ENTER_MPROF();
}

#pragma save_all_regs
void
___rouent2( struct PGI_PROFENT_32* profent )
{
    SCOREP_COMPILER_ENTER_MPROF();
}

/**
 * called at the end of each instrumented routine
 */
#pragma save_all_regs
void
___rouret( void )
{
    SCOREP_COMPILER_EXIT_MPROF();
}

#pragma save_all_regs
void
___rouret2( void )
{
    SCOREP_COMPILER_EXIT_MPROF();
}

#elif __x86_64__

/**
 * Called at the beginning of each instrumented routine
 *
 * The profent is in register %r9, which is by the amd64 calling convention
 * the 6th argument
 */
#pragma save_all_regs
void
___instent64( void*                  arg0,
              void*                  arg1,
              void*                  arg2,
              void*                  arg3,
              void*                  arg4,
              struct PGI_PROFENT_64* profent )
{
    SCOREP_COMPILER_ENTER_MINST();
}

/**
 * called at the end of each instrumented routine
 */
#pragma save_all_regs
void
___instret64( void*                  arg0,
              void*                  arg1,
              void*                  arg2,
              void*                  arg3,
              void*                  arg4,
              struct PGI_PROFENT_64* profent )
{
    SCOREP_COMPILER_EXIT_MINST();
}

#pragma save_all_regs
void
___instentavx( void*                  arg0,
               void*                  arg1,
               void*                  arg2,
               void*                  arg3,
               void*                  arg4,
               struct PGI_PROFENT_64* profent )
{
    SCOREP_COMPILER_ENTER_MINST();
}


#pragma save_all_regs
void
___instretavx( void*                  arg0,
               void*                  arg1,
               void*                  arg2,
               void*                  arg3,
               void*                  arg4,
               struct PGI_PROFENT_64* profent )
{
    SCOREP_COMPILER_EXIT_MINST();
}

/**
 * Called at the beginning of each instrumented routine
 *
 * The profent is in register %r9, which is by the amd64 calling convention
 * the 6th argument
 */
#pragma save_all_regs
void
___rouent64( void*                  arg0,
             void*                  arg1,
             void*                  arg2,
             void*                  arg3,
             void*                  arg4,
             struct PGI_PROFENT_64* profent )
{
    SCOREP_COMPILER_ENTER_MINST();
}

#pragma save_all_regs
void
___rouret64( void )
{
    SCOREP_COMPILER_EXIT_MPROF();
}

#endif

#pragma save_all_regs
void
___linent2( void )
{
}
