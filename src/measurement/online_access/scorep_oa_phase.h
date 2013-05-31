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

#ifndef SCOREP_OA_PHASE_H_
#define SCOREP_OA_PHASE_H_

/**
 * @file        scorep_oa_phase.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declarations of Online Access phase enter and exit functions.
 *
 * @status alpha
 */

#include <SCOREP_Types.h>

void
scorep_oa_phase_enter
(
    const SCOREP_RegionHandle handle
);

void
scorep_oa_phase_exit
(
    const SCOREP_RegionHandle handle
);

#endif /* SCOREP_OA_PHASE_H_ */
