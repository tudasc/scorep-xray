#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

        fprintf( stderr, "Variable:      %s/%s\n",
                 variables[ i ].nameSpace, variables[ i ].name );
        fprintf( stderr, "  Type:        %s\n",
                 silc_config_type_to_string( variables[ i ].type ) );
        fprintf( stderr, "  Default:     %s\n", variables[ i ].defaultValue );
        fprintf( stderr, "  Description: %s\n", variables[ i ].shortHelp );

        /* set the variable to its default value,
           ignoring any errors for now */
        parse_value( variables[ i ].defaultValue,
                     variables[ i ].type,
                     variables[ i ].variableReference,
                     variables[ i ].variableContext );

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
            parse_value( environment_variable_value,
                         variables[ i ].type,
                         variables[ i ].variableReference,
                         variables[ i ].variableContext );
        }
        else
        {
            fprintf( stderr, "    Variable is unset\n" );
        }
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

        case SILC_CONFIG_TYPE_PATH:
        case SILC_CONFIG_TYPE_STRING:
        case SILC_CONFIG_TYPE_NUMBER:
        case SILC_CONFIG_TYPE_SIZE:
        case SILC_CONFIG_TYPE_SET:

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
