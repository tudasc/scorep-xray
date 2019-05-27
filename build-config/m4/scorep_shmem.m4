## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014, 2016-2017, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


## file       scorep_shmem.m4

# The macros SCOREP_SHMEM_COMPILER and SCOREP_SHMEM_WORKING are based on
# AX_MPI http://www.nongnu.org/autoconf-archive/ax_mpi.html by Steven G. Johnson
# and Julian C. Cummings. AX_MPI came with following license:
#
# LICENSE
#
#   Copyright (c) 2008 Steven G. Johnson <stevenj@alum.mit.edu>
#   Copyright (c) 2008 Julian C. Cummings <cummings@cacr.caltech.edu>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.


dnl ----------------------------------------------------------------------------

AC_DEFUN([SCOREP_SHMEM_WORKING], [
AS_IF([test x = "x$SHMEMLIBS"], [
    AC_LANG_CASE(
    [C],
       [AC_CHECK_FUNC([start_pes], [SHMEMLIBS=" "])],
    [C++],
       [AC_CHECK_FUNC([start_pes], [SHMEMLIBS=" "])],
    [Fortran 77], [
       AC_MSG_CHECKING([for START_PES])
       AC_LINK_IFELSE([AC_LANG_PROGRAM([], [dnl
      call START_PES(42)
])], [
            SHMEMLIBS=" "
            AC_MSG_RESULT([yes])
        ], [
            AC_MSG_RESULT([no])
        ])
    ],
    [Fortran], [
        AC_MSG_CHECKING([for START_PES])
        AC_LINK_IFELSE([AC_LANG_PROGRAM([], [dnl
      call START_PES(42)
])], [
            SHMEMLIBS=" "
            AC_MSG_RESULT([yes])
        ], [
            AC_MSG_RESULT([no])
        ])
    ])
])
AC_LANG_CASE(
[Fortran 77], [
    AS_IF([test x = "x$SHMEMLIBS"], [
        AC_CHECK_LIB($SHMEM_LIB_NAME, [START_PES], [SHMEMLIBS=" "])
    ])
],
[Fortran], [
    AS_IF([test x = "x$SHMEMLIBS"], [
        AC_CHECK_LIB($SHMEM_LIB_NAME, [START_PES], [SHMEMLIBS=" "])
    ])
])
AS_IF([test x = "x$SHMEMLIBS"], [
    AC_CHECK_LIB($SHMEM_LIB_NAME, [start_pes], [SHMEMLIBS=" "])
])

dnl We have to use AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[],[]) and not AC_CHECK_HEADER because the
dnl latter uses $CPP, not $CC (which may be oshcc).
AC_LANG_CASE(
[C], [
    AS_IF([test x != "x$SHMEMLIBS"], [
        AC_MSG_CHECKING([for shmem.h])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <shmem.h>]], [[]])], [
            AC_MSG_RESULT([yes])
        ], [
            SHMEMLIBS=""
            AC_MSG_RESULT([no])
        ])
    ])
],
[C++], [
    AS_IF([test x != "x$SHMEMLIBS"], [
        AC_MSG_CHECKING([for shmem.h])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <shmem.h>]], [[]])], [
            AC_MSG_RESULT([yes])
        ], [
            SHMEMLIBS=""
            AC_MSG_RESULT([no])
        ])
    ])
],
[Fortran 77], [
    AS_IF([test x != "x$SHMEMLIBS"], [
        AC_MSG_CHECKING([for shmem.fh])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[      include 'shmem.fh'])], [
            AC_MSG_RESULT([yes])
        ], [
            SHMEMLIBS=""
            AC_MSG_RESULT([no])
        ])
    ])
],
[Fortran], [
    AS_IF([test x != "x$SHMEMLIBS"], [
        AC_MSG_CHECKING([for shmem.fh])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[      include 'shmem.fh'])], [
            AC_MSG_RESULT([yes])
        ], [
            SHMEMLIBS=""
            AC_MSG_RESULT([no])
        ])
    ])
])

AC_SUBST([SHMEMLIBS])

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
AS_IF([test x = "x$SHMEMLIBS"], [
    $2
], [
    ifelse([$1], , [AC_DEFINE([HAVE_SHMEM], [1], [Define if you have the SHMEM library.])], [$1])
        :
])
])


dnl ----------------------------------------------------------------------------

## Check whether the SHMEM implementation supports the profiling interface

AC_DEFUN([_SCOREP_SHMEM_PROFILING_INTERFACE], [

dnl Do not check for prerequisite of SHMEM profiling interface on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for SHMEM profiling interface on frontend.])])

AC_LANG_PUSH([C])

scorep_shmem_has_pshmem_header="yes"
scorep_shmem_has_pshmem_functions="yes"

dnl If oshcc is from Open MPI and the version is known bad, fallback to wrapping.
dnl Known bad means no weak symbols at all.
AS_CASE([$scorep_shmem_openmpi_version],
        [1.8|1.8.1], [scorep_shmem_has_pshmem_header="no"
                     scorep_shmem_has_pshmem_functions="no"])

AC_CHECK_HEADER([pshmem.h],
                [],
                [scorep_shmem_has_pshmem_header="no"])

AC_MSG_CHECKING([for pshmem symbols])
AS_IF([test "x${scorep_shmem_has_pshmem_header}" = "xyes"],
      [AC_LINK_IFELSE([AC_LANG_SOURCE([
                        #include<pshmem.h>

                        int main()
                        {
                            pstart_pes(0);
                            pshmem_barrier_all();
                            return  0;
                        }
                        ])],
       [],
       [scorep_shmem_has_pshmem_functions="no"])],
      [AC_LINK_IFELSE([AC_LANG_SOURCE([
                        #include<shmem.h>

                        int main()
                        {
                            pstart_pes(0);
                            pshmem_barrier_all();
                            return  0;
                        }
                        ])],
       [],
       [scorep_shmem_has_pshmem_functions="no"])])
AC_MSG_RESULT([$scorep_shmem_has_pshmem_functions])

AC_LANG_POP([C])

# generating output
AS_IF([test "x${scorep_shmem_has_pshmem_functions}" = "xyes"],
      [touch scorep_have_pshmem_support.txt])
])

dnl ----------------------------------------------------------------------------

## _SCOREP_SHMEM_CHECK_SYMBOLS( PREFIX, FOUND-SYMBOL-VARIABLE )
AC_DEFUN([_SCOREP_SHMEM_CHECK_SYMBOLS], [

## == currently unsupported SHMEM symbols ==
##
##           shmalloc_nb,
##           shmem_add,
##           shmem_alltoall,
##           shmem_alltoallv,
##           shmem_alltoallv_packed,
##           shmem_broadcast,
##           shmem_complexd_put,
##           shmem_cswap,
##           shmem_double_get_nb,
##           shmem_double_put_nb,
##           shmem_double_put_signal,
##           shmem_double_put_signal_nb,
##           shmem_fadd,
##           shmem_finc,
##           shmem_float128_g,
##           shmem_float128_get,
##           shmem_float128_get_nb,
##           shmem_float128_iget,
##           shmem_float128_iput,
##           shmem_float128_max_to_all,
##           shmem_float128_min_to_all,
##           shmem_float128_p,
##           shmem_float128_prod_to_all,
##           shmem_float128_put,
##           shmem_float128_put_nb,
##           shmem_float128_sum_to_all,
##           shmem_float_get_nb,
##           shmem_float_put_nb,
##           shmem_float_put_signal,
##           shmem_float_put_signal_nb,
##           shmem_get,
##           shmem_get128_nb,
##           shmem_get16_nb,
##           shmem_get32_nb,
##           shmem_get64_nb,
##           shmem_getmem_nb,
##           shmem_get_nb,
##           shmem_global_exit,
##           shmem_group_create_strided,
##           shmem_group_delete,
##           shmem_group_inquire,
##           shmem_iget,
##           shmem_inc,
##           shmem_int_get_nb,
##           shmem_int_put_nb,
##           shmem_int_put_signal,
##           shmem_int_put_signal_nb,
##           shmem_iput,
##           shmem_local_npes,
##           shmem_local_pes,
##           shmem_local_ptr,
##           shmem_long_get_nb,
##           shmem_long_put_nb,
##           shmem_long_put_signal,
##           shmem_long_put_signal_nb,
##           shmem_longdouble_get_nb,
##           shmem_longdouble_put_nb,
##           shmem_longlong_get_nb,
##           shmem_longlong_put_nb,
##           shmem_longlong_put_signal,
##           shmem_longlong_put_signal_nb,
##           shmem_put,
##           shmem_put128_nb,
##           shmem_put128_signal,
##           shmem_put128_signal_nb,
##           shmem_put16_nb,
##           shmem_put16_signal,
##           shmem_put16_signal_nb,
##           shmem_put32_signal,
##           shmem_put32_signal_nb,
##           shmem_put64_nb,
##           shmem_put64_signal,
##           shmem_put64_signal_nb,
##           shmem_put_nb,
##           shmem_put_signal,
##           shmem_put_signal_nb,
##           shmem_putmem_nb,
##           shmem_putmem_signal,
##           shmem_putmem_signal_nb,
##           shmem_query_thread,
##           shmem_short_get_nb,
##           shmem_short_put_nb,
##           shmem_short_put_signal,
##           shmem_short_put_signal_nb,
##           shmem_stack,
##

SCOREP_CHECK_SYMBOLS([SHMEM], $1, $2,
           [my_pe,
           _my_pe,
           num_pes,
           _num_pes,
           shfree,
           shmalloc,
           shmem_addr_accessible,
           shmemalign,
           shmem_barrier,
           shmem_barrier_all,
           shmem_broadcast32,
           shmem_broadcast64,
           shmem_char_g,
           shmem_char_p,
           shmem_char_get,
           shmem_char_put,
           shmem_clear_cache_inv,
           shmem_clear_cache_line_inv,
           shmem_clear_event,
           shmem_clear_lock,
           shmem_collect32,
           shmem_collect64,
           shmem_complexd_prod_to_all,
           shmem_complexd_sum_to_all,
           shmem_complexf_prod_to_all,
           shmem_complexf_sum_to_all,
           shmem_double_g,
           shmem_double_get,
           shmem_double_iget,
           shmem_double_iput,
           shmem_double_max_to_all,
           shmem_double_min_to_all,
           shmem_double_p,
           shmem_double_prod_to_all,
           shmem_double_put,
           shmem_double_sum_to_all,
           shmem_double_swap,
           shmem_fcollect32,
           shmem_fcollect64,
           shmem_fence,
           shmem_finalize,
           shmem_float_g,
           shmem_float_get,
           shmem_float_iget,
           shmem_float_iput,
           shmem_float_max_to_all,
           shmem_float_min_to_all,
           shmem_float_p,
           shmem_float_prod_to_all,
           shmem_float_put,
           shmem_float_sum_to_all,
           shmem_float_swap,
           shmem_get128,
           shmem_get16,
           shmem_get32,
           shmem_get64,
           shmem_getmem,
           shmem_iget128,
           shmem_iget16,
           shmem_iget32,
           shmem_iget64,
           shmem_init,
           shmem_init_thread,
           shmem_int_add,
           shmem_int_and_to_all,
           shmem_int_cswap,
           shmem_int_fadd,
           shmem_int_finc,
           shmem_int_g,
           shmem_int_get,
           shmem_int_iget,
           shmem_int_inc,
           shmem_int_iput,
           shmem_int_max_to_all,
           shmem_int_min_to_all,
           shmem_int_or_to_all,
           shmem_int_p,
           shmem_int_prod_to_all,
           shmem_int_put,
           shmem_int_sum_to_all,
           shmem_int_swap,
           shmem_int_wait,
           shmem_int_wait_until,
           shmem_int_xor_to_all,
           shmem_iput128,
           shmem_iput16,
           shmem_iput32,
           shmem_iput64,
           shmem_long_add,
           shmem_long_and_to_all,
           shmem_long_cswap,
           shmem_longdouble_g,
           shmem_longdouble_get,
           shmem_longdouble_iget,
           shmem_longdouble_iput,
           shmem_longdouble_max_to_all,
           shmem_longdouble_min_to_all,
           shmem_longdouble_p,
           shmem_longdouble_prod_to_all,
           shmem_longdouble_put,
           shmem_longdouble_sum_to_all,
           shmem_long_fadd,
           shmem_long_finc,
           shmem_long_g,
           shmem_long_get,
           shmem_long_iget,
           shmem_long_inc,
           shmem_long_iput,
           shmem_longlong_add,
           shmem_longlong_and_to_all,
           shmem_longlong_cswap,
           shmem_longlong_fadd,
           shmem_longlong_finc,
           shmem_longlong_g,
           shmem_longlong_get,
           shmem_longlong_iget,
           shmem_longlong_inc,
           shmem_longlong_iput,
           shmem_longlong_max_to_all,
           shmem_longlong_min_to_all,
           shmem_longlong_or_to_all,
           shmem_longlong_p,
           shmem_longlong_prod_to_all,
           shmem_longlong_put,
           shmem_longlong_sum_to_all,
           shmem_longlong_swap,
           shmem_longlong_wait,
           shmem_longlong_wait_until,
           shmem_longlong_xor_to_all,
           shmem_long_max_to_all,
           shmem_long_min_to_all,
           shmem_long_or_to_all,
           shmem_long_p,
           shmem_long_prod_to_all,
           shmem_long_put,
           shmem_long_sum_to_all,
           shmem_long_swap,
           shmem_long_wait,
           shmem_long_wait_until,
           shmem_long_xor_to_all,
           shmem_my_pe,
           shmem_n_pes,
           shmem_pe_accessible,
           shmem_ptr,
           shmem_put128,
           shmem_put16,
           shmem_put32,
           shmem_put64,
           shmem_putmem,
           shmem_quiet,
           shmem_set_cache_inv,
           shmem_set_cache_line_inv,
           shmem_set_event,
           shmem_set_lock,
           shmem_short_add,
           shmem_short_and_to_all,
           shmem_short_cswap,
           shmem_short_fadd,
           shmem_short_finc,
           shmem_short_g,
           shmem_short_get,
           shmem_short_iget,
           shmem_short_inc,
           shmem_short_iput,
           shmem_short_max_to_all,
           shmem_short_min_to_all,
           shmem_short_or_to_all,
           shmem_short_p,
           shmem_short_prod_to_all,
           shmem_short_put,
           shmem_short_sum_to_all,
           shmem_short_swap,
           shmem_short_wait,
           shmem_short_wait_until,
           shmem_short_xor_to_all,
           shmem_swap,
           shmem_team_alltoall,
           shmem_team_alltoallv,
           shmem_team_alltoallv_packed,
           shmem_team_barrier,
           shmem_team_create_strided,
           shmem_team_free,
           shmem_team_mype,
           shmem_team_npes,
           shmem_team_split,
           shmem_team_translate_pe,
           shmem_test_event,
           shmem_test_lock,
           shmem_udcflush,
           shmem_udcflush_line,
           shmem_wait,
           shmem_wait_event,
           shmem_wait_until,
           shrealloc,
           start_pes])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_SHMEM_CHECK_INTERCEPTION], [

dnl check if we assume having the profiling interface
_SCOREP_SHMEM_PROFILING_INTERFACE

dnl check also if library wrapping is possible
SCOREP_LIBRARY_WRAPPING

dnl now check for all functions, depending on the previous check either the
dnl p symbols or the normal symbols

AS_IF([test "x${scorep_shmem_has_pshmem_functions}" = "xyes"],
      [_SCOREP_SHMEM_CHECK_SYMBOLS([p])],
      [scorep_shmem_wrap_symbols=""
       _SCOREP_SHMEM_CHECK_SYMBOLS([], [scorep_shmem_wrap_symbols])
       dnl Always ensure to wrap shmem_finalize, if absent from the SHMEM implementation.
       dnl Score-P always provide one.
       AS_CASE([" ${scorep_shmem_wrap_symbols} "],
               [*" shmem_finalize "*], [: good, shmem_finalize is already in the list],
               [AS_VAR_APPEND([scorep_shmem_wrap_symbols], [" shmem_finalize"])])])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([SCOREP_SHMEM], [
AC_REQUIRE([SCOREP_LIBRARY_WRAPPING])dnl
AC_REQUIRE([_SCOREP_PDT_SHMEM_INSTRUMENTATION])dnl

AC_DEFINE_UNQUOTED([SCOREP_SHMEM_NAME], ["${SHMEM_NAME}"],
                   [Name of the implemented SHMEM specification.])

dnl First check for a SHMEM compiler
AS_IF([test "x${scorep_shmem_c_supported}" = "xyes"],
      [scorep_shmem_supported="yes"],
      [scorep_shmem_supported="no"])

dnl Detect SHMEM from Open MPI
dnl Do not test for 'Open MPI' as the OpenSHMEM reference uses mpicc under the
dnl hood and this may be Open MPI
scorep_shmem_openmpi_version=
oshcc_version="`$CC -showme:version 2>&1`"
AS_CASE([$oshcc_version],
        [*": Open SHMEM "*],
        [# extract version
        oshcc_version="${oshcc_version#*Open SHMEM }"
        oshcc_version="${oshcc_version%% *}"
        scorep_shmem_openmpi_version="${oshcc_version%%[[a-z]]*}"
])
AS_UNSET([oshcc_version])

dnl any Open MPI version does have a broken shmem_collect*
AS_IF([test "x$scorep_shmem_openmpi_version" != "x"],
      [AC_DEFINE([HAVE_BROKEN_SHMEM_COLLECT],
                 [1],
                 [Define if shmem_collect* cannot handle 0 nelems.])])

dnl than check for an interception method
shmem_interception_summary="no"
scorep_shmem_has_pshmem_functions="no"
scorep_shmem_has_pshmem_header="no"
AS_IF([test "x${scorep_shmem_supported}" = "xno"],
      [AC_MSG_WARN([No suitable SHMEM compilers found. Score-P SHMEM support disabled.])],
      [_SCOREP_SHMEM_CHECK_INTERCEPTION
       _SCOREP_SHMEM_COMPLIANCE
       AS_IF([test "x${scorep_shmem_has_pshmem_functions}" = "xyes"],
             [shmem_interception_summary="yes, using SHMEM profiling interface"],
             [test "x${scorep_libwrap_linktime_support}" = "xyes"],
             [shmem_interception_summary="yes, using linktime library wrapping"])
])

AFS_SUMMARY([intercepting SHMEM calls], [${shmem_interception_summary}])

AC_SCOREP_COND_HAVE([SHMEM_PROFILING_INTERFACE],
                    [test "x${scorep_shmem_has_pshmem_functions}" = "xyes"],
                    [Defined if SHMEM implementation provides a profiling interface.])

AC_SCOREP_COND_HAVE([SHMEM_PROFILING_HEADER],
                    [test "x${scorep_shmem_has_pshmem_header}" = "xyes"],
                    [Defined if SHMEM implementation provides a profiling header file.])

AM_CONDITIONAL([HAVE_SHMEM_SUPPORT],         [test "x${scorep_shmem_supported}" = "xyes" && test "x${shmem_interception_summary}" != "xno"])
AM_CONDITIONAL([HAVE_SHMEM_FORTRAN_SUPPORT], [test "x${scorep_shmem_f77_supported}" = "xyes" && test "x${shmem_interception_summary}" != "xno"])
])


dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_SHMEM_CHECK_COMPLIANCE],
         [AFS_CHECK_COMPLIANCE([@%:@include <shmem.h>], $@)])

AC_DEFUN([_SCOREP_SHMEM_COMPLIANCE], [
    AC_LANG_PUSH([C])

    dnl Cray MPT < 6.3 does have int as return type and only one argument
    _AFS_CHECK_VARIANTS_REC([@%:@include <shmem.h>],
        [int], [return 0],
        [shmem_init_thread], [CRAY_ONE_ARG], [(int required)])

    dnl Cray MPT >= 6.3 does have void as return type and two arguments
    _AFS_CHECK_VARIANTS_REC([@%:@include <shmem.h>],
        [void], [return],
        [shmem_init_thread], [CRAY_TWO_ARGS], [(int required, int* provided)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_finalize], [(void)])

    dnl Open MPI < 1.8.2 does have int as return type
    _AFS_CHECK_VARIANTS_REC([@%:@include <shmem.h>],
        [int], [return 0],
        [shmem_finalize], [OPENMPI], [(void)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [char], [return 0],
        [shmem_char_g], [(char* addr, int pe)],
        [CONST],        [(const char* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [short], [return 0],
        [shmem_short_g], [(short* addr, int pe)],
        [CONST],         [(const short* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [int], [return 0],
        [shmem_int_g], [(int* addr, int pe)],
        [CONST],       [(const int* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [long], [return 0],
        [shmem_long_g], [(long* addr, int pe)],
        [CONST],        [(const long* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [float], [return 0.0],
        [shmem_float_g], [(float* addr, int pe)],
        [CONST],         [(const float* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [double], [return 0.0],
        [shmem_double_g], [(double* addr, int pe)],
        [CONST],          [(const double* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [long long], [return 0],
        [shmem_longlong_g], [(long long* addr, int pe)],
        [CONST],            [(const long long* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [long double], [return 0.0],
        [shmem_longdouble_g], [(long double* addr, int pe)],
        [CONST],              [(const long double* addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_get], [(long double* addr, const long double* src, size_t len, int pe)],
        [CRAY],                 [(void* addr, const void* src, size_t len, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_iget], [(long double* addr, const long double* src, ptrdiff_t tst, ptrdiff_t sst, size_t len, int pe)],
        [CRAY],                  [(void* addr, const void* src, ptrdiff_t tst, ptrdiff_t sst, size_t len, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_put], [(long double* addr, const long double* src, size_t len, int pe)],
        [CRAY],                 [(void* addr, const void* src, size_t len, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_iput], [(long double* addr, const long double* src, ptrdiff_t tst, ptrdiff_t sst, size_t len, int pe)],
        [CRAY],                  [(void* addr, const void* src, ptrdiff_t tst, ptrdiff_t sst, size_t len, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_and_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                  [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],            [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_and_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],                [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],          [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_and_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                 [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],           [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_and_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                     [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],               [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_or_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                 [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],           [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_or_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],               [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],         [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_or_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],          [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_or_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                    [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],              [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_xor_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                  [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],            [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_xor_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],                [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],          [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_xor_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                 [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],           [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_xor_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                     [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],               [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_max_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                  [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],            [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_max_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],                [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],          [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_max_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                 [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],           [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_max_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                     [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],               [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_float_max_to_all], [(float *target, float *source,       int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST],                  [(float *target, const float *source, int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST_SIZET],            [(float *target, const float *source, size_t nreduce, int peStart, int logPeStride, int peSize, float *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_double_max_to_all], [(double *target, double *source,       int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST],                   [(double *target, const double *source, int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST_SIZET],             [(double *target, const double *source, size_t nreduce, int peStart, int logPeStride, int peSize, double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_max_to_all], [(long double *target, long double *source,       int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST],                       [(long double *target, const long double *source, int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST_SIZET],                 [(long double *target, const long double *source, size_t nreduce, int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_min_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                  [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],            [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_min_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],                [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],          [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_min_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                 [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],           [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_min_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                     [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],               [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_float_min_to_all], [(float *target, float *source,       int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST],                  [(float *target, const float *source, int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST_SIZET],            [(float *target, const float *source, size_t nreduce, int peStart, int logPeStride, int peSize, float *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_double_min_to_all], [(double *target, double *source,       int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST],                   [(double *target, const double *source, int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST_SIZET],             [(double *target, const double *source, size_t nreduce, int peStart, int logPeStride, int peSize, double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_min_to_all], [(long double *target, long double *source,       int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST],                       [(long double *target, const long double *source, int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST_SIZET],                 [(long double *target, const long double *source, size_t nreduce, int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_sum_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                  [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],            [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_sum_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],                [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],          [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_sum_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                 [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],           [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_sum_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                     [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],               [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_float_sum_to_all], [(float *target, float *source,       int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST],                  [(float *target, const float *source, int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST_SIZET],            [(float *target, const float *source, size_t nreduce, int peStart, int logPeStride, int peSize, float *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_double_sum_to_all], [(double *target, double *source,       int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST],                   [(double *target, const double *source, int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST_SIZET],             [(double *target, const double *source, size_t nreduce, int peStart, int logPeStride, int peSize, double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_sum_to_all], [(long double *target, long double *source,       int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST],                       [(long double *target, const long double *source, int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST_SIZET],                 [(long double *target, const long double *source, size_t nreduce, int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_complexf_sum_to_all], [(float complex *target, float complex *source,       int nreduce,    int peStart, int logPeStride, int peSize, float complex *pWork, long *pSync)],
        [CONST],                     [(float complex *target, const float complex *source, int nreduce,    int peStart, int logPeStride, int peSize, float complex *pWork, long *pSync)],
        [CONST_SIZET],               [(float complex *target, const float complex *source, size_t nreduce, int peStart, int logPeStride, int peSize, float complex *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_complexd_sum_to_all], [(double complex *target, double complex *source,       int nreduce,    int peStart, int logPeStride, int peSize, double complex *pWork, long *pSync)],
        [CONST],                     [(double complex *target, const double complex *source, int nreduce,    int peStart, int logPeStride, int peSize, double complex *pWork, long *pSync)],
        [CONST_SIZET],               [(double complex *target, const double complex *source, size_t nreduce, int peStart, int logPeStride, int peSize, double complex *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_short_prod_to_all], [(short *target, short *source,       int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST],                   [(short *target, const short *source, int nreduce,    int peStart, int logPeStride, int peSize, short *pWork, long *pSync)],
        [CONST_SIZET],             [(short *target, const short *source, size_t nreduce, int peStart, int logPeStride, int peSize, short *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_int_prod_to_all], [(int *target, int *source,       int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST],                 [(int *target, const int *source, int nreduce,    int peStart, int logPeStride, int peSize, int *pWork, long *pSync)],
        [CONST_SIZET],           [(int *target, const int *source, size_t nreduce, int peStart, int logPeStride, int peSize, int *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_long_prod_to_all], [(long *target, long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST],                  [(long *target, const long *source, int nreduce,    int peStart, int logPeStride, int peSize, long *pWork, long *pSync)],
        [CONST_SIZET],            [(long *target, const long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longlong_prod_to_all], [(long long *target, long long *source,       int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST],                      [(long long *target, const long long *source, int nreduce,    int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)],
        [CONST_SIZET],                [(long long *target, const long long *source, size_t nreduce, int peStart, int logPeStride, int peSize, long long *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_float_prod_to_all], [(float *target, float *source,       int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST],                   [(float *target, const float *source, int nreduce,    int peStart, int logPeStride, int peSize, float *pWork, long *pSync)],
        [CONST_SIZET],             [(float *target, const float *source, size_t nreduce, int peStart, int logPeStride, int peSize, float *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_double_prod_to_all], [(double *target, double *source,       int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST],                    [(double *target, const double *source, int nreduce,    int peStart, int logPeStride, int peSize, double *pWork, long *pSync)],
        [CONST_SIZET],              [(double *target, const double *source, size_t nreduce, int peStart, int logPeStride, int peSize, double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_longdouble_prod_to_all], [(long double *target, long double *source,       int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST],                        [(long double *target, const long double *source, int nreduce,    int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)],
        [CONST_SIZET],                  [(long double *target, const long double *source, size_t nreduce, int peStart, int logPeStride, int peSize, long double *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_complexf_prod_to_all], [(float complex *target, float complex *source,       int nreduce,    int peStart, int logPeStride, int peSize, float complex *pWork, long *pSync)],
        [CONST],                      [(float complex *target, const float complex *source, int nreduce,    int peStart, int logPeStride, int peSize, float complex *pWork, long *pSync)],
        [CONST_SIZET],                [(float complex *target, const float complex *source, size_t nreduce, int peStart, int logPeStride, int peSize, float complex *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [void], [return],
        [shmem_complexd_prod_to_all], [(double complex *target, double complex *source,       int nreduce,    int peStart, int logPeStride, int peSize, double complex *pWork, long *pSync)],
        [CONST],                      [(double complex *target, const double complex *source, int nreduce,    int peStart, int logPeStride, int peSize, double complex *pWork, long *pSync)],
        [CONST_SIZET],                [(double complex *target, const double complex *source, size_t nreduce, int peStart, int logPeStride, int peSize, double complex *pWork, long *pSync)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
        [int], [return 1],
        [shmem_addr_accessible],      [(void *addr, int pe)],
        [CONST],                      [(const void *addr, int pe)])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void*], [return NULL],
         [shmem_ptr],                 [( void *ptr, int pe )],
         [CONST],                     [( const void *ptr, int pe )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_set_lock],            [( long *lock )],
         [VOLATILE],                  [( volatile long *lock )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_clear_lock],          [( long *lock )],
         [VOLATILE],                  [( volatile long *lock )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [int], [return 0],
         [shmem_test_lock],           [( long *lock )],
         [VOLATILE],                  [( volatile long *lock )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_short_wait],          [( short *addr, short value )],
         [VOLATILE],                  [( volatile short *addr, short value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_int_wait],            [( int *addr, int value )],
         [VOLATILE],                  [( volatile int *addr, int value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_long_wait],           [( long *addr, long value )],
         [VOLATILE],                  [( volatile long *addr, long value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_longlong_wait],       [( long long *addr, long long value )],
         [VOLATILE],                  [( volatile long long *addr, long long value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_wait],                [( long *addr, long value )],
         [VOLATILE],                  [( volatile long *addr, long value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_short_wait_until],    [( short *addr, int cmp, short value )],
         [VOLATILE],                  [( volatile short *addr, int cmp, short value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_int_wait_until],      [( int *addr, int cmp, int value )],
         [VOLATILE],                  [( volatile int *addr, int cmp, int value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_long_wait_until],     [( long *addr, int cmp, long value )],
         [VOLATILE],                  [( volatile long *addr, int cmp, long value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_longlong_wait_until], [( long long *addr, int cmp, long long value )],
         [VOLATILE],                  [( volatile long long *addr, int cmp, long long value )])

    _SCOREP_SHMEM_CHECK_COMPLIANCE(
         [void], [return],
         [shmem_wait_until],          [( long *addr, int cmp, long value )],
         [VOLATILE],                  [( volatile long *addr, int cmp, long value )])

    AC_LANG_POP([C])
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_SCOREP_SHMEM_INCLUDE], [
AC_REQUIRE([AC_PROG_GREP])
scorep_have_shmem_include="no"
AC_LANG_PUSH([C])
AC_PREPROC_IFELSE([AC_LANG_PROGRAM([[#include <shmem.h>]], [])],
                  [GREP_OPTIONS= ${GREP} '/shmem.h"' conftest.i > conftest_shmem_includes
                   AS_IF([test $? -eq 0],
                         [scorep_shmem_include=`cat conftest_shmem_includes | GREP_OPTIONS= ${GREP} '/shmem.h"' | \
                          head -1 | sed -e 's#^.* "##' -e 's#/shmem.h".*##'`
                          scorep_have_shmem_include="yes"],
                         [])
                   rm -f conftest_shmem_includes],
                  [])
AC_LANG_POP([C])

AS_IF([test "x${scorep_have_shmem_include}" = "xyes"],
      [AC_SUBST([SCOREP_SHMEM_INCLUDE], [${scorep_shmem_include}])],
      [])
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_SCOREP_PDT_SHMEM_INSTRUMENTATION], [
AC_REQUIRE([_SCOREP_SHMEM_INCLUDE])

AS_IF([test "x${scorep_have_shmem_include}" = "xyes"],
      [AC_SUBST([SCOREP_HAVE_PDT_SHMEM_INSTRUMENTATION], [1])
       AFS_SUMMARY([PDT SHMEM instrumentation], [yes, if PDT available])],
      [AC_SUBST([SCOREP_HAVE_PDT_SHMEM_INSTRUMENTATION], [0])
       AC_MSG_WARN([cannot determine shmem.h include path. PDT SHMEM instrumentation will be disabled.])
       AFS_SUMMARY([PDT SHMEM instrumentation], [no, shmem.h include path could not be determined.])])
])
