/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 *  @status     alpha
 *
 *  @file       scorep_metric_management.c
 *
 *  @author     Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *  @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 *  @brief This module handles implementations of different metric sources.
 *
 *  We have to distinguish different kinds of metrics regarding to their
 *  scope (validity). On the one hand there is a global set of metrics
 *  that is recorded by each location for every enter/leave event. This
 *  kind of metrics is call 'synchronous strict' metrics. On the other
 *  hand there are additional scoped metrics. The additional scoped
 *  metrics consists of per-process metrics. For per-process metrics only
 *  one location of each location group records this kind of metrics.
 */

#include <config.h>

#include <SCOREP_Metric_Management.h>

#include <stdlib.h>
#include <assert.h>

#include "SCOREP_Config.h"
#include <SCOREP_Memory.h>
#include <UTILS_Debug.h>
#include <SCOREP_RuntimeManagement.h>
#include <tracing/SCOREP_Tracing_Events.h>


#include "scorep_definitions.h"

/* Include to implement metric service */
#include "SCOREP_Metric.h"
/* Include to manage different metric sources */
#include "SCOREP_Metric_Source.h"
/* Include to write definitions */
#include "SCOREP_Definitions.h"

/* Include header files of supported metric sources */
#if HAVE( PAPI )
#include "scorep_metric_papi.h"
#endif
#if HAVE( GETRUSAGE )
#include "scorep_metric_rusage.h"
#endif

#include <unistd.h>
#include <sys/types.h>



/** List of metric sources. */
static const SCOREP_MetricSource* scorep_metric_sources[] = {
#if HAVE( PAPI )
    &SCOREP_Metric_Papi,
#endif
#if HAVE( GETRUSAGE )
    &SCOREP_Metric_Rusage
#endif
};

/** Number of registered metric sources */
#define SCOREP_NUMBER_OF_METRIC_SOURCES sizeof( scorep_metric_sources ) / sizeof( scorep_metric_sources[ 0 ] )

/** Additional metrics are managed in a per-location based data structure.
 *  There will be one @ LocationMetricSet per metric type (e.g. per-process
 *  metric). */
typedef struct SCOREP_Metric_LocationMetricSet SCOREP_Metric_LocationMetricSet;
struct SCOREP_Metric_LocationMetricSet
{
    /** Internal metric set of the metric service */
    SCOREP_Metric_EventSet* event_set[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

    /** The corresponding definition */
    SCOREP_SamplingSetHandle sampling_set;

    /** Number of metrics contained in event sets */
    uint32_t metrics_counts[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

    /** Offsets for metric values in global value array */
    uint32_t metrics_offsets[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

    /** Next additional metric */
    SCOREP_Metric_LocationMetricSet* next;
};


/** Type for per-location based data related to metrics. */
typedef struct SCOREP_Metric_LocationData SCOREP_Metric_LocationData;

/** Per-location based data related to metrics.
 *  The value array will be used by all metric kinds. */
struct SCOREP_Metric_LocationData
{
    /** Event sets of 'synchronous strict' metrics */
    SCOREP_Metric_EventSet* event_set[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

    /** Additional scoped metrics of location */
    SCOREP_Metric_LocationMetricSet* additional_metrics;

    /** This location records any metrics (regardless of metric type,
     * 'synchronous strict' or additional metric) */
    bool has_metrics;

    /** Array of all metric values (including 'synchronous strict' and
     *  additional metrics) */
    uint64_t* values;
};


/** Data structure to manage global set of 'synchronous strict' metrics */
typedef struct scorep_synchronous_strict_metrics
{
    /** Number of overall active metrics */
    uint32_t overall_number_of_metrics;

    /** Vector that contains number of requested 'synchronous strict' metrics in each metric source */
    uint32_t counts[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

    /** Vector of offsets for each metric. Metric source s starts writing its 'synchronous strict'
     *  metric values to the global array at values[ offset[ s ] ]. */
    uint32_t offsets[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

    /** Array of all metric handles used by the global sampling set of 'synchronous strict' metrics */
    SCOREP_MetricHandle* metrics;

    /** Global sampling set of 'synchronous strict' metrics */
    SCOREP_SamplingSetHandle sampling_set;
} scorep_synchronous_strict_metrics;


/** Flag indicating status of metric management */
static bool scorep_metric_management_initialized;

/** 'Synchronous strict' metrics */
static scorep_synchronous_strict_metrics synchronous_strict_metrics;

/** Our subsystem id, used to address our per-location metric data */
static size_t scorep_metric_subsystem_id;


/* *********************************************************************
 * Function prototypes
 **********************************************************************/

static void
initialize_location_metric_cb( SCOREP_Location* location,
                               void*            data );

static void
finalize_location_metric_cb( SCOREP_Location* location,
                             void*            data );


/* *********************************************************************
 * Service management
 **********************************************************************/

/** @brief Registers configuration variables for the metric services.
 *
 *  @param subsystem_id         Identifier of metric subsystem.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_register( size_t subsystem_id )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register metric management." );

    scorep_metric_subsystem_id = subsystem_id;

    /* Register metric sources */
    for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
    {
        scorep_metric_sources[ i ]->metric_source_register();
    }

    return SCOREP_SUCCESS;
}

/** @brief Called on deregistration of the metric service.
 */
static void
scorep_metric_deregister()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " deregister metric management." );

    /* Deregister metric sources */
    for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
    {
        scorep_metric_sources[ i ]->metric_source_deregister();
    }
}

/** @brief Called on initialization of the metric service.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_initialize_service()
{
    /* Call only, if not previously initialized */
    if ( !scorep_metric_management_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize metric management." );

        synchronous_strict_metrics.overall_number_of_metrics = 0;
        synchronous_strict_metrics.sampling_set              = SCOREP_INVALID_SAMPLING_SET;

        for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
        {
            /* Initialize metric source. metric_source_initialize() will return number of'synchronous
             * strict' metrics defined by the user and recorded by every location. */
            synchronous_strict_metrics.counts[ i ] = scorep_metric_sources[ i ]->metric_source_initialize();

            /* Number of active metrics (valid up to now) indicates the required offset we have to use,
             * if we want to store values from this source in the shared values array at a later time. */
            synchronous_strict_metrics.offsets[ i ] = synchronous_strict_metrics.overall_number_of_metrics;

            /* Update number of active metrics */
            synchronous_strict_metrics.overall_number_of_metrics += synchronous_strict_metrics.counts[ i ];
        }

        /* Set initialization flag */
        scorep_metric_management_initialized = true;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of metric management done." );
    }

    return SCOREP_SUCCESS;
}

/** @brief Service finalization.
 */
static void
scorep_metric_finalize_service()
{
    /* Call only, if previously initialized */
    if ( scorep_metric_management_initialized )
    {
        for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
        {
            /* Finalize metric source */
            scorep_metric_sources[ source_index ]->metric_source_finalize();

            /* Reset number of requested metrics for recent source */
            synchronous_strict_metrics.counts[ source_index ] = 0;
        }
        synchronous_strict_metrics.overall_number_of_metrics = 0;
        synchronous_strict_metrics.sampling_set              = SCOREP_INVALID_SAMPLING_SET;

        free( synchronous_strict_metrics.metrics );

        /* Set initialization flag */
        scorep_metric_management_initialized = false;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalization of metric management done." );
    }
}

/** @brief Initialize per-location data structures.
 *
 *  @param location             Location data.
 *  @param data                 Additional user-defined data (not used yet).
 */
static void
initialize_location_metric_cb( SCOREP_Location* location,
                               void*            data )
{
    /* Call only, if previously initialized */
    if ( scorep_metric_management_initialized )
    {
        /* Get the thread local data related to metrics */
        SCOREP_Metric_LocationData* metric_data =
            SCOREP_Location_GetSubsystemData( location, scorep_metric_subsystem_id );
        UTILS_ASSERT( metric_data != NULL );

        /* Collection of event sets for all metric sources and metric types */
        SCOREP_Metric_EventSet** event_set_collection[ SCOREP_NUMBER_OF_METRIC_SOURCES ];

        /* Number of elements in metric values buffer */
        uint64_t buffer_size = 0;

        /* Initialize metric sources for this location */
        for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
        {
            /* Set up event sets for this location */
            event_set_collection[ source_index ] = scorep_metric_sources[ source_index ]->metric_source_initialize_location( location );

            /* Event sets of 'synchronous strict' metrics can be already set,
             * all other metric types will be handled later on */
            metric_data->event_set[ source_index ] = event_set_collection[ source_index ][ SYNCHRONOUS_STRICT_METRICS_INDEX ];
        }

        /* First entries in metric values buffer are always used by 'synchronous strict' metrics which are recorded by each location */
        buffer_size += synchronous_strict_metrics.overall_number_of_metrics;

        /*
         * First: Define metric and sampling set handles of global 'synchronous strict' metrics
         */

        /* Only the master thread defines handles of 'synchronous strict' metrics */
        if ( synchronous_strict_metrics.overall_number_of_metrics > 0
             && SCOREP_Location_GetId( location ) == 0 )
        {
            /* Now we know how many metrics are used, so we can allocate memory to store their handles */
            synchronous_strict_metrics.metrics = malloc( synchronous_strict_metrics.overall_number_of_metrics * sizeof( SCOREP_MetricHandle ) );
            UTILS_ASSERT( synchronous_strict_metrics.metrics );

            uint64_t recent_metric_index = 0;
            for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
            {
                for ( uint32_t j = 0; j < synchronous_strict_metrics.counts[ i ]; j++ )
                {
                    /* Get properties of used metrics */
                    SCOREP_Metric_Properties props = scorep_metric_sources[ i ]->metric_source_props( metric_data->event_set[ i ], j );

                    /* Write metric member definition */
                    SCOREP_MetricHandle metric_handle = SCOREP_DefineMetric( props.name,
                                                                             props.description,
                                                                             props.source_type,
                                                                             props.mode,
                                                                             props.value_type,
                                                                             props.base,
                                                                             props.exponent,
                                                                             props.unit,
                                                                             props.profiling_type );
                    synchronous_strict_metrics.metrics[ recent_metric_index ] = metric_handle;
                    recent_metric_index++;
                }
            }

            /* Write definition of sampling set */
            synchronous_strict_metrics.sampling_set = SCOREP_DefineSamplingSet( ( uint8_t )synchronous_strict_metrics.overall_number_of_metrics,
                                                                                synchronous_strict_metrics.metrics,
                                                                                SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT );
        }


        /*
         * Second: Handle different kinds of additional metrics.
         *         Define metric and scoped sampling set handles of
         *         each metric type.
         */

        /* Temporary store for all metric handles of a sampling set */
        SCOREP_MetricHandle* all_additional_metric_handles = NULL;

        /* Values of additional metrics are stored after values of
        * 'synchronous strict' ones. Therefore, number of 'synchronous
        * strict' metrics defines first offset in values array. */
        uint32_t offset = synchronous_strict_metrics.overall_number_of_metrics;

        /* Reference to previously used location metric set data structure */
        SCOREP_Metric_LocationMetricSet* previous_location_metric_set = NULL;
        for ( uint32_t metric_type = PER_PROCESS_METRICS_INDEX; metric_type < NUMBER_OF_RESERVED_METRICS; metric_type++ )
        {
            /* Location metric set data structure to work with */
            SCOREP_Metric_LocationMetricSet* current_location_metric_set = NULL;
            /* Number of requested metrics per source */
            uint32_t current_metrics_vector[ SCOREP_NUMBER_OF_METRIC_SOURCES ];
            /* Sum of requested metrics */
            uint32_t current_overall_number_of_metrics = 0;

            /* Get number of requested metrics */
            for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
            {
                current_metrics_vector[ source_index ] =
                    scorep_metric_sources[ source_index ]->metric_source_num_of_metrics( event_set_collection[ source_index ][ metric_type ] );
                current_overall_number_of_metrics += current_metrics_vector[ source_index ];
            }

            /* The user requested some metrics of currently processed type (e.g. per-process metrics) */
            if ( current_overall_number_of_metrics > 0 )
            {
                /* Create a new location metric set */
                current_location_metric_set = malloc( sizeof( SCOREP_Metric_LocationMetricSet ) );
                UTILS_ASSERT( current_location_metric_set );

                for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
                {
                    current_location_metric_set->metrics_counts[ source_index ]  = current_metrics_vector[ source_index ];
                    current_location_metric_set->metrics_offsets[ source_index ] = offset;
                    offset                                                      += current_metrics_vector[ source_index ];
                }

                buffer_size += current_overall_number_of_metrics;

                /* Allocate memory to store metric handles */
                all_additional_metric_handles = realloc( all_additional_metric_handles, current_overall_number_of_metrics * sizeof( SCOREP_MetricHandle ) );
                UTILS_ASSERT( all_additional_metric_handles );

                uint64_t recent_metric_index = 0;
                for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
                {
                    /* Store corresponding event sets */
                    current_location_metric_set->event_set[ source_index ] = event_set_collection[ source_index ][ metric_type ];

                    for ( uint32_t metric_index = 0; metric_index < current_metrics_vector[ source_index ]; metric_index++ )
                    {
                        /* Get properties of used metrics */
                        SCOREP_Metric_Properties props =
                            scorep_metric_sources[ source_index ]->metric_source_props( event_set_collection[ source_index ][ metric_type ], metric_index );

                        /* Write metric member definition */
                        SCOREP_MetricHandle metric_handle = SCOREP_DefineMetric( props.name,
                                                                                 props.description,
                                                                                 props.source_type,
                                                                                 props.mode,
                                                                                 props.value_type,
                                                                                 props.base,
                                                                                 props.exponent,
                                                                                 props.unit,
                                                                                 props.profiling_type );
                        all_additional_metric_handles[ recent_metric_index++ ] = metric_handle;
                    }
                }

                /* Write definition of sampling set */
                SCOREP_SamplingSetHandle current_sampling_set_handle = SCOREP_INVALID_SAMPLING_SET;
                current_sampling_set_handle = SCOREP_DefineSamplingSet( ( uint8_t )current_overall_number_of_metrics,
                                                                        all_additional_metric_handles,
                                                                        SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS );

                /* Get handle of current location as recorder */
                SCOREP_LocationHandle    current_location_handle            = SCOREP_Location_GetLocationHandle( location );
                SCOREP_SamplingSetHandle current_scoped_sampling_set_handle = SCOREP_INVALID_SAMPLING_SET;
                SCOREP_MetricScope       scope;
                SCOREP_AnyHandle         scope_handle;

                /* Determine scope and corresponding handle */
                if ( metric_type == PER_PROCESS_METRICS_INDEX )
                {
                    scope        = SCOREP_METRIC_SCOPE_LOCATION_GROUP;
                    scope_handle = SCOREP_GetLocationGroup();
                    if ( scope_handle == SCOREP_INVALID_LOCATION_GROUP )
                    {
                        UTILS_WARNING( "Can not get handle for location group." );
                    }
                }

                current_location_metric_set->sampling_set = SCOREP_DefineScopedSamplingSet( current_sampling_set_handle,
                                                                                            current_location_handle,
                                                                                            scope,
                                                                                            scope_handle );
                current_location_metric_set->next = previous_location_metric_set;
                previous_location_metric_set      = current_location_metric_set;

                /* Set list of additional metrics for this location */
                metric_data->additional_metrics = current_location_metric_set;
            } // END 'if ( current_overall_number_of_metrics > 0 )'
        }     // END 'for all metric types'

        /* Allocate memory for metric values buffer */
        if ( buffer_size != 0 )
        {
            metric_data->values = malloc( buffer_size * sizeof( uint64_t ) );
            UTILS_ASSERT( metric_data->values );
            metric_data->has_metrics = true;
        }
        else
        {
            metric_data->values = NULL;
        }

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric management has initialized location." );
    }
}

/** @brief  Location specific initialization function for metric services.
 *
 *  @param location             Location data.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_initialize_location( SCOREP_Location* location )
{
    UTILS_ASSERT( location != NULL );

    /* Create per-location metric management data */
    SCOREP_Metric_LocationData* metric_data =
        SCOREP_Location_AllocForMisc( location,
                                      sizeof( *metric_data ) );

    SCOREP_Location_SetSubsystemData( location,
                                      scorep_metric_subsystem_id,
                                      metric_data );

    metric_data->has_metrics        = false;
    metric_data->additional_metrics = NULL;
    metric_data->values             = NULL;

    /* All initialization is done in separate function that is re-used
     * by SCOREP_Metric_Reinitialize() */
    initialize_location_metric_cb( location, NULL );

    return SCOREP_SUCCESS;
}

/** @brief Finalize per-location data structures.
 *
 *  @param location             Location data.
 *  @param data                 Additional user-defined data (not used yet).
 */
static void
finalize_location_metric_cb( SCOREP_Location* location,
                             void*            data )
{
    /* Call only, if previously initialized */
    if ( scorep_metric_management_initialized )
    {
        /* Get the thread local data related to metrics */
        SCOREP_Metric_LocationData* metric_data =
            SCOREP_Location_GetSubsystemData( location, scorep_metric_subsystem_id );
        UTILS_ASSERT( metric_data != NULL );

        SCOREP_Metric_LocationMetricSet* location_metric_set = metric_data->additional_metrics;
        SCOREP_Metric_LocationMetricSet* tmp;
        while ( location_metric_set != NULL )
        {
            /* For each metric source (e.g. PAPI or Resource Usage) */
            for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
            {
                /* Check whether this metric source has additional metrics */
                if ( location_metric_set->metrics_counts[ source_index ] > 0 )
                {
                    /* Free event set of additional metric */
                    scorep_metric_sources[ source_index ]->metric_source_free_additional_metric_event_set( location_metric_set->event_set[ source_index ] );
                }
            }
            /* Save pointer to currently handled metric set */
            tmp = location_metric_set;
            /* Set location_metric_set to next metric set*/
            location_metric_set = location_metric_set->next;
            /* Free currently handled metric set */
            free( tmp );
        }

        /* Handle synchronous strict metrics and finalize location in metric source */
        for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
        {
            scorep_metric_sources[ source_index ]->metric_source_finalize_location( metric_data->event_set[ source_index ] );
        }

        free( metric_data->values );

        metric_data->has_metrics = false;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric management has finalized location." );
    }
}

/** @brief Location specific finalization function for metric services.
 *
 *  @param location             Location data.
 *
 *  @param location Reference to location that will finalize its metric related data structures.
 */
static void
scorep_metric_finalize_location( SCOREP_Location* location )
{
    /* All finalization is done in separate function that is re-used
     * by SCOREP_Metric_Reinitialize() */
    finalize_location_metric_cb( location, NULL );
}


/* *********************************************************************
 * Functions called directly by measurement environment
 **********************************************************************/

/** @brief  Get recent values of all metrics.
 *
 *  @param location             Location data.
 *
 *  @return Returns pointer to value array filled with recent metric values,
 *          or NULL if we don't have metrics to read from.
 */
uint64_t*
SCOREP_Metric_Read( SCOREP_Location* location )
{
    /* Call only if previously initialized */
    if ( !scorep_metric_management_initialized )
    {
        return NULL;
    }

    /* Get the thread local data related to metrics */
    SCOREP_Metric_LocationData* metric_data =
        SCOREP_Location_GetSubsystemData( location, scorep_metric_subsystem_id );
    UTILS_ASSERT( metric_data != NULL );

    if ( !metric_data->has_metrics )
    {
        /* Location does not record any metrics */
        return NULL;
    }

    /* If we want to record metrics we need memory to store values */
    assert( metric_data->values != NULL );

    /* Handle 'synchronous strict' metrics */
    for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
    {
        if ( synchronous_strict_metrics.counts[ source_index ] > 0 )
        {
            /* Read values of selected metric */
            scorep_metric_sources[ source_index ]->metric_source_read( metric_data->event_set[ source_index ],
                                                                       &metric_data->values[ synchronous_strict_metrics.offsets[ source_index ] ] );
        }
    }

    /* Handle additional scoped metric */
    SCOREP_Metric_LocationMetricSet* location_metric_set = metric_data->additional_metrics;
    while ( location_metric_set != NULL )
    {
        for ( size_t source_index = 0; source_index < SCOREP_NUMBER_OF_METRIC_SOURCES; source_index++ )
        {
            if ( location_metric_set->metrics_counts[ source_index ] > 0 )
            {
                /* Read values of selected metric */
                scorep_metric_sources[ source_index ]->metric_source_read( location_metric_set->event_set[ source_index ],
                                                                           &( metric_data->values[ location_metric_set->metrics_offsets[ source_index ] ] ) );
            }
        }
        location_metric_set = location_metric_set->next;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric management has read metric values." );

    return metric_data->values;
}

/** @brief  Reinitialize metric management. This functionality is used by
 *          Score-P Online Access to change recorded metrics between
 *          separate phases of program execution.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
SCOREP_ErrorCode
SCOREP_Metric_Reinitialize()
{
    /* Finalize each location (frees internal buffers) */
    SCOREP_Location_ForAll( finalize_location_metric_cb, NULL );

    /* Finalize metric service */
    scorep_metric_finalize_service();

    /* Reinitialize metric service */
    scorep_metric_initialize_service();

    /* Reinitialize each location */
    SCOREP_Location_ForAll( initialize_location_metric_cb, NULL );

    return SCOREP_SUCCESS;
}

/** @brief  Returns the sampling set handle to the measurement system.
 *
 *  @return Returns the sampling set handle to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetSamplingSet()
{
    return synchronous_strict_metrics.sampling_set;
}

/** @brief  Returns the handle of a synchronous metric to the measurement system.
 *
 *  @return Returns the handle of a synchronous metric to the measurement system.
 */
SCOREP_MetricHandle
SCOREP_Metric_GetSynchronousStrictMetricHandle( uint32_t index )
{
    assert( index < synchronous_strict_metrics.overall_number_of_metrics );

    return synchronous_strict_metrics.metrics[ index ];
}

/** @brief  Returns the number of a synchronous metrics.
 *
 *  @return Returns the number of a synchronous metrics.
 */
uint32_t
SCOREP_Metric_GetNumberOfSynchronousStrictMetrics()
{
    return synchronous_strict_metrics.overall_number_of_metrics;
}

/** @brief  Returns all handles of additional scoped metrics for current location.
 *
 *  @return Returns all handles of additional scoped metrics for current location.
 */
SCOREP_MetricHandle*
SCOREP_Metric_GetAdditionalScopedMetricHandles( SCOREP_Location* location )
{
    /* Get the thread local data related to metrics */
    SCOREP_Metric_LocationData* metric_data =
        SCOREP_Location_GetSubsystemData( location, scorep_metric_subsystem_id );
    UTILS_ASSERT( metric_data != NULL );

    if ( !metric_data->has_metrics )
    {
        /* Location does not record any metrics */
        return NULL;
    }

    uint32_t number_of_additional_metrics =
        SCOREP_Metric_GetNumberOfAdditionalScopedMetrics( location );

    if ( number_of_additional_metrics == 0 )
    {
        /* Location does not record any additional metrics */
        return NULL;
    }

    SCOREP_MetricHandle* metric_handles = malloc( number_of_additional_metrics * sizeof( SCOREP_MetricHandle ) );
    UTILS_ASSERT( metric_handles );

    /* Process additional metrics */
    SCOREP_Metric_LocationMetricSet* location_metric_set = metric_data->additional_metrics;
    uint32_t                         index               = 0;
    while ( location_metric_set != NULL )
    {
        /* Check array bounds */
        UTILS_ASSERT( index < number_of_additional_metrics );

        /* Within current scope level (e.g. per process level) do:
         *
         * -> For each metric source */
        for ( uint32_t metric_src = 0; metric_src < SCOREP_NUMBER_OF_METRIC_SOURCES; ++metric_src )
        {
            /* -> For metric that is recorded by current source and scope level */
            for ( uint32_t j = 0; j < location_metric_set->metrics_counts[ metric_src ]; ++j )
            {
                /* SCOREP_GetSamplingSet always returns a sampling
                 * set handle, no matter if the given metric handle
                 * is a normal or scoped one. */
                SCOREP_SamplingSetHandle sampling_set = SCOREP_GetSamplingSet( location_metric_set->sampling_set );
                if ( sampling_set == SCOREP_INVALID_SAMPLING_SET )
                {
                    continue;
                }

                /* -> For each individual metric contained in this sampling set */
                SCOREP_SamplingSet_Definition* sampling_set_definition = SCOREP_LOCAL_HANDLE_DEREF( sampling_set, SamplingSet );
                for ( uint32_t metric_index = 0; metric_index < sampling_set_definition->number_of_metrics; ++metric_index )
                {
                    metric_handles[ index ] = sampling_set_definition->metric_handles[ metric_index ];
                    index++;
                }
            }
        }

        /* Go to next scope level */
        location_metric_set = location_metric_set->next;
    }

    return metric_handles;
}

/** @brief Writes all metrics of a location in tracing mode.
 *
 *  @param location             Location data.
 *  @param timestamp            Time when event occurred.
 */void
SCOREP_Metric_WriteToTrace( SCOREP_Location* location,
                            uint64_t         timestamp )
{
    /* Get the thread local data related to metrics */
    SCOREP_Metric_LocationData* metric_data =
        SCOREP_Location_GetSubsystemData( location, scorep_metric_subsystem_id );
    UTILS_ASSERT( metric_data != NULL );

    if ( !metric_data->has_metrics )
    {
        /* Location does not record any metrics */
        return;
    }

    /* If we want to record metrics we need memory to store values */
    assert( metric_data->values != NULL );

    /* (1) 'synchronous strict' metrics */
    if ( synchronous_strict_metrics.sampling_set != SCOREP_INVALID_SAMPLING_SET )
    {
        SCOREP_Tracing_Metric( location,
                               timestamp,
                               synchronous_strict_metrics.sampling_set,
                               metric_data->values );
    }

    /* (2) additional metric types */
    SCOREP_Metric_LocationMetricSet* location_metric_set = metric_data->additional_metrics;
    while ( location_metric_set != NULL )
    {
        SCOREP_Tracing_Metric( location,
                               timestamp,
                               location_metric_set->sampling_set,
                               &( metric_data->values[ location_metric_set->metrics_offsets[ 0 ] ] ) );

        location_metric_set = location_metric_set->next;
    }
}

/** @brief Get number of additional scorep metrics recorded by @a location.
 *
 *  @param location             Location data.
 */
uint32_t
SCOREP_Metric_GetNumberOfAdditionalScopedMetrics( SCOREP_Location* location )
{
    /* Get the thread local data related to metrics */
    SCOREP_Metric_LocationData* metric_data =
        SCOREP_Location_GetSubsystemData( location, scorep_metric_subsystem_id );
    UTILS_ASSERT( metric_data != NULL );

    SCOREP_Metric_LocationMetricSet* location_metric_set =
        metric_data->additional_metrics;

    uint32_t number_of_additional_scorep_metrics = 0;

    /* One location_metric_set for each scope level (e.g. system tree level) */
    while ( location_metric_set != NULL )
    {
        /* Get number of metrics for each metric source within current scope level */
        for ( uint32_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; ++i )
        {
            number_of_additional_scorep_metrics += location_metric_set->metrics_counts[ i ];
        }
        /* Go to next scope level */
        location_metric_set = location_metric_set->next;
    }

    return number_of_additional_scorep_metrics;
}

/* *********************************************************************
 * Subsystem declaration
 **********************************************************************/

/**
 * Implementation of the metric service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Metric_Service =
{
    "METRIC",
    &scorep_metric_register,
    &scorep_metric_initialize_service,
    &scorep_metric_initialize_location,
    &scorep_metric_finalize_location,
    &scorep_metric_finalize_service,
    &scorep_metric_deregister
};
