/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


/* *INDENT-OFF* */
/* *INDENT-ON*  */


#include <SCOREP_Config.h>
#include <SCOREP_Timer_Avail.h>

#include <stddef.h>

static const SCOREP_ConfigType_SetEntry timer_table[] = {
#if HAVE( BACKEND_SCOREP_TIMER_BGL )
    {
        "bgl",
        TIMER_BGL,
        "Low overhead global Blue Gene/L rts_get_timebase timer."
    },
#endif /* BACKEND_SCOREP_TIMER_BGL */

#if HAVE( BACKEND_SCOREP_TIMER_BGP )
    {
        "bgp",
        TIMER_BGP,
        "Low overhead global Blue Gene/P _bgp_GetTimeBase timer."
    },
#endif /* BACKEND_SCOREP_TIMER_BGP */

#if HAVE( BACKEND_SCOREP_TIMER_BGQ )
    {
        "bgq",
        TIMER_BGQ,
        "Low overhead global Blue Gene/Q __mftb timer."
    },
#endif /* BACKEND_SCOREP_TIMER_BGQ */

#if HAVE( BACKEND_SCOREP_TIMER_MINGW )
    {
        "mingw",
        TIMER_MINGW,
        "Low overhead Windows QueryPerformanceCounter timer."
    },
#endif /* BACKEND_SCOREP_TIMER_MINGW */

#if HAVE( BACKEND_SCOREP_TIMER_AIX )
    {
        "aix",
        TIMER_AIX,
        "Low overhead AIX read_real_time timer."
    },
#endif /* BACKEND_SCOREP_TIMER_AIX */

#if HAVE( BACKEND_SCOREP_TIMER_MAC )
    {
        "mac",
        TIMER_MAC,
        "macOS/Mach absolute time timer."
    },
#endif /* BACKEND_SCOREP_TIMER_MAC */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
    {
        "tsc",
        TIMER_TSC,
        "Low overhead time stamp counter (" HAVE_BACKEND_SCOREP_TIMER_TSC_NAME ") timer."
    },
#endif /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
    {
        "gettimeofday",
        TIMER_GETTIMEOFDAY,
        "gettimeofday timer."
    },
#endif /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
    {
        "clock_gettime",
        TIMER_CLOCK_GETTIME,
        "clock_gettime timer with " HAVE_BACKEND_SCOREP_TIMER_CLOCK_GETTIME_CLK_ID_NAME " as clock."
    },
#endif /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

    { NULL, 0, NULL }
};

extern timer_type scorep_timer;

static const SCOREP_ConfigVariable scorep_timer_confvars[] = {
    {
        "timer",
        SCOREP_CONFIG_TYPE_OPTIONSET,
        &scorep_timer,
        ( void* )timer_table,
        HAVE_BACKEND_SCOREP_TIMER_DEFAULT, /* set by configure */
        "Timer used during measurement",
        "The following timers are available for this installation:"
    },
    SCOREP_CONFIG_TERMINATOR
};
