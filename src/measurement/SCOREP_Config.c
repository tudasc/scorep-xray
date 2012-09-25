/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file        SCOREP_Config.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief Runtime configuration subsystem.
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <inttypes.h>

#include <SCOREP_Config.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include <SCOREP_Hashtab.h>

#include "scorep_types.h"

#define SCOREP_DEBUG_MODULE_NAME CONFIG

static size_t
hash_variable( const void* key );

static int32_t
compare_variable( const void* key,
                  const void* item_key );

#define ENV_NAME_LEN_MAX 80
struct scorep_config_variable
{
    SCOREP_ConfigVariable          data;
    char                           env_var_name[ ENV_NAME_LEN_MAX ];
    struct scorep_config_variable* next;
};

static size_t
hash_name_space( const void* key );

static int32_t
compare_name_space( const void* key,
                    const void* item_key );

struct scorep_config_name_space
{
    const char*                      name;
    size_t                           name_len;
    SCOREP_Hashtab*                  variables;
    struct scorep_config_variable*   variables_head;
    struct scorep_config_variable**  variables_tail;
    struct scorep_config_name_space* next;
};

static SCOREP_Hashtab*                   name_spaces;
static struct scorep_config_name_space*  name_spaces_head;
static struct scorep_config_name_space** name_spaces_tail = &name_spaces_head;

static void
check_name( const char* name,
            size_t      name_len,
            bool        isNameSpace );

static void
string_to_lower( char* str );

static void
string_to_upper( char* str );

static bool
parse_value( const char*       value,
             SCOREP_ConfigType type,
             void*             variableReference,
             void*             variableContext );


static void
dump_value( FILE*             out,
            const char*       name,
            SCOREP_ConfigType type,
            void*             variableReference,
            void*             variableContext );


SCOREP_ErrorCode
SCOREP_ConfigInit( void )
{
    /* prevent calling me twice */
    UTILS_ASSERT( !name_spaces );

    name_spaces = SCOREP_Hashtab_CreateSize( 32,
                                             hash_name_space,
                                             compare_name_space );

    if ( !name_spaces )
    {
        return UTILS_ERROR( SCOREP_ERROR_MEM_FAULT,
                            "Can't allocate hash table for config susbsystem" );
    }

    return SCOREP_SUCCESS;
}

void
SCOREP_ConfigFini( void )
{
    UTILS_ASSERT( name_spaces );

    struct scorep_config_name_space* name_space = name_spaces_head;
    while ( name_space )
    {
        struct scorep_config_name_space* next_name_space = name_space->next;
        struct scorep_config_variable*   variable        = name_space->variables_head;
        while ( variable )
        {
            struct scorep_config_variable* next_variable = variable->next;
            free( variable );
            variable = next_variable;
        }

        SCOREP_Hashtab_Free( name_space->variables );
        free( name_space );
        name_space = next_name_space;
    }
    SCOREP_Hashtab_Free( name_spaces );
    name_spaces = NULL;
}

static struct scorep_config_name_space*
get_name_space( const char* name, size_t nameLen, bool create )
{
    struct scorep_config_name_space key =
    {
        .name     = name,
        .name_len = nameLen
    };
    struct scorep_config_name_space* name_space;

    size_t                hashHint;
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find( name_spaces,
                                                       &key,
                                                       &hashHint );

    if ( entry )
    {
        return entry->value;
    }

    if ( !create )
    {
        return NULL;
    }

    name_space = calloc( 1, sizeof( *name_space ) + nameLen + 1 );
    UTILS_ASSERT( name_space );

    char* name_buffer = ( char* )name_space + sizeof( *name_space );
    memcpy( name_buffer, name, nameLen + 1 );
    string_to_lower( name_buffer );

    name_space->name     = name_buffer;
    name_space->name_len = nameLen;

    name_space->variables = SCOREP_Hashtab_CreateSize(
        32,
        hash_variable,
        compare_variable );

    name_space->variables_head = NULL;
    name_space->variables_tail = &name_space->variables_head;

    SCOREP_Hashtab_Insert( name_spaces,
                           name_space,
                           name_space,
                           &hashHint );

    /* Maintain registration order */
    name_space->next  = NULL;
    *name_spaces_tail = name_space;
    name_spaces_tail  = &name_space->next;

    return name_space;
}


static struct scorep_config_variable*
get_variable( struct scorep_config_name_space* nameSpace,
              const char*                      name,
              bool                             create )
{
    struct scorep_config_variable key =
    {
        .data.name = name
    };
    struct scorep_config_variable* variable;

    size_t                hashHint;
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find(
        nameSpace->variables,
        &key,
        &hashHint );

    if ( entry )
    {
        return entry->value;
    }

    if ( !create )
    {
        return NULL;
    }

    size_t name_len = strlen( name );
    variable = calloc( 1, sizeof( *variable ) + name_len + 1 );
    UTILS_ASSERT( variable );

    char* name_buffer = ( char* )variable + sizeof( *variable );
    memcpy( name_buffer, name, name_len + 1 );
    string_to_lower( name_buffer );

    variable->data.name = name_buffer;

    /* build env name */
    sprintf( variable->env_var_name,
             "SCOREP_%.32s%s%.32s",
             nameSpace->name,
             nameSpace->name_len ? "_" : "",
             variable->data.name );
    string_to_upper( variable->env_var_name );

    SCOREP_Hashtab_Insert( nameSpace->variables,
                           variable,
                           variable,
                           &hashHint );

    /* Maintain registration order */
    variable->next             = NULL;
    *nameSpace->variables_tail = variable;
    nameSpace->variables_tail  = &variable->next;

    return variable;
}


SCOREP_ErrorCode
SCOREP_ConfigRegister( const char*            nameSpaceName,
                       SCOREP_ConfigVariable* variables )
{
    UTILS_ASSERT( name_spaces );
    UTILS_ASSERT( nameSpaceName );

    size_t name_space_len = strlen( nameSpaceName );
    UTILS_BUG_ON( name_space_len > 32, "Name space is too long." );
    check_name( nameSpaceName, name_space_len, true );

    UTILS_DEBUG( "Register new variables in name space '%s::'",
                 nameSpaceName );

    struct scorep_config_name_space* name_space;
    name_space = get_name_space( nameSpaceName,
                                 name_space_len,
                                 true );

    while ( variables->name )
    {
        bool successfully_parsed;

        /* fail, if the programmer does not use the config system correctly */
        UTILS_BUG_ON( !variables->name, "Missing variable name." );
        UTILS_BUG_ON( !variables->variableReference, "Missing variable reference." );
        UTILS_BUG_ON( !variables->defaultValue, "Missing default value." );
        /* the variableContext is checked in the parse_value function */

        size_t name_len = strlen( variables->name );
        UTILS_BUG_ON( name_len == 1 || name_len > 32, "Variable name too long." );
        check_name( variables->name, name_len, false );

        struct scorep_config_variable* variable;
        variable = get_variable( name_space,
                                 variables->name,
                                 true );

        variable->data.type              = variables->type;
        variable->data.variableReference = variables->variableReference;
        variable->data.variableContext   = variables->variableContext;
        variable->data.defaultValue      = variables->defaultValue;
        variable->data.shortHelp         = variables->shortHelp;
        variable->data.longHelp          = variables->longHelp;

        UTILS_DEBUG( "Variable:      '%s::%s'",
                     nameSpaceName,
                     variable->data.name );
        UTILS_DEBUG( "  Type:        %s",
                     scorep_config_type_to_string( variable->data.type ) );
        UTILS_DEBUG( "  Default:     %s", variable->data.defaultValue );
        UTILS_DEBUG( "  Description: %s", variable->data.shortHelp );

        /* set the variable to its default value */
        successfully_parsed = parse_value( variable->data.defaultValue,
                                           variable->data.type,
                                           variable->data.variableReference,
                                           variable->data.variableContext );

        /* This is actually not user input, but a programming error */
        /* therefore we can bug here */
        UTILS_BUG_ON( !successfully_parsed,
                      "Default value could not be parsed." );

        variables++;
    }

    return SCOREP_SUCCESS;
}

static bool force_conditional_registrations;

SCOREP_ErrorCode
SCOREP_ConfigRegisterCond( const char*            nameSpaceName,
                           SCOREP_ConfigVariable* variables,
                           bool                   isAvailable )
{
    if ( isAvailable || force_conditional_registrations )
    {
        return SCOREP_ConfigRegister( nameSpaceName,
                                      variables );
    }

    return SCOREP_SUCCESS;
}

void
SCOREP_ConfigForceConditionalRegister( void )
{
    force_conditional_registrations = true;
}


SCOREP_ErrorCode
SCOREP_ConfigApplyEnv( void )
{
    UTILS_ASSERT( name_spaces );

    static bool once_run;
    UTILS_BUG_ON( once_run, "SCOREP_ConfigApplyEnv() can only be called once." );
    once_run = true;

    UTILS_DEBUG( "Apply environment to config variables" );

    for ( struct scorep_config_name_space* name_space = name_spaces_head;
          name_space;
          name_space = name_space->next )
    {
        for ( struct scorep_config_variable* variable = name_space->variables_head;
              variable;
              variable = variable->next )
        {
            const char* environment_variable_value =
                getenv( variable->env_var_name );

            UTILS_DEBUG( "Variable:      '%s::%s'",
                         name_space->name,
                         variable->data.name );

            if ( environment_variable_value )
            {
                UTILS_DEBUG( "  Value of environment variable: %s", environment_variable_value );

                /* set the variable to the value of the environment variable */
                bool successfully_parsed;
                successfully_parsed = parse_value( environment_variable_value,
                                                   variable->data.type,
                                                   variable->data.variableReference,
                                                   variable->data.variableContext );

                if ( !successfully_parsed )
                {
                    return UTILS_ERROR( SCOREP_ERROR_EINVAL,
                                        "Can't set variable '%s::%s' to "
                                        "value `%s' from environment variable %s",
                                        name_space->name,
                                        variable->data.name,
                                        environment_variable_value,
                                        variable->env_var_name );
                }
            }
            else
            {
                UTILS_DEBUG( "  Environment variable is unset" );
            }
        }
    }

    return SCOREP_SUCCESS;
}


SCOREP_ErrorCode
SCOREP_ConfigSetValue( const char* nameSpaceName,
                       const char* variableName,
                       const char* variableValue )
{
    size_t                           name_space_len = strlen( nameSpaceName );
    struct scorep_config_name_space* name_space     =
        get_name_space( nameSpaceName, name_space_len, false );
    if ( !name_space )
    {
        return UTILS_ERROR( SCOREP_ERROR_INDEX_OUT_OF_BOUNDS,
                            "Unknown name space: %s::", nameSpaceName );
    }
    UTILS_DEBUG( "Using name space %s", name_space->name );

    struct scorep_config_variable* variable =
        get_variable( name_space, variableName, false );
    if ( !variable )
    {
        return UTILS_ERROR( SCOREP_ERROR_INDEX_OUT_OF_BOUNDS,
                            "Unknown config variable: %s::%s",
                            nameSpaceName, variableName );
    }
    UTILS_DEBUG( "Using variable %s", variable->env_var_name );

    bool successfully_parsed;
    successfully_parsed = parse_value( variableValue,
                                       variable->data.type,
                                       variable->data.variableReference,
                                       variable->data.variableContext );

    if ( !successfully_parsed )
    {
        return UTILS_ERROR( SCOREP_ERROR_PARSE_INVALID_VALUE,
                            "Invalid value for config variable %s::%s: %s",
                            nameSpaceName, variableName, variableValue );
    }

    return SCOREP_SUCCESS;
}


SCOREP_ErrorCode
SCOREP_ConfigDump( FILE* dumpFile )
{
    UTILS_ASSERT( dumpFile );

    UTILS_DEBUG( "Dump config variables to file" );

    for ( struct scorep_config_name_space* name_space = name_spaces_head;
          name_space;
          name_space = name_space->next )
    {
        for ( struct scorep_config_variable* variable = name_space->variables_head;
              variable;
              variable = variable->next )
        {
            dump_value( dumpFile,
                        variable->env_var_name,
                        variable->data.type,
                        variable->data.variableReference,
                        variable->data.variableContext );
        }
    }

    return SCOREP_SUCCESS;
}


static inline const char*
config_type_as_string( SCOREP_ConfigType type )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_BOOL:
            return "Boolean";
        case SCOREP_CONFIG_TYPE_SET:
        case SCOREP_CONFIG_TYPE_BITSET:
            return "Set";
        case SCOREP_CONFIG_TYPE_NUMBER:
            return "Number";
        case SCOREP_CONFIG_TYPE_SIZE:
            return "Number with size suffixes";
        case SCOREP_CONFIG_TYPE_STRING:
            return "String";
        case SCOREP_CONFIG_TYPE_PATH:
            return "Path";
        case SCOREP_INVALID_CONFIG_TYPE:
        default:
            return "Invalid";
    }
}


void
SCOREP_ConfigHelp( bool full, bool html )
{
    const char* sep = "";

    for ( struct scorep_config_name_space* name_space = name_spaces_head;
          name_space;
          name_space = name_space->next )
    {
        for ( struct scorep_config_variable* variable = name_space->variables_head;
              variable;
              variable = variable->next )
        {
            printf( "%s%s%s%s%s%s%s\n",
                    sep,
                    html ? " <dt>" : "",
                    html ? "@anchor " : "",
                    html ? variable->env_var_name : "",
                    html ? "<tt>" : "",
                    variable->env_var_name,
                    html ? "</tt></dt>" : "" );
            printf( "%s%s%s\n",
                    html ? " <dd>\n  " : "  Description: ",
                    variable->data.shortHelp,
                    html ? "<br>" : "" );
            printf( "%sType:%s%s%s\n",
                    html ? "  <dl>\n   <dt>" : "    ",
                    html ? "</dt><dd>" : "        ",
                    config_type_as_string( variable->data.type ),
                    html ? "</dd>" : "" );
            printf( "%sDefault:%s%s%s\n",
                    html ? "   <dt>" : "  ",
                    html ? "</dt><dd>" : "        ",
                    variable->data.defaultValue,
                    html ? "</dd>\n  </dl>" : "" );

            if ( full && strlen( variable->data.longHelp ) )
            {
                printf( "%s\n", html ? "  <br>" : "\n  Full description:" );
                const char* curr = variable->data.longHelp;
                const char* next;
                do
                {
                    next = strchr( curr, '\n' );
                    if ( !next )
                    {
                        next = curr + strlen( curr );
                    }
                    int len = ( int )( next - curr );
                    printf( "  %.*s%s\n", len, curr, html ? "<br>" : "" );
                    curr = next + 1;
                }
                while ( *next );
            }
            printf( "%s", html ? " </dd>" : "" );
            sep = "\n";
        }
    }
}


static size_t
hash_name_space( const void* key )
{
    const struct scorep_config_name_space* name_space = key;

    return SCOREP_Hashtab_HashString( name_space->name );
}


static int32_t
compare_name_space( const void* key, const void* item_key )
{
    const struct scorep_config_name_space* name_space_0 = key;
    const struct scorep_config_name_space* name_space_1 = item_key;

    if ( name_space_0->name_len == name_space_1->name_len )
    {
        return strcasecmp( name_space_0->name, name_space_1->name );
    }

    return 1;
}


size_t
hash_variable( const void* key )
{
    const struct scorep_config_variable* variable = key;

    return SCOREP_Hashtab_HashString( variable->data.name );
}


int32_t
compare_variable( const void* key,
                  const void* item_key )
{
    const struct scorep_config_variable* variable_0 = key;
    const struct scorep_config_variable* variable_1 = item_key;

    return strcasecmp( variable_0->data.name, variable_1->data.name );
}


void
string_to_upper( char* str )
{
    while ( *str )
    {
        if ( isalpha( *str ) )
        {
            *str = toupper( *str );
        }
        str++;
    }
}

void
string_to_lower( char* str )
{
    while ( *str )
    {
        if ( isalpha( *str ) )
        {
            *str = tolower( *str );
        }
        str++;
    }
}

void
check_name( const char* name, size_t nameLen, bool isNameSpace )
{
    const char*       str  = name;
    const char* const last = name + nameLen - 1;

    /* name spaces can be empty */
    if ( isNameSpace && nameLen == 0 )
    {
        return;
    }

    /* first character needs to be in [a-z] */
    UTILS_BUG_ON( !isalpha( *str ), "Invalid first character in config entity name." );
    str++;

    /* never allow underscores in name spaces */
    bool allow_underscore = !isNameSpace;
    while ( str <= last )
    {
        /* Do not allow an underscore for the last character */
        allow_underscore = allow_underscore && ( str < last );
        UTILS_BUG_ON( !isalnum( *str ) && ( !allow_underscore || *str != '_' ),
                      "Invalid character in config entity name." );
        str++;
    }
}

static bool
parse_bool( const char* value,
            bool*       boolReference );

static bool
parse_number( const char* value,
              uint64_t*   numberReference );

static bool
parse_size( const char* value,
            uint64_t*   sizeNumberReference );

static bool
parse_string( const char* value,
              char**      stringReference );

static bool
parse_set( const char* value,
           char***     stringListReference,
           char**      acceptedValues );

static bool
parse_bitset( const char*                 value,
              uint64_t*                   bitsetReference,
              SCOREP_ConfigType_SetEntry* acceptedValues );

bool
parse_value( const char*       value,
             SCOREP_ConfigType type,
             void*             variableReference,
             void*             variableContext )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_BOOL:
            return parse_bool( value, variableReference );

        case SCOREP_CONFIG_TYPE_NUMBER:
            return parse_number( value, variableReference );

        case SCOREP_CONFIG_TYPE_SIZE:
            return parse_size( value, variableReference );

        case SCOREP_CONFIG_TYPE_SET:
            UTILS_BUG_ON( !variableContext, "Missing config variable context." );
            return parse_set( value, variableReference, variableContext );

        case SCOREP_CONFIG_TYPE_BITSET:
            UTILS_BUG_ON( !variableContext, "Missing config variable context." );
            return parse_bitset( value, variableReference, variableContext );

        case SCOREP_CONFIG_TYPE_STRING:
            return parse_string( value, variableReference );

        case SCOREP_CONFIG_TYPE_PATH:

        case SCOREP_INVALID_CONFIG_TYPE:
        default:
            return false;
    }
}


bool
parse_bool( const char* value,
            bool*       boolReference )
{
    /* try symbolic constants */
    if ( 0 == strcasecmp( value, "true" ) ||
         0 == strcasecmp( value, "yes" ) ||
         0 == strcasecmp( value, "on" ) )
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

    UTILS_ASSERT( numberString );
    UTILS_ASSERT( numberReference );
    UTILS_ASSERT( endPtr );

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

bool
parse_number( const char* value,
              uint64_t*   numberReference )
{
    const char* orig_value = value;
    int         parse_success;

    parse_success = parse_uint64( value, numberReference, &value );
    if ( 0 != parse_success )
    {
        UTILS_ERROR( parse_success == ERANGE
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
        UTILS_ERROR( SCOREP_ERROR_EINVAL,
                     "Unrecognized characters after number `%s'",
                     orig_value );
        return false;
    }

    /* pass */
    return true;
}


bool
parse_size( const char* value,
            uint64_t*   sizeNumberReference )
{
    const char* orig_value = value;
    int         parse_success;

    parse_success = parse_uint64( value, sizeNumberReference, &value );
    if ( 0 != parse_success )
    {
        UTILS_ERROR( parse_success == ERANGE
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
            UTILS_ERROR( SCOREP_ERROR_EINVAL,
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
        UTILS_ERROR( SCOREP_ERROR_EINVAL,
                     "Unrecognized characters `%s' after size in config value `%s'",
                     value, orig_value );
        return false;
    }

    /* check for overflow */
    if ( ( *sizeNumberReference * scale_factor ) < *sizeNumberReference )
    {
        UTILS_ERROR( SCOREP_ERROR_EOVERFLOW,
                     "Resulting value does not fit into variable: `%s'",
                     value );
        return false;
    }
    *sizeNumberReference *= scale_factor;

    /* pass */
    return true;
}


bool
parse_string( const char* value,
              char**      stringReference )
{
    free( *stringReference );
    *stringReference = UTILS_CStr_dup( value );
    if ( !*stringReference )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_FAULT, "Can't duplicate string" );
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

    string_to_upper( str );

    return str;
}

bool
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
        UTILS_ERROR_POSIX();
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
            if ( 0 == strcasecmp( entry, string_list[ i ] ) )
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
            if ( 0 == strcasecmp( entry, *acceptedValue ) )
            {
                /* found entry in accepted values list */
                break;
            }
        }
        if ( acceptedValues && !*acceptedValue )
        {
            UTILS_WARNING( "Value '%s' not in accepted set.", entry );
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


bool
parse_bitset( const char*                 value,
              uint64_t*                   bitsetReference,
              SCOREP_ConfigType_SetEntry* acceptedValues )
{
    char* value_copy = malloc( strlen( value ) + 1 );
    if ( !value_copy )
    {
        UTILS_ERROR_POSIX();
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
            if ( 0 == strcasecmp( entry, acceptedValue->name ) )
            {
                /* found entry in accepted values list
                   add its value to the set */
                *bitsetReference |= acceptedValue->value;
                break;
            }
        }
        if ( !acceptedValue )
        {
            UTILS_WARNING( "Value '%s' not in accepted set.", entry );
            continue;
        }
    }

    free( value_copy );

    return success;
}

/* quotes a string for shell consumption */
static char*
single_quote_string( const char* str )
{
    size_t length = strlen( str );
    /* original length plus two ' */
    size_t new_length = length + 2;

    const char* string_it = str;
    while ( *string_it )
    {
        switch ( *string_it )
        {
            case '\'':
            case '!':
                /* each escaped character c will be replaced by (literally) '\c' */
                new_length += 3;
                break;
        }
        string_it++;
    }

    char* new_string = calloc( new_length + 1, sizeof( char ) );
    if ( !new_string )
    {
        UTILS_ERROR_POSIX();
        return NULL;
    }

    char* new_string_it = new_string;
    string_it        = str;
    *new_string_it++ = '\'';
    while ( *string_it )
    {
        switch ( *string_it )
        {
            case '\'':
            case '!':
                *new_string_it++ = '\'';
                *new_string_it++ = '\\';
                *new_string_it++ = *string_it;
                *new_string_it++ = '\'';
                break;
            default:
                *new_string_it++ = *string_it;
                break;
        }
        string_it++;
    }
    *new_string_it++ = '\'';
    *new_string_it   = '\0';

    return new_string;
}

static void
dump_set( FILE*       out,
          const char* name,
          char**      stringList )
{
    const char* sep = "";

    fprintf( out, "%s=", name );
    while ( *stringList )
    {
        char* quoted_string = single_quote_string( *stringList );
        if ( !quoted_string )
        {
            break;
        }
        fprintf( out, "%s%s", sep, quoted_string );
        sep = ",";
        free( quoted_string );

        stringList++;
    }
    fprintf( out, "\n" );
}

static void
dump_bitset( FILE*                       out,
             const char*                 name,
             uint64_t                    bitmask,
             SCOREP_ConfigType_SetEntry* acceptedValues )
{
    const char* sep = "";

    fprintf( out, "%s=", name );
    while ( acceptedValues->name )
    {
        if ( ( bitmask & acceptedValues->value ) == acceptedValues->value )
        {
            char* quoted_string = single_quote_string( acceptedValues->name );
            if ( !quoted_string )
            {
                break;
            }
            fprintf( out, "%s%s", sep, quoted_string );
            sep = ",";
            free( quoted_string );

            bitmask &= ~acceptedValues->value;
        }

        acceptedValues++;
    }
    fprintf( out, "\n" );
}

void
dump_value( FILE*             out,
            const char*       name,
            SCOREP_ConfigType type,
            void*             variableReference,
            void*             variableContext )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_BOOL:
            fprintf( out,
                     "%s=%s\n",
                     name,
                     *( bool* )variableReference ? "true" : "false" );
            break;

        case SCOREP_CONFIG_TYPE_SET:
            dump_set( out, name, *( char*** )variableReference );
            break;

        case SCOREP_CONFIG_TYPE_BITSET:
            dump_bitset( out,
                         name,
                         *( uint64_t* )variableReference,
                         variableContext );
            break;

        case SCOREP_CONFIG_TYPE_NUMBER:
        case SCOREP_CONFIG_TYPE_SIZE:
            fprintf( out,
                     "%s=%" PRIu64 "\n",
                     name,
                     *( uint64_t* )variableReference );
            break;

        case SCOREP_CONFIG_TYPE_STRING:
        {
            char* quoted_value = single_quote_string( *( const char** )variableReference );
            if ( !quoted_value )
            {
                break;
            }
            fprintf( out,
                     "%s=%s\n",
                     name,
                     quoted_value );
            free( quoted_value );

            break;
        }

        case SCOREP_CONFIG_TYPE_PATH:
        case SCOREP_INVALID_CONFIG_TYPE:
        default:
            break;
    }
}
