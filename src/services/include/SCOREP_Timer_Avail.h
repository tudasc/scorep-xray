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
 */

#ifndef SCOREP_TIMER_AVAIL_H
#define SCOREP_TIMER_AVAIL_H

/**
 * @file
 * Provides an enum containing available timers.
 * This enum was extracted from SCOREP_Timer_Ticks.h to prevent scorep-info
 * to link against @SCOREP_TIMER_LIBS@ (which might not work on cross-compile
 * machines).
 */


/* *INDENT-OFF* */
/* *INDENT-ON* */


/* Naming convention: following timer_type enum and variables are supposed to
 * be implementation detail. They need to be public to be able to inline
 * SCOREP_Timer_GetClockTicks(). They must not be used by clients of
 * SCOREP_Timer_Ticks.h.
 */
typedef uint64_t timer_type;
enum timer_type
{
#if HAVE( BACKEND_SCOREP_TIMER_BGL )
    TIMER_BGL,
#endif /* BACKEND_SCOREP_TIMER_BGL */

#if HAVE( BACKEND_SCOREP_TIMER_BGP )
    TIMER_BGP,
#endif /* BACKEND_SCOREP_TIMER_BGP */

#if HAVE( BACKEND_SCOREP_TIMER_BGQ )
    TIMER_BGQ,
#endif /* BACKEND_SCOREP_TIMER_BGQ */

#if HAVE( BACKEND_SCOREP_TIMER_MINGW )
    TIMER_MINGW,
#endif /* BACKEND_SCOREP_TIMER_MINGW */

#if HAVE( BACKEND_SCOREP_TIMER_AIX )
    TIMER_AIX,
#endif /* BACKEND_SCOREP_TIMER_AIX */

#if HAVE( BACKEND_SCOREP_TIMER_MAC )
    TIMER_MAC,
#endif /* BACKEND_SCOREP_TIMER_MAC */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
    TIMER_TSC,
#endif /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
    TIMER_GETTIMEOFDAY,
#endif /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
    TIMER_CLOCK_GETTIME,
#endif /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

    TIMER_INVALID
};


#endif /* SCOREP_TIMER_AVAIL_H */
