AC_DEFUN([AC_SILC_TIMER], [
# init all timers to "no". then evaluate user arguments and availability 
# and set one to "yes"
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
