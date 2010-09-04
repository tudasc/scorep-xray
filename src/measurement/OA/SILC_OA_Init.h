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


#ifndef SILC_OA_INIT_H
#define SILC_OA_INIT_H


/**
 * @file        SILC_OA_Init.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA initialization and finalization functions
 *
 * @status alpha
 */

#include "silc_utility/SILC_Utils.h"
//extern int connection;					///@TODO move to SILC_OA_Status.h

SILC_Error_Code
SILC_OA_Init
(
);

int8_t
SILC_OA_Initialized
(
);

SILC_Error_Code
SULC_OA_Finalize
(
);

#endif /* SILC_OA_INIT_H */
