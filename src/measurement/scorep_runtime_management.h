/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
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
 * @note The directory is only available if any substrate actually needs it.
 * In this case, SCOREP_GetExperimentDirName() returns NULL
 *
 * @todo rename directory in scorep_finalize().
 */
const char*
SCOREP_GetExperimentDirName( void );


/**
 * Create a directory with a temporary name (accessible via
 * SCOREP_GetExperimentDirName()) to store all measurement data inside.
 *
 * @note There is a MPI implementation and a non-MPI implementation.
 *
 */
void
SCOREP_CreateExperimentDir( void );


void
SCOREP_RenameExperimentDir( void );

/**
 * Called by the tracing component before a buffer flush happens.
 */
void
SCOREP_OnTracingBufferFlushBegin( bool final );


/**
 * Called by the tracing component after a buffer flush happened.
 */
void
SCOREP_OnTracingBufferFlushEnd( uint64_t timestamp );


/**
 * Returns the directory the application was started in. May abort if
 * directory cannot be obtained.
 */
const char*
SCOREP_GetWorkingDirectory( void );

#endif /* SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H */
