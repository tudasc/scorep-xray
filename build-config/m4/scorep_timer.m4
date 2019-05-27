## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2013, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2016, 2018,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##



# SCOREP_TIMER
# ------------
# ...
# Defines that are prefixed by HAVE_BACKEND_SCOREP_ need to be communicated to the frontend.
AC_DEFUN([SCOREP_TIMER],
[AS_UNSET([scorep_timers])
AS_UNSET([scorep_timer_cppflags])
AS_UNSET([scorep_timer_ldflags])
AS_UNSET([scorep_timer_libs])
AS_UNSET([scorep_timer_default])

# Workaround: will create ac_fn_c_check_type shell function that is
# needed for hrtime_t check. For unknown reasons, the hrtime_t check
# does not create this function which leads to a
#    <dir>/configure: line <n>: ac_fn_c_check_type: command not found.
# error.
AC_CHECK_TYPE([dummy])

_SCOREP_TIMER_CHECK([gettimeofday], [_SCOREP_TIMER_GETTIMEOFDAY])
_SCOREP_TIMER_CHECK([clock_gettime], [_SCOREP_TIMER_CLOCK_GETTIME], [], [], [-lrt])

AS_CASE([${ac_scorep_platform}],
    [bgl],   [_SCOREP_TIMER_CHECK([bgl], [_SCOREP_TIMER_BGL], [-I/bgl/BlueLight/ppcfloor/bglsys/include])],
    [bgp],   [_SCOREP_TIMER_CHECK([bgp], [_SCOREP_TIMER_BGP], [-I/bgsys/drivers/ppcfloor/arch/include],
                                                              [-L/bgsys/drivers/ppcfloor/runtime/SPI],
                                                              [-lSPI.cna -lrt])],
    [bgq],   [_SCOREP_TIMER_CHECK([bgq], [_SCOREP_TIMER_BGQ], [-I/bgsys/drivers/ppcfloor])],
    [mingw], [_SCOREP_TIMER_CHECK([mingw], [_SCOREP_TIMER_MINGW])],
    [mac],   [_SCOREP_TIMER_CHECK([mac], [_SCOREP_TIMER_MAC])],
    [aix],   [_SCOREP_TIMER_CHECK([aix], [_SCOREP_TIMER_AIX])],
    [mic],   [# Do nothing but prevent selection of TSC.
             ],
    [_SCOREP_TIMER_CHECK([tsc], [_SCOREP_TIMER_TSC], [], [-lm])])

# Output
AS_VAR_SET_IF([scorep_timers],
    [AC_DEFINE_UNQUOTED(HAVE_BACKEND_SCOREP_TIMER_DEFAULT, ["${scorep_timer_default}"], [Default timer])
    AFS_SUMMARY_PUSH
    AS_VAR_SET_IF([scorep_timer_cppflags],
        [AFS_SUMMARY([Timer CPPFLAGS], [${scorep_timer_cppflags}])])
    AS_VAR_SET_IF([scorep_timer_ldflags],
        [AFS_SUMMARY([Timer LDFLAGS], [${scorep_timer_ldflags}])])
    AS_VAR_SET_IF([scorep_timer_libs],
        [AFS_SUMMARY([Timer LIBS], [${scorep_timer_libs}])])
     AFS_SUMMARY_POP([Timer support], [yes, providing ${scorep_timers}])],
    [AC_MSG_FAILURE([no suitable timer detected.])])

AC_SUBST([SCOREP_TIMER_CPPFLAGS], [${scorep_timer_cppflags}])
AC_SUBST([SCOREP_TIMER_LDFLAGS], [${scorep_timer_ldflags}])
AC_SUBST([SCOREP_TIMER_LIBS], [${scorep_timer_libs}])
])# SCOREP_TIMER


# _SCOREP_TIMER_CHECK( TIMER_NAME, CHECK [, CPPFLAGS [, LDFLAGS [, LIBS]]])
# ----------------------------------------------------------------------
# ...
AC_DEFUN([_SCOREP_TIMER_CHECK],
[# Check for timer $1
have_timer=no
m4_ifblank($1, [m4_fatal([Provide timer name as first argument.])])
m4_ifblank($2, [m4_fatal([Provide timer check as second argument.])])
timer_name=$1
m4_ifnblank([$3],
    [timer_cppflags="$3"
    timer_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS ${timer_cppflags}"],
    [AS_UNSET([timer_cppflags])])
m4_ifnblank([$4],
    [timer_ldflags="$4"
    timer_save_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS ${timer_ldflags}"],
    [AS_UNSET([timer_ldflags])])

# If LIBS are given, first execute CHECK without LIBS. If this fails,
# execute it again, this time with LIBS.
AS_UNSET([timer_libs])
$2
m4_ifnblank([$5],
    [AS_IF([test "x${have_timer}" = xno],
        [timer_libs="$5"
        timer_save_LIBS="$LIBS"
        LIBS="$LIBS ${timer_libs}"
        $2
        ])])

AS_IF([test "x${have_timer}" = xyes],
    [AC_DEFINE(HAVE_BACKEND_SCOREP_TIMER_[]m4_toupper($1), [1], [Timer $1 available])
    scorep_timer_default=$1
    AS_VAR_SET_IF([scorep_timers],
        [scorep_timers="${scorep_timers}, ${timer_name}"],
        [scorep_timers=${timer_name}])
    AS_VAR_SET_IF([timer_cppflags],
        [AS_VAR_SET_IF([scorep_timer_cppflags],
            [scorep_timer_cppflags="${scorep_timer_cppflags} ${timer_cppflags}"],
            [scorep_timer_cppflags=${timer_cppflags}])])
    AS_VAR_SET_IF([timer_ldflags],
        [AS_VAR_SET_IF([scorep_timer_ldflags],
            [scorep_timer_ldflags="${scorep_timer_ldflags} ${timer_ldflags}"],
            [scorep_timer_ldflags=${timer_ldflags}])])
    AS_VAR_SET_IF([timer_libs],
        [AS_VAR_SET_IF([scorep_timer_libs],
            [scorep_timer_libs="${scorep_timer_libs} ${timer_libs}"],
            [scorep_timer_libs=${timer_libs}])])])

AS_VAR_SET_IF([timer_cppflags], [CPPFLAGS="$timer_save_CPPFLAGS"])
AS_VAR_SET_IF([timer_ldflags], [LDFLAGS="$timer_save_LDFLAGS"])
AS_VAR_SET_IF([timer_libs], [LIBS="$timer_save_LIBS"])
# Check for timer $1 done
])# _SCOREP_TIMER_CHECK


# _SCOREP_TIMER_GETTIMEOFDAY
# --------------------------
# ...
AC_DEFUN([_SCOREP_TIMER_GETTIMEOFDAY],
[AC_MSG_CHECKING([for gettimeofday timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        #include <sys/time.h>
        ]],
        [[
        struct timeval tp;
        gettimeofday( &tp, 0 );
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_GETTIMEOFDAY


# _SCOREP_TIMER_CLOCK_GETTIME
# ---------------------------
# ...
AC_DEFUN([_SCOREP_TIMER_CLOCK_GETTIME],
[for tmp_clk_id in CLOCK_REALTIME CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW; do
    AC_MSG_CHECKING([for clock_gettime (${tmp_clk_id}) timer])
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
            [[
            #include <time.h>
            ]],
            [
            struct timespec tp;
            clock_getres( ${tmp_clk_id}, &tp );
            clock_gettime( ${tmp_clk_id}, &tp );
            ])
        ],
        [have_timer=yes
        timer_clk_id=${tmp_clk_id}])
    AC_MSG_RESULT([$have_timer])
done
AS_IF([test "x${have_timer}" = xyes],
    [AC_DEFINE_UNQUOTED([SCOREP_TIMER_CLOCK_GETTIME_CLK_ID],
        [${timer_clk_id}],
        [The clk_id used in clock_gettime calls.])
     AC_DEFINE_UNQUOTED([HAVE_BACKEND_SCOREP_TIMER_CLOCK_GETTIME_CLK_ID_NAME],
        ["${timer_clk_id}"],
        [The clk_id as string used in clock_gettime calls.])
     # overwrite timer_name, so that we see which clock we use
     timer_name="clock_gettime(${timer_clk_id})"])
])# _SCOREP_TIMER_CLOCK_GETTIME


# _SCOREP_TIMER_BGL
# -----------------
# ...
AC_DEFUN([_SCOREP_TIMER_BGL],
[AC_MSG_CHECKING([for Blue Gene/L timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        #include <bglpersonality.h>
        #include <rts.h>
        ]],
        [[
        BGLPersonality mybgl;
        rts_get_personality(&mybgl, sizeof(BGLPersonality));
        double clockspeed = 1.0/(double)BGLPersonality_clockHz(&mybgl);
        double wtime = rts_get_timebase() * clockspeed;
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_BGL


# _SCOREP_TIMER_BGP
# -----------------
# ...
AC_DEFUN([_SCOREP_TIMER_BGP],
[AC_MSG_CHECKING([for Blue Gene/P timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        #include <common/bgp_personality.h>
        #include <common/bgp_personality_inlines.h>
        #include <spi/kernel_interface.h>
        ]],
        [[
        _BGP_Personality_t mybgp;
        Kernel_GetPersonality( &mybgp, sizeof( _BGP_Personality_t ) );
        long ticks_per_sec = ( long )BGP_Personality_clockMHz( &mybgp ) * 1e6;
        _bgp_GetTimeBase();
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_BGP


# _SCOREP_TIMER_BGQ
# -----------------
# ...
AC_DEFUN([_SCOREP_TIMER_BGQ],
[AC_MSG_CHECKING([for Blue Gene/Q timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        /* DEFAULT_FREQ_MHZ in firmware/include/personality.h
         * __mftb is build-in, see
         * http://publib.boulder.ibm.com/infocenter/comphelp/v101v121/index.jsp?topic=/com.ibm.xlcpp101.aix.doc/compiler_ref/bif_mftb.html
         */
        #include <firmware/include/personality.h>
        #ifdef __GNUC__
        #include <ppu_intrinsics.h>
        #endif
        #ifndef DEFAULT_FREQ_MHZ
        # DEFAULT_FREQ_MHZ_not_defined_Check_your_includes
        #endif
        ]],
        [[
        double clockspeed      = 1.0e-6 / DEFAULT_FREQ_MHZ;
        double time_in_seconds = ( __mftb() * clockspeed );
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_BGQ


# _SCOREP_TIMER_MINGW
# -------------------
# ...
AC_DEFUN([_SCOREP_TIMER_MINGW],
[AC_MSG_CHECKING([for Windows QueryPerformanceCounter timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        #include <windows.h>
        #include <stdint.h>
        #define COMPILE_TIME_ASSERT( pred ) switch ( 0 ) { case 0: case pred: ; }
        ]],
        [[
        /* QuadPart is a LONGLONG */
        COMPILE_TIME_ASSERT( sizeof( unit64_t ) >= sizeof( LONGLONG ) );
        LARGE_INTEGER t1;
        QueryPerformanceCounter( &t1 );
        uint64_t ticks = (uint64_t )t1.QuadPart;
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency( &frequency );
        uint64_t ticks_per_sec_mingw = ( uint64_t )frequency.QuadPart;
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_MINGW


# _SCOREP_TIMER_MAC
# -----------------
# ...
AC_DEFUN([_SCOREP_TIMER_MAC],
[AC_MSG_CHECKING([for macOS/Mach absolute time timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        #include <stdint.h>
        #include <inttypes.h>
        #include <mach/mach_time.h>
        ]],
        [[
        mach_timebase_info_data_t timebase;
        mach_timebase_info( &timebase );
        double ticks_to_nsec_mac = timebase.numer / ( double )timebase.denom;
        return ( uint64_t )( ticks_to_nsec_mac * mach_absolute_time() );
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_MAC


# _SCOREP_TIMER_AIX
# -----------------
# ...
AC_DEFUN([_SCOREP_TIMER_AIX],
[AC_MSG_CHECKING([for AIX read_real_time timer])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[
        #include <stdint.h>
        #include <sys/time.h>
        #include <sys/systemcfg.h>
        ]],
        [[
        timebasestruct_t tmp;
        read_real_time( &tmp, TIMEBASE_SZ );
        /* Convert to seconds and nanoseconds, if necessary. */
        time_base_to_time( &tmp, TIMEBASE_SZ );
        uint64_t nanoseconds = ( uint64_t )tmp.tb_high * UINT64_C( 1e9 ) + ( uint64_t )tmp.tb_low;
        ]])
    ],
    [have_timer=yes])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_AIX


# _SCOREP_TIMER_TSC
# -----------------
# ...
AC_DEFUN([_SCOREP_TIMER_TSC],
[# prerequisites (modified) for cycle.h tsc detection, see http://www.fftw.org/cycle.h

AC_CHECK_HEADERS([sys/time.h intrinsics.h])

# This alone would not trigger the generation of the shell function
# ac_fn_c_check_type, there we added a dummy check at the start of the
# timer macro.
AC_CHECK_TYPE([hrtime_t],
    [AC_DEFINE([HAVE_HRTIME_T], [1], [Define to 1 if hrtime_t is defined in <sys/time.h>])],
    [],
    [[
    #if HAVE_SYS_TIME_H
    #include <sys/time.h>
    #endif
    ]])

AC_CHECK_FUNCS([gethrtime read_real_time time_base_to_time])

# Cray UNICOS _rtc() (reaql-time clock) intrinsic
AC_MSG_CHECKING([for _rtc intrinsic])
rtc_ok=yes
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [
        #ifdef HAVE_INTRINSICS_H
        #include <intrinsics.h>
        #endif
        ],
        [
        _rtc()
        ])
    ],
    [AC_DEFINE(HAVE__RTC, 1, [Define if you have the UNICOS _rtc() intrinsic.])],
    [rtc_ok=no])
AC_MSG_RESULT($rtc_ok)
# end of prerequisites

AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [
        #define HAVE( H ) ( defined( HAVE_ ## H ) && HAVE_ ## H )
        #include "${srcdir}/../src/services/timer/scorep_timer_tsc.h"
        ],
        [
        #include "${srcdir}/../src/services/timer/scorep_timer_tsc.inc.c"
        ])
    ],
    [have_timer=yes
    # assign name
    for tsc in X86_64 \
        SPARC \
        CRAY \
        POWERPC64 \
        ARMV8 \
        POWERPC32 \
        X86_32 \
        X86_64_MSVC \
        IA64_INTEL \
        IA64_GCC \
        IA64_HPUX \
        IA64_MSVC \
        SOLARIS; do
        AC_LINK_IFELSE(
            [AC_LANG_PROGRAM(
                [
                #define HAVE( H ) ( defined( HAVE_ ## H ) && HAVE_ ## H )
                #include "${srcdir}/../src/services/timer/scorep_timer_tsc.h"
                ],
                [
                #if HAVE( SCOREP_${tsc}_TSC )
                #else
                # tsc_ne_SCOREP_${tsc}_TSC
                #endif
                ])
            ],
            [timer_name="${timer_name} (${tsc})"
            AC_DEFINE_UNQUOTED(HAVE_BACKEND_SCOREP_TIMER_TSC_NAME, ["${tsc}"], [TSC timer])
            break])
    done])
AC_MSG_CHECKING([for time stamp counter (TSC) timer])
AC_MSG_RESULT([$have_timer])
])# _SCOREP_TIMER_TSC
