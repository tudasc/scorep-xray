/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @status     alpha
 * @file       SCOREP_Metric_Init.c
 * @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 * @brief Metric service interface support to the measurement system.
 *
 * This file contains metric service initialization and finalization
 * functions which are common for all metric services. All extern
 * functions are implemented by @ref scorep_metric_management.c. This
 * mangement instance handles different metric sources.
 */

#include <config.h>
#include <stdio.h>

#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_utility/SCOREP_Utils.h>

#include "SCOREP_Metric.h"
#include "SCOREP_Types.h"
#include "scorep_utility/SCOREP_Error.h"

/**
   The service initialize function is metric specific. Thus it is contained in each
   metric service implementation.
 */
extern SCOREP_Error_Code
scorep_metric_initialize_service();

/**
   The service finalize function is metric specific. Thus it is contained in each
   metric service implementation.
 */
extern void
scorep_metric_finalize_service();

/**
   Registers configuration variables for the metric services.
 */
extern SCOREP_Error_Code
scorep_metric_register();

/**
   Location specific initialization function for metric services.
 */
extern SCOREP_Error_Code
scorep_metric_initialize_location();

/**
   Location specific finalization function for metric services.
 */
extern void
scorep_metric_finalize_location( void* location );

/**
   Called on dereigstration of the metric service.
 */
extern void
scorep_metric_deregister();


/**
   Implementation of the metric service initialization/finalization struct
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
