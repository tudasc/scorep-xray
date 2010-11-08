/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Thread_Types.h>
#include <scorep_compiler_data.h>
#include <SCOREP_Compiler_Init.h>

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

/**
   Contains the callstack data for each location.
 */
typedef struct
{
    /**
        Pointer to the current callstack position. Because it is needed for
        each thread, it is made thread private.
     */
    SCOREP_RegionHandle* callstack_top;

    /**
        Pointer to the callstack starting position. Because it is needed for
        each thread, it is made thread private.
     */
    SCOREP_RegionHandle* callstack_base;

    /**
        Counts the current level of nesting. Because it is needed for each
        thread, it is made thread private.
     */
    uint32_t callstack_count;

    /**
       Location ID
     */
    uint64_t location_id;
} scorep_compiler_location_data;

/**
    Defines the maximum size of a callstack.
 */
static const uint32_t scorep_compiler_callstack_max = 30;

/**
 * static variable to control initialize status of adapter
 */
static int scorep_compiler_initialize = 1;

/**
    Hash table for mapping location id to the location's callstack.
 */
SCOREP_Hashtab* scorep_compiler_location_table = NULL;

/**
 * Mutex for exclusive access to the region hash table.
 */
static SCOREP_Mutex scorep_compiler_region_mutex;


/* **************************************************************************************
 * declarations from scorep_thread.h
 ***************************************************************************************/

extern SCOREP_Thread_LocationData*
SCOREP_Thread_GetLocationData();

extern uint64_t
SCOREP_Thread_GetLocationId( SCOREP_Thread_LocationData* locationData );

/* **************************************************************************************
 * location table access
 ***************************************************************************************/

inline uint64_t
scorep_compiler_get_location_id()
{
    SCOREP_Thread_LocationData* data = SCOREP_Thread_GetLocationData();
    SCOREP_ASSERT( data != NULL );
    return SCOREP_Thread_GetLocationId( data );
}

inline scorep_compiler_location_data*
scorep_compiler_get_location_data()
{
    uint64_t              location_id = scorep_compiler_get_location_id();
    SCOREP_Hashtab_Entry* entry       = SCOREP_Hashtab_Find( scorep_compiler_location_table,
                                                             &location_id,
                                                             NULL );
    if ( !entry )
    {
        return NULL;
    }
    else
    {
        return ( scorep_compiler_location_data* )entry->value;
    }
}

/* **************************************************************************************
 * Initialization / Finalization
 ***************************************************************************************/

/* Creates the callstack array for a new thread. */
scorep_compiler_location_data*
scorep_compiler_create_location_data( uint64_t id )
{
    /* Create location struct */
    scorep_compiler_location_data* data = NULL;
    data = ( scorep_compiler_location_data* )malloc( sizeof( scorep_compiler_location_data ) );

    /* Allocate memory for region handle stack */
    data->callstack_base = ( SCOREP_RegionHandle* )
                           malloc( scorep_compiler_callstack_max *
                                   sizeof( SCOREP_RegionHandle ) );
    data->callstack_top   = data->callstack_base;
    data->callstack_count = 0;
    data->location_id     = id;

    return data;
}

/**
   Deletes one file table entry.#
   @param entry Pointer to the entry to be deleted.
 */
void
scorep_compiler_delete_location_entry( SCOREP_Hashtab_Entry* entry )
{
    SCOREP_ASSERT( entry );
    scorep_compiler_location_data* data =  ( scorep_compiler_location_data* )entry->value;
    free( data->callstack_base );
    free( data );
}

/* Initialize the location table */
void
scorep_compiler_init_location_table()
{
    scorep_compiler_location_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashInt64,
                                                                &SCOREP_Hashtab_CompareInt64 );
}

/* Finalize the location table */
void
scorep_compiler_final_location_table()
{
    SCOREP_Hashtab_Foreach( scorep_compiler_location_table,
                            &scorep_compiler_delete_location_entry );
    SCOREP_Hashtab_Free( scorep_compiler_location_table );
    scorep_compiler_location_table = NULL;
}

/* Location initialization */
SCOREP_Error_Code
scorep_compiler_init_location()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " PGI compiler adapter init loacation!" );

    uint64_t                       location_id = scorep_compiler_get_location_id();
    scorep_compiler_location_data* data        = scorep_compiler_create_location_data( location_id );

    SCOREP_MutexLock( scorep_compiler_region_mutex );
    SCOREP_Hashtab_Insert( scorep_compiler_location_table, &data->location_id,
                           data, NULL );
    SCOREP_MutexUnlock( scorep_compiler_region_mutex );

    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_final_location()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " compiler adapter final loacation!" );
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

        /* Initialize location table */
        scorep_compiler_init_location_table();

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
        scorep_compiler_final_file_table();

        /* Finalize location table */
        scorep_compiler_final_location_table();

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );

        scorep_compiler_initialize = 1;
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize PGI compiler adapter!" );
    }
}

/**
   Registers configuration variables for the compiler adapters. Currently no
   configuration variables exist for PGI compiler adapters.
 */
SCOREP_Error_Code
scorep_compiler_register()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " register PGI compiler adapter!" );

    return SCOREP_SUCCESS;
}

/**
   Called on dereigstration of the compiler adapter. Currently, no action is performed
   on deregistration.
 */
void
scorep_compiler_deregister()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " PGI compiler adapter deregister!n" );
}

/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Adapter SCOREP_Compiler_Adapter =
{
    SCOREP_ADAPTER_COMPILER,
    "COMPILER",
    &scorep_compiler_register,
    &scorep_compiler_init_adapter,
    &scorep_compiler_init_location,
    &scorep_compiler_final_location,
    &scorep_compiler_finalize,
    &scorep_compiler_deregister
};

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
    /* Ensure the compiler adapter is initialized */
    if ( scorep_compiler_initialize )
    {
        SCOREP_InitMeasurement();
    }
    scorep_compiler_location_data* location_data = scorep_compiler_get_location_data();
    if ( location_data == NULL )
    {
        scorep_compiler_init_location();
        location_data = scorep_compiler_get_location_data();
    }

    /* Register new regions */
    if ( !p->isseen )
    {
        /* get file id beloning to file name */
        p->file_handle = ( double )scorep_compiler_get_file( p->file_name ) + ( double )0.1;
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( !p->isseen )
        {
            p->region_handle = ( double )0.1 + ( double )
                               SCOREP_DefineRegion( p->region_name,
                                                    p->file_handle,
                                                    p->lineno,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_ADAPTER_COMPILER,
                                                    SCOREP_REGION_FUNCTION
                                                    );
        }
        p->isseen = 1;
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }

    /* Check callstack */
    location_data->callstack_count++;
    if ( location_data->callstack_count < scorep_compiler_callstack_max )
    {
        /* Update callstack */
        *location_data->callstack_top = p->region_handle;
        location_data->callstack_top++;

        /* Enter event */
        if ( p->region_handle != SCOREP_INVALID_REGION )
        {
            SCOREP_EnterRegion( p->region_handle );
        }
    }
}

#pragma save_all_regs
void
___rouent( struct s1* p )
{
    ___rouent2( p );
}

/**
 * called at the end of each instrumented routine
 */
#pragma save_all_regs
void
___rouret2( void )
{
    scorep_compiler_location_data* location_data = scorep_compiler_get_location_data();
    if ( location_data == NULL )
    {
        return;
    }
    if ( location_data->callstack_count < scorep_compiler_callstack_max )
    {
        location_data->callstack_top--;
        /* Exit event */
        SCOREP_ExitRegion( *location_data->callstack_top );
    }

    location_data->callstack_count--;
}

#pragma save_all_regs
void
___rouret( void )
{
    ___rouret2();
}

#pragma save_all_regs
void
___linent2( void )
{
}
