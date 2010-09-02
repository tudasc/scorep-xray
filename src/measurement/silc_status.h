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

#ifndef SILC_STATUS_H
#define SILC_STATUS_H



/**
 * @file       silc_status.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdbool.h>


// different impl for MPI and non-MPI
void
SILC_Status_Initialize();


bool
SILC_IsTracingEnabled();


bool
SILC_IsProfilingEnabled();

bool
SILC_IsOAEnabled();

void
SILC_Otf2_OnFlush();


bool
SILC_Otf2_HasFlushed();


bool
SILC_IsExperimentDirCreated();


void
SILC_OnExperimentDirCreation();

#endif /* SILC_STATUS_H */
