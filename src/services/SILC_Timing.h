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

#ifndef SILC_TIMING_H
#define SILC_TIMING_H

/**
 * @file        SILC_Timing.h
 * @maintainer  Christian Roessel <c.roessel@fz-juelich.de>
 *
 * @brief       Provide platform independant timing functionality.
 *
 * @todo DD: please check the units and the docs in general.
 */

#include <stdint.h>

/**
 * Initialize Timer before usage of SILC_GetClockTicks() and
 * SILC_GetClockResolution(). Multiple calls do no harm.
 *
 * @note For performance reasons we don't check if the timer is initialized in
 * SILC_GetClockTicks() and SILC_GetClockResolution().
 *
 * @note The init function is not thread safe.
 */
void
SILC_Timer_Initialize();


/**
 * Platform independant timing functions to be used for providing timestamps
 * to @ref SILC_Events events.
 *
 * @note This may be combined (for performance reasons) with some HWC
 * functionality in future.
 *
 * @return Wall clock time in ticks.
 */
uint64_t
SILC_GetClockTicks
(
);



/**
 * Platform independant clock resolution in order to convert wall clock times
 * to seconds.
 *
 * @return Clock resolution in ticks/second.
 */
uint64_t
SILC_GetClockResolution
(
);


#endif /* SILC_TIMING_H */
