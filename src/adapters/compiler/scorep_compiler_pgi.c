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
 * @file       SCOREP_compiler_pgi.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief provided support for PGI-compiler profiling for 7.x and 8.x compilers.
 * Note: The PGI9.x compiler version uses GNU instrument function methods to provide
 * function enter and exit events.
 */

#include <config.h>
#include <stdio.h>
#include <UTILS_Debug.h>
#include <SCOREP_Location.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_RuntimeManagement.h>
#include <scorep_compiler_data.h>
#include <SCOREP_Compiler_Init.h>
#include <SCOREP_Filter.h>
#include <SCOREP_Memory.h>



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

#elif __x68_64__

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


extern size_t scorep_compiler_subsystem_id;


/**
    Defines the maximum size of a callstack.
 */
#define CALLSTACK_MAX 30

/**
   Contains the callstack data for each location.
 */
typedef struct
{
    /**
        Pointer to the callstack starting position.
     */
    SCOREP_RegionHandle callstack_base[ CALLSTACK_MAX ];

    /**
        Counts the current level of nesting.
     */
    uint32_t callstack_count;

    /**
        Current callstack position.
     */
    uint32_t callstack_top;
} pgi_location_data;

/**
 * static variable to control initialize status of adapter
 */
static int scorep_compiler_initialize = 1;

/**
 * static variable that indicates whether the adapter is already finalized.
 */
static int scorep_compiler_finalized = 0;

/**
 * Mutex for exclusive access to the region hash table.
 */
static SCOREP_Mutex scorep_compiler_region_mutex;


/* **************************************************************************************
 * location table access
 ***************************************************************************************/

static inline pgi_location_data*
scorep_compiler_get_location_data( SCOREP_Location* locationData )
{
    pgi_location_data* pgi_data = SCOREP_Location_GetSubsystemData(
        locationData,
        scorep_compiler_subsystem_id );

    return pgi_data;
}

/* **************************************************************************************
 * Initialization / Finalization
 ***************************************************************************************/

/* Location initialization */
SCOREP_ErrorCode
scorep_compiler_init_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "PGI compiler adapter init location!" );
    /* Create location struct */
    pgi_location_data* pgi_data = SCOREP_Location_AllocForMisc( locationData,
                                                                sizeof( *pgi_data ) );

    pgi_data->callstack_top   = 0;
    pgi_data->callstack_count = 0;

    SCOREP_Location_SetSubsystemData( locationData,
                                      scorep_compiler_subsystem_id,
                                      pgi_data );

    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_finalize_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "PGI compiler adapter finalize location!" );
}

/* Adapter initialization */
SCOREP_ErrorCode
scorep_compiler_init_adapter()
{
    if ( scorep_compiler_initialize )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " inititialize PGI compiler adapter!" );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize file table */
        scorep_compiler_init_file_table();

        /* Set flag */
        scorep_compiler_initialize = 0;
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
        /* Finalize file table */
        scorep_compiler_finalize_file_table();

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );

        scorep_compiler_initialize = 1;
        scorep_compiler_finalized  = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize PGI compiler adapter!" );
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
__rouinit()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "PGI init routine" );

    if ( scorep_compiler_initialize )
    {
        /* Check whether adapter is already finalized */
        if ( scorep_compiler_finalized )
        {
            return;
        }

        SCOREP_InitMeasurement();
    }
}


/**
 * called during program termination
 */

#pragma save_all_regs
void
__rouexit()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                        "Termination routine from PGI compiler instrumentation called" );
}

static inline void
pgi_enter_region( SCOREP_RegionHandle* region,
                  char*                region_name,
                  char*                file_name,
                  int                  lineno )
{
    /* Ensure the compiler adapter is initialized */
    if ( scorep_compiler_initialize )
    {
        /* Check whether adapter is already finalized */
        if ( scorep_compiler_finalized )
        {
            return;
        }

        SCOREP_InitMeasurement();
    }

    SCOREP_Location* data = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( data != NULL );
    pgi_location_data* pgi_data = scorep_compiler_get_location_data( data );
    if ( pgi_data == NULL )
    {
        scorep_compiler_init_location( data );
        pgi_data = scorep_compiler_get_location_data( data );
    }

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
                 ( !SCOREP_Filter_Match( file_name, region_name, false ) ) )
            {
                *region = SCOREP_DefineRegion( region_name,
                                               NULL,
                                               SCOREP_DefineSourceFile( file_name ),
                                               lineno,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_ADAPTER_COMPILER,
                                               SCOREP_REGION_FUNCTION );
            }
            else
            {
                *region = SCOREP_INVALID_REGION;
            }
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }

    /* Check callstack */
    if ( pgi_data->callstack_count < CALLSTACK_MAX )
    {
        /* Update callstack */
        pgi_data->callstack_base[ pgi_data->callstack_top ] = *region;
        pgi_data->callstack_top++;

        /* Enter event */
        if ( *region != SCOREP_INVALID_REGION )
        {
            SCOREP_EnterRegion( *region );
        }
    }
    pgi_data->callstack_count++;
}

#if __i386__

#pragma save_all_regs
void
___rouent( struct PGI_PROFENT_32* profent )
{
    pgi_enter_region( &profent->handle,
                      profent->fcnm,
                      profent->flnm,
                      profent->lineno );
}

#pragma save_all_regs
void
___rouent2( struct PGI_PROFENT_32* profent )
{
    pgi_enter_region( &profent->handle,
                      profent->fcnm,
                      profent->flnm,
                      profent->lineno );
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
___rouent64( void*                  arg0,
             void*                  arg1,
             void*                  arg2,
             void*                  arg3,
             void*                  arg4,
             struct PGI_PROFENT_64* profent )
{
    pgi_enter_region( &profent->handle,
                      profent->fcnm,
                      profent->flnm,
                      profent->lineno );
}

#endif

/**
 * called at the end of each instrumented routine
 */
#pragma save_all_regs
void
___rouret( void )
{
    /* Check whether adapter is already finalized */
    if ( scorep_compiler_finalized )
    {
        return;
    }

    SCOREP_Location* data = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( data != NULL );
    pgi_location_data* location_data = scorep_compiler_get_location_data( data );
    if ( location_data == NULL )
    {
        return;
    }

    location_data->callstack_count--;
    if ( location_data->callstack_count < CALLSTACK_MAX )
    {
        location_data->callstack_top--;
        SCOREP_RegionHandle region_handle = location_data->callstack_base[ location_data->callstack_top ];

        /* Check whether the top element of the callstack has a valid region handle.
           If the region is filtered the top pointer is SCOREP_INVALID_REGION.
         */
        if ( region_handle != SCOREP_INVALID_REGION )
        {
            SCOREP_ExitRegion( region_handle );
        }
    }
}

#pragma save_all_regs
void
___rouret2( void )
{
    ___rouret();
}

#pragma save_all_regs
void
___rouret64( void )
{
    ___rouret();
}

#pragma save_all_regs
void
___linent2( void )
{
}
