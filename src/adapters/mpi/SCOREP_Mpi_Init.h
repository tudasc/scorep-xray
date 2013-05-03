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

#ifndef SCOREP_MPI_INIT_H
#define SCOREP_MPI_INIT_H

/** @file SCOREP_Mpi_Init.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA
    @ingroup    MPI_Wrapper

    @brief Contains the initialization function definitions for the measurement system.
 */

#include "SCOREP_Subsystem.h"

/** The struct which contain the init functions */
extern const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter;

#endif // SCOREP_MPI_INIT_H
