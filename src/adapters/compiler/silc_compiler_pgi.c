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
 * @file       SILC_compiler_pgi.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief provided support for PGI-compiler profiling for 7.x and 8.x compilers.
 * Note: The PGI9.x compiler version uses GNU instrument function methods to provide
 * function enter and exit events.
 */

#include <config.h>
#include <stdio.h>
#include <silc_utility/SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_DefinitionLocking.h>
#include <SILC_RuntimeManagement.h>
#include <SILC_Thread_Types.h>
#include <silc_compiler_data.h>
#include <SILC_Compiler_Init.h>

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
    SILC_RegionHandle* callstack_top;

    /**
        Pointer to the callstack starting position. Because it is needed for
        each thread, it is made thread private.
     */
    SILC_RegionHandle* callstack_base;

    /**
        Counts the current level of nesting. Because it is needed for each
        thread, it is made thread private.
     */
    uint32_t callstack_count;

    /**
       Location ID
     */
    uint64_t location_id;
} silc_compiler_location_data;

/**
    Defines the maximum size of a callstack.
 */
static const uint32_t silc_compiler_callstack_max = 30;

/**
 * static variable to control initialize status of adapter
 */
static int silc_compiler_initialize = 1;

/**
    Hash table for mapping location id to the location's callstack.
 */
SILC_Hashtab* silc_compiler_location_table = NULL;



/* **************************************************************************************
 * declarations from silc_thread.h
 ***************************************************************************************/

extern SILC_Thread_LocationData*
SILC_Thread_GetLocationData();

extern uint64_t
SILC_Thread_GetLocationId( SILC_Thread_LocationData* locationData );

/* **************************************************************************************
 * location table access
 ***************************************************************************************/

inline uint64_t
silc_compiler_get_location_id()
{
    SILC_Thread_LocationData* data = SILC_Thread_GetLocationData();
    SILC_ASSERT( data != NULL );
    return SILC_Thread_GetLocationId( data );
}

inline silc_compiler_location_data*
silc_compiler_get_location_data()
{
    uint64_t            location_id = silc_compiler_get_location_id();
    SILC_Hashtab_Entry* entry       = SILC_Hashtab_Find( silc_compiler_location_table,
                                                         &location_id,
                                                         NULL );
    if ( !entry )
    {
        return NULL;
    }
    else
    {
        return ( silc_compiler_location_data* )entry->value;
    }
}

/* **************************************************************************************
 * Initialization / Finalization
 ***************************************************************************************/

/* Creates the callstack array for a new thread. */
silc_compiler_location_data*
silc_compiler_create_location_data( uint64_t id )
{
    /* Create location struct */
    silc_compiler_location_data* data = NULL;
    data = ( silc_compiler_location_data* )malloc( sizeof( silc_compiler_location_data ) );

    /* Allocate memory for region handle stack */
    data->callstack_base = ( SILC_RegionHandle* )
                           malloc( silc_compiler_callstack_max *
                                   sizeof( SILC_RegionHandle ) );
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
silc_compiler_delete_location_entry( SILC_Hashtab_Entry* entry )
{
    SILC_ASSERT( entry );
    silc_compiler_location_data* data =  ( silc_compiler_location_data* )entry->value;
    free( data->callstack_base );
    free( data );
}

/* Initialize the location table */
void
silc_compiler_init_location_table()
{
    silc_compiler_location_table = SILC_Hashtab_CreateSize( 10, &SILC_Hashtab_HashInt64,
                                                            &SILC_Hashtab_CompareInt64 );
}

/* Finalize the location table */
void
silc_compiler_final_location_table()
{
    SILC_Hashtab_Foreach( silc_compiler_location_table,
                          &silc_compiler_delete_location_entry );
    SILC_Hashtab_Free( silc_compiler_location_table );
    silc_compiler_location_table = NULL;
}

/* Location initialization */
SILC_Error_Code
silc_compiler_init_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " PGI compiler adapter init loacation!" );

    uint64_t                     location_id = silc_compiler_get_location_id();
    silc_compiler_location_data* data        = silc_compiler_create_location_data( location_id );

    SILC_LockRegionDefinition();
    SILC_Hashtab_Insert( silc_compiler_location_table, &data->location_id,
                         data, NULL );
    SILC_UnlockRegionDefinition();

    return SILC_SUCCESS;
}

/* Location finalization */
void
silc_compiler_final_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter final loacation!" );
}

/* Adapter initialization */
SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " inititialize PGI compiler adapter!" );

        /* Initialize location table */
        silc_compiler_init_location_table();

        /* Initialize file table */
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

        /* Finalize location table */
        silc_compiler_final_location_table();

        silc_compiler_initialize = 1;
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize PGI compiler adapter!" );
    }
}

/**
   Registers configuration variables for the compiler adapters. Currently no
   configuration variables exist for PGI compiler adapters.
 */
SILC_Error_Code
silc_compiler_register()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " register PGI compiler adapter!" );

    return SILC_SUCCESS;
}

/**
   Called on dereigstration of the compiler adapter. Currently, no action is performed
   on deregistration.
 */
void
silc_compiler_deregister()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " PGI compiler adapter deregister!n" );
}

/* Implementation of the compiler adapter initialization/finalization struct */
const SILC_Adapter SILC_Compiler_Adapter =
{
    SILC_ADAPTER_COMPILER,
    "COMPILER",
    &silc_compiler_register,
    &silc_compiler_init_adapter,
    &silc_compiler_init_location,
    &silc_compiler_final_location,
    &silc_compiler_finalize,
    &silc_compiler_deregister
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
__rouexit()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
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
    if ( silc_compiler_initialize )
    {
        SILC_InitMeasurement();
    }
    silc_compiler_location_data* location_data = silc_compiler_get_location_data();
    if ( location_data == NULL )
    {
        silc_compiler_init_location();
        location_data = silc_compiler_get_location_data();
    }

    /* Register new regions */
    if ( !p->isseen )
    {
        /* get file id beloning to file name */
        p->file_handle = ( double )silc_compiler_get_file( p->file_name ) + ( double )0.1;
        SILC_LockRegionDefinition();
        if ( !p->isseen )
        {
            p->region_handle = ( double )0.1 + ( double )
                               SILC_DefineRegion( p->region_name,
                                                  p->file_handle,
                                                  p->lineno,
                                                  SILC_INVALID_LINE_NO,
                                                  SILC_ADAPTER_COMPILER,
                                                  SILC_REGION_FUNCTION
                                                  );
        }
        p->isseen = 1;
        SILC_UnlockRegionDefinition();
    }

    /* Check callstack */
    location_data->callstack_count++;
    if ( location_data->callstack_count < silc_compiler_callstack_max )
    {
        /* Update callstack */
        *location_data->callstack_top = p->region_handle;
        location_data->callstack_top++;

        /* Enter event */
        if ( p->region_handle != SILC_INVALID_REGION )
        {
            SILC_EnterRegion( p->region_handle );
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
    silc_compiler_location_data* location_data = silc_compiler_get_location_data();
    if ( location_data == NULL )
    {
        return;
    }
    if ( location_data->callstack_count < silc_compiler_callstack_max )
    {
        location_data->callstack_top--;
        /* Exit event */
        SILC_ExitRegion( *location_data->callstack_top );
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
