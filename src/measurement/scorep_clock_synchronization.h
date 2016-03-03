/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
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
