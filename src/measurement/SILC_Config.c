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


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


/**
 * @file        SILC_Config.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief Runtime configuration subsystem.
 *
 */


#include <SILC_Config.h>


#include "silc_types.h"


static inline bool
parse_value
(
    const char*     value,
    SILC_ConfigType type,
    void*           variableReference,
    void*           variableContext
);


static inline void
dump_value( const char*     prefix,
            SILC_ConfigType type,
            void*           variableReference,
            void*           variableContext );


SILC_Error_Code
SILC_ConfigRegister
(
    SILC_ConfigVariable* variables,
    uint32_t             numberOfVariables
)
{
    fprintf( stderr, "%s: Register %u new variables:\n",
             __func__, numberOfVariables );
    for ( uint32_t i = 0; i < numberOfVariables; ++i )
    {
        /* "SILC" (+ "_" + namespace)? + "_" + name + 1 */
        char environment_variable_name[ 7 + 2 * 32 ];
        bool successfully_parsed;

        fprintf( stderr, "Variable:      %s/%s\n",
                 variables[ i ].nameSpace, variables[ i ].name );
        fprintf( stderr, "  Type:        %s\n",
                 silc_config_type_to_string( variables[ i ].type ) );
        fprintf( stderr, "  Default:     %s\n", variables[ i ].defaultValue );
        fprintf( stderr, "  Description: %s\n", variables[ i ].shortHelp );

        /* set the variable to its default value */
        successfully_parsed = parse_value( variables[ i ].defaultValue,
                                           variables[ i ].type,
                                           variables[ i ].variableReference,
                                           variables[ i ].variableContext );

        if ( !successfully_parsed )
        {
            fprintf( stderr, "  Can't set variable to default value." );
        }

        sprintf( environment_variable_name, "SILC%s%.32s_%.32s",
                 variables[ i ].nameSpace ? "_" : "",
                 variables[ i ].nameSpace ? variables[ i ].nameSpace : "",
                 variables[ i ].name );

        fprintf( stderr, "  Environmental Name: %s\n",
                 environment_variable_name );

        const char* environment_variable_value =
            getenv( environment_variable_name );
        if ( environment_variable_value )
        {
            fprintf( stderr, "    Value: %s\n", environment_variable_value );

            /* set the variable to the value of the environment variable */
            successfully_parsed = parse_value( environment_variable_value,
                                               variables[ i ].type,
                                               variables[ i ].variableReference,
                                               variables[ i ].variableContext );

            if ( !successfully_parsed )
            {
                fprintf( stderr, "  Can't set variable to value of environment variable." );
            }
        }
        else
        {
            fprintf( stderr, "    Variable is unset\n" );
        }

        dump_value( "  Final value: ",
                    variables[ i ].type,
                    variables[ i ].variableReference,
                    variables[ i ].variableContext );
    }

    return SILC_SUCCESS;
}


static inline bool
parse_bool
(
    const char* value,
    bool*       boolReference
);

static inline bool
parse_set( const char* value,
           char***     stringListReference,
           char**      acceptedValues );

static inline bool
parse_bitset( const char*               value,
              uint64_t*                 bitsetReference,
              SILC_ConfigType_SetEntry* acceptedValues );

static inline bool
parse_value
(
    const char*     value,
    SILC_ConfigType type,
    void*           variableReference,
    void*           variableContext
)
{
    switch ( type )
    {
        case SILC_CONFIG_TYPE_BOOL:
            return parse_bool( value, variableReference );

        case SILC_CONFIG_TYPE_SET:
            return parse_set( value, variableReference, variableContext );

        case SILC_CONFIG_TYPE_BITSET:
            return parse_bitset( value, variableReference, variableContext );

        case SILC_CONFIG_TYPE_PATH:
        case SILC_CONFIG_TYPE_STRING:
        case SILC_CONFIG_TYPE_NUMBER:
        case SILC_CONFIG_TYPE_SIZE:

        case SILC_INVALID_CONFIG_TYPE:
        default:
            return false;
    }
}


static inline bool
parse_bool
(
    const char* value,
    bool*       boolReference
)
{
    /* try symbolic constants */
    if ( 0 == strcmp( value, "true" ) ||
         0 == strcmp( value, "yes" ) ||
         0 == strcmp( value, "on" ) )
    {
        *boolReference = true;
        return true;
    }

    /* try to parse a number */
    char* ptr;
    long  number = strtol( value, &ptr, 10 );

    /* its only a valid number if we have consumed the whole string and
       the string was not empty */
    if ( ptr != value && *ptr == '\0' )
    {
        /* any non-zero value is true */
        *boolReference = !!number;
        return true;
    }

    /* any remaining value is false */
    *boolReference = false;
    return true;
}

/**
 * @brief remove leading and trailing whitespaces
 *
 * @note alters the input string
 *
 * @internal
 */
static char*
trim_string( char* str )
{
    if ( !str )
    {
        return str;
    }

    /* remove leading spaces */
    while ( isspace( *str ) )
    {
        str++;
    }

    /* remove trailing spaces only if strlen(str) > 0 */
    if ( *str )
    {
        char* end = str + strlen( str ) - 1;
        while ( isspace( *end ) )
        {
            *end-- = '\0';
        }
    }

    return str;
}

static inline bool
parse_set( const char* value,
           char***     stringListReference,
           char**      acceptedValues )
{
    /* count number of separator charachters and use it as an upper bound
       for the number of entries in the string list */
    size_t      string_list_alloc = 1;
    const char* value_position    = value;
    while ( *value_position )
    {
        if ( strchr( " ,:;", *value_position ) )
        {
            string_list_alloc++;
        }
        value_position++;
    }

    /* allocate memory for array and string, including terminating NULL */
    void* alloc_result = realloc( *stringListReference,
                                  ( string_list_alloc + 1 ) * sizeof( char* ) +
                                  ( strlen( value ) + 1 ) * sizeof( char ) );
    if ( !alloc_result )
    {
        SILC_ERROR_POSIX();
        return false;
    }
    *stringListReference = NULL;
    char** string_list = alloc_result;
    char*  value_copy  = alloc_result + ( string_list_alloc + 1 ) * sizeof( char* );
    strcpy( value_copy, value );

    size_t string_list_len = 0;
    char*  saveptr;
    bool   success = true;
    for ( char* entry = strtok_r( value_copy, " ,:;", &saveptr );
          trim_string( entry );
          entry = strtok_r( NULL, " ,:;", &saveptr ) )
    {
        if ( string_list_len >= string_list_alloc )
        {
            /* something strange has happened, we have more entries as in the
               first run */
            success = false;
            goto out;
        }

        /* check for duplicates */
        size_t i;
        for ( i = 0; i < string_list_len; i++ )
        {
            if ( 0 == strcmp( entry, string_list[ i ] ) )
            {
                break;
            }
        }
        if ( i < string_list_len )
        {
            continue;
        }

        /* check if entry is in acceptedValues */
        char** acceptedValue = acceptedValues;
        while ( acceptedValues && *acceptedValue )
        {
            if ( 0 == strcmp( entry, *acceptedValue ) )
            {
                /* found entry in accepted values list */
                break;
            }
            acceptedValue++;
        }
        if ( acceptedValues && !*acceptedValue )
        {
            fprintf( stderr, " value '%s' not in accepted set\n", entry );
            continue;
        }

        /* not an duplicate and also an accepted value => add it to the list */
        string_list[ string_list_len++ ] = entry;
    }

out:
    /* NULL terminate list */
    string_list[ string_list_len ] = NULL;

    *stringListReference = string_list;

    return success;
}


static inline bool
parse_bitset( const char*               value,
              uint64_t*                 bitsetReference,
              SILC_ConfigType_SetEntry* acceptedValues )
{
    char* value_copy = strdup( value );
    if ( !value_copy )
    {
        SILC_ERROR_POSIX();
        return false;
    }

    *bitsetReference = 0;

    char* saveptr;
    bool  success = true;
    for ( char* entry = strtok_r( value_copy, " ,:;", &saveptr );
          trim_string( entry );
          entry = strtok_r( NULL, " ,:;", &saveptr ) )
    {
        /* check if entry is in acceptedValues */
        SILC_ConfigType_SetEntry* acceptedValue = acceptedValues;
        while ( acceptedValue->name )
        {
            if ( 0 == strcmp( entry, acceptedValue->name ) )
            {
                /* found entry in accepted values list
                   add its value to the set */
                *bitsetReference |= acceptedValue->value;
                break;
            }
            acceptedValue++;
        }
        if ( !acceptedValue )
        {
            fprintf( stderr, " value '%s' not in accepted set\n", entry );
            continue;
        }
    }

    free( value_copy );

    return success;
}


static inline bool
dump_set( const char* prefix,
          char**      stringList )
{
    const char* prefix_printed = prefix;
    const char* empty_set      = "<empty set>";
    while ( *stringList )
    {
        fprintf( stderr, "%s%s", prefix, *stringList );
        stringList++;
        prefix         = ", ";
        prefix_printed = "";
        empty_set      = "";
    }

    fprintf( stderr, "%s%s\n", prefix_printed, empty_set );
}

static inline bool
dump_bitset( const char*               prefix,
             uint64_t                  bitmask,
             SILC_ConfigType_SetEntry* acceptedValues )
{
    const char* prefix_printed = prefix;
    const char* empty_set      = "<empty set>";
    while ( acceptedValues->name )
    {
        if ( ( bitmask & acceptedValues->value ) == acceptedValues->value )
        {
            fprintf( stderr, "%s%s", prefix, acceptedValues->name );
            bitmask       &= ~acceptedValues->value;
            prefix         = ", ";
            prefix_printed = "";
            empty_set      = "";
        }
        acceptedValues++;
    }

    fprintf( stderr, "%s%s\n", prefix_printed, empty_set );
}

static inline void
dump_value( const char*     prefix,
            SILC_ConfigType type,
            void*           variableReference,
            void*           variableContext )
{
    switch ( type )
    {
        case SILC_CONFIG_TYPE_BOOL:
            fprintf( stderr, "%s%s\n", prefix,
                     *( bool* )variableReference ? "true" : "false" );
            break;

        case SILC_CONFIG_TYPE_SET:
            dump_set( prefix, *( char*** )variableReference );
            break;

        case SILC_CONFIG_TYPE_BITSET:
            dump_bitset( prefix,
                         *( uint64_t* )variableReference,
                         variableContext );
            break;

        case SILC_CONFIG_TYPE_PATH:
        case SILC_CONFIG_TYPE_STRING:
        case SILC_CONFIG_TYPE_NUMBER:
        case SILC_CONFIG_TYPE_SIZE:
            fprintf( stderr, "%stype not implemented\n", prefix );
            break;

        case SILC_INVALID_CONFIG_TYPE:
        default:
            fprintf( stderr, "%sinvalid type\n", prefix );
            break;
    }
}
