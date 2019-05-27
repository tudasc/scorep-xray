/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2016, 2019,
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


#include <config.h>

#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>

#include <SCOREP_InMeasurement.h>

#if HAVE( BACKEND_SCOREP_TIMER_TSC ) && !HAVE( SCOREP_ARMV8_TSC )
#include <scorep_ipc.h>
#include <UTILS_IO.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#endif  /* HAVE( BACKEND_SCOREP_TIMER_TSC ) && ! HAVE( SCOREP_ARMV8_TSC ) */

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

#if HAVE( BACKEND_SCOREP_TIMER_TSC ) && !HAVE( SCOREP_ARMV8_TSC )
static uint64_t timer_tsc_t0;
static uint64_t timer_cmp_t0;
static uint64_t timer_cmp_freq;
#endif  /* HAVE( BACKEND_SCOREP_TIMER_TSC ) && ! HAVE( SCOREP_ARMV8_TSC ) */

#if HAVE( BACKEND_SCOREP_TIMER_MAC )
double scorep_ticks_to_nsec_mac;
#endif  /* BACKEND_SCOREP_TIMER_MAC */

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

#if HAVE( BACKEND_SCOREP_TIMER_MAC )
        case TIMER_MAC:
        {
            mach_timebase_info_data_t timebase;
            mach_timebase_info( &timebase );
            scorep_ticks_to_nsec_mac = timebase.numer / ( double )timebase.denom;
        }
        break;
#endif  /* BACKEND_SCOREP_TIMER_MAC */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
        case TIMER_TSC:
        {
# if HAVE( SCOREP_ARMV8_TSC )
            /* Nothing to do here for ARMV8, frequency can be accessed directly,
             * see SCOREP_Timer_GetClockResolution(). */
# else      /* ! HAVE( SCOREP_ARMV8_TSC ) */
#  if HAVE( SCOREP_X86_64_TSC ) || HAVE( SCOREP_X86_32_TSC )
            /* Check for nonstop_tsc and constant_tsc in /proc/cpuinfo.
             * This seems to be relevant for x86 CPUs only. It is known for
             * Fujitsu and Power8 systems to not provide this info. */
            FILE*            fp;
            char*            line           = NULL;
            size_t           length         = 0;
            char             proc_cpuinfo[] = "/proc/cpuinfo";
            SCOREP_ErrorCode status;

            fp = fopen( proc_cpuinfo, "r" );
            if ( fp == NULL )
            {
                UTILS_ERROR_POSIX( "Cannot check for \'nonstop_tsc\' and \'constant_tsc\' "
                                   "in %s. Switch to a timer different from "
                                   "\'tsc\' if you have issues with timings.", proc_cpuinfo );
            }
            else
            {
                bool constant_and_nonstop_tsc = false;
                while ( ( status = UTILS_IO_GetLine( &line, &length, fp ) ) == SCOREP_SUCCESS )
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
                    if ( status != SCOREP_ERROR_END_OF_BUFFER )
                    {
                        UTILS_ERROR( status, "Error reading %s for timer consistency check.", proc_cpuinfo );
                    }
                    UTILS_WARNING( "From %s, could not determine if tsc timer is (\'nonstop_tsc\' "
                                   "&& \'constant_tsc\'). Timings likely to be unreliable. "
                                   "Switch to a timer different from SCOREP_TIMER=\'tsc\' if you have "
                                   "issues with timings.", proc_cpuinfo );
                }
                free( line );
            }
#  endif    /* HAVE( SCOREP_X86_64_TSC ) || HAVE( SCOREP_X86_32_TSC ) */

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
#  if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
            struct timespec time;
            int             result = clock_gettime( SCOREP_TIMER_CLOCK_GETTIME_CLK_ID, &time );
            UTILS_ASSERT( result == 0 );
            timer_cmp_t0   = ( uint64_t )time.tv_sec * UINT64_C( 1000000000 ) + ( uint64_t )time.tv_nsec;
            timer_cmp_freq = UINT64_C( 1000000000 );
#  else      /* HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY ) */
            struct timeval tp;
            gettimeofday( &tp, 0 );
            timer_cmp_t0   =  ( uint64_t )tp.tv_sec * UINT64_C( 1000000 ) + ( uint64_t )tp.tv_usec;
            timer_cmp_freq = UINT64_C( 1000000 );
#  endif
# endif     /* ! HAVE( SCOREP_ARMV8_TSC ) */
        }
        break;
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
        case TIMER_GETTIMEOFDAY:
            break;
#endif  /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
        case TIMER_CLOCK_GETTIME:
            break;
#endif  /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

        default:
            UTILS_FATAL( "Invalid timer selected, shouldn't happen." );
    }

    is_initialized = true;
}


uint64_t
SCOREP_Timer_GetClockResolution( void )
{
    UTILS_BUG_ON( !( SCOREP_IS_MEASUREMENT_PHASE( POST ) ),
                  "Do not call SCOREP_Timer_GetClockResolution before finalization." );
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

#if HAVE( BACKEND_SCOREP_TIMER_MAC )
        case TIMER_MAC:
            return UINT64_C( 1000000000 );
#endif  /* BACKEND_SCOREP_TIMER_MAC */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
        case TIMER_TSC:
        {
# if HAVE( SCOREP_ARMV8_TSC )
            uint32_t timer_tsc_freq;
            asm ( "mrs %[result], CNTFRQ_EL0\n\t" :[ result ] "=r" ( timer_tsc_freq ) );
            return ( uint64_t )timer_tsc_freq;
# else      /* ! HAVE( SCOREP_ARMV8_TSC ) */
            uint64_t timer_tsc_t1 = SCOREP_Timer_GetClockTicks();
            /* Either BACKEND_SCOREP_TIMER_CLOCK_GETTIME or
             * BACKEND_SCOREP_TIMER_GETTIMEOFDAY are available, see check in
             * scorep_timer_tsc.h */
#  if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
            struct timespec time;
            int             result = clock_gettime( SCOREP_TIMER_CLOCK_GETTIME_CLK_ID, &time );
            UTILS_ASSERT( result == 0 );
            uint64_t timer_cmp_t1 = ( uint64_t )time.tv_sec * UINT64_C( 1000000000 ) + ( uint64_t )time.tv_nsec;
#  else      /* HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY ) */
            struct timeval tp;
            gettimeofday( &tp, 0 );
            uint64_t timer_cmp_t1 =  ( uint64_t )tp.tv_sec * UINT64_C( 1000000 ) + ( uint64_t )tp.tv_usec;
#  endif
            /* Use interpolation to determine frequency.
             * Interpolate and check consistency only once during finalization. */
            static bool     first_visit = true;
            static uint64_t timer_tsc_freq;
            if ( first_visit )
            {
                first_visit = false;
                UTILS_BUG_ON( timer_cmp_t1 - timer_cmp_t0 == 0, "Start and stop timestamps must differ." );
                timer_tsc_freq = ( double )( timer_tsc_t1 - timer_tsc_t0 ) / ( timer_cmp_t1 - timer_cmp_t0 ) * timer_cmp_freq;

                /* Assert that all processes use roughly the same frequency.
                 * Problems with non-uniform frequencies to be solved by timer
                 * per location-group record.*/
                if ( SCOREP_Ipc_GetRank() != 0 )
                {
                    SCOREP_Ipc_Gather( &timer_tsc_freq, 0, 1, SCOREP_IPC_UINT64_T, 0 );
                }
                else
                {
                    int size = SCOREP_Ipc_GetSize();
                    UTILS_BUG_ON( size == 0 );
                    uint64_t frequencies[ size ];
                    SCOREP_Ipc_Gather( &timer_tsc_freq,
                                       frequencies,
                                       1,
                                       SCOREP_IPC_UINT64_T,
                                       0 );

                    /* Compute average of (non-negative) frequencies using integers, see
                     * https://www.quora.com/How-can-I-compute-the-average-of-a-large-array-of-integers-without-running-into-overflow */
                    uint64_t avg_frequency = 0;
                    {
                        uint64_t x = 0;
                        uint64_t y = 0;
                        for ( uint64_t i = 0; i < size; ++i )
                        {
                            x += frequencies[ i ] / size;
                            uint64_t b = frequencies[ i ] % size;
                            if ( y >= size - b )
                            {
                                x++;
                                y -= size - b;
                            }
                            else
                            {
                                y += b;
                            }
                        }
                        /* Average is exactly x + y / N, with 0 <= y < N.
                         * Take the integer part only. */
                        avg_frequency = x;
                    }

                    /* HPC CPU clock frequencies are in the order of 10^9 since
                     * 2000. We observed CPUs frequencies to differ in the order
                     * of 10^3. Thus, we will 'accept' frequencies in the range
                     * [average_freq - threshold, average_freq + threshold] where
                     * threshold is 'average_freq / 10^5' (i.e. in the order of
                     * 10^4 or 0.01%).
                     * For slower CPUs make sure the threshold larger than (the
                     * arbitrarily chosen) 10. */
                    uint64_t dividend  = 100000;
                    uint64_t threshold = avg_frequency / dividend; /* Usually in the order of 10^4 */
                    while ( threshold < 10 &&  dividend > 0 )
                    {
                        dividend  = dividend / 10;
                        threshold = avg_frequency / dividend;
                    }
                    /* Check for frequency outliers. */
                    uint64_t outliers[ size ];
                    memset( outliers, 0, sizeof( uint64_t ) * size );
                    bool has_outlier = false;
                    for ( int i = 0; i < size; ++i )
                    {
                        uint64_t abs_diff;
                        if ( avg_frequency > frequencies[ i ] )
                        {
                            abs_diff = avg_frequency - frequencies[ i ];
                        }
                        else
                        {
                            abs_diff = frequencies[ i ] - avg_frequency;
                        }
                        if ( abs_diff > threshold )
                        {
                            has_outlier   = true;
                            outliers[ i ] = frequencies[ i ];
                        }
                    }

                    /* Report if there are outliers. */
                    if ( has_outlier )
                    {
                        UTILS_WARNING( "Calculated timer (tsc) frequencies differ from average frequency "
                                       "(%" PRIu64 " Hz) by more than %" PRIu64 " Hz. "
                                       "Consider using a timer with a fixed frequency like gettimeofday "
                                       "or clock_gettime. Prolonging the measurement duration might "
                                       "mitigate the frequency variations.",
                                       avg_frequency, threshold );
                        for ( int i = 0; i < size; ++i )
                        {
                            char* is_outlier = ( outliers[ i ] != 0 ) ? " (outlier)" : "";
                            printf( "rank[%d]:\t frequency = %" PRIu64 " Hz%s\n", i, frequencies[ i ], is_outlier );
                        }
                    }
                }
            }
            return timer_tsc_freq;
# endif /* ! HAVE( SCOREP_ARMV8_TSC ) */
        }
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
        case TIMER_GETTIMEOFDAY:
            return UINT64_C( 1000000 );
#endif  /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
        case TIMER_CLOCK_GETTIME:
            return UINT64_C( 1000000000 );
#endif  /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

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
        case TIMER_AIX:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_AIX */

#if HAVE( BACKEND_SCOREP_TIMER_MAC )
        case TIMER_MAC:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_MAC */

#if HAVE( BACKEND_SCOREP_TIMER_TSC )
        case TIMER_TSC:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_TSC */

#if HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
        case TIMER_GETTIMEOFDAY:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_GETTIMEOFDAY */

#if HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME )
        case TIMER_CLOCK_GETTIME:
            return false;
#endif  /* BACKEND_SCOREP_TIMER_CLOCK_GETTIME */

        default:
            UTILS_FATAL( "Invalid timer selected, shouldn't happen." );
    }
}
