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

#include <config.h>

#include <SCOREP_Subsystem.h>

#include "scorep_metric_papi_confvars.inc.c"

#include "scorep_metric_rusage_confvars.inc.c"

#include "scorep_metric_plugins_confvars.inc.c"

#include "scorep_metric_perf_confvars.inc.c"

static SCOREP_ErrorCode
metric_subsystem_register( size_t subsystem_id )
{
    SCOREP_ConfigRegisterCond( "metric",
                               scorep_metric_papi_confvars,
                               HAVE_BACKEND_PAPI );
    SCOREP_ConfigRegisterCond( "metric",
                               scorep_metric_rusage_confvars,
                               HAVE_BACKEND_GETRUSAGE );
    SCOREP_ConfigRegisterCond( "metric",
                               scorep_metric_plugins_confvars,
                               HAVE_BACKEND_DLFCN_SUPPORT );
    SCOREP_ConfigRegisterCond( "metric",
                               scorep_metric_perf_confvars,
                               HAVE_BACKEND_METRIC_PERF );

    return SCOREP_SUCCESS;
}

/**
 * Implementation of the metric service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Subsystem_MetricService =
{
    .subsystem_name     = "METRIC (config variables only)",
    .subsystem_register = &metric_subsystem_register,
};
