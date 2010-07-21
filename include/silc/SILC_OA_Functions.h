/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SILC_OA_FUNCTIONS_H
#define SILC_OA_FUNCTIONS_H


/**
 * @file        SILC_OA_Functions.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declarations of Online Access module user interface.
 *
 * @status ALPHA
 */


#include "SILC_User_Types.h"

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
SILC_OA_PhaseBegin
(
    const SILC_RegionHandle* handle
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
SILC_OA_PhaseEnd
(
    const SILC_RegionHandle* handle
);


#endif /* SILC_OA_FUNCTIONS_H */
