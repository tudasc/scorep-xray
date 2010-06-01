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
 * @file        silc_parameter_registration.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Handling of string parameters
 *
 *
 */

#include <SILC_Utils.h>
#include <SILC_Types.h>
#include <SILC_DefinitionLocking.h>

#include "silc_definitions.h"

/**
   Hashtable which contain all parameter string values. It is used to find a previously
   assigned handle to the same string. This is essential to the parameter string handling
   because it assumes that different strings means different handles and vice versa.
 */
static SILC_Hashtab* parameter_table = NULL;

/* Initialize parameter table */
void
silc_parameter_table_initialize()
{
    parameter_table = SILC_Hashtab_CreateSize( 256, &SILC_Hashtab_HashString,
                                               &SILC_Hashtab_CompareStrings );
}

/* Deletes a parameter table entry */
void
silc_delete_parameter_table_entry( SILC_Hashtab_Entry* entry )
{
    SILC_ASSERT( entry );

    free( ( SILC_StringHandle* )entry->value );
    free( ( char* )entry->key );
}

/* Delete parameter table */
void
silc_parameter_table_finalize()
{
    SILC_Hashtab_Foreach( parameter_table, &silc_delete_parameter_table_entry );
    SILC_Hashtab_Free( parameter_table );
    parameter_table = NULL;
}

/* Finds a string in the hashtable, or registers it there */
SILC_StringHandle
silc_get_parameter_string_handle( const char* value )
{
    size_t              index;
    SILC_Hashtab_Entry* entry = NULL;

    if ( value == NULL )
    {
        return SILC_INVALID_STRING;
    }

    SILC_LockParameterDefinition();

    entry = SILC_Hashtab_Find( parameter_table, value, &index );

    /* If not found, register string */
    if ( !entry )
    {
        /* Reserve own storage for string */
        char* string_value = ( char* )malloc( ( strlen( value ) + 1 ) * sizeof( char ) );
        strcpy( string_value, value );

        /* Register new string to measurement system */
        SILC_StringHandle* handle = malloc( sizeof( SILC_StringHandle ) );
        *handle = SILC_DefineString( string_value );

        /* Store handle in hashtable */
        SILC_Hashtab_Insert( parameter_table, ( void* )string_value, handle, &index );

        SILC_UnlockParameterDefinition();
        return *handle;
    }

    SILC_UnlockParameterDefinition();
    return *( SILC_StringHandle* )entry->value;
}
