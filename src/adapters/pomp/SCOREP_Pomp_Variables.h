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

#ifndef SCOREP_POMP_VARIABLES_H
#define SCOREP_POMP_VARIABLES_H

/**
 * @file       SCOREP_Pomp_Variables.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Declaration of global variables, used in several source files of the
          POMP2 adapter.
 */

#include "SCOREP_PublicTypes.h"
#include <stdbool.h>

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
                                                              Defined in SCOREP_Pomp_User.c
****************************************************************************************/

/** Contains the region handle for all implicit barriers */
extern SCOREP_RegionHandle scorep_pomp_implicit_barrier_region;

/** Flag to indicate, wether POMP traceing is enable/disabled */
extern bool scorep_pomp_is_tracing_on;

/** Flag to indicate wether the adapter is initialized */
extern bool scorep_pomp_is_initialized;

/** @} */

#endif // SCOREP_POMP_VARIABLES_H
