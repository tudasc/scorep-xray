/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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


#ifndef SCOREP_OA_INIT_H
#define SCOREP_OA_INIT_H


/**
 * @file        SCOREP_OA_Init.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA initialization and finalization functions
 *
 * @status alpha
 */

#include "scorep_utility/SCOREP_Utils.h"
//extern int connection;					///@TODO move to SCOREP_OA_Status.h

SCOREP_Error_Code
SCOREP_OA_Init
(
);

int8_t
SCOREP_OA_Initialized
(
);

SCOREP_Error_Code
SULC_OA_Finalize
(
);

#endif /* SCOREP_OA_INIT_H */
