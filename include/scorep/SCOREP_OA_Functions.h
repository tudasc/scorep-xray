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


#ifndef SCOREP_OA_FUNCTIONS_H
#define SCOREP_OA_FUNCTIONS_H


/**
 * @file        SCOREP_OA_Functions.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declarations of Online Access module user interface.
 *
 * @status ALPHA
 */


#include "SCOREP_User_Types.h"

/**
 * This function is called at the beginning of phase. It first checks whether OA module is configured
 * to suspend application at the current point (determined by EA execution requests). Then the message is sent
 * to respective EA saying that application is suspended at the beginning of the current phase and is
 * waiting for requests. When the requests are received they are parsed and required actions are executed.
 * OA is waiting for the requests inside infinite loop until the execution request is received. After
 * that, all the processes in MPI_COMM_WORLD are synchronized with a call to PMPI_Barrier and application
 * released.
 */

void
SCOREP_OA_PhaseBegin
(
    const SCOREP_RegionHandle* handle
);

/**
 * This function is called at the end of phase. It checks whether OA module is configured to suspend
 * application at this point (determined by EA execution requests). Then the message is sent to EA
 * saying that application is suspended at the end of current phase and is waiting for requests.
 * When the requests are received they are parsed and required actions are executed. OA is waiting for
 * the requests inside infinite loop until the execution request is received. After that application is
 * released.
 */

void
SCOREP_OA_PhaseEnd
(
    const SCOREP_RegionHandle* handle
);


#endif /* SCOREP_OA_FUNCTIONS_H */
