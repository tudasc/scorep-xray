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

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>


/**
 * @file        SCOREP_Config.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief Runtime configuration subsystem.
 *
 */


#include <scorep_utility/SCOREP_Debug.h>
#include <SCOREP_Config.h>

#include <scorep_utility/SCOREP_Utils.h>

#include "scorep_types.h"


static inline bool
parse_value
(
    const char*       value,
    SCOREP_ConfigType type,
    void*             variableReference,
    void*             variableContext
);


static inline void
dump_value( const char*       prefix,
            SCOREP_ConfigType type,
            void*             variableReference,
            void*             variableContext );


SCOREP_Error_Code
SCOREP_ConfigRegister
(
    const char*            nameSpace,
    SCOREP_ConfigVariable* variables
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                         "Register new variables in name space %s",
                         nameSpace ? nameSpace : "" );

    while ( variables->name )
    {
        /* "SCOREP" (+ "_" + namespace)? + "_" + name + 1 */
        char environment_variable_name[ 9 + 2 * 32 ];
        bool successfully_parsed;

        /* fail, if the programmer does not use the config system right */
        assert( variables->variableReference );
        assert( variables->defaultValue );
        /* the variableContext is checked in the parse_value function */

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                             "Variable:      %s%s%s",
                             nameSpace ? nameSpace : "",
                             nameSpace ? "/" : "",
                             variables->name );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                             "  Type:        %s",
                             scorep_config_type_to_string( variables->type ) );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                             "  Default:     %s", variables->defaultValue );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                             "  Description: %s", variables->shortHelp );

        /* set the variable to its default value */
        successfully_parsed = parse_value( variables->defaultValue,
                                           variables->type,
                                           variables->variableReference,
                                           variables->variableContext );

        if ( !successfully_parsed )
        {
            SCOREP_ERROR( SCOREP_ERROR_EINVAL,
                          "Can't set variable '%s%s%s' to "
                          "default value `%s'",
                          nameSpace ? "_" : "",
                          nameSpace ? nameSpace : "",
                          variables->name,
                          variables->defaultValue );
            /* This is actually not user input, but a programming error */
            /* therefore we can assert here */
            assert( successfully_parsed );
        }

        /* generate environment variable name */
        sprintf( environment_variable_name, "SCOREP%s%.32s_%.32s",
                 nameSpace ? "_" : "",
                 nameSpace ? nameSpace : "",
                 variables->name );

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                             "  Environment variable name: %s",
                             environment_variable_name );

        const char* environment_variable_value =
            getenv( environment_variable_name );
        if ( environment_variable_value )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                                 "    Value: %s", environment_variable_value );

            /* set the variable to the value of the environment variable */
            successfully_parsed = parse_value( environment_variable_value,
                                               variables->type,
                                               variables->variableReference,
                                               variables->variableContext );

            if ( !successfully_parsed )
            {
                return SCOREP_ERROR( SCOREP_ERROR_EINVAL,
                                     "Can't set variable '%s%s%s' to "
                                     "value `%s' from environment variable",
                                     nameSpace ? "_" : "",
                                     nameSpace ? nameSpace : "",
                                     variables->name,
                                     environment_variable_value );
            }
        }
        else
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                                 "    Variable is unset" );
        }

        SCOREP_DEBUG_ONLY(
            dump_value( "  Final value: ",
                        variables->type,
                        variables->variableReference,
                        variables->variableContext ) );

        variables++;
    }

    return SCOREP_SUCCESS;
}


static inline bool
parse_bool( const char* value,
            bool*       boolReference );

static inline bool
parse_number( const char* value,
              uint64_t*   numberReference );

static inline bool
parse_size( const char* value,
            uint64_t*   sizeNumberReference );

static inline bool
parse_string( const char* value,
              char**      stringReference );

static inline bool
parse_set( const char* value,
           char***     stringListReference,
           char**      acceptedValues );

static inline bool
parse_bitset( const char*                 value,
              uint64_t*                   bitsetReference,
              SCOREP_ConfigType_SetEntry* acceptedValues );

static inline bool
parse_value( const char*       value,
             SCOREP_ConfigType type,
             void*             variableReference,
             void*             variableContext )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_BOOL:
            /* assert( !variableContext ); */
            return parse_bool( value, variableReference );

        case SCOREP_CONFIG_TYPE_NUMBER:
            /* assert( !variableContext ); */
            return parse_number( value, variableReference );

        case SCOREP_CONFIG_TYPE_SIZE:
            /* assert( !variableContext ); */
            return parse_size( value, variableReference );

        case SCOREP_CONFIG_TYPE_SET:
            assert( variableContext );
            return parse_set( value, variableReference, variableContext );

        case SCOREP_CONFIG_TYPE_BITSET:
            assert( variableContext );
            return parse_bitset( value, variableReference, variableContext );

        case SCOREP_CONFIG_TYPE_STRING:
            /* assert( !variableContext ); */
            return parse_string( value, variableReference );

        case SCOREP_CONFIG_TYPE_PATH:

        case SCOREP_INVALID_CONFIG_TYPE:
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

static int
parse_uint64( const char*        numberString,
              uint64_t* const    numberReference,
              const char** const endPtr )
{
    uint64_t number = 0;

    assert( numberString );
    assert( numberReference );
    assert( endPtr );

    /*
     * Ignore leading whitespace, but also ignore this whether we have consumed
     * real number charachters. That is an whitespace only string is not
     * a valid number
     */
    while ( isspace( *numberString ) )
    {
        numberString++;
    }

    const char* value_iterator = numberString;
    while ( *value_iterator && isdigit( *value_iterator ) )
    {
        uint64_t new_number = 10 * number + ( *value_iterator - '0' );

        /* Check for overflow */
        if ( new_number < number )
        {
            return ERANGE;
        }

        number = new_number;
        value_iterator++;
    }

    /* Have we consumed at least one digit? */
    if ( value_iterator == numberString )
    {
        return EINVAL;
    }

    *endPtr          = value_iterator;
    *numberReference = number;

    return 0;
}

static inline bool
parse_number( const char* value,
              uint64_t*   numberReference )
{
    const char* orig_value = value;
    int         parse_success;

    parse_success = parse_uint64( value, numberReference, &value );
    if ( 0 != parse_success )
    {
        SCOREP_ERROR( parse_success == ERANGE
                      ? SCOREP_ERROR_ERANGE
                      : SCOREP_ERROR_EINVAL,
                      "Can't parse number in config value: `%s'",
                      orig_value );
        return false;
    }

    /* skip whitespace after the number */
    while ( isspace( *value ) )
    {
        value++;
    }

    /* Have we consumed the complete string */
    if ( *value != '\0' )
    {
        SCOREP_ERROR( SCOREP_ERROR_EINVAL,
                      "Unrecognized characters after number `%s'",
                      orig_value );
        return false;
    }

    /* pass */
    return true;
}


static inline bool
parse_size( const char* value,
            uint64_t*   sizeNumberReference )
{
    const char* orig_value = value;
    int         parse_success;

    parse_success = parse_uint64( value, sizeNumberReference, &value );
    if ( 0 != parse_success )
    {
        SCOREP_ERROR( parse_success == ERANGE
                      ? SCOREP_ERROR_ERANGE
                      : SCOREP_ERROR_EINVAL,
                      "Can't parse size in config value: `%s'",
                      value );
        return false;
    }

    /* skip whitespace after the number */
    while ( isspace( *value ) )
    {
        value++;
    }

    /* Check for any known suffixe */
    uint64_t scale_factor    = 1;
    bool     has_byte_suffix = false;
    switch ( toupper( *value ) )
    {
        /* Zetta is 2^70, which is of course too big for uint64_t */
        case 'E':
            scale_factor *= 1024;
        /* fall through */
        case 'P':
            scale_factor *= 1024;
        /* fall through */
        case 'T':
            scale_factor *= 1024;
        /* fall through */
        case 'G':
            scale_factor *= 1024;
        /* fall through */
        case 'M':
            scale_factor *= 1024;
        /* fall through */
        case 'K':
            scale_factor *= 1024;
            value++;
            break;

        case 'B':
            /* We allow the 'b' suffix only once */
            has_byte_suffix = true;
            value++;
        /* fall through */
        case '\0':
            break;

        default:
            SCOREP_ERROR( SCOREP_ERROR_EINVAL,
                          "Invalid scale factor '%s' in config value `%s'",
                          value, orig_value );
            return false;
    }

    /* Skip the optional 'b' suffix, but only once */
    if ( !has_byte_suffix && toupper( *value ) == 'B' )
    {
        value++;
    }

    /* skip whitespace after the suffix */
    while ( isspace( *value ) )
    {
        value++;
    }

    /* Have we consumed the complete string */
    if ( *value != '\0' )
    {
        SCOREP_ERROR( SCOREP_ERROR_EINVAL,
                      "Unrecognized characters `%s' after size in config value `%s'",
                      value, orig_value );
        return false;
    }

    /* check for overflow */
    if ( ( *sizeNumberReference * scale_factor ) < *sizeNumberReference )
    {
        SCOREP_ERROR( SCOREP_ERROR_EOVERFLOW,
                      "Resulting value does not fit into variable: `%s'",
                      value );
        return false;
    }
    *sizeNumberReference *= scale_factor;

    /* pass */
    return true;
}


static inline bool
parse_string( const char* value,
              char**      stringReference )
{
    free( *stringReference );
    *stringReference = SCOREP_CStr_dup( value );
    if ( !*stringReference )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_FAULT, "Can't duplicate string" );
        return false;
    }

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
        SCOREP_ERROR_POSIX();
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
parse_bitset( const char*                 value,
              uint64_t*                   bitsetReference,
              SCOREP_ConfigType_SetEntry* acceptedValues )
{
    char* value_copy = malloc( strlen( value ) + 1 );
    if ( !value_copy )
    {
        SCOREP_ERROR_POSIX();
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
        SCOREP_ConfigType_SetEntry* acceptedValue;
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

    SCOREP_DEBUG_PREFIX( SCOREP_DEBUG_CONFIG );

    while ( *stringList )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CONFIG,
                                 "%s%s", prefix, *stringList );
        stringList++;
        prefix         = ", ";
        prefix_printed = "";
        empty_set      = "";
    }

    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CONFIG,
                             "%s%s\n", prefix_printed, empty_set );
}

static inline void
dump_bitset( const char*                 prefix,
             uint64_t                    bitmask,
             SCOREP_ConfigType_SetEntry* acceptedValues )
{
    const char* prefix_printed = prefix;
    const char* empty_set      = "<empty set>";

    SCOREP_DEBUG_PREFIX( SCOREP_DEBUG_CONFIG );

    while ( acceptedValues->name )
    {
        if ( ( bitmask & acceptedValues->value ) == acceptedValues->value )
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CONFIG,
                                     "%s%s", prefix, acceptedValues->name );
            bitmask       &= ~acceptedValues->value;
            prefix         = ", ";
            prefix_printed = "";
            empty_set      = "";
        }
        acceptedValues++;
    }

    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CONFIG,
                             "%s%s\n", prefix_printed, empty_set );
}

static inline void
dump_value( const char*       prefix,
            SCOREP_ConfigType type,
            void*             variableReference,
            void*             variableContext )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_BOOL:
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                                 "%s%s", prefix,
                                 *( bool* )variableReference ? "true" : "false" );
            break;

        case SCOREP_CONFIG_TYPE_SET:
            dump_set( prefix, *( char*** )variableReference );
            break;

        case SCOREP_CONFIG_TYPE_BITSET:
            dump_bitset( prefix,
                         *( uint64_t* )variableReference,
                         variableContext );
            break;

        case SCOREP_CONFIG_TYPE_NUMBER:
        case SCOREP_CONFIG_TYPE_SIZE:
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG, "%s%" PRIu64,
                                 prefix, *( uint64_t* )variableReference );

            break;

        case SCOREP_CONFIG_TYPE_STRING:
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG, "%s\"%s\"",
                                 prefix, *( const char** )variableReference );

            break;

        case SCOREP_CONFIG_TYPE_PATH:
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                                 "%stype not implemented", prefix );
            break;

        case SCOREP_INVALID_CONFIG_TYPE:
        default:
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                                 "%sinvalid type", prefix );
            break;
    }
}
