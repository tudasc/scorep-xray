/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @status      alpha
 * @file        scorep_parameter_registration.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Handling of string parameters
 *
 *
 */

#include <config.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Types.h>
#include <SCOREP_DefinitionLocking.h>

#include "scorep_definitions.h"

/**
   Hashtable which contain all parameter string values. It is used to find a previously
   assigned handle to the same string. This is essential to the parameter string handling
   because it assumes that different strings means different handles and vice versa.
 */
static SCOREP_Hashtab* parameter_table = NULL;

/* Initialize parameter table */
void
scorep_parameter_table_initialize()
{
    parameter_table = SCOREP_Hashtab_CreateSize( 256, &SCOREP_Hashtab_HashString,
                                                 &SCOREP_Hashtab_CompareStrings );
}

/* Deletes a parameter table entry */
void
scorep_delete_parameter_table_entry( SCOREP_Hashtab_Entry* entry )
{
    SCOREP_ASSERT( entry );

    free( ( SCOREP_StringHandle* )entry->value );
    free( ( char* )entry->key );
}

/* Delete parameter table */
void
scorep_parameter_table_finalize()
{
    SCOREP_Hashtab_Foreach( parameter_table, &scorep_delete_parameter_table_entry );
    SCOREP_Hashtab_Free( parameter_table );
    parameter_table = NULL;
}

/* Finds a string in the hashtable, or registers it there */
SCOREP_StringHandle
scorep_get_parameter_string_handle( const char* value )
{
    size_t                index;
    SCOREP_Hashtab_Entry* entry = NULL;

    if ( value == NULL )
    {
        return SCOREP_INVALID_STRING;
    }

    SCOREP_LockParameterDefinition();

    entry = SCOREP_Hashtab_Find( parameter_table, value, &index );

    /* If not found, register string */
    if ( !entry )
    {
        /* Reserve own storage for string */
        char* string_value = ( char* )malloc( ( strlen( value ) + 1 ) * sizeof( char ) );
        strcpy( string_value, value );

        /* Register new string to measurement system */
        SCOREP_StringHandle* handle = malloc( sizeof( SCOREP_StringHandle ) );
        *handle = SCOREP_DefineString( string_value );

        /* Store handle in hashtable */
        SCOREP_Hashtab_Insert( parameter_table, ( void* )string_value, handle, &index );

        SCOREP_UnlockParameterDefinition();
        return *handle;
    }

    SCOREP_UnlockParameterDefinition();
    return *( SCOREP_StringHandle* )entry->value;
}
