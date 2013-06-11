/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/services/metric/scorep_metric_rusage_confvars.inc.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


/** Contains the name of requested metrics. */
static char* scorep_metrics_rusage = NULL;

/** Contains the name of requested per-process metrics. */
static char* scorep_metrics_rusage_per_process = NULL;

/** Contains the separator of metric names. */
static char* scorep_metrics_rusage_separator = NULL;

/** List of configuration variables for the resource usage metric source.
 *
 *  This list contains variables to specify 'synchronous strict' and
 *  per-process metrics. Furthermore, a variable for the character
 *  that separates single metric names is defined.
 *
 *  Configuration variables for the resource usage metric source.
 *  Current configuration variables are:
 *  @li @c SCOREP_METRIC_RUSAGE list of requested metric names.
 *  @li @c SCOREP_METRIC_RUSAGE_PER_PROCESS list of requested metric names recorded per-process.
 *  @li @c SCOREP_METRIC_RUSAGE_SEP character that separates single metric names.
 */
static SCOREP_ConfigVariable scorep_metric_rusage_configs[] = {
    {
        "rusage",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_rusage,
        NULL,
        "",
        "Resource usage metric names to measure",
        "List of requested resource usage metric names that will be collected during program run."
    },
    {
        "rusage_per_process",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_rusage_per_process,
        NULL,
        "",
        "Resource usage metric names to measure per-process",
        "List of requested resource usage metric names that will be recorded only by first thread of a process."
    },
    {
        "rusage_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_rusage_separator,
        NULL,
        ",",
        "Separator of resource usage metric names.",
        "Character that separates metric names in SCOREP_METRIC_RUSAGE and "
        "SCOREP_METRIC_RUSAGE_PER_PROCESS."
    },
    SCOREP_CONFIG_TERMINATOR
};
