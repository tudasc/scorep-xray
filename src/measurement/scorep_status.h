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
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_STATUS_H
#define SCOREP_STATUS_H



/**
 * @file
 *
 *
 */


#include <stdbool.h>


void
SCOREP_Status_Initialize( void );

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


/**
 * Indicates whether we are a MPP application or not.
 * This eases the initialization process.
 *
 * @return In MPP mode return true, false otherwise.
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
 * Indicates whether MPP initialization (e.g., MPI_Init()) was already called or not. In non-MPP mode always true.
 * Not suitable to check is this is in MPP program. For that, use SCOREP_Status_IsMpp() instead.
 *
 * @return In MPP mode true if initialization (e.g., MPI_Init()) has been called earlier, false otherwise.
 *         In non-MPP mode true after SCOREP_Status_OnMppInit() was called.
 */
bool
SCOREP_Status_IsMppInitialized( void );


/**
 * Indicates whether MPP finalization (e.g., MPI_Finalize()) was already called or not. In non-MPP mode always true.
 *
 * @return In MPP mode true if finalization (e.g., MPI_Finalize()) has been called before, false otherwise.
 *         In non-MPP mode always true.
 */
bool
SCOREP_Status_IsMppFinalized( void );


/**
 * Indicates whether current process is master on this node.
 */
bool
SCOREP_Status_IsProcessMasterOnNode( void );


#endif /* SCOREP_STATUS_H */
