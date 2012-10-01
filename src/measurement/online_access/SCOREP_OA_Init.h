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

#include <stdint.h>
#include <stdbool.h>

extern bool scorep_oa_is_requested;

#define SCOREP_OA_IS_REQUESTED            ( scorep_oa_is_requested )

int8_t
SCOREP_OA_Init
(
);

int8_t
SCOREP_OA_Initialized
(
);

void
SCOREP_OA_Finalize
(
);

void
SCOREP_OA_Register
(
);

#endif /* SCOREP_OA_INIT_H */
