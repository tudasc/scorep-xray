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
#include <inttypes.h>


/**
 * @file        SILC_Config.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief Runtime configuration subsystem.
 *
 */


#include <SILC_Debug.h>
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
    const char*          nameSpace,
    SILC_ConfigVariable* variables
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                       "Register new variables in name space %s",
                       nameSpace ? nameSpace : "" );

    while ( variables->name )
    {
        /* "SILC" (+ "_" + namespace)? + "_" + name + 1 */
        char environment_variable_name[ 7 + 2 * 32 ];
        bool successfully_parsed;

        SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                           "Variable:      %s%s%s",
                           nameSpace ? nameSpace : "",
                           nameSpace ? "/" : "",
                           variables->name );
        SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                           "  Type:        %s",
                           silc_config_type_to_string( variables->type ) );
        SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                           "  Default:     %s", variables->defaultValue );
        SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                           "  Description: %s", variables->shortHelp );

        /* set the variable to its default value */
        successfully_parsed = parse_value( variables->defaultValue,
                                           variables->type,
                                           variables->variableReference,
                                           variables->variableContext );

        if ( !successfully_parsed )
        {
            fprintf( stderr, "Can't set variable to default value\n" );
            /* should this be an error? */
        }

        /* generate environment variable name */
        sprintf( environment_variable_name, "SILC%s%.32s_%.32s",
                 nameSpace ? "_" : "",
                 nameSpace ? nameSpace : "",
                 variables->name );

        SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                           "  Environment variable name: %s",
                           environment_variable_name );

        const char* environment_variable_value =
            getenv( environment_variable_name );
        if ( environment_variable_value )
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                               "    Value: %s", environment_variable_value );

            /* set the variable to the value of the environment variable */
            successfully_parsed = parse_value( environment_variable_value,
                                               variables->type,
                                               variables->variableReference,
                                               variables->variableContext );

            if ( !successfully_parsed )
            {
                fprintf( stderr, "Can't set variable to default value\n" );
                /* should this be an error? */
            }
        }
        else
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                               "    Variable is unset" );
        }

        SILC_DEBUG_ONLY(
            dump_value( "  Final value: ",
                        variables->type,
                        variables->variableReference,
                        variables->variableContext );
            )

        variables++;
    }

    return SILC_SUCCESS;
}


static inline bool
parse_bool( const char* value,
            bool*       boolReference );

static inline bool
parse_number( const char* value,
              uint64_t*   numberReference );

static inline bool
parse_set( const char* value,
           char***     stringListReference,
           char**      acceptedValues );

static inline bool
parse_bitset( const char*               value,
              uint64_t*                 bitsetReference,
              SILC_ConfigType_SetEntry* acceptedValues );

static inline bool
parse_value( const char*     value,
             SILC_ConfigType type,
             void*           variableReference,
             void*           variableContext )
{
    switch ( type )
    {
        case SILC_CONFIG_TYPE_BOOL:
            return parse_bool( value, variableReference );

        case SILC_CONFIG_TYPE_NUMBER:
            return parse_number( value, variableReference );

        case SILC_CONFIG_TYPE_SET:
            return parse_set( value, variableReference, variableContext );

        case SILC_CONFIG_TYPE_BITSET:
            return parse_bitset( value, variableReference, variableContext );

        case SILC_CONFIG_TYPE_PATH:
        case SILC_CONFIG_TYPE_STRING:
        case SILC_CONFIG_TYPE_SIZE:

        case SILC_INVALID_CONFIG_TYPE:
        default:
            return false;
    }
}


static inline bool
parse_bool( const char* value,
            bool*       boolReference )
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


static inline bool
parse_number( const char* value,
              uint64_t*   numberReference )
{
    char* ptr;
    int   number_of_consumed_characters;
    int   number_of_recognized_itmes = sscanf( value,
                                               "%" SCNu64 "%n",
                                               numberReference,
                                               &number_of_consumed_characters );

    /* does sscanf has recognized a number? */
    if ( 1 != number_of_recognized_itmes )
    {
        SILC_ERROR( SILC_ERROR_PARSE_INVALID_VALUE,
                    "Can't parse number in config variable: %s",
                    value );
        return false;
    }

    /* does sscanf has consumed at least one digit and also
       the complete string
       Q: what about whitespace after the number? */
    if ( number_of_consumed_characters == 0 ||
         value[ number_of_consumed_characters ] != '\0' )
    {
        SILC_ERROR( SILC_ERROR_PARSE_INVALID_VALUE,
                    "Can't parse number in config variable: %s",
                    value );
        return false;
    }

    /* pass */
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
    char*  value_copy  = ( char* )alloc_result + ( string_list_alloc + 1 ) * sizeof( char* );
    strcpy( value_copy, value );

    size_t string_list_len = 0;
    char*  entry;
    bool   success          = true;
    char*  value_for_strtok = value_copy;
    while ( ( entry = trim_string( strtok( value_for_strtok, " ,:;" ) ) ) )
    {
        /* all but the first call to strtok should be NULL */
        value_for_strtok = NULL;

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
        char** acceptedValue;
        for ( acceptedValue = acceptedValues;
              acceptedValues && *acceptedValue;
              acceptedValue++ )
        {
            if ( 0 == strcmp( entry, *acceptedValue ) )
            {
                /* found entry in accepted values list */
                break;
            }
        }
        if ( acceptedValues && !*acceptedValue )
        {
            fprintf( stderr, " value '%s' not in accepted set\n", entry );
            continue;
        }

        /* not a duplicate and its also an accepted value => add it to the list */
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
    char* value_copy = malloc( strlen( value ) + 1 );
    if ( !value_copy )
    {
        SILC_ERROR_POSIX();
        return false;
    }
    strcpy( value_copy, value );

    *bitsetReference = 0;

    char* entry;
    bool  success          = true;
    char* value_for_strtok = value_copy;
    while ( ( entry = trim_string( strtok( value_for_strtok, " ,:;" ) ) ) )
    {
        /* all but the first call to strtok should be NULL */
        value_for_strtok = NULL;

        /* check if entry is in acceptedValues */
        SILC_ConfigType_SetEntry* acceptedValue;
        for ( acceptedValue = acceptedValues;
              acceptedValue->name;
              acceptedValue++ )
        {
            if ( 0 == strcmp( entry, acceptedValue->name ) )
            {
                /* found entry in accepted values list
                   add its value to the set */
                *bitsetReference |= acceptedValue->value;
                break;
            }
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


static inline void
dump_set( const char* prefix,
          char**      stringList )
{
    const char* prefix_printed = prefix;
    const char* empty_set      = "<empty set>";

    SILC_DEBUG_PREFIX( SILC_DEBUG_CONFIG );

    while ( *stringList )
    {
        SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_CONFIG,
                               "%s%s", prefix, *stringList );
        stringList++;
        prefix         = ", ";
        prefix_printed = "";
        empty_set      = "";
    }

    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_CONFIG,
                           "%s%s\n", prefix_printed, empty_set );
}

static inline void
dump_bitset( const char*               prefix,
             uint64_t                  bitmask,
             SILC_ConfigType_SetEntry* acceptedValues )
{
    const char* prefix_printed = prefix;
    const char* empty_set      = "<empty set>";

    SILC_DEBUG_PREFIX( SILC_DEBUG_CONFIG );

    while ( acceptedValues->name )
    {
        if ( ( bitmask & acceptedValues->value ) == acceptedValues->value )
        {
            SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_CONFIG,
                                   "%s%s", prefix, acceptedValues->name );
            bitmask       &= ~acceptedValues->value;
            prefix         = ", ";
            prefix_printed = "";
            empty_set      = "";
        }
        acceptedValues++;
    }

    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_CONFIG,
                           "%s%s\n", prefix_printed, empty_set );
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
            SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                               "%s%s", prefix,
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
            SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                               "%stype not implemented", prefix );
            break;

        case SILC_INVALID_CONFIG_TYPE:
        default:
            SILC_DEBUG_PRINTF( SILC_DEBUG_CONFIG,
                               "%sinvalid type", prefix );
            break;
    }
}
