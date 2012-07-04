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

#ifndef SCOREP_OA_REQUEST_H_
#define SCOREP_OA_REQUEST_H_

/**
 * @file        scorep_oa_request.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief Functions for processing and storing of OA measurements requests
 *
 */

#include "SCOREP_Types.h"

/**
 * Initializes requests submittion, prepares Score-P measurement reconfiguration
 */
void
SCOREP_OA_RequestBegin
(
);

/**
 * Configures Score-P measurment core according to the received measurement requests
 */
void
SCOREP_OA_RequestsSubmit
(
);

/**
 * Adds one measurement request to the requests to configure Score-P measurement core with
 *
 * @param request_string a string containing name of the metric to measure
 */
void
SCOREP_OA_RequestsAddMetric
(
    uint32_t metric_code
);

/**
 * Provides the measurement requests received over OA.
 */
void*
SCOREP_OA_RequestsGet
(
);

/**
 * Frees datastructure allocated for storing requests related information
 */
void
SCOREP_OA_RequestsDismiss
(
);

#endif /* SCOREP_OA_REQUEST_H_ */
