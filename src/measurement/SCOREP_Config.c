/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
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
#include <UTILS_IO.h>
#define SCOREP_DEBUG_MODULE_NAME CONFIG
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include <SCOREP_Hashtab.h>

#include "scorep_type_utils.h"

static size_t
hash_variable( const void* key );

static int32_t
compare_variable( const void* key,
                  const void* item_key );

/*
 * Naming conventions of Score-P configure variables:
 * SCOREP_<namespace>_<varname>
 *
 * ENV_NAME_LEN_MAX restricts the maximum length of the complete
 * configure variable name.
 * ENV_NAME_SUB_LEN_MAX restricts the maximum length of the <namespace>
 * resp. <varname> part. Hence, ENV_NAME_SUB_LEN_MAX is
 * ( ( ENV_NAME_LEN_MAX - strlen(SCOREP_) - strlen( _ ) ) / 2 )
 *
 */
#define ENV_NAME_LEN_MAX     90
#define ENV_NAME_SUB_LEN_MAX 41
struct scorep_config_variable
{
    SCOREP_ConfigVariable          data;
    char                           env_var_name[ ENV_NAME_LEN_MAX + 1 ];
    bool                           is_evaluated;
    bool                           is_changed;
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
check_bitset( const char*                 nameSpaceName,
              const char*                 variableName,
              SCOREP_ConfigType_SetEntry* acceptedValues );

static void
string_to_lower( char* str );

static void
string_to_upper( char* str );

static bool
equal_icase_stringn( const char* str1,
                     const char* str2,
                     size_t      length );

static bool
equal_icase_string( const char* str1,
                    const char* str2 );

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
        return entry->value.ptr;
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

    SCOREP_Hashtab_InsertPtr( name_spaces,
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

    /* !entry && !create: don't create a new entry if not requested
     * entry && create:  creating the same variable twice is erroneous
     */
    if ( !entry == !create )
    {
        return NULL;
    }

    if ( entry )
    {
        return entry->value.ptr;
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
             "SCOREP_%.*s%s%.*s",
             ENV_NAME_SUB_LEN_MAX,
             nameSpace->name,
             nameSpace->name_len ? "_" : "",
             ENV_NAME_SUB_LEN_MAX,
             variable->data.name );
    string_to_upper( variable->env_var_name );

    SCOREP_Hashtab_InsertPtr( nameSpace->variables,
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
SCOREP_ConfigRegister( const char*                  nameSpaceName,
                       const SCOREP_ConfigVariable* variables )
{
    UTILS_ASSERT( name_spaces );
    UTILS_ASSERT( nameSpaceName );

    size_t name_space_len = strlen( nameSpaceName );
    UTILS_BUG_ON( name_space_len > ENV_NAME_SUB_LEN_MAX,
                  "Name space is too long." );
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

        UTILS_BUG_ON( !variables->shortHelp, "Missing short description value." );
        UTILS_BUG_ON( strpbrk( variables->shortHelp, "\n\r\v\t" ),
                      "Short description should not contain any control characters like \\n/\\r/\\v/\\t." );
        UTILS_BUG_ON( !variables->longHelp, "Missing long description value." );

        size_t name_len = strlen( variables->name );
        UTILS_BUG_ON( name_len == 1, "Variable name needs to be longer than 1 character." );
        UTILS_BUG_ON( name_len > ENV_NAME_SUB_LEN_MAX, "Variable name too long." );
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

        /* sanity check possible bitset members */
        if ( SCOREP_CONFIG_TYPE_BITSET == variable->data.type )
        {
            check_bitset( nameSpaceName,
                          variable->data.name,
                          variable->data.variableContext );
        }

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
SCOREP_ConfigRegisterCond( const char*                  nameSpaceName,
                           const SCOREP_ConfigVariable* variables,
                           bool                         isAvailable )
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

    UTILS_DEBUG( "Apply environment to config variables" );

    for ( struct scorep_config_name_space* name_space = name_spaces_head;
          name_space;
          name_space = name_space->next )
    {
        for ( struct scorep_config_variable* variable = name_space->variables_head;
              variable;
              variable = variable->next )
        {
            if ( variable->is_evaluated )
            {
                /* Environment was already applied to this variable, skip it */
                continue;
            }

            const char* environment_variable_value =
                getenv( variable->env_var_name );

            variable->is_evaluated = true;

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
                variable->is_changed = true;

                if ( !successfully_parsed )
                {
                    return UTILS_ERROR( SCOREP_ERROR_EINVAL,
                                        "Can't set variable '%s::%s' to "
                                        "value '%s' from environment variable %s",
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
                            "Invalid value for config variable '%s::%s': %s",
                            nameSpaceName, variableName, variableValue );
    }

    return SCOREP_SUCCESS;
}


SCOREP_ConfigVariable*
SCOREP_ConfigGetData( const char* nameSpaceName,
                      const char* variableName )
{
    size_t                           name_space_len = strlen( nameSpaceName );
    struct scorep_config_name_space* name_space     =
        get_name_space( nameSpaceName, name_space_len, false );
    if ( !name_space )
    {
        UTILS_ERROR( SCOREP_ERROR_INDEX_OUT_OF_BOUNDS,
                     "Unknown name space: %s::", nameSpaceName );
    }
    UTILS_DEBUG( "Using name space %s", name_space->name );

    struct scorep_config_variable* variable =
        get_variable( name_space, variableName, false );
    if ( !variable )
    {
        return NULL;
    }

    return &variable->data;
}


static SCOREP_ErrorCode
config_dump( FILE* dumpFile, bool allVariables, bool withValue )
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
            if ( variable->is_changed || allVariables )
            {
                char buffer[ 200 ];
                if ( allVariables )
                {
                    sprintf( buffer, "%s", variable->env_var_name );
                }
                else
                {
                    sprintf( buffer, "    %s", variable->env_var_name );
                }
                if ( withValue )
                {
                    dump_value( dumpFile,
                                buffer,
                                variable->data.type,
                                variable->data.variableReference,
                                variable->data.variableContext );
                }
                else
                {
                    fprintf( dumpFile, "%s\n", buffer );
                }
            }
        }
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
SCOREP_ConfigDump( FILE* dumpFile )
{
    return config_dump( dumpFile, true, true );
}

SCOREP_ErrorCode
SCOREP_ConfigDumpChangedVars( FILE* dumpFile )
{
    return config_dump( dumpFile, false, false );
}


static inline const char*
config_type_as_string( SCOREP_ConfigType type )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_STRING:
            return "String";
        case SCOREP_CONFIG_TYPE_PATH:
            return "Path";
        case SCOREP_CONFIG_TYPE_BOOL:
            return "Boolean";
        case SCOREP_CONFIG_TYPE_NUMBER:
            return "Number";
        case SCOREP_CONFIG_TYPE_SIZE:
            return "Number with size suffixes";
        case SCOREP_CONFIG_TYPE_BITSET:
            return "Set";
        case SCOREP_CONFIG_TYPE_OPTIONSET:
            return "Option";
        case SCOREP_INVALID_CONFIG_TYPE:
        default:
            return "Invalid";
    }
}

static inline bool
config_type_need_quotes( SCOREP_ConfigType type )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_STRING:
        case SCOREP_CONFIG_TYPE_PATH:
            return true;
        default:
            return false;
    }
}

static void
dump_line( const char* line,
           int         length,
           bool        html,
           FILE*       out )
{
    const char* nbsp_repl = html ? "&nbsp;" : " ";
    while ( length-- )
    {
        if ( *line == '\240' )
        {
            fprintf( out, "%s", nbsp_repl );
        }
        else
        {
            putc( *line, out );
        }
        line++;
    }
}

static void
wrap_lines( const char* message,
            int         width,
            int         indent,
            int         firstIndent,
            bool        html,
            FILE*       out )
{
    int         column_width = width - indent;
    int         reminder     = column_width;
    int         nl           = 0;
    const char* sep          = "";
    const char* curr         = message;
    while ( true )
    {
        /* skip any whitespace */
        curr += strspn( curr, " \t\n\r\v" );

        /* stop if we reached the end */
        if ( !*curr )
        {
            break;
        }

        /* print separator to previous word */
        switch ( nl )
        {
            case 0:
                if ( firstIndent <= 0 )
                {
                    fprintf( out, "\n%*s%s",
                             indent, "",
                             html ? "<p>" : "" );
                }
                else
                {
                    fprintf( out, "%*s%s",
                             firstIndent, "",
                             html ? "<p>" : "" );
                }
                sep = "";
                break;

            case 4:
                /* a paragraph, empty line and fall thru to linebreak */
                fprintf( out, "%s\n%s",
                         html ? "</p>" : "",
                         html ? "<p>" : "" );

            case 3:
                fprintf( out, "%s", html ? "<br/>" : "" );

            case 2:
                fprintf( out, "\n%*s", indent, "" );
                reminder = column_width;
                sep      = "";
                break;
        }

        /* get length of next word */
        int length = strcspn( curr, " \t\n\r\v" );
        nl = 1;
        if ( curr[ length ] == '\n' )
        {
            nl = 3;
            if ( curr[ length + 1 ] == '\n' )
            {
                nl = 4;
            }
        }

        if ( length < reminder || reminder == column_width )
        {
            fprintf( out, "%s", sep );
            dump_line( curr, length, html, out );
            curr     += length;
            reminder -= length + strlen( sep );
        }
        else
        {
            /* word does not fit anymore in this line, but it was not the first word */
            nl = 2;
        }
        sep = " ";
    }

    fprintf( out, "%s\n",
             html ? "</p>" : "" );
}
#define WRAP_MARGIN 80

void
SCOREP_ConfigManifestSectionHeader( FILE* out, const char* section )
{
    fprintf( out, "\n      * %s:\n\n", section );
}

void
SCOREP_ConfigManifestSectionEntry( FILE* out, const char* fileName, const char* descriptionFormatString, ... )
{
    char name[ 250 ];
    char buffer[ 1000 ];
    sprintf( name, "        * `%s`", fileName );

    va_list va;
    va_start( va, descriptionFormatString );
    vsprintf( buffer, descriptionFormatString, va );

    if ( strlen( name ) < 31 ) //ends before the second column
    {
        //start description in the same line
        int len = 31 - strlen( name ) + 1;

        fprintf( out, "%s", name );
        wrap_lines( buffer, WRAP_MARGIN, 32, len, false, out );
    }
    else
    {
        //start description in a new line
        wrap_lines( name, WRAP_MARGIN, 0, 8, false, out );
        wrap_lines( buffer, WRAP_MARGIN, 32, 32, false, out );
    }
}

void
SCOREP_ConfigHelp( bool  full,
                   bool  html,
                   FILE* out )
{
    const char* sep = html ? "<dl>\n" : "";
    for ( struct scorep_config_name_space* name_space = name_spaces_head;
          name_space;
          name_space = name_space->next )
    {
        for ( struct scorep_config_variable* variable = name_space->variables_head;
              variable;
              variable = variable->next )
        {
            fprintf( out, "%s%s%s%s%s%s%s\n",
                     sep,
                     html ? " <dt>" : "",
                     html ? "@anchor " : "",
                     html ? variable->env_var_name : "",
                     html ? "<tt>" : "",
                     variable->env_var_name,
                     html ? "</tt></dt>" : "" );

            fprintf( out, "%s",
                     html ? " <dd>\n  " : "  Description:" );
            wrap_lines( variable->data.shortHelp, WRAP_MARGIN, 15, 1, html, out );

            fprintf( out, "%sType:%s%s%s\n",
                     html ? "  <br/>\n  <dl>\n   <dt>" : "         ",
                     html ? "</dt><dd>" : " ",
                     config_type_as_string( variable->data.type ),
                     html ? "</dd>" : "" );
            fprintf( out, "%sDefault:%s%s%s%s%s\n",
                     html ? "   <dt>" : "      ",
                     html ? "</dt><dd>" : " ",
                     config_type_need_quotes( variable->data.type ) ? ( html ? "&quot;" : "\"" ) : "",
                     variable->data.defaultValue,
                     config_type_need_quotes( variable->data.type ) ? ( html ? "&quot;" : "\"" ) : "",
                     html ? "</dd>\n  </dl>" : "" );

            if ( full )
            {
                if ( strlen( variable->data.longHelp ) )
                {
                    fprintf( out, "%s\n", html ? "  " : "\n  Full description:" );
                    wrap_lines( variable->data.longHelp, WRAP_MARGIN, 2, 2, html, out );
                }

                if ( SCOREP_CONFIG_TYPE_BITSET == variable->data.type
                     || SCOREP_CONFIG_TYPE_OPTIONSET == variable->data.type )
                {
                    SCOREP_ConfigType_SetEntry* acceptedValues = variable->data.variableContext;

                    int column_width = 0;
                    if ( html )
                    {
                        fprintf( out, "  <dl>\n" );
                    }
                    else
                    {
                        if ( SCOREP_CONFIG_TYPE_BITSET == variable->data.type )
                        {
                            column_width = strlen( "none/no" );
                        }
                        while ( acceptedValues->name )
                        {
                            size_t length = strlen( acceptedValues->name );
                            if ( length > ( size_t )column_width )
                            {
                                column_width = length;
                            }
                            acceptedValues++;
                        }
                    }

                    acceptedValues = variable->data.variableContext;
                    while ( acceptedValues->name )
                    {
                        fprintf( out, "    %s%s%s",
                                 html ? "<dt>" : "",
                                 acceptedValues->name,
                                 html ? "</dt>\n    <dd>\n" : ": " );

                        wrap_lines( acceptedValues->description,
                                    WRAP_MARGIN,
                                    column_width + 6,
                                    column_width - strlen( acceptedValues->name ),
                                    html, out );

                        if ( html )
                        {
                            fprintf( out, "    </dd>\n" );
                        }

                        acceptedValues++;
                    }

                    if ( SCOREP_CONFIG_TYPE_BITSET == variable->data.type )
                    {
                        fprintf( out, "    %snone/no%s%*s%s%s\n",
                                 html ? "<dt>" : "",
                                 html ? "</dt>\n    <dd>" : ": ",
                                 html ? 0 : ( column_width - ( int )strlen( "none/no" ) ), "",
                                 "Disable feature",
                                 html ? "</dd>" : "" );
                    }

                    if ( html )
                    {
                        fprintf( out, "  </dl>\n" );
                    }
                }
            }

            fprintf( out, "%s", html ? " </dd>" : "" );
            sep = html ? "\n <br/>\n" : "\n";
        }
    }
    if ( html )
    {
        fprintf( out, "%s</dl>\n", sep );
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

/** Comapre two strings for equality for a given length.
 *  @a length must less or equal the length of both strings.
 */
static bool
equal_icase_stringn( const char* str1, const char* str2, size_t length )
{
    while ( length-- )
    {
        if ( toupper( str1[ length ] ) != toupper( str2[ length ] ) )
        {
            return false;
        }
    }

    return true;
}

static bool
equal_icase_string( const char* str1, const char* str2 )
{
    size_t length1 = strlen( str1 );
    size_t length2 = strlen( str2 );

    if ( length1 == length2 )
    {
        return equal_icase_stringn( str1, str2, length1 );
    }

    return false;
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

void
check_bitset( const char*                 nameSpaceName,
              const char*                 variableName,
              SCOREP_ConfigType_SetEntry* acceptedValues )
{
    while ( acceptedValues->name )
    {
        UTILS_BUG_ON( 0 == acceptedValues->value,
                      "Possible set members for variable %s::%s includes the 0 value!",
                      nameSpaceName, variableName );
        UTILS_BUG_ON( equal_icase_string( acceptedValues->name, "no" )
                      || equal_icase_string( acceptedValues->name, "none" ),
                      "Invalid set member name for variable %s::%s: %s",
                      nameSpaceName,
                      variableName,
                      acceptedValues->name );

        acceptedValues++;
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
parse_bitset( const char*                 value,
              uint64_t*                   bitsetReference,
              SCOREP_ConfigType_SetEntry* acceptedValues );

static bool
parse_optionset( const char*                 value,
                 uint64_t*                   optionReference,
                 SCOREP_ConfigType_SetEntry* acceptedValues );

bool
parse_value( const char*       value,
             SCOREP_ConfigType type,
             void*             variableReference,
             void*             variableContext )
{
    switch ( type )
    {
        case SCOREP_CONFIG_TYPE_STRING:
        case SCOREP_CONFIG_TYPE_PATH:
            return parse_string( value, variableReference );

        case SCOREP_CONFIG_TYPE_BOOL:
            return parse_bool( value, variableReference );

        case SCOREP_CONFIG_TYPE_NUMBER:
            return parse_number( value, variableReference );

        case SCOREP_CONFIG_TYPE_SIZE:
            return parse_size( value, variableReference );

        case SCOREP_CONFIG_TYPE_BITSET:
            UTILS_BUG_ON( !variableContext, "Missing config variable context." );
            return parse_bitset( value, variableReference, variableContext );

        case SCOREP_CONFIG_TYPE_OPTIONSET:
            UTILS_BUG_ON( !variableContext, "Missing config variable context." );
            return parse_optionset( value, variableReference, variableContext );

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
    if ( equal_icase_string( value, "true" ) ||
         equal_icase_string( value, "yes" ) ||
         equal_icase_string( value, "on" ) )
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
     * real number characters. That is a whitespace only string is not
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
                     "Can't parse number in config value: '%s'",
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
                     "Unrecognized characters after number '%s'",
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
                     "Can't parse size in config value: '%s'",
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
                         "Invalid scale factor '%s' in config value '%s'",
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
                     "Unrecognized characters '%s' after size in config value '%s'",
                     value, orig_value );
        return false;
    }

    /* check for overflow */
    if ( ( *sizeNumberReference * scale_factor ) < *sizeNumberReference )
    {
        UTILS_ERROR( SCOREP_ERROR_EOVERFLOW,
                     "Resulting value does not fit into variable: '%s'",
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

    return str;
}

static bool
string_in_alias_list( const char* value, const char* aliases )
{
    size_t value_length = strlen( value );

    const char* curr = aliases;
    const char* next;
    do
    {
        next = strchr( curr, '/' );
        if ( !next )
        {
            next = curr + strlen( curr );
        }
        size_t length = next - curr;
        if ( value_length == length && equal_icase_stringn( value, curr, length ) )
        {
            return true;
        }

        curr = next + 1;
    }
    while ( *next );

    return false;
}

bool
parse_bitset( const char*                 value,
              uint64_t*                   bitsetReference,
              SCOREP_ConfigType_SetEntry* acceptedValues )
{
    *bitsetReference = 0;

    if ( equal_icase_string( value, "none" ) ||
         equal_icase_string( value, "no" ) )
    {
        return true;
    }

    char* value_copy = malloc( strlen( value ) + 1 );
    if ( !value_copy )
    {
        UTILS_ERROR_POSIX();
        return false;
    }
    strcpy( value_copy, value );

    char* entry;
    bool  success          = true;
    char* value_for_strtok = value_copy;
    while ( ( entry = trim_string( strtok( value_for_strtok, " ,:;" ) ) ) )
    {
        if ( 0 == strlen( entry ) )
        {
            continue;
        }

        /* all but the first call to strtok should be NULL */
        value_for_strtok = NULL;

        /* '~' denotes remove from bitset */
        bool invert = false;
        if ( *entry == '~' )
        {
            invert = true;
            entry++;
        }

        /* check if entry is in acceptedValues */
        SCOREP_ConfigType_SetEntry* acceptedValue;
        for ( acceptedValue = acceptedValues;
              acceptedValue->name;
              acceptedValue++ )
        {
            if ( string_in_alias_list( entry, acceptedValue->name ) )
            {
                /* found entry in accepted values list
                   add its value to the set */
                if ( invert )
                {
                    *bitsetReference &= ~acceptedValue->value;
                }
                else
                {
                    *bitsetReference |= acceptedValue->value;
                }
                break;
            }
        }
        if ( !acceptedValue->name )
        {
            UTILS_WARNING( "Value '%s' not in accepted set.", entry );
            success = false;
            break;
        }
    }

    free( value_copy );

    return success;
}

bool
parse_optionset( const char*                 value,
                 uint64_t*                   optionReference,
                 SCOREP_ConfigType_SetEntry* acceptedValues )
{
    *optionReference = 0;
    bool success = true;

    char* value_copy = malloc( strlen( value ) + 1 );
    if ( !value_copy )
    {
        UTILS_ERROR_POSIX();
        return false;
    }
    strcpy( value_copy, value );
    value = trim_string( value_copy );

    size_t value_length = strlen( value );
    if ( 0 == value_length )
    {
        free( value_copy );
        return true;
    }

    /* check if entry is in acceptedValues */
    SCOREP_ConfigType_SetEntry* acceptedValue = acceptedValues;
    while ( acceptedValue->name )
    {
        if ( string_in_alias_list( value, acceptedValue->name ) )
        {
            /* found entry in accepted values list */
            *optionReference = acceptedValue->value;
            break;
        }

        acceptedValue++;
    }

    if ( !acceptedValue->name )
    {
        UTILS_WARNING( "Value '%s' not in accepted set.", value );
        success = false;
    }

    free( value_copy );

    return success;
}

/* quotes a string for shell consumption */
static char*
single_quote_stringn( const char* str, size_t length )
{
    const char* end = str + length;
    /* original length plus two ' */
    size_t new_length = length + 2;

    const char* string_it = str;
    while ( string_it < end )
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
    while ( string_it < end )
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

static char*
single_quote_string( const char* str )
{
    return single_quote_stringn( str, strlen( str ) );
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
            /* extract first name of alias list */
            const char* curr = acceptedValues->name;
            const char* next = strchr( curr, '/' );
            if ( !next )
            {
                next = curr + strlen( curr );
            }

            char* quoted_string = single_quote_stringn( curr, next - curr );
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

static void
dump_optionset( FILE*                       out,
                const char*                 name,
                uint64_t                    option,
                SCOREP_ConfigType_SetEntry* acceptedValues )
{
    fprintf( out, "%s=", name );
    while ( acceptedValues->name )
    {
        if ( option == acceptedValues->value )
        {
            /* extract first name of alias list */
            const char* curr = acceptedValues->name;
            const char* next = strchr( curr, '/' );
            if ( !next )
            {
                next = curr + strlen( curr );
            }

            char* quoted_string = single_quote_stringn( curr, next - curr );
            if ( quoted_string )
            {
                fprintf( out, "%s", quoted_string );
                free( quoted_string );
            }

            break;
        }

        acceptedValues++;
    }
    fprintf( out, "\n" );
}

static void
dump_size( FILE*       out,
           const char* name,
           uint64_t    value )
{
    const char* suffix = " KMGTPE";

    while ( value % 1024 == 0 && suffix[ 1 ] )
    {
        value = value / 1024;
        suffix++;
    }
    fprintf( out,
             "%s=%" PRIu64 "%.*s\n",
             name,
             value,
             *suffix == ' ' ? 0 : 1,
             suffix );
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
        case SCOREP_CONFIG_TYPE_STRING:
        case SCOREP_CONFIG_TYPE_PATH:
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

        case SCOREP_CONFIG_TYPE_BOOL:
            fprintf( out,
                     "%s=%s\n",
                     name,
                     *( bool* )variableReference ? "true" : "false" );
            break;

        case SCOREP_CONFIG_TYPE_BITSET:
            dump_bitset( out,
                         name,
                         *( uint64_t* )variableReference,
                         variableContext );
            break;

        case SCOREP_CONFIG_TYPE_OPTIONSET:
            dump_optionset( out,
                            name,
                            *( uint64_t* )variableReference,
                            variableContext );
            break;

        case SCOREP_CONFIG_TYPE_NUMBER:
            fprintf( out,
                     "%s=%" PRIu64 "\n",
                     name,
                     *( uint64_t* )variableReference );
            break;

        case SCOREP_CONFIG_TYPE_SIZE:
            dump_size( out,
                       name,
                       *( uint64_t* )variableReference );

        case SCOREP_INVALID_CONFIG_TYPE:
        default:
            break;
    }
}

bool
SCOREP_ConfigCopyFile( const char* nameSpaceName,
                       const char* name,
                       const char* relativeSourceDir,
                       const char* targetDir )
{
    UTILS_DEBUG( "Copy file based on variable %s from namespace %s", nameSpaceName, name );

    UTILS_ASSERT( nameSpaceName );
    size_t name_space_len = strlen( nameSpaceName );
    UTILS_BUG_ON( name_space_len > ENV_NAME_SUB_LEN_MAX,
                  "Name space is too long." );
    check_name( nameSpaceName, name_space_len, true );

    struct scorep_config_name_space* name_space;
    name_space = get_name_space( nameSpaceName,
                                 name_space_len,
                                 false );
    UTILS_ASSERT( name_space );

    size_t name_len = strlen( name );
    UTILS_BUG_ON( name_len == 1, "Variable name needs to be longer than 1 character." );
    UTILS_BUG_ON( name_len > ENV_NAME_SUB_LEN_MAX, "Variable name too long." );
    check_name( name, name_len, false );

    const struct scorep_config_variable* variable;
    variable = get_variable( name_space,
                             name,
                             false );

    if ( variable == NULL )
    {
        /* didn't find the variable. */
        return false;
    }
    const char* value = *( const char** )variable->data.variableReference;

    if (   value == NULL || *value == '\0'  || !variable->data.variableContext )
    {
        /* empty value, e.g., default setting, or empty target name,
           therefore nothing needs to be done. */
        return false;
    }
    char* input_file_name = UTILS_IO_JoinPath(
        2, relativeSourceDir, value );

    if ( UTILS_IO_DoesFileExist( input_file_name ) )
    {
        char* output_file_name = UTILS_IO_JoinPath(
            2, targetDir, ( const char* )variable->data.variableContext );

        if ( output_file_name )
        {
            SCOREP_ErrorCode error = UTILS_IO_FileCopy( input_file_name, output_file_name );
            if ( error )
            {
                UTILS_WARNING(  "Cannot copy file '%s' to '%s'.", input_file_name, output_file_name  );
            }
        }
        free( output_file_name );
    }

    free( input_file_name );
    return true;
}
