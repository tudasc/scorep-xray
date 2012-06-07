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
#include <SCOREP_Debug.h>
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
 * @brief Data structure to be used by the PGI compiler.
 * Container structure to map profiling informations, like function names
 * and region handles.
 */
struct s1
{
    long       l1;
    long       l2;
    double     file_handle;
    double     region_handle;
    long       isseen;
    char*      c;
    void*      p1;
    long       lineno;
    void*      p2;
    struct s1* p3;
    int        i1;
    int        i2;
    char*      file_name;
    char*      region_name;
};


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
SCOREP_Error_Code
scorep_compiler_init_location( SCOREP_Location* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "PGI compiler adapter init location!" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "PGI compiler adapter finalize location!" );
}

/* Adapter initialization */
SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    if ( scorep_compiler_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " inititialize PGI compiler adapter!" );

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
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize PGI compiler adapter!" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "PGI init routine" );

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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                         "Termination routine from PGI compiler instrumentation called" );
}

/**
 * called at the beginning of each instrumented routine
 */
#pragma save_all_regs
void
___rouent2( struct s1* p )
{
    /* For the reason of having 8 bytes the handles are stored in double fields.
       However, we want to use them as 64 byte integers. */
    SCOREP_SourceFileHandle* file   = ( SCOREP_SourceFileHandle* )&( p->file_handle );
    SCOREP_RegionHandle*     region = ( SCOREP_RegionHandle* )&( p->region_handle );

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
    SCOREP_ASSERT( data != NULL );
    pgi_location_data* pgi_data = scorep_compiler_get_location_data( data );
    if ( pgi_data == NULL )
    {
        scorep_compiler_init_location( data );
        pgi_data = scorep_compiler_get_location_data( data );
    }

    /* Register new regions */
    if ( !p->isseen )
    {
        /* get file id belonging to file name */
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( !p->isseen )
        {
            p->isseen = 1;
            SCOREP_IO_SimplifyPath( p->file_name );

            *file = scorep_compiler_get_file( p->file_name );
            if ( ( strncmp( p->region_name, "POMP", 4 ) != 0 ) &&
                 ( strncmp( p->region_name, "Pomp", 4 ) != 0 ) &&
                 ( strncmp( p->region_name, "pomp", 4 ) != 0 ) &&
                 ( !SCOREP_Filter_Match( p->file_name, p->region_name, false ) ) )
            {
                *region = SCOREP_DefineRegion( p->region_name,
                                               *file,
                                               p->lineno,
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

#pragma save_all_regs
void
___rouent( struct s1* p )
{
    ___rouent2( p );
}

#pragma save_all_regs
void
___rouent64( struct s1* p )
{
    static bool print_warning = true;
    if ( print_warning )
    {
        print_warning = false;
        fprintf( stderr,
                 "Warning: You instruented the application with an unsupported interface."
                 "\n" );
    }
}

/**
 * called at the end of each instrumented routine
 */
#pragma save_all_regs
void
___rouret2( void )
{
    /* Check whether adapter is already finalized */
    if ( scorep_compiler_finalized )
    {
        return;
    }

    SCOREP_Location* data = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_ASSERT( data != NULL );
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
___rouret( void )
{
    ___rouret2();
}

#pragma save_all_regs
void
___rouret64( void )
{
    /* The ...64 bit versions are not supported. */
}

#pragma save_all_regs
void
___linent2( void )
{
}
