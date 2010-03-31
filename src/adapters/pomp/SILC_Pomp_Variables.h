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

#ifndef SILC_POMP_VARIABLES_H
#define SILC_POMP_VARIABLES_H

/**
 * @file       SILC_Pomp_Variables.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Declaration of global variables, used in several source files of the
          POMP adapter.
 */

#include "SILC_PublicTypes.h"
#include <stdbool.h>

/** @ingroup POMP
    @{
 */

/* **************************************************************************************
                                                              Defined in SILC_Pomp_User.c
****************************************************************************************/

/** Contains the region handle for all implicit barriers */
extern SILC_RegionHandle silc_pomp_implicit_barrier_region;

/** Flag to indicate, wether POMP traceing is enable/disabled */
extern bool silc_pomp_is_tracing_on;

/** Flag to indicate wether the adapter is initialized */
extern bool silc_pomp_is_initialized;

/** @} */

#endif // SILC_POMP_VARIABLES_H
