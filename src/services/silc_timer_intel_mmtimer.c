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

/**
 * @file       silc_timer_intel_mmtimer.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * intel_mmtimer timer.
 */


#include "SILC_Timing.h"
#include <config.h>


#if HAVE( HAVE_MMTIMER_H )
#include <mmtimer.h>
#elif HAVE( HAVE_LINUX_MMTIMER_H )
#include <linux/mmtimer.h>
#elif HAVE( HAVE_SN_MMTIMER_H )
#include <sn/mmtimer.h>
#else
#error "You need to include mmtimer.h"
#endif


#include <assert.h>
#include <stdbool.h>


void
SILC_InitTimer()
{
    assert( false ); // implement me
}


uint64_t
SILC_GetWallClockTime()
{
    assert( false ); // implement me
    return 0;
}


uint64_t
SILC_GetClockResolution()
{
    assert( false ); // implement me
    return 0;
}
