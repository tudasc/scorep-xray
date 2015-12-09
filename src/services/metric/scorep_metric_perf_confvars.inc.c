/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


/** Contains the name of requested metrics. */
static char* scorep_metrics_perf = NULL;

/** Contains the name of requested per-process metrics. */
static char* scorep_metrics_perf_per_process = NULL;

/** Contains the separator of metric names. */
static char* scorep_metrics_perf_separator = NULL;

/**
 *  List of configuration variables for the PERF metric adapter.
 *
 *  This list contains variables to specify 'strictly synchronous' and
 *  per-process metrics. Furthermore, a variable for the character
 *  that separates single metric names is defined.
 *
 *  Current configuration variables are:
 *  @li @c SCOREP_METRIC_PERF list of requested metric names.
 *  @li @c SCOREP_METRIC_PERF_PER_PROCESS list of requested metric names recorded per-process.
 *  @li @c SCOREP_METRIC_PERF_SEP character that separates single metric names.
 */
static const SCOREP_ConfigVariable scorep_metric_perf_confvars[] = {
    {
        "perf",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_perf,
        NULL,
        "",
        "PERF metric names to measure",
        "List of requested PERF metric names that will be collected during program run."
    },
    {
        "perf_per_process",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_perf_per_process,
        NULL,
        "",
        "PERF metric names to measure per-process",
        "List of requested PERF metric names that will be recorded only by first thread of a process."
    },
    {
        "perf_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_perf_separator,
        NULL,
        ",",
        "Separator of PERF metric names",
        "Character that separates metric names in SCOREP_METRIC_PERF and "
        "SCOREP_METRIC_PERF_PER_PROCESS."
    },
    SCOREP_CONFIG_TERMINATOR
};
