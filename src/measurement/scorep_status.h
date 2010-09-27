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

#ifndef SCOREP_STATUS_H
#define SCOREP_STATUS_H



/**
 * @file       scorep_status.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdbool.h>


// different impl for MPI and non-MPI
void
SCOREP_Status_Initialize();


bool
SCOREP_IsTracingEnabled();


bool
SCOREP_IsProfilingEnabled();

bool
SCOREP_IsOAEnabled();

void
SCOREP_Otf2_OnFlush();


bool
SCOREP_Otf2_HasFlushed();


bool
SCOREP_IsExperimentDirCreated();


void
SCOREP_OnExperimentDirCreation();

#endif /* SCOREP_STATUS_H */
