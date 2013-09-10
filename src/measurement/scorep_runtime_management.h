/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H
#define SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H



/**
 * @file
 *
 *
 */

#include <SCOREP_Location.h>
#include <stdbool.h>


/**
 * Toplevel relative experiment directory. In the non MPI case a valid name is
 * available after the first call to SCOREP_CreateExperimentDir(). In the MPI
 * case a valid and unique name is available on all processes after the call
 * to SCOREP_CreateExperimentDir() from within SCOREP_InitMppMeasurement().
 *
 * @note The name is a temporary name used during measurement. At
 * scorep_finalize() we rename the temporary directory to something like
 * scorep_<prgname>_nProcs_x_nLocations.
 *
 * @todo rename directory in scorep_finalize().
 */
const char*
SCOREP_GetExperimentDirName( void );


/**
 * Create a directory with a temporary name (accessible via
 * SCOREP_GetExperimentDirName()) to store all measurment data inside.
 *
 * @note There is a MPI implementation and a non-MPI implementation.
 *
 */
void
SCOREP_CreateExperimentDir( void );


void
SCOREP_RenameExperimentDir( void );


#endif /* SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H */
