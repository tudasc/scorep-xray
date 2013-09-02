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

#ifndef SCOREP_STATUS_H
#define SCOREP_STATUS_H



/**
 * @file       src/measurement/scorep_status.h
 *
 *
 */


#include <stdbool.h>


// different impl for MPI and non-MPI
void
SCOREP_Status_Initialize( void );

// different impl for MPI and non-MPI
void
SCOREP_Status_Finalize( void );

bool
SCOREP_IsTracingEnabled( void );


bool
SCOREP_IsProfilingEnabled( void );

bool
SCOREP_IsOAEnabled( void );

void
SCOREP_Status_OnOtf2Flush( void );


bool
SCOREP_Status_HasOtf2Flushed( void );


bool
SCOREP_Status_IsExperimentDirCreated( void );


void
SCOREP_OnExperimentDirCreation( void );


/**
 * Indicates whether we are a MPI application or not. This eases the
 * initialization process.
 *
 * @return In MPI mode return true, false otherwise.
 */
bool
SCOREP_Status_IsMpp( void );


int
SCOREP_Status_GetRank( void );


int
SCOREP_Status_GetSize( void );


void
SCOREP_Status_OnMppInit( void );


void
SCOREP_Status_OnMppFinalize( void );


/**
 * Indicates whether MPI_Init() was already called or not. In non-MPI mode always true
 *
 * @return In MPI mode true if MPI_Init() has been called earlier, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SCOREP_Status_IsMppInitialized( void );


/**
 * Indicates whether MPI_Finalize() was already called or not. In non-MPI mode always true
 *
 * @return In MPI mode true if MPI_Finalize() has been called before, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SCOREP_Status_IsMppFinalized( void );


/**
 * Indicates whether current process is master on this node.
 */
bool
SCOREP_Status_IsProcessMasterOnNode( void );


#endif /* SCOREP_STATUS_H */
