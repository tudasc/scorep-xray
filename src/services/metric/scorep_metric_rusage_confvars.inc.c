/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
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

#include <SCOREP_Config.h>

/** Contains the name of requested metrics. */
static char* scorep_metric_rusage = NULL;

/** Contains the name of requested per-process metrics. */
static char* scorep_metric_rusage_per_process = NULL;

/** Contains the separator of metric names. */
static char* scorep_metric_rusage_separator = NULL;

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
static const SCOREP_ConfigVariable scorep_metric_rusage_confvars[] = {
    {
        "rusage",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metric_rusage,
        NULL,
        "",
        "Resource usage metric names to measure",
        "List of requested resource usage metric names that will be collected during program run."
    },
    {
        "rusage_per_process",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metric_rusage_per_process,
        NULL,
        "",
        "Resource usage metric names to measure per-process",
        "List of requested resource usage metric names that will be recorded only by first thread of a process."
    },
    {
        "rusage_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metric_rusage_separator,
        NULL,
        ",",
        "Separator of resource usage metric names",
        "Character that separates metric names in SCOREP_METRIC_RUSAGE and "
        "SCOREP_METRIC_RUSAGE_PER_PROCESS."
    },
    SCOREP_CONFIG_TERMINATOR
};
