/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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


#ifndef SCOREP_CLOCK_SYNCHRONIZATION_H
#define SCOREP_CLOCK_SYNCHRONIZATION_H

/**
 * @file
 *
 *
 */


#include <stdint.h>

void
SCOREP_SynchronizeClocks( void );

void
SCOREP_BeginEpoch( void );

void
SCOREP_EndEpoch( void );

void
SCOREP_GetGlobalEpoch( uint64_t* globalEpochBegin,
                       uint64_t* globalEpochEnd );


#endif /* SCOREP_CLOCK_SYNCHRONIZATION_H */
