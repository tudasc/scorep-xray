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
                 variables[ i ].nameSpace, variables[ i ].name );
        fprintf( stderr, "  Type:        %s\n",
                 silc_config_type_to_string( variables[ i ].type ) );
        fprintf( stderr, "  Default:     %s\n", variables[ i ].defaultValue );
        fprintf( stderr, "  Description: %s\n", variables[ i ].shortHelp );
    }

    return SILC_SUCCESS;
}
