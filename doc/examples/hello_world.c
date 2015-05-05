#include <scorep/SCOREP_MetricPlugins.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Maximum number of metrics */
#define NUMBER_METRICS 5

/* Maximum string length */
#define MAX_BUFFER_LENGTH 255

/* Number of individual metrics */
static int32_t num_metrics = 0;

int32_t
init()
{
    return 0;
}

int32_t
add_counter( char* eventName )
{
    int id = num_metrics;
    num_metrics++;

    return id;
}

SCOREP_Metric_Plugin_MetricProperties*
get_event_info( char* eventName )
{
    SCOREP_Metric_Plugin_MetricProperties* return_values;
    char                                   name_buffer[ MAX_BUFFER_LENGTH ];
    int                                    i;

    /* If wildcard is specified, add some default counters */
    if ( strcmp( eventName, "*" ) == 0 )
    {
        return_values = malloc( ( NUMBER_METRICS + 1 ) * sizeof( SCOREP_Metric_Plugin_MetricProperties ) );
        for ( i = 0; i < NUMBER_METRICS; i++ )
        {
            snprintf( name_buffer, MAX_BUFFER_LENGTH, "sync counter #%i", i );
            return_values[ i ].name        = strdup( name_buffer );
            return_values[ i ].description = NULL;
            return_values[ i ].unit        = NULL;
            return_values[ i ].mode        = SCOREP_METRIC_MODE_ABSOLUTE_LAST;
            return_values[ i ].value_type  = SCOREP_METRIC_VALUE_UINT64;
            return_values[ i ].base        = SCOREP_METRIC_BASE_DECIMAL;
            return_values[ i ].exponent    = 0;
        }
        return_values[ NUMBER_METRICS ].name = NULL;
    }
    else
    {
        /* If no wildcard is given create one counter with the passed name */
        return_values = malloc( 2 * sizeof( SCOREP_Metric_Plugin_MetricProperties ) );
        snprintf( name_buffer, MAX_BUFFER_LENGTH, "sync counter #%s", eventName );
        return_values[ 0 ].name        = strdup( name_buffer );
        return_values[ 0 ].description = NULL;
        return_values[ 0 ].unit        = NULL;
        return_values[ 0 ].mode        = SCOREP_METRIC_MODE_ABSOLUTE_LAST;
        return_values[ 0 ].value_type  = SCOREP_METRIC_VALUE_UINT64;
        return_values[ 0 ].base        = SCOREP_METRIC_BASE_DECIMAL;
        return_values[ 0 ].exponent    = 0;
        return_values[ 1 ].name        = NULL;
    }
    return return_values;
}

bool
get_value( int32_t   counterIndex,
           uint64_t* value )
{
    *value = counterIndex;

    return true;
}

void
fini()
{
    return;
}

SCOREP_METRIC_PLUGIN_ENTRY( HelloWorld )
{
    /* Initialize info data (with zero) */
    SCOREP_Metric_Plugin_Info info;
    memset( &info, 0, sizeof( SCOREP_Metric_Plugin_Info ) );

    /* Set up */
    info.plugin_version     = SCOREP_METRIC_PLUGIN_VERSION;
    info.run_per            = SCOREP_METRIC_PER_PROCESS;
    info.sync               = SCOREP_METRIC_SYNC;
    info.initialize         = init;
    info.finalize           = fini;
    info.get_event_info     = get_event_info;
    info.add_counter        = add_counter;
    info.get_optional_value = get_value;

    return info;
}
