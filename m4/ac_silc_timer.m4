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
AC_DEFUN([SILC_TIMER_BGP_GET_TIMEBASE_AVAILABLE],[
silc_timer_bgp_get_timebase_available="no"
AC_MSG_CHECKING([for bgp_get_timebase timer])
silc_timer_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I/bgsys/drivers/ppcfloor/arch/include"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>]],
                                [[_bgp_GetTimeBase();]])],
               [silc_timer_bgp_get_timebase_available="yes"], [])
CPPFLAGS="$silc_timer_save_CPPFLAGS"
AC_MSG_RESULT([$silc_timer_bgp_get_timebase_available])
])

##

AC_DEFUN([SILC_TIMER_CLOCK_GETTIME_AVAILABLE],[
silc_timer_clock_gettime_available="no"

silc_timer_save_LIBS="$LIBS"
AC_SEARCH_LIBS([clock_gettime], [rt], [silc_timer_have_librt="yes"])
LIBS="$silc_timer_save_LIBS"

if test "x${silc_timer_have_librt}" = "xyes"; then
    silc_timer_librt="$ac_cv_search_clock_gettime"

    AC_MSG_CHECKING([for clock_gettime timer])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]],
                                       [[struct timespec tp;
clock_getres(  CLOCK_REALTIME, &tp );
clock_gettime( CLOCK_REALTIME, &tp );]])],
                      [silc_timer_clock_gettime_available="yes"], [])
    AC_MSG_RESULT([$silc_timer_clock_gettime_available])
fi
])

##

AC_DEFUN([SILC_TIMER_CRAY_RTCLOCK_AVAILABLE],[
# not thouroghly tested yet. Do we need to link a particular library?
silc_timer_cray_rtclock_available="no"
AC_MSG_CHECKING([for cray_rtclock timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <time.h>
#include <unistd.h>]],
                                [[double clockspeed = 1.0 / sysconf( _SC_CLK_TCK );
double wtime = rtclock() * clockspeed;]])],
               [silc_timer_cray_rtclock_available="yes"], [])
AC_MSG_RESULT([$silc_timer_cray_rtclock_available])
])

##

AC_DEFUN([SILC_TIMER_CYCLE_COUNTER_ITC_AVAILABLE],[
# Check for the itc cycle conter on __ia64__ platforms. See also the tsc
# timer:SILC_TIMER_CYCLE_COUNTER_TSC_AVAILABLE
silc_timer_cycle_counter_itc_available="no"
AC_MSG_CHECKING([for cycle_counter_itc timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <asm/intrinsics.h>]],
                                [[#ifdef __ia64__
volatile long long r = (long long) __getReg(_IA64_REG_AR_ITC);
#else
#error "This test makes sense only on __ia64__"
#endif]])],
               [silc_timer_cycle_counter_itc_available="yes"], [])
AC_MSG_RESULT([$silc_timer_cycle_counter_itc_available])
])

##

AC_DEFUN([SILC_TIMER_CYCLE_COUNTER_TSC_AVAILABLE],[
# According to http://en.wikipedia.org/wiki/Time_Stamp_Counter the TSC timer
# is no longer recommended, at least for x86 processors.
# We can't really check for TSC without running a program on the
# backend. A pragmatic solution is to check the cpu type.
# See also the itc timer: SILC_TIMER_CYCLE_COUNTER_ITC_AVAILABLE
silc_timer_cycle_counter_tsc_available="no"
AC_MSG_CHECKING([for cycle_counter_tsc timer])
case $host_cpu in
     i*86 | x86* | powerpc*)
          silc_timer_cycle_counter_tsc_available="yes"
     ;;
esac
AC_MSG_RESULT([$silc_timer_cycle_counter_tsc_available])
])

## 

AC_DEFUN([SILC_TIMER_CRAY_DCLOCK_AVAILABLE],[
# not tested yet as I don't have access to a cray xt
silc_timer_cray_dclock_available="no"
AC_MSG_CHECKING([for cray_dclock timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#ifndef __LIBCATAMOUNT__
#error "__LIBCATAMOUNT__ not defined."
#endif
#include <catamount/dclock.h>
]],
                                [[double wtime = dclock();]])],
               [silc_timer_cray_dclock_available="yes"], [])
AC_MSG_RESULT([$silc_timer_cray_dclock_available])
])

## 

AC_DEFUN([SILC_TIMER_SUN_GETHRTIME_AVAILABLE],[
## The gethrtime() function returns the current high-resolution real
## time. Time is expressed as nanoseconds since some arbitrary time in the
## past; it is not correlated in any way to the time of day, and thus is not
## subject to resetting or drifting by way of adjtime(2) or
## settimeofday(3C). The hi-res timer is ideally suited to performance
## measurement tasks, where cheap, accurate interval timing is required.
silc_timer_sun_gethrtime_available="no"
AC_MSG_CHECKING([for sun_gethrtime timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/time.h>]],
                                [[hrtime_t wtime = gethrtime();]])],
               [silc_timer_sun_gethrtime_available="yes"], [])
AC_MSG_RESULT([$silc_timer_sun_gethrtime_available])
])

##

AC_DEFUN([SILC_TIMER_GETTIMEOFDAY_AVAILABLE],[
silc_timer_gettimeofday_available="no"
AC_MSG_CHECKING([for gettimeofday timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/time.h>]],
                                [[struct timeval tp; gettimeofday( &tp, 0 );]])], 
               [silc_timer_gettimeofday_available="yes"], [])
AC_MSG_RESULT([$silc_timer_gettimeofday_available])
])

##

AC_DEFUN([SILC_TIMER_INTEL_MMTIMER_AVAILABLE],[
silc_timer_intel_mmtimer_available="no"
mmtimer_header_available="no"
AC_CHECK_HEADERS([mmtimer.h], [mmtimer_header_available="yes"],
    [AC_CHECK_HEADERS([linux/mmtimer.h], [mmtimer_header_available="yes"],
        [AC_CHECK_HEADERS([sn/mmtimer.h], [mmtimer_header_available="yes"])])])
if test "x${mmtimer_header_available}" = "xyes"; then
    AC_CHECK_FILE([/dev/mmtimer], [silc_timer_intel_mmtimer_available="yes"])
fi
AC_MSG_CHECKING([for intel_mmtimer timer])
AC_MSG_RESULT([$silc_timer_intel_mmtimer_available])
])

##

AC_DEFUN([SILC_TIMER_PAPI_REAL_CYC_AVAILABLE],[
silc_timer_papi_real_cyc_available="no"
AC_MSG_CHECKING([for papi_real_cyc timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#error "not yet implemented, define SILC_PAPI first"]],
                                [[]])],
               [silc_timer_papi_real_cyc_available="yes"], [])
AC_MSG_RESULT([$silc_timer_papi_real_cyc_available])
])

##

AC_DEFUN([SILC_TIMER_PAPI_REAL_USEC_AVAILABLE],[
silc_timer_papi_real_usec_available="no"
AC_MSG_CHECKING([for papi_real_usec timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#error "not yet implemented, define SILC_PAPI first"]],
                                [[]])],
               [silc_timer_papi_real_usec_available="yes"], [])
AC_MSG_RESULT([$silc_timer_papi_real_usec_available])
])

##

AC_DEFUN([SILC_TIMER_IBM_REALTIME_AVAILABLE],[
## The read_real_time() subroutine and the time_base_to_time() conversion
## subroutine are part of the Standard C Library on IBM AIX systems. They are
## probably not found on non-AIX systems.
## They are designed to measure elapsed time using the processor real time
## clock, or time base registers.
## On IBM SP systems, the resolution is one nanosecond.
silc_timer_ibm_realtime_available="no"
AC_MSG_CHECKING([for ibm_realtime timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/time.h>]],
                                [[timebasestruct_t t;
read_real_time( &t, TIMEBASE_SZ );
time_base_to_time( &t, TIMEBASE_SZ );]])],
               [silc_timer_ibm_realtime_available="yes"], [])
AC_MSG_RESULT([$silc_timer_ibm_realtime_available])
])

##

AC_DEFUN([SILC_TIMER_CRAY_RTC_AVAILABLE],[
# not tested yet as I don't have access to a cray x1
silc_timer_cray_rtc_available="no"
AC_MSG_CHECKING([for cray_rtc timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <intrinsics.h>]],
                                [[double clockspeed = 1.0 / sysconf( _SC_SV2_USER_TIME_RATE );
unsigned long long wtime = _rtc();]])],
               [silc_timer_cray_rtc_available="yes"], [])
AC_MSG_RESULT([$silc_timer_cray_rtc_available])
])

##

AC_DEFUN([SILC_TIMER_RTS_GET_TIMEBASE_AVAILABLE],[
# not tested yet as I don't have access to a bgl
silc_timer_bgl_rts_get_timebase_available="no"
silc_timer_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I/bgl/BlueLight/ppcfloor/bglsys/include"
AC_MSG_CHECKING([for bgl_rts_get_timebase timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <bglpersonality.h>
#include <rts.h>]],
                                [[BGLPersonality mybgl;
rts_get_personality(&mybgl, sizeof(BGLPersonality));
double clockspeed = 1.0/(double)BGLPersonality_clockHz(&mybgl);
double wtime = rts_get_timebase() * clockspeed;]])],
               [silc_timer_bgl_rts_get_timebase_available="yes"], [])
CPPFLAGS="$silc_timer_save_CPPFLAGS"
AC_MSG_RESULT([$silc_timer_bgl_rts_get_timebase_available])
])

##

AC_DEFUN([SILC_TIMER_IBM_SWITCH_CLOCK_AVAILABLE],[
silc_timer_ibm_switch_clock_available="no"

silc_timer_save_LIBS="$LIBS"
AC_SEARCH_LIBS([swclockInit], [swclock], [silc_timer_have_libswclock="yes"])
LIBS="$silc_timer_save_LIBS"

if test "x${silc_timer_have_libswclock}" = "xyes"; then
    silc_timer_libswclock="$ac_cv_search_swclockInit"

    AC_MSG_CHECKING([for ibm_switch_clock timer])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <swclock.h>]],
                                    [[swclock_handle_t handle = swclockInit();
swclockRead(handle);]])],
                   [silc_timer_ibm_switch_clock_available="yes"], [])
    AC_MSG_RESULT([$silc_timer_ibm_switch_clock_available])
fi
])

##

AC_DEFUN([SILC_TIMER_NEC_SYSSX_HGTIME_AVAILABLE],[
# not tested yet as I don't have access to a nec sx
silc_timer_nec_syssx_hgtime_available="no"
AC_MSG_CHECKING([for nec_syssx_hgtime timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/syssx.h>]],
                                [[unsigned long long val;
syssx(HGTIME, &val);]])],
               [silc_timer_nec_syssx_hgtime_available="yes"], [])
AC_MSG_RESULT([$silc_timer_nec_syssx_hgtime_available])
])


###############################################################################

AC_DEFUN([SILC_TIMER], [
# set e.g. to silc_timer_librt if appropriate 
silc_timer_lib=""

silc_user_timer_given="no"
silc_timer_given="no"
silc_timer=""

# init all timers to "no". then evaluate user arguments and availability 
# and set one of them to "yes"
silc_timer_bgl_rts_get_timebase="no"
silc_timer_bgp_get_timebase="no"
silc_timer_clock_gettime="no"
silc_timer_cray_dclock="no"
silc_timer_cray_rtc="no"
silc_timer_cray_rtclock="no"
silc_timer_cycle_counter_itc="no"
silc_timer_cycle_counter_tsc="no"
silc_timer_gettimeofday="no"
silc_timer_ibm_realtime="no"
silc_timer_ibm_switch_clock="no"
silc_timer_intel_mmtimer="no"
silc_timer_nec_syssx_hgtime="no"
silc_timer_papi_real_cyc="no"
silc_timer_papi_real_usec="no"
silc_timer_sun_gethrtime="no"

AC_LANG_PUSH([C])
SILC_TIMER_BGP_GET_TIMEBASE_AVAILABLE
SILC_TIMER_CLOCK_GETTIME_AVAILABLE
SILC_TIMER_CRAY_DCLOCK_AVAILABLE
SILC_TIMER_CRAY_RTCLOCK_AVAILABLE
SILC_TIMER_CRAY_RTC_AVAILABLE
SILC_TIMER_CYCLE_COUNTER_ITC_AVAILABLE
SILC_TIMER_CYCLE_COUNTER_TSC_AVAILABLE
SILC_TIMER_GETTIMEOFDAY_AVAILABLE
SILC_TIMER_IBM_REALTIME_AVAILABLE
SILC_TIMER_IBM_SWITCH_CLOCK_AVAILABLE
SILC_TIMER_INTEL_MMTIMER_AVAILABLE
SILC_TIMER_NEC_SYSSX_HGTIME_AVAILABLE
SILC_TIMER_PAPI_REAL_CYC_AVAILABLE
SILC_TIMER_PAPI_REAL_USEC_AVAILABLE
SILC_TIMER_RTS_GET_TIMEBASE_AVAILABLE
SILC_TIMER_SUN_GETHRTIME_AVAILABLE
AC_LANG_POP([C])
# now all *_available variables are set

SILC_TIMER_ENABLE_SPECIFIC

SILC_TIMER_PLATFORM_DEFAULTS

AS_IF([test "x${silc_user_timer_given}" = "xno" && test "x${silc_timer_given}" = "xno"],
      [AC_MSG_ERROR([No suitable timer found. You may specify one with the --enable-timer options])],
      [AC_MSG_NOTICE([Using timer ${silc_timer}])])


AM_CONDITIONAL([SILC_TIMER_BGL_RTS_GET_TIMEBASE], [test "x${silc_timer_bgl_rts_get_timebase}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_BGP_GET_TIMEBASE],     [test "x${silc_timer_bgp_get_timebase}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CLOCK_GETTIME],        [test "x${silc_timer_clock_gettime}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_DCLOCK],          [test "x${silc_timer_cray_dclock}"       = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_RTCLOCK],         [test "x${silc_timer_cray_rtclock}"      = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_RTC],             [test "x${silc_timer_cray_rtc}"          = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CYCLE_COUNTER_ITC],    [test "x${silc_timer_cycle_counter_itc}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CYCLE_COUNTER_TSC],    [test "x${silc_timer_cycle_counter_tsc}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_GETTIMEOFDAY],         [test "x${silc_timer_gettimeofday}"      = "xyes"])
AM_CONDITIONAL([SILC_TIMER_IBM_REALTIME],         [test "x${silc_timer_ibm_realtime}"      = "xyes"])
AM_CONDITIONAL([SILC_TIMER_IBM_SWITCH_CLOCK],     [test "x${silc_timer_ibm_switch_clock}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_INTEL_MMTIMER],        [test "x${silc_timer_intel_mmtimer}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_NEC_SYSSX_HGTIME],     [test "x${silc_timer_nec_syssx_hgtime}"  = "xyes"])
AM_CONDITIONAL([SILC_TIMER_PAPI_REAL_CYC],        [test "x${silc_timer_papi_real_cyc}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_PAPI_REAL_USEC],       [test "x${silc_timer_papi_real_usec}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_SUN_GETHRTIME],        [test "x${silc_timer_sun_gethrtime}"     = "xyes"])

AS_IF([test "x${silc_timer_clock_gettime}"    = "xyes"], [silc_timer_lib=${silc_timer_librt}],
      [test "x${silc_timer_ibm_switch_clock}" = "xyes"], [silc_timer_lib=${silc_timer_libswclock}])
AC_SUBST([TIMER_LIB], ["$silc_timer_lib"])
])

###############################################################################

AC_DEFUN([SILC_TIMER_PLATFORM_DEFAULTS], [
AS_IF([test "x${silc_user_timer_given}" = "xno"], [
    AS_CASE([${ac_silc_platform}], 
            ["altix"],    [AS_IF([test "x$silc_timer_intel_mmtimer_available" = "xyes"], 
                                 [silc_timer_intel_mmtimer="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="intel_mmtimer"],
                                 [test "x$silc_timer_clock_gettime_available" = "xyes"], 
                                 [silc_timer_clock_gettime="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="clock_gettime"])],
            ["bgl"],      [AS_IF([test "x$silc_timer_bgl_rts_get_timebase_available" = "xyes"],
                                 [silc_timer_bgl_rts_get_timebase="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="rts_get_timebase"])],
            ["bgp"],      [AS_IF([test "x$silc_timer_bgp_get_timebase_available" = "xyes"],
                                 [silc_timer_bgp_get_timebase="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="bgp_get_timebase"])],
            ["crayxt"],   [AS_IF([test "x$silc_timer_cray_dclock_available" = "xyes"],
                                 [silc_timer_cray_dclock="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cray_dclock"],
                                 [test "x$silc_timer_cycle_counter_itc_available" = "xyes"],
                                 [silc_timer_cycle_counter_itc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cycle_counter_itc"],
                                 [test "x$silc_timer_cycle_counter_tsc_available" = "xyes"],
                                 [silc_timer_cycle_counter_tsc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cycle_counter_tsc"],
                                 [test "x$silc_timer_clock_gettime_available" = "xyes"],
                                 [silc_timer_clock_gettime="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="clock_gettime"],
                                 [test "x$silc_timer_gettimeofday_available" = "xyes"],
                                 [silc_timer_gettimeofday="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="gettimeofday"])],
            ["sicortex"], [AS_IF([test "x$silc_timer_gettimeofday_available" = "xyes"],
                                 [silc_timer_gettimeofday="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="gettimeofday"])],
            ["sun"],      [AS_IF([test "x$silc_timer_sun_gethrtime_available" = "xyes"],
                                 [silc_timer_sun_gethrtime="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="sun_gethrtime"])],
            ["mac"],      [AS_IF([test "x$silc_timer_cycle_counter_tsc_available" = "xyes"],
                                 [silc_timer_cycle_counter_tsc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cycle_counter_tsc"],
                                 [test "x$silc_timer_cycle_counter_itc_available" = "xyes"],
                                 [silc_timer_cycle_counter_itc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cycle_counter_itc"],
                                 [test "x$silc_timer_gettimeofday_available" = "xyes"],
                                 [silc_timer_gettimeofday="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="gettimeofday"])],
            ["ibm"],      [AS_IF([test "x$silc_timer_ibm_realtime_available" = "xyes"],
                                 [silc_timer_ibm_realtime="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="ibm_realtime"],
                                 [test "x$silc_timer_ibm_switch_clock_available" = "xyes"],
                                 [silc_timer_ibm_switch_clock="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="ibm_switch_clock"])],
            ["crayx1"],   [AS_IF([test "x$silc_timer_cray_rtc_available" = "xyes"],
                                 [silc_timer_cray_rtc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cray_rtc"],
                                 [test "x$silc_timer_gettimeofday_available" = "xyes"],
                                 [silc_timer_gettimeofday="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="gettimeofday"])],
            ["necsx"],    [AS_IF([test "x$silc_timer_nec_syssx_hgtime_available" = "xyes"],
                                 [silc_timer_nec_syssx_hgtime="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="nec_syssx_hgtime"])],
            ["linux"],    [AS_IF([test "x$silc_timer_cycle_counter_itc_available" = "xyes"],
                                 [silc_timer_cycle_counter_itc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cycle_counter_itc"],
                                 [test "x$silc_timer_cycle_counter_tsc_available" = "xyes"],
                                 [silc_timer_cycle_counter_tsc="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="cycle_counter_tsc"],
                                 [test "x$silc_timer_clock_gettime_available" = "xyes"],
                                 [silc_timer_clock_gettime="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="clock_gettime"],
                                 [test "x$silc_timer_gettimeofday_available" = "xyes"],
                                 [silc_timer_gettimeofday="yes"; 
                                  silc_timer_given="yes";
                                  silc_timer="gettimeofday"])],
            [AC_MSG_ERROR([Encountered unsupported platform \"$ac_silc_platform\" during timer checks.])])
])
])

###############################################################################

AC_DEFUN([SILC_TIMER_ENABLE_SPECIFIC], [
AC_ARG_ENABLE([timer-bgp_get_timebase],
              [AS_HELP_STRING([--enable-timer-bgp_get_timebase],
                              [enable bgp_get_timebase timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_bgp_get_timebase_available}" = "xno"; then
                       AC_MSG_WARN(["timer-bgp_get_timebase not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="bgp_get_timebase"
                       silc_timer_bgp_get_timebase="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-clock_gettime],
              [AS_HELP_STRING([--enable-timer-clock_gettime],
                              [enable clock_gettime timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_clock_gettime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-clock_gettime not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="clock_gettime"
                       silc_timer_clock_gettime="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cray_rtclock],
              [AS_HELP_STRING([--enable-timer-cray_rtclock],
                              [enable cray_rtclock timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_cray_rtclock_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cray_rtclock not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="cray_rtclock"
                       silc_timer_cray_rtclock="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cycle_counter_itc],
              [AS_HELP_STRING([--enable-timer-cycle_counter_itc],
                              [enable cycle_counter_itc timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_cycle_counter_itc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cycle_counter_itc not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="cycle_counter_itc"
                       silc_timer_cycle_counter_itc="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cycle_counter_tsc],
              [AS_HELP_STRING([--enable-timer-cycle_counter_tsc],
                              [enable cycle_counter_tsc timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_cycle_counter_tsc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cycle_counter_tsc not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="cycle_counter_tsc"
                       silc_timer_cycle_tsc_counter="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-cray_dclock],
              [AS_HELP_STRING([--enable-timer-cray_dclock],
                              [enable cray_dclock timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_cray_dclock_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cray_dclock not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="cray_dclock"
                       silc_timer_cray_dclock="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-sun_gethrtime],
              [AS_HELP_STRING([--enable-timer-sun_gethrtime],
                              [enable sun_gethrtime timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_sun_gethrtime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-sun_gethrtime not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="sun_gethrtime"
                       silc_timer_sun_gethrtime="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-gettimeofday],
              [AS_HELP_STRING([--enable-timer-gettimeofday],
                              [enable gettimeofday timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_gettimeofday_available}" = "xno"; then
                       AC_MSG_WARN(["timer-gettimeofday not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="gettimeofday"
                       silc_timer_gettimeofday="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-intel_mmtimer],
              [AS_HELP_STRING([--enable-timer-intel_mmtimer],
                              [enable intel_mmtimer timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_intel_mmtimer_available}" = "xno"; then
                       AC_MSG_WARN(["timer-intel_mmtimer not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="intel_mmtimer"
                       silc_timer_intel_mmtimer="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-papi_real_cyc],
              [AS_HELP_STRING([--enable-timer-papi_real_cyc],
                              [enable papi_real_cyc timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_papi_real_cyc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-papi_real_cyc not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="papi_real_cyc"
                       silc_timer_papi_real_cyc="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-papi_real_usec],
              [AS_HELP_STRING([--enable-timer-papi_real_usec],
                              [enable papi_real_usec timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_papi_real_usec_available}" = "xno"; then
                       AC_MSG_WARN(["timer-papi_real_usec not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="papi_real_usec"
                       silc_timer_papi_real_usec="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-ibm_realtime],
              [AS_HELP_STRING([--enable-timer-ibm_realtime],
                              [enable ibm_realtime timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_ibm_realtime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-ibm_realtime not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="ibm_realtime"
                       silc_timer_ibm_realtime="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer_cray_rtc],
              [AS_HELP_STRING([--enable-timer-cray_rtc],
                              [enable cray_rtc timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_cray_rtc_available}" = "xno"; then
                       AC_MSG_WARN(["timer-cray_rtc not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="cray_rtc"
                       silc_timer_cray_rtc="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-bgl_rts_get_timebase],
              [AS_HELP_STRING([--enable-timer-bgl_rts_get_timebase],
                              [enable bgl_rts_get_timebase timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_bgl_rts_get_timebase_available}" = "xno"; then
                       AC_MSG_WARN(["timer-bgl_rts_get_timebase not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="bgl_rts_get_timebase"
                       silc_timer_bgl_rts_get_timebase="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-ibm_switch_clock],
              [AS_HELP_STRING([--enable-timer-ibm_switch_clock],
                              [enable ibm_switch_clock timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_ibm_switch_clock_available}" = "xno"; then
                       AC_MSG_WARN(["timer-ibm_switch_clock not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="ibm_switch_clock"
                       silc_timer_ibm_switch_clock="yes"
                   fi
               fi
              ])


AC_ARG_ENABLE([timer-nec_syssx_hgtime],
              [AS_HELP_STRING([--enable-timer-nec_syssx_hgtime],
                              [enable nec_syssx_hgtime timer if available instead of platform default])],
              [if test "x${silc_user_timer_given}" = "xyes"; then
                   AC_MSG_WARN(["Several timers provided, using $silc_timer"])
               else
                   if test "x${silc_timer_nec_syssx_hgtime_available}" = "xno"; then
                       AC_MSG_WARN(["timer-nec_syssx_hgtime not available"])
                   else
                       silc_user_timer_given="yes"
                       silc_timer="nec_syssx_hgtime"
                       silc_timer_nec_syssx_hgtime="yes"
                   fi
               fi
              ])
])

