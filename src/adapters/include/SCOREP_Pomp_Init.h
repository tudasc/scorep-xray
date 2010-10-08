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

#ifndef SCOREP_POMP_INIT_H
#define SCOREP_POMP_INIT_H

/**
 * @file       SCOREP_Pomp_Init.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Declares the intialization struct of the POMP adapter
 */

#include "SCOREP_Adapter.h"

/**
   The POMP adapter struct
 */
extern struct SCOREP_Adapter SCOREP_Pomp_Adapter;

/**
   Flag that indicates whether the POMP2 adapter is initialized.
 */
extern bool scorep_pomp_is_initialized;

#define SCOREP_POMP2_ENSURE_INITIALIZED if ( !scorep_pomp_is_initialized ) { POMP2_Init(); }

#endif // SCOREP_POMP_INIT_H
