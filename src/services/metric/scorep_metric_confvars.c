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
 * @file       src/services/metric/scorep_metric_confvars.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>


#include <UTILS_Error.h>
#include <UTILS_Debug.h>


#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME METRIC


#include "scorep_metric_papi_confvars.inc.c"

#include "scorep_metric_rusage_confvars.inc.c"

static SCOREP_ErrorCode
scorep_metric_register( size_t subsystem_id )
{
    SCOREP_ConfigRegisterCond( "metric",
                               scorep_metric_papi_configs,
                               HAVE_BACKEND_PAPI );
    SCOREP_ConfigRegisterCond( "metric",
                               scorep_metric_rusage_configs,
                               HAVE_BACKEND_GETRUSAGE );

    return SCOREP_SUCCESS;
}

/**
 * Implementation of the metric service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Metric_Service =
{
    "METRIC (config variables only)",
    &scorep_metric_register,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
