/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <config.h>
/* no other include here */
#if HAVE( BACKEND_SCOREP_TIMER_TSC )
# if HAVE( GETLINE )
#  ifdef _POSIX_C_SOURCE
#   if _POSIX_C_SOURCE < 200809L
#   undef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200809L
#   endif
#  else
#   define _POSIX_C_SOURCE 200809L
#  endif
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <sys/types.h>
# endif /* GETLINE */
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


timer_type scorep_timer;  /**< Timer used throughout measurement. Can take
                           * one value out of the enum timer_type. Can be set
                           * before measurement using the SCOREP_TIMER
                           * environment variable. It defaults to the first
                           * available timer. Available timers are detected
                           * at configuration time. */

static bool is_initialized = false;

#if HAVE( BACKEND_SCOREP_TIMER_BGL )
static uint64_t ticks_per_sec_bgl;
#endif /* BACKEND_SCOREP_TIMER_BGL */

#if HAVE( BACKEND_SCOREP_TIMER_BGP )
static uint64_t ticks_per_sec_bgp;
#endif /* BACKEND_SCOREP_TIMER_BGP */

#if HAVE( BACKEND_SCOREP_TIMER_MINGW )
static uint64_t ticks_per_sec_mingw;
#endif /* BACKEND_SCOREP_TIMER_MINGW */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
static uint64_t timer_tsc_t0;
static uint64_t timer_cmp_t0;
static uint64_t timer_cmp_freq;
#endif  /* BACKEND_SCOREP_TIMER_TSC */


#include "scorep_timer_confvars.inc.c"

void
SCOREP_Timer_Register( void )
{
    SCOREP_ConfigRegister( "", scorep_timer_confvars );
}


void
SCOREP_Timer_Initialize( void )
{
    if ( is_initialized )
    {
        return;
    }

    switch ( scorep_timer )
    {
#if HAVE( BACKEND_SCOREP_TIMER_BGL )
        case TIMER_BGL:
        {
            BGLPersonality mybgl;
            rts_get_personality( &mybgl, sizeof( BGLPersonality ) );
            ticks_per_sec_bgl = ( uint64_t )BGLPersonality_clockHz( &mybgl );
        }
        break;
#endif  /* BACKEND_SCOREP_TIMER_BGL */

#if HAVE( BACKEND_SCOREP_TIMER_BGP )
        case TIMER_BGP:
        {
            _BGP_Personality_t mybgp;
            Kernel_GetPersonality( &mybgp, sizeof( _BGP_Personality_t ) );
            ticks_per_sec_bgp = ( uint64_t )BGP_Personality_clockMHz( &mybgp ) * UINT64_C( 1000000 );
        }
        break;
#endif  /* BACKEND_SCOREP_TIMER_BGP */

#if HAVE( BACKEND_SCOREP_TIMER_BGQ )
        case TIMER_BGQ:
            break;
#endif  /* BACKEND_SCOREP_TIMER_BGQ */

#if HAVE( BACKEND_SCOREP_TIMER_MINGW )
        case TIMER_MINGW:
        {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency( &frequency );
            ticks_per_sec_mingw = ( uint64_t )frequency.QuadPart;
        }
        break;
#endif  /* BACKEND_SCOREP_TIMER_MINGW */

#if HAVE( BACKEND_SCOREP_TIMER_AIX )
        case TIMER_AIX:
            break;
#endif  /* BACKEND_SCOREP_TIMER_AIX */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
        case TIMER_TSC:
        {
# if HAVE( GETLINE )
            FILE*   fp;
            char*   line   = NULL;
            size_t  length = 0;
            ssize_t read;

            fp = fopen( "/proc/cpuinfo", "r" );
            if ( fp == NULL )
            {
                UTILS_WARNING( "Cannot check for \'nonstop_tsc\' and \'constant_tsc\' "
                               "in /proc/cpuinfo. Switch to a timer different from "
                               "\'tsc\' if you have issues with timings." );
            }
            else
            {
                bool constant_and_nonstop_tsc = false;
                while ( ( read = getline( &line, &length, fp ) ) != -1 )
                {
                    if ( strstr( line, "nonstop_tsc" ) != NULL &&
                         strstr( line, "constant_tsc" ) != NULL )
                    {
                        constant_and_nonstop_tsc = true;
                        break;
                    }
                }
                if ( !constant_and_nonstop_tsc )
                {
                    UTILS_WARNING( "tsc timer is not (\'nonstop_tsc\' && \'constant_tsc\'). "
                                   "Timings likely to be unreliable. Please switch to a timer." );
                }
                free( line );
            }
#else       /* GETLINE */
            UTILS_WARNING( "Cannot check for \'nonstop_tsc\' and \'constant_tsc\' "
                           "in /proc/cpuinfo. Switch to a timer different from "
                           "\'tsc\' if you have issues with timings." );
#endif      /* ! GETLINE */

            /* TODO: assert that all processes use TIMER_TSC running at the
             * same frequency. For this we need to MPP communicate but MPP might
             * not be initialized here. The frequency is interpolated at the end
             * of the measurement, if we detect different frequencies there, this
             * measurement is useless. Both problems can be solved by introducing
             * a timer record per process/location. */
            timer_tsc_t0 = SCOREP_Timer_GetClockTicks();
            /* Either BACKEND_SCOREP_TIMER_CLOCK_GETTIME or
             * BACKEND_SCOREP_TIMER_GETTIMEOFDAY are available, see check in
             * scorep_timer_tsc.h */
# if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
            struct timespec time;
            int             result = clock_gettime( SCOREP_TIMER_CLOCK_GETTIME_CLK_ID, &time );
            UTILS_ASSERT( result == 0 );
            timer_cmp_t0   = ( uint64_t )time.tv_sec * UINT64_C( 1000000000 ) + ( uint64_t )time.tv_nsec;
            timer_cmp_freq = UINT64_C( 1000000000 );
# else      /* HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY ) */
            struct timeval tp;
            gettimeofday( &tp, 0 );
            timer_cmp_t0   =  ( uint64_t )tp.tv_sec * UINT64_C( 1000000 ) + ( uint64_t )tp.tv_usec;
            timer_cmp_freq = UINT64_C( 1000000 );
# endif
        }
        break;
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
        case TIMER_CLOCK_GETTIME:
            break;
#endif  /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
        case TIMER_GETTIMEOFDAY:
            break;
#endif  /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

        default:
            UTILS_FATAL( "Invalid timer selected, shouldn't happen." );
    }

    is_initialized = true;
}


uint64_t
SCOREP_Timer_GetClockResolution( void )
{
    switch ( scorep_timer )
    {
#if HAVE( BACKEND_SCOREP_TIMER_BGL )
        case TIMER_BGL:
            return ticks_per_sec_bgl;
#endif  /* BACKEND_SCOREP_TIMER_BGL */

#if HAVE( BACKEND_SCOREP_TIMER_BGP )
        case TIMER_BGP:
            return ticks_per_sec_bgp;
#endif  /* BACKEND_SCOREP_TIMER_BGP */

#if HAVE( BACKEND_SCOREP_TIMER_BGQ )
        case TIMER_BGQ:
            return DEFAULT_FREQ_MHZ * UINT64_C( 1000000 );
#endif  /* BACKEND_SCOREP_TIMER_BGQ */

#if HAVE( BACKEND_SCOREP_TIMER_MINGW )
        case TIMER_MINGW:
            return ticks_per_sec_mingw;
#endif  /* BACKEND_SCOREP_TIMER_MINGW */

#if HAVE( BACKEND_SCOREP_TIMER_AIX )
        case TIMER_AIX:
            return UINT64_C( 1000000000 );
#endif  /* BACKEND_SCOREP_TIMER_AIX */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
        case TIMER_TSC:
        {
            uint64_t timer_tsc_t1 = SCOREP_Timer_GetClockTicks();
            /* Either BACKEND_SCOREP_TIMER_CLOCK_GETTIME or
             * BACKEND_SCOREP_TIMER_GETTIMEOFDAY are available, see check in
             * scorep_timer_tsc.h */
# if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
            struct timespec time;
            int             result = clock_gettime( SCOREP_TIMER_CLOCK_GETTIME_CLK_ID, &time );
            UTILS_ASSERT( result == 0 );
            uint64_t timer_cmp_t1 = ( uint64_t )time.tv_sec * UINT64_C( 1000000000 ) + ( uint64_t )time.tv_nsec;
# else      /* HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY ) */
            struct timeval tp;
            gettimeofday( &tp, 0 );
            uint64_t timer_cmp_t1 =  ( uint64_t )tp.tv_sec * UINT64_C( 1000000 ) + ( uint64_t )tp.tv_usec;
# endif
            /* Use interpolation to determine frequency.
             * Interpolate only once during finalization. */
            static uint64_t timer_tsc_freq = 0;
            if ( timer_tsc_freq == 0 )
            {
                timer_tsc_freq = ( double )( timer_tsc_t1 - timer_tsc_t0 ) / ( timer_cmp_t1 - timer_cmp_t0 ) * timer_cmp_freq;
            }

            /* TODO: assert that all processes use roughly the same frequency.
             * Collect all frequencies by MPP communication to rank 0. If
             * frequencies differ too much, print a warning that the data is
             * useless for trace experiments. Problem to be solved by timer per
             * process/location record. */
            return timer_tsc_freq;
        }
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
        case TIMER_CLOCK_GETTIME:
            return UINT64_C( 1000000000 );
#endif  /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
        case TIMER_GETTIMEOFDAY:
            return UINT64_C( 1000000 );
#endif  /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

        default:
            UTILS_FATAL( "Invalid timer selected, shouldn't happen." );
    }
}


bool
SCOREP_Timer_ClockIsGlobal( void )
{
    switch ( scorep_timer )
    {
#if HAVE( BACKEND_SCOREP_TIMER_BGL )
        case TIMER_BGL:
            return true;
#endif  /* BACKEND_SCOREP_TIMER_BGL */

#if HAVE( BACKEND_SCOREP_TIMER_BGP )
        case TIMER_BGP:
            return true;
#endif  /* BACKEND_SCOREP_TIMER_BGP */

#if HAVE( BACKEND_SCOREP_TIMER_BGQ )
        case TIMER_BGQ:
            return true;
#endif  /* BACKEND_SCOREP_TIMER_BGQ */

#if HAVE( BACKEND_SCOREP_TIMER_MINGW )
        case TIMER_MINGW:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_MINGW */

#if HAVE( BACKEND_SCOREP_TIMER_AIX )
        case TIMER_MINGW:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_AIX */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
        case TIMER_TSC:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
        case TIMER_CLOCK_GETTIME:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
        case TIMER_GETTIMEOFDAY:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

        default:
            UTILS_FATAL( "Invalid timer selected, shouldn't happen." );
    }
}
