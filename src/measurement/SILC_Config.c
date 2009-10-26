#include <stdlib.h>
#include <stdio.h>

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

/**
 * @brief returns a string for the given config variable type
 */
static inline const char*
configtype_to_string
(
    SILC_ConfigType configType
)
{
    switch ( configType )
    {
        case SILC_CONFIG_TYPE_PATH:
            return "path";
        case SILC_CONFIG_TYPE_STRING:
            return "string";
        case SILC_CONFIG_TYPE_BOOL:
            return "boolean";
        case SILC_CONFIG_TYPE_NUMBER:
            return "number";
        case SILC_CONFIG_TYPE_SIZE:
            return "size";
        case SILC_CONFIG_TYPE_SET:
            return "set";
        default:
            return "unknown";
    }
}

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
        fprintf( stderr, "Variable:      %s/%s\n",
                 variables[ i ].nameSapce, variables[ i ].name );
        fprintf( stderr, "  Type:        %s\n",
                 configtype_to_string( variables[ i ].type ) );
        fprintf( stderr, "  Default:     %s\n", variables[ i ].defaultValue );
        fprintf( stderr, "  Description: %s\n", variables[ i ].shortHelp );
    }

    return SILC_SUCCESS;
}
