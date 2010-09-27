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

#ifndef SCOREP_TIMING_H
#define SCOREP_TIMING_H

/**
 * @file        SCOREP_Timing.h
 * @maintainer  Christian Roessel <c.roessel@fz-juelich.de>
 *
 * @brief       Provide platform independant timing functionality.
 *
 * @todo DD: please check the units and the docs in general.
 */

#include <stdint.h>

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
SCOREP_Timer_Initialize();


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
SCOREP_GetClockTicks
(
);



/**
 * Platform independant clock resolution in order to convert wall clock times
 * to seconds.
 *
 * @return Clock resolution in ticks/second.
 */
uint64_t
SCOREP_GetClockResolution
(
);


#endif /* SCOREP_TIMING_H */
