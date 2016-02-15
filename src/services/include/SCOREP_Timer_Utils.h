/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_TIMER_UTILS_H
#define SCOREP_TIMER_UTILS_H

/**
 * @file
 *
 * @brief Provide platform independent timing functionality.
 */

#include <stdint.h>
#include <stdbool.h>


/**
 * Register config variables to select one of the available timers.
 */
void
SCOREP_Timer_Register( void );


/**
 * Initialize Timer before usage of SCOREP_Timer_GetClockTicks() and
 * SCOREP_Timer_GetClockResolution(). Multiple calls do no harm.
 *
 * @note For performance reasons we don't check if the timer is initialized in
 * SCOREP_Timer_GetClockTicks() and SCOREP_Timer_GetClockResolution().
 *
 * @note The init function is not thread safe.
 */
void
SCOREP_Timer_Initialize( void );


/**
 * Platform independent clock resolution in order to convert clock ticks
 * obtained via SCOREP_Timer_GetClockTicks() to seconds. This function is
 * supposed to be used during finalization only as it may, depending on the
 * selected timer, interpolate the frequency. This interpolation is done once
 * and accuracy increases with longer interpolation period.
 *
 * @return Clock resolution in ticks/second.
 */
uint64_t
SCOREP_Timer_GetClockResolution( void );


/**
 * @return true if we have a global clock, e.g. on Blue Gene systems. Usually false.
 */
bool
SCOREP_Timer_ClockIsGlobal( void );


#endif /* SCOREP_TIMER_UTILS_H */
