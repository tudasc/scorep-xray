#ifndef SILC_TIMING_H
#define SILC_TIMING_H

/**
 * @file        SILC_Timing.h
 * @maintainer  Christian Roessel <c.roessel@fz-juelich.de>
 *
 * @brief       Provide platform independant timing functionality.
 *
 * @todo DD: please check the units and the docs in general. is a init
 * function needed?
 */



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
SILC_GetWallClockTime
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
