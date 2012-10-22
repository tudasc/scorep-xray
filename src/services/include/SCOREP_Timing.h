/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_TIMING_H
#define SCOREP_TIMING_H

/**
 * @file        SCOREP_Timing.h
 * @maintainer  Christian Roessel <c.roessel@fz-juelich.de>
 *
 * @brief       Provide platform independant timing functionality.
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * Initialize Timer before usage of SCOREP_GetClockTicks() and
 * SCOREP_GetClockResolution(). Multiple calls do no harm.
 *
 * @note For performance reasons we don't check if the timer is initialized in
 * SCOREP_GetClockTicks() and SCOREP_GetClockResolution().
 *
 * @note The init function is not thread safe.
 */
void
SCOREP_Timer_Initialize( void );


/**
 * Platform independant timing functions to be used for providing timestamps
 * to @ref SCOREP_Events events.
 *
 * @note This may be combined (for performance reasons) with some HWC
 * functionality in future.
 *
 * @return Wall clock time in ticks.
 */
uint64_t
SCOREP_GetClockTicks( void );



/**
 * Platform independant clock resolution in order to convert wall clock times
 * to seconds.
 *
 * @return Clock resolution in ticks/second.
 */
uint64_t
SCOREP_GetClockResolution( void );


/**
 * @return true if we have a global clock, e.g. on BG/P systems. False otherwise.
 */
bool
SCOREP_ClockIsGlobal( void );


#endif /* SCOREP_TIMING_H */
