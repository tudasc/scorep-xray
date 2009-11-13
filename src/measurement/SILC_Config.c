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

#include "silc_types.h"

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
        }
        else
        {
            fprintf( stderr, "    Variable is unset\n" );
        }
    }

    return SILC_SUCCESS;
}
