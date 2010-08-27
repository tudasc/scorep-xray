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
 * @file       SILC_compiler_intel.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-fcollect' flag of the intel
 * compiler.
 */

#include <config.h>
#include <stdio.h>
#include <unistd.h>

#include <SILC_Types.h>
#include <silc_utility/SILC_Utils.h>
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

/**
 * Hashtable to map region name to region id.
 */
static SILC_Hashtab* silc_compiler_name_table = NULL;

/* ***************************************************************************************
   Hashtable functions to map names to id.
*****************************************************************************************/

/**
 * Initialize name table
 */
static void
silc_compiler_init_name_table()
{
    silc_compiler_name_table = SILC_Hashtab_CreateSize( 1024, &SILC_Hashtab_HashString,
                                                        &SILC_Hashtab_CompareStrings );
}

/**
   Deletes one name table entry.
   @param entry Pointer to the entry to be deleted.
 */
static void
silc_compiler_delete_name_entry( SILC_Hashtab_Entry* entry )
{
    SILC_ASSERT( entry );

    free( ( int32_t* )entry->value );
    free( ( char* )entry->key );
}

/* Finalize the name table */
void
silc_compiler_final_name_table()
{
    SILC_Hashtab_Foreach( silc_compiler_name_table, &silc_compiler_delete_name_entry );
    SILC_Hashtab_Free( silc_compiler_name_table );
    silc_compiler_name_table = NULL;
}

/* Returns the id for a given region name. */
int32_t
silc_compiler_get_id_from_name( const char* name )
{
    SILC_Hashtab_Entry* entry = NULL;
    const char*         region_name;

    /* Check input */
    if ( name == NULL )
    {
        return 0;
    }

    /* Tne intel compiler prepends the filename to the function name.
       -> Need to remove the file name. */
    region_name = name;
    while ( *name != '\0' )
    {
        if ( *name == ':' )
        {
            region_name = name + 1;
            break;
        }
        name++;
    }

    /* Look up in hash table */
    entry = SILC_Hashtab_Find( silc_compiler_name_table, region_name, NULL );

    /* If not found, unknown region */
    if ( !entry )
    {
        return 0;
    }

    return *( int32_t* )entry->value;
}

/**
 * Adds an entry to the name table
 */
void
silc_compiler_name_add( const char* name,
                        int32_t     id )
{
    /* Reserve own storage for region name */
    char* region_name = ( char* )malloc( ( strlen( name ) + 1 ) * sizeof( char ) );
    strcpy( region_name, name );

    /* Reserve storage for id */
    int32_t* id_copy = malloc( sizeof( int32_t ) );
    *id_copy = id;

    /* Store handle in hashtable */
    SILC_Hashtab_Insert( silc_compiler_name_table, ( void* )region_name,
                         id_copy, NULL );
}

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
    silc_compiler_hash_node* hash_node = NULL;

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
        *id = silc_compiler_get_id_from_name( str );

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
    else if ( *id != SILC_COMPILER_FILTER_ID )
    {
        hash_node = silc_compiler_hash_get( *id );
    }

    /* Enter event */
    if ( hash_node != NULL )
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
        silc_compiler_init_name_table();

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
        silc_compiler_final_name_table();

        /* Set initilaization flag */
        silc_compiler_initialize = 1;
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize intel compiler adapter." );
    }
}
