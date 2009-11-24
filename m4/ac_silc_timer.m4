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

AC_MSG_RESULT([$ac_silc_timer_bgp_get_timebase_available])
])

AC_DEFUN([AC_SILC_TIMER_CLOCK_GETTIME_AVAILABLE],[
ac_silc_timer_clock_gettime_available="no"
AC_MSG_CHECKING([for clock gettime timer])

AC_MSG_RESULT([$ac_silc_timer_clock_gettime_available])
])

AC_DEFUN([AC_SILC_TIMER_CRAY_RTCLOCK_AVAILABLE],[
ac_silc_timer_cray_rtclock_available="no"
AC_MSG_CHECKING([for cray rtclock timer])

AC_MSG_RESULT([$ac_silc_timer_cray_rtclock_available])
])

AC_DEFUN([AC_SILC_TIMER_CYCLE_COUNTER_AVAILABLE],[
ac_silc_timer_cycle_counter_available="no"
AC_MSG_CHECKING([for cycle counter timer])

AC_MSG_RESULT([$ac_silc_timer_cycle_counter_available])
])

AC_DEFUN([AC_SILC_TIMER_CRAY_DCLOCK_AVAILABLE],[
ac_silc_timer_cray_dclock_available="no"
AC_MSG_CHECKING([for cray dclock timer])

AC_MSG_RESULT([$ac_silc_timer_cray_dclock_available])
])

AC_DEFUN([AC_SILC_TIMER_SUN_GETHRTIME_AVAILABLE],[
ac_silc_timer_sun_gethrtime_available="no"
AC_MSG_CHECKING([for sun gethrtime timer])

AC_MSG_RESULT([$ac_silc_timer_sun_gethrtime_available])
])

AC_DEFUN([AC_SILC_TIMER_GETTIMEOFDAY_AVAILABLE],[
ac_silc_timer_gettimeofday_available="no"
AC_MSG_CHECKING([for gettimeofday timer])

AC_MSG_RESULT([$ac_silc_timer_gettimeofday_available])
])

AC_DEFUN([AC_SILC_TIMER_INTEL_MMTIMER_AVAILABLE],[
ac_silc_timer_intel_mmtimer_available="no"
AC_MSG_CHECKING([for intel mmtimer timer])

AC_MSG_RESULT([$ac_silc_timer_intel_mmtimer_available])
])

AC_DEFUN([AC_SILC_TIMER_PAPI_REAL_CYC_AVAILABLE],[
ac_silc_timer_papi_real_cyc_available="no"
AC_MSG_CHECKING([for papi real cyc timer])

AC_MSG_RESULT([$ac_silc_timer_papi_real_cyc_available])
])

AC_DEFUN([AC_SILC_TIMER_PAPI_REAL_USEC_AVAILABLE],[
ac_silc_timer_papi_real_usec_available="no"
AC_MSG_CHECKING([for papi real usec timer])

AC_MSG_RESULT([$ac_silc_timer_papi_real_usec_available])
])

AC_DEFUN([AC_SILC_TIMER_POWER_REALTIME_AVAILABLE],[
ac_silc_timer_power_realtime_available="no"
AC_MSG_CHECKING([for power realtime timer])

AC_MSG_RESULT([$ac_silc_timer_power_realtime_available])
])

AC_DEFUN([AC_SILC_TIMER_CRAY_RTC_AVAILABLE],[
ac_silc_timer_cray_rtc_available="no"
AC_MSG_CHECKING([for cray rtc timer])

AC_MSG_RESULT([$ac_silc_timer_cray_rtc_available])
])

AC_DEFUN([AC_SILC_TIMER_RTS_GET_TIMEBASE_AVAILABLE],[
ac_silc_timer_rts_get_timebase_available="no"
AC_MSG_CHECKING([for rts get timebase timer])

AC_MSG_RESULT([$ac_silc_timer_rts_get_timebase_available])
])

AC_DEFUN([AC_SILC_TIMER_IBM_SWITCH_CLOCK_AVAILABLE],[
ac_silc_timer_ibm_switch_clock_available="no"
AC_MSG_CHECKING([for ibm switch clock timer])

AC_MSG_RESULT([$ac_silc_timer_ibm_switch_clock_available])
])

AC_DEFUN([AC_SILC_TIMER_NEC_SYSSX_HGTIME_AVAILABLE],[
ac_silc_timer_nec_syssx_hgtime_available="no"
AC_MSG_CHECKING([for nec syssx hgtime timer])

AC_MSG_RESULT([$ac_silc_timer_nec_syssx_hgtime_available])
])



AC_DEFUN([AC_SILC_TIMER], [
# init all timers to "no". then evaluate user arguments and availability 
# and set one of them to "yes"
ac_silc_timer_bgp_get_timebase="no"
ac_silc_timer_clock_gettime="no"
ac_silc_timer_cray_rtclock="no"
ac_silc_timer_cycle_counter="no"
ac_silc_timer_cray_dclock="no"
ac_silc_timer_sun_gethrtime="no"
ac_silc_timer_gettimeofday="no"
ac_silc_timer_intel_mmtimer="no"
ac_silc_timer_papi_real_cyc="no"
ac_silc_timer_papi_real_usec="no"
ac_silc_timer_power_realtime="no"
ac_silc_timer_cray_rtc="no"
ac_silc_timer_rts_get_timebase="no"
ac_silc_timer_ibm_switch_clock="no"
ac_silc_timer_nec_syssx_hgtime="no"




AM_CONDITIONAL([SILC_TIMER_BGP_GET_TIMEBASE], [test "x${ac_silc_timer_bgp_get_timebase}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CLOCK_GETTIME],    [test "x${ac_silc_timer_clock_gettime}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_RTCLOCK],     [test "x${ac_silc_timer_cray_rtclock}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CYCLE_COUNTER],    [test "x${ac_silc_timer_cycle_counter}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_DCLOCK],      [test "x${ac_silc_timer_cray_dclock}"      = "xyes"])
AM_CONDITIONAL([SILC_TIMER_SUN_GETHRTIME],    [test "x${ac_silc_timer_sun_gethrtime}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_GETTIMEOFDAY],     [test "x${ac_silc_timer_gettimeofday}"     = "xyes"])
AM_CONDITIONAL([SILC_TIMER_INTEL_MMTIMER],    [test "x${ac_silc_timer_intel_mmtimer}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_PAPI_REAL_CYC],    [test "x${ac_silc_timer_papi_real_cyc}"    = "xyes"])
AM_CONDITIONAL([SILC_TIMER_PAPI_REAL_USEC],   [test "x${ac_silc_timer_papi_real_usec}"   = "xyes"])
AM_CONDITIONAL([SILC_TIMER_POWER_REALTIME],   [test "x${ac_silc_timer_power_realtime}"   = "xyes"])
AM_CONDITIONAL([SILC_TIMER_CRAY_RTC],         [test "x${ac_silc_timer_cray_rtc}"         = "xyes"])
AM_CONDITIONAL([SILC_TIMER_RTS_GET_TIMEBASE], [test "x${ac_silc_timer_rts_get_timebase}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_IBM_SWITCH_CLOCK], [test "x${ac_silc_timer_ibm_switch_clock}" = "xyes"])
AM_CONDITIONAL([SILC_TIMER_NEC_SYSSX_HGTIME], [test "x${ac_silc_timer_nec_syssx_hgtime}" = "xyes"])
])

