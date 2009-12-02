## -*- mode: autoconf -*-

## 
## This file is part of the SILC project (http://www.silc.de)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene USA
##    Forschungszentrum Juelich GmbH, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


## individual timer tests
AC_DEFUN([AC_SILC_TIMER_BGP_GET_TIMEBASE_AVAILABLE],[
ac_silc_timer_bgp_get_timebase_available="no"
AC_MSG_CHECKING([for bgp get timebase timer])
ac_silc_timer_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I/bgsys/drivers/ppcfloor/arch/include"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>]],
                                [[_bgp_GetTimeBase();]])],
               [ac_silc_timer_bgp_get_timebase_available="yes"], [])
CPPFLAGS="$ac_silc_timer_save_CPPFLAGS"
AC_MSG_RESULT([$ac_silc_timer_bgp_get_timebase_available])
])

##

AC_DEFUN([AC_SILC_TIMER_CLOCK_GETTIME_AVAILABLE],[
ac_silc_timer_clock_gettime_available="no"

ac_silc_timer_save_LIBS="$LIBS"
AC_SEARCH_LIBS([clock_gettime], [rt], [ac_silc_timer_have_librt="yes"])
LIBS="$ac_silc_timer_save_LIBS"

if test "x${ac_silc_timer_have_librt}" = "xyes"; then
    ac_silc_librt="$ac_cv_search_clock_gettime"

    AC_MSG_CHECKING([for clock gettime timer])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]],
                                       [[struct timespec tp;
clock_getres(  CLOCK_REALTIME, &tp );
clock_gettime( CLOCK_REALTIME, &tp );]])],
                      [ac_silc_timer_clock_gettime_available="yes"], [])
    AC_MSG_RESULT([$ac_silc_timer_clock_gettime_available])
fi
])

##

AC_DEFUN([AC_SILC_TIMER_CRAY_RTCLOCK_AVAILABLE],[
# not thouroghly tested yet. Do we need to link a particular library?
ac_silc_timer_cray_rtclock_available="no"
AC_MSG_CHECKING([for cray rtclock timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <time.h>
#include <unistd.h>]],
                                [[double clockspeed = 1.0 / sysconf( _SC_CLK_TCK );
double wtime = rtclock() * clockspeed;]])],
               [ac_silc_timer_cray_rtclock_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_cray_rtclock_available])
])

##

AC_DEFUN([AC_SILC_TIMER_CYCLE_COUNTER_ITC_AVAILABLE],[
# Check for the itc cycle conter on __ia64__ platforms. See also the tsc
# timer:AC_SILC_TIMER_CYCLE_COUNTER_TSC_AVAILABLE
ac_silc_timer_cycle_counter_itc_available="no"
AC_MSG_CHECKING([for cycle counter itc timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <asm/intrinsics.h>]],
                                [[#ifdef __ia64__
volatile long long r = (long long) __getReg(_IA64_REG_AR_ITC);
#else
#error "This test makes sense only on __ia64__"
#endif]])],
               [ac_silc_timer_cycle_counter_itc_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_cycle_counter_itc_available])
])

##

AC_DEFUN([AC_SILC_TIMER_CYCLE_COUNTER_TSC_AVAILABLE],[
# According to http://en.wikipedia.org/wiki/Time_Stamp_Counter the TSC timer
# is no longer recommended, at least for x86 processors.
# We can't really check for TSC without running a program on the
# backend. A pragmatic solution is to check the cpu type.
# See also the itc timer: AC_SILC_TIMER_CYCLE_COUNTER_ITC_AVAILABLE
ac_silc_timer_cycle_counter_tsc_available="no"
AC_MSG_CHECKING([for cycle counter tsc timer])
case $host_cpu in
     i*86 | x86* | powerpc*)
          ac_silc_timer_cycle_counter_tsc_available="yes"
     ;;
esac
AC_MSG_RESULT([$ac_silc_timer_cycle_counter_tsc_available])
])

## 

AC_DEFUN([AC_SILC_TIMER_CRAY_DCLOCK_AVAILABLE],[
# not tested yet as I don't have access to a cray xt
ac_silc_timer_cray_dclock_available="no"
AC_MSG_CHECKING([for cray dclock timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#ifndef __LIBCATAMOUNT__
#error "__LIBCATAMOUNT__ not defined."
#endif
#include <catamount/dclock.h>
]],
                                [[double wtime = dclock();]])],
               [ac_silc_timer_cray_dclock_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_cray_dclock_available])
])

## 

AC_DEFUN([AC_SILC_TIMER_SUN_GETHRTIME_AVAILABLE],[
## The gethrtime() function returns the current high-resolution real
## time. Time is expressed as nanoseconds since some arbitrary time in the
## past; it is not correlated in any way to the time of day, and thus is not
## subject to resetting or drifting by way of adjtime(2) or
## settimeofday(3C). The hi-res timer is ideally suited to performance
## measurement tasks, where cheap, accurate interval timing is required.
ac_silc_timer_sun_gethrtime_available="no"
AC_MSG_CHECKING([for sun gethrtime timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/time.h>]],
                                [[hrtime_t wtime = gethrtime();]])],
               [ac_silc_timer_sun_gethrtime_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_sun_gethrtime_available])
])

##

AC_DEFUN([AC_SILC_TIMER_GETTIMEOFDAY_AVAILABLE],[
ac_silc_timer_gettimeofday_available="no"
AC_MSG_CHECKING([for gettimeofday timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/time.h>]],
                                [[struct timeval tp; gettimeofday( &tp, 0 );]])], 
               [ac_silc_timer_gettimeofday_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_gettimeofday_available])
])

##

AC_DEFUN([AC_SILC_TIMER_INTEL_MMTIMER_AVAILABLE],[
ac_silc_timer_intel_mmtimer_available="no"
mmtimer_header_available="no"
AC_CHECK_HEADERS([mmtimer.h], [mmtimer_header_available="yes"],
    [AC_CHECK_HEADERS([linux/mmtimer.h], [mmtimer_header_available="yes"],
        [AC_CHECK_HEADERS([sn/mmtimer.h], [mmtimer_header_available="yes"])])])
if test "x${mmtimer_header_available}" = "xyes"; then
    AC_CHECK_FILE([/dev/mmtimer], [ac_silc_timer_intel_mmtimer_available="yes"])
fi
AC_MSG_CHECKING([for intel mmtimer timer])
AC_MSG_RESULT([$ac_silc_timer_intel_mmtimer_available])
])

##

AC_DEFUN([AC_SILC_TIMER_PAPI_REAL_CYC_AVAILABLE],[
ac_silc_timer_papi_real_cyc_available="no"
AC_MSG_CHECKING([for papi real cyc timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#error "not yet implemented, define AC_SILC_PAPI first"]],
                                [[]])],
               [ac_silc_timer_papi_real_cyc_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_papi_real_cyc_available])
])

##

AC_DEFUN([AC_SILC_TIMER_PAPI_REAL_USEC_AVAILABLE],[
ac_silc_timer_papi_real_usec_available="no"
AC_MSG_CHECKING([for papi real usec timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#error "not yet implemented, define AC_SILC_PAPI first"]],
                                [[]])],
               [ac_silc_timer_papi_real_usec_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_papi_real_usec_available])
])

##

AC_DEFUN([AC_SILC_TIMER_POWER_REALTIME_AVAILABLE],[
## The read_real_time() subroutine and the time_base_to_time() conversion
## subroutine are part of the Standard C Library on IBM AIX systems. They are
## probably not found on non-AIX systems.
## They are designed to measure elapsed time using the processor real time
## clock, or time base registers.
## On IBM SP systems, the resolution is one nanosecond.
ac_silc_timer_power_realtime_available="no"
AC_MSG_CHECKING([for power realtime timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/time.h>]],
                                [[timebasestruct_t t;
read_real_time( &t, TIMEBASE_SZ );
time_base_to_time( &t, TIMEBASE_SZ );]])],
               [ac_silc_timer_power_realtime_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_power_realtime_available])
])

##

AC_DEFUN([AC_SILC_TIMER_CRAY_RTC_AVAILABLE],[
# not tested yet as I don't have access to a cray x1
ac_silc_timer_cray_rtc_available="no"
AC_MSG_CHECKING([for cray rtc timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <intrinsics.h>]],
                                [[double clockspeed = 1.0 / sysconf( _SC_SV2_USER_TIME_RATE );
unsigned long long wtime = _rtc();]])],
               [ac_silc_timer_cray_rtc_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_cray_rtc_available])
])

##

AC_DEFUN([AC_SILC_TIMER_RTS_GET_TIMEBASE_AVAILABLE],[
# not tested yet as I don't have access to a bgl
ac_silc_timer_rts_get_timebase_available="no"
ac_silc_timer_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I/bgl/BlueLight/ppcfloor/bglsys/include"
AC_MSG_CHECKING([for rts get timebase timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <bglpersonality.h>
#include <rts.h>]],
                                [[BGLPersonality mybgl;
rts_get_personality(&mybgl, sizeof(BGLPersonality));
double clockspeed = 1.0/(double)BGLPersonality_clockHz(&mybgl);
double wtime = rts_get_timebase() * clockspeed;]])],
               [ac_silc_timer_rts_get_timebase_available="yes"], [])
CPPFLAGS="$ac_silc_timer_save_CPPFLAGS"
AC_MSG_RESULT([$ac_silc_timer_rts_get_timebase_available])
])

##

AC_DEFUN([AC_SILC_TIMER_IBM_SWITCH_CLOCK_AVAILABLE],[
ac_silc_timer_ibm_switch_clock_available="no"
AC_MSG_CHECKING([for ibm switch clock timer])
ac_silc_timer_save_LIBS="$LIBS"
LIBS="$LIBS -lswclock"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <swclock.h>]],
                                [[swclock_handle_t handle = swclockInit();
swclockRead(handle);]])],
               [ac_silc_timer_ibm_switch_clock_available="yes"], [])
LIBS="$ac_silc_timer_save_LIBS"
AC_MSG_RESULT([$ac_silc_timer_ibm_switch_clock_available])
])

##

AC_DEFUN([AC_SILC_TIMER_NEC_SYSSX_HGTIME_AVAILABLE],[
# not tested yet as I don't have access to a nec sx
ac_silc_timer_nec_syssx_hgtime_available="no"
AC_MSG_CHECKING([for nec syssx hgtime timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/syssx.h>]],
                                [[unsigned long long val;
syssx(HGTIME, &val);]])],
               [ac_silc_timer_nec_syssx_hgtime_available="yes"], [])
AC_MSG_RESULT([$ac_silc_timer_nec_syssx_hgtime_available])
])


###############################################################################

AC_DEFUN([AC_SILC_TIMER], [
# set e.g. to ac_silc_librt if appropriate 
ac_silc_timer_lib=""

ac_silc_user_timer_given="no"
ac_silc_timer_given="no"
ac_silc_timer=""

# init all timers to "no". then evaluate user arguments and availability 
# and set one of them to "yes"
ac_silc_timer_bgp_get_timebase="no"
ac_silc_timer_clock_gettime="no"
ac_silc_timer_cray_dclock="no"
ac_silc_timer_cray_rtc="no"
ac_silc_timer_cray_rtclock="no"
ac_silc_timer_cycle_counter_itc="no"
ac_silc_timer_cycle_counter_tsc="no"
ac_silc_timer_gettimeofday="no"
ac_silc_timer_ibm_switch_clock="no"
ac_silc_timer_intel_mmtimer="no"
ac_silc_timer_nec_syssx_hgtime="no"
ac_silc_timer_papi_real_cyc="no"
ac_silc_timer_papi_real_usec="no"
ac_silc_timer_power_realtime="no"
ac_silc_timer_rts_get_timebase="no"
ac_silc_timer_sun_gethrtime="no"

AC_LANG_PUSH([C])
AC_SILC_TIMER_BGP_GET_TIMEBASE_AVAILABLE
AC_SILC_TIMER_CLOCK_GETTIME_AVAILABLE
AC_SILC_TIMER_CRAY_DCLOCK_AVAILABLE
AC_SILC_TIMER_CRAY_RTCLOCK_AVAILABLE
AC_SILC_TIMER_CRAY_RTC_AVAILABLE
AC_SILC_TIMER_CYCLE_COUNTER_ITC_AVAILABLE
AC_SILC_TIMER_CYCLE_COUNTER_TSC_AVAILABLE
AC_SILC_TIMER_GETTIMEOFDAY_AVAILABLE
AC_SILC_TIMER_IBM_SWITCH_CLOCK_AVAILABLE
AC_SILC_TIMER_INTEL_MMTIMER_AVAILABLE
AC_SILC_TIMER_NEC_SYSSX_HGTIME_AVAILABLE
AC_SILC_TIMER_PAPI_REAL_CYC_AVAILABLE
AC_SILC_TIMER_PAPI_REAL_USEC_AVAILABLE
AC_SILC_TIMER_POWER_REALTIME_AVAILABLE
AC_SILC_TIMER_RTS_GET_TIMEBASE_AVAILABLE
AC_SILC_TIMER_SUN_GETHRTIME_AVAILABLE
AC_LANG_POP([C])
# now all *_available variables are set

AC_SILC_TIMER_ENABLE_SPECIFIC

AC_SILC_TIMER_PLATFORM_DEFAULTS

AS_IF([test "x${ac_silc_user_timer_given}" = "xno" && test "x${ac_silc_timer_given}" = "xno"],
      [AC_MSG_ERROR([No suitable timer found. You may specify one with the --enable-timer options])],
      [AC_MSG_NOTICE([Using timer ${ac_silc_timer}])])

AM_CONDITIONAL([SILC_TIMER_BGP_GET_TIMEBASE],  [test "x${ac_silc_timer_bgp_get_timebase}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CLOCK_GETTIME],     [test "x${ac_silc_timer_clock_gettime}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_RTCLOCK],      [test "x${ac_silc_timer_cray_rtclock}"      = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CYCLE_COUNTER_TSC], [test "x${ac_silc_timer_cycle_counter_tsc}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CYCLE_COUNTER_ITC], [test "x${ac_silc_timer_cycle_counter_itc}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_DCLOCK],       [test "x${ac_silc_timer_cray_dclock}"       = "xyes"])
AM_CONDITIONAL([SILC_TIMER_SUN_GETHRTIME],     [test "x${ac_silc_timer_sun_gethrtime}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_GETTIMEOFDAY],      [test "x${ac_silc_timer_gettimeofday}"      = "xyes"])
AM_CONDITIONAL([SILC_TIMER_INTEL_MMTIMER],     [test "x${ac_silc_timer_intel_mmtimer}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_PAPI_REAL_CYC],     [test "x${ac_silc_timer_papi_real_cyc}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_PAPI_REAL_USEC],    [test "x${ac_silc_timer_papi_real_usec}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_POWER_REALTIME],    [test "x${ac_silc_timer_power_realtime}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_RTC],          [test "x${ac_silc_timer_cray_rtc}"          = "xyes"])
AM_CONDITIONAL([SILC_TIMER_RTS_GET_TIMEBASE],  [test "x${ac_silc_timer_rts_get_timebase}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_IBM_SWITCH_CLOCK],  [test "x${ac_silc_timer_ibm_switch_clock}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_NEC_SYSSX_HGTIME],  [test "x${ac_silc_timer_nec_syssx_hgtime}"  = "xyes"])

AC_SUBST([TIMER_LIB], ["$ac_silc_timer_lib"])
])

###############################################################################

AC_DEFUN([AC_SILC_TIMER_PLATFORM_DEFAULTS], [
AS_IF([test "x${ac_silc_user_timer_given}" = "xno"], [
    AS_CASE([${ac_silc_platform}], 
            ["altix"],    [AS_IF([test "x$ac_silc_timer_intel_mmtimer_available" = "xyes"], 
                                 [ac_silc_timer_intel_mmtimer="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="intel-mmtimer"],
                                 [test "x$ac_silc_timer_clock_gettime_available" = "xyes"], 
                                 [ac_silc_timer_clock_gettime="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="clock-gettime"])],
            ["bgl"],      [AS_IF([test "x$ac_silc_timer_rts_get_timebase_available" = "xyes"],
                                 [ac_silc_timer_rts_get_timebase="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="rts-get-timebase"])],
            ["bgp"],      [AS_IF([test "x$ac_silc_timer_bgp_get_timebase_available" = "xyes"],
                                 [ac_silc_timer_bgp_get_timebase="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="bgp-get-timebase"])],
            ["crayxt"],   [AS_IF([test "x$ac_silc_timer_cray_dclock_available" = "xyes"],
                                 [ac_silc_timer_cray_dclock="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cray-dclock"],
                                 [test "x$ac_silc_timer_cycle_counter_itc_available" = "xyes"],
                                 [ac_silc_timer_cycle_counter_itc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cycle-counter-itc"],
                                 [test "x$ac_silc_timer_cycle_counter_tsc_available" = "xyes"],
                                 [ac_silc_timer_cycle_counter_tsc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cycle-counter-tsc"],
                                 [test "x$ac_silc_timer_clock_gettime_available" = "xyes"],
                                 [ac_silc_timer_clock_gettime="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="clock-gettime"],
                                 [test "x$ac_silc_timer_gettimeofday_available" = "xyes"],
                                 [ac_silc_timer_gettimeofday="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="gettimeofday"])],
            ["sicortex"], [AS_IF([test "x$ac_silc_timer_gettimeofday_available" = "xyes"],
                                 [ac_silc_timer_gettimeofday="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="gettimeofday"])],
            ["sun"],      [AS_IF([test "x$ac_silc_timer_sun_gethrtime_available" = "xyes"],
                                 [ac_silc_timer_sun_gethrtime="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="sun-gethrtime"])],
            ["mac"],      [AS_IF([test "x$ac_silc_timer_cycle_counter_tsc_available" = "xyes"],
                                 [ac_silc_timer_cycle_counter_tsc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cycle-counter-tsc"],
                                 [test "x$ac_silc_timer_cycle_counter_itc_available" = "xyes"],
                                 [ac_silc_timer_cycle_counter_itc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cycle-counter-itc"],
                                 [test "x$ac_silc_timer_gettimeofday_available" = "xyes"],
                                 [ac_silc_timer_gettimeofday="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="gettimeofday"])],
            ["ibm"],      [AS_IF([test "x$ac_silc_timer_power_realtime_available" = "xyes"],
                                 [ac_silc_timer_power_realtime="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="power-realtime"],
                                 [test "x$ac_silc_timer_ibm_switch_clock_available" = "xyes"],
                                 [ac_silc_timer_ibm_switch_clock="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="ibm-switch-clock"])],
            ["crayx1"],   [AS_IF([test "x$ac_silc_timer_cray_rtc_available" = "xyes"],
                                 [ac_silc_timer_cray_rtc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cray-rtc"],
                                 [test "x$ac_silc_timer_gettimeofday_available" = "xyes"],
                                 [ac_silc_timer_gettimeofday="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="gettimeofday"])],
            ["necsx"],    [AS_IF([test "x$ac_silc_timer_nec_syssx_hgtime_available" = "xyes"],
                                 [ac_silc_timer_nec_syssx_hgtime="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="nec-syssx-hgtime"])],
            ["linux"],    [AS_IF([test "x$ac_silc_timer_cycle_counter_itc_available" = "xyes"],
                                 [ac_silc_timer_cycle_counter_itc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cycle-counter-itc"],
                                 [test "x$ac_silc_timer_cycle_counter_tsc_available" = "xyes"],
                                 [ac_silc_timer_cycle_counter_tsc="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="cycle-counter-tsc"],
                                 [test "x$ac_silc_timer_clock_gettime_available" = "xyes"],
                                 [ac_silc_timer_clock_gettime="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="clock-gettime"],
                                 [test "x$ac_silc_timer_gettimeofday_available" = "xyes"],
                                 [ac_silc_timer_gettimeofday="yes"; 
                                  ac_silc_timer_given="yes";
                                  ac_silc_timer="gettimeofday"])],
            [AC_MSG_ERROR([Encountered unsupported platform \"$ac_silc_platform\" during timer checks.])])
])
])

###############################################################################

AC_DEFUN([AC_SILC_TIMER_ENABLE_SPECIFIC], [
AC_ARG_ENABLE([timer-bgp-get-timebase],
              [AS_HELP_STRING([--enable-timer-bgp-get-timebase],
                              [enable bgp-get-timebase timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_bgp_get_timebase_available}" = "xno"; then
                       AC_MSG_WARN(["timer-bgp-get-timebase not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="bgp-get-timebase"
                       ac_silc_timer_bgp_get_timebase="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-clock-gettime],
              [AS_HELP_STRING([--enable-timer-clock-gettime],
                              [enable clock-gettime timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_clock_gettime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-clock-gettime not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="clock-gettime"
                       ac_silc_timer_clock_gettime="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cray-rtclock],
              [AS_HELP_STRING([--enable-timer-cray-rtclock],
                              [enable cray-rtclock timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_cray_rtclock_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cray-rtclock not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="cray-rtclock"
                       ac_silc_timer_cray_rtclock="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cycle-counter-itc],
              [AS_HELP_STRING([--enable-timer-cycle-counter-itc],
                              [enable cycle-counter-itc timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_cycle_counter_itc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cycle-counter-itc not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="cycle-counter-itc"
                       ac_silc_timer_cycle_counter_itc="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cycle-counter-tsc],
              [AS_HELP_STRING([--enable-timer-cycle-counter-tsc],
                              [enable cycle-counter-tsc timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_cycle_counter_tsc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cycle-counter-tsc not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="cycle-counter-tsc"
                       ac_silc_timer_cycle_tsc_counter="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cray-dclock],
              [AS_HELP_STRING([--enable-timer-cray-dclock],
                              [enable cray-dclock timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_cray_dclock_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cray-dclock not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="cray-dclock"
                       ac_silc_timer_cray_dclock="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-sun-gethrtime],
              [AS_HELP_STRING([--enable-timer-sun-gethrtime],
                              [enable sun-gethrtime timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_sun_gethrtime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-sun-gethrtime not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="sun-gethrtime"
                       ac_silc_timer_sun_gethrtime="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-gettimeofday],
              [AS_HELP_STRING([--enable-timer-gettimeofday],
                              [enable gettimeofday timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_gettimeofday_available}" = "xno"; then
                       AC_MSG_WARN(["timer-gettimeofday not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="gettimeofday"
                       ac_silc_timer_gettimeofday="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-intel-mmtimer],
              [AS_HELP_STRING([--enable-timer-intel-mmtimer],
                              [enable intel-mmtimer timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_intel_mmtimer_available}" = "xno"; then
                       AC_MSG_WARN(["timer-intel-mmtimer not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="intel-mmtimer"
                       ac_silc_timer_intel_mmtimer="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-papi-real-cyc],
              [AS_HELP_STRING([--enable-timer-papi-real-cyc],
                              [enable papi-real-cyc timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_papi_real_cyc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-papi-real-cyc not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="papi-real-cyc"
                       ac_silc_timer_papi_real_cyc="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-papi-real-usec],
              [AS_HELP_STRING([--enable-timer-papi-real-usec],
                              [enable papi-real-usec timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_papi_real_usec_available}" = "xno"; then
                       AC_MSG_WARN(["timer-papi-real-usec not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="papi-real-usec"
                       ac_silc_timer_papi_real_usec="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-power-realtime],
              [AS_HELP_STRING([--enable-timer-power-realtime],
                              [enable power-realtime timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_power_realtime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-power-realtime not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="power-realtime"
                       ac_silc_timer_power_realtime="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cray-rtc],
              [AS_HELP_STRING([--enable-timer-cray-rtc],
                              [enable cray-rtc timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_cray_rtc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cray-rtc not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="cray-rtc"
                       ac_silc_timer_cray_rtc="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-rts-get-timebase],
              [AS_HELP_STRING([--enable-timer-rts-get-timebase],
                              [enable rts-get-timebase timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_rts_get_timebase_available}" = "xno"; then
                       AC_MSG_WARN(["timer-rts-get-timebase not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="rts-get-timebase"
                       ac_silc_timer_rts_get_timebase="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-ibm-switch-clock],
              [AS_HELP_STRING([--enable-timer-ibm-switch-clock],
                              [enable ibm-switch-clock timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_ibm_switch_clock_available}" = "xno"; then
                       AC_MSG_WARN(["timer-ibm-switch-clock not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="ibm-switch-clock"
                       ac_silc_timer_ibm_switch_clock="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-nec-syssx-hgtime],
              [AS_HELP_STRING([--enable-timer-nec-syssx-hgtime],
                              [enable nec-syssx-hgtime timer if available instead of platform default])],
              [if test "x${ac_silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $ac_silc_timer"])
               else
                   if test "x${ac_silc_timer_nec_syssx_hgtime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-nec-syssx-hgtime not available"])
                   else
                       ac_silc_user_timer_given="yes"
                       ac_silc_timer="nec-syssx-hgtime"
                       ac_silc_timer_nec_syssx_hgtime="yes"
                   fi
               fi
              ])
])

