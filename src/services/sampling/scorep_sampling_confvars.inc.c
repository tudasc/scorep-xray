/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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
 *
 */

/**
 * Set the sampling event and period: <event_name>(@<period>)
 * Possible values are:
 *  - timer (POSIX timer), period in us, default == 1000
 *  - PAPI events e.g., (PAPI_<event>), period in number of events, default == 1000000
 *  - perf event (perf_[hw|sw|raw]_<event>), period in number of events, default == 1000000
 * As default this value is not set, thus sampling is disabled.
 * Multiple sampling definitions can be set separated via SCOREP_SAMPLING_SEP.
 */
static char* scorep_sampling_events;

/**
 * Used to separate multiple SCOREP_SAMPLING_EVENTS
 */
static char* scorep_sampling_separator;

/**
 * Array of configuration variables.
 * They are registered to the measurement system and are filled during
 * until the initialization function is called.
 */
static const SCOREP_ConfigVariable scorep_sampling_confvars[] = {
    {
        "events",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_sampling_events,
        NULL,
#if HAVE_BACKEND( METRIC_PERF )
        "perf_cycles@10000000",
#elif HAVE_BACKEND( PAPI )
        "PAPI_TOT_CYC@10000000",
#else
        "timer@10000",
#endif
        "Set the sampling event and period: <event>[@<period>]",
        "This selects the interrupt source for sampling.\n"
        "This is only in effect if SCOREP_ENABLE_UNWINDING is on.\n"
        "\n"
        "Possible values:\n"
#if HAVE_BACKEND( METRIC_PERF )
        "\240-\240perf event (perf_<event>, see \"perf\240list\")\n"
        "\240\240\240\240\240period in number of events, default:\24010000000\n"
        "\240\240\240\240\240e.g., perf_cycles@2000000\n"
#endif
#if HAVE_BACKEND( PAPI )
        "\240-\240PAPI event (PAPI_<event>, see \"papi_avail\")\n"
        "\240\240\240\240\240period in number of events, default:\24010000000\n"
        "\240\240\240\240\240e.g., PAPI_TOT_CYC@2000000\n"
#endif
        "\240-\240timer (POSIX timer, invalid for multi-threaded)\n"
        "\240\240\240\240\240period in us, default:\24010000\n"
        "\240\240\240\240\240e.g., timer@2000\n"
        ,
    },
    {
        "sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_sampling_separator,
        NULL,
        ",",
        "Separator of sampling event names",
        "Character that separates sampling event names in SCOREP_SAMPLING_EVENTS"
    },
    SCOREP_CONFIG_TERMINATOR
};
