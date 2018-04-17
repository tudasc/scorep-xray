/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief  Global vars and their parsing for the GCC plug-in.
 *
 */

int scorep_plugin_verbosity        = 0;
int scorep_plugin_symbol_verbosity = 2;

SCOREP_Filter* scorep_instrument_filter = NULL;

static int
scorep_plugin_parameters_init( struct plugin_name_args* plugin_info )
{
    scorep_instrument_filter = SCOREP_Filter_New();

    for ( int i = 0; i < plugin_info->argc; i++ )
    {
        const struct plugin_argument* arg = &plugin_info->argv[ i ];
        if ( strcmp( arg->key, "filter" ) == 0 )
        {
            if ( !arg->value )
            {
                UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                             "Missing argument for filter paramter." );
                return 1;
            }
            SCOREP_ErrorCode status = SCOREP_Filter_ParseFile( scorep_instrument_filter, arg->value );

            if ( status != SCOREP_SUCCESS )
            {
                UTILS_ERROR( status,
                             "Couldn't load filter file '%s'.",
                             arg->value );
                return 1;
            }
        }
        else if ( strcmp( arg->key, "verbosity" ) == 0 )
        {
            if ( !arg->value )
            {
                /* without argument, just increase the verbosity */
                scorep_plugin_verbosity++;
                continue;
            }

            char* endptr;
            long  value = strtol( arg->value, &endptr, 0 );
            if ( endptr == arg->value || *endptr || value < 0 )
            {
                UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                             "Invalid value for verbosity: '%s'.",
                             arg->value );
                return 1;
            }

            scorep_plugin_verbosity = value;
        }
        else if ( strcmp( arg->key, "symbol-verbosity" ) == 0 )
        {
            if ( !arg->value )
            {
                UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                             "Missing argument for symbol-verbosity paramter." );
                return 1;
            }

            char* endptr;
            long  value = strtol( arg->value, &endptr, 0 );
            if ( endptr == arg->value || *endptr || value < 0 || value > 2 )
            {
                UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                             "Invalid value for symbol verbosity: '%s'.",
                             arg->value );
                return 1;
            }

            scorep_plugin_symbol_verbosity = value;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                         "Invalid parameter '%s'.",
                         arg->key );
            return 1;
        }
    }

    return 0;
}

static void
scorep_plugin_parameters_fini( void* event_data,
                               void* data )
{
    SCOREP_Filter_Delete( scorep_instrument_filter );
}
