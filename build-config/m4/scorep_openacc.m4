## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015-2016, 2019,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_openacc.m4

AC_DEFUN([SCOREP_OPENACC_FLAG_TEST],[
    AC_LANG_PUSH([C])
    save_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $1"

    AC_MSG_CHECKING([whether compiler understands $1])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
        [AC_MSG_RESULT([yes])
         scorep_compiler_acc_flags="$1"],
        [AC_MSG_RESULT([no])])

   CFLAGS="$save_CFLAGS"
   AC_LANG_POP([C])
])

AC_DEFUN([SCOREP_OPENACC], [
AC_REQUIRE([AX_COMPILER_VENDOR])

scorep_enable_openacc="yes"
scorep_have_openacc="no"
scorep_have_openacc_prof="no"
scorep_compiler_acc_flags=""

AC_ARG_ENABLE([openacc],
              [AS_HELP_STRING([--enable-openacc],
                              [Enable or disable support for OpenACC. (defaults to yes)])],
              [AS_IF([test "x$enableval" = "xyes"], [scorep_enable_openacc="yes"], [scorep_enable_openacc="no"])])

# advertise the $OPENACC_INCLUDE and $OPENACC_PROFILING_INCLUDE variables in the --help output
AC_ARG_VAR([OPENACC_INCLUDE],           [Include path to openacc.h header.])
AC_ARG_VAR([OPENACC_PROFILING_INCLUDE], [Include path to acc_prof.h header.])

dnl get path to OpenACC header
AC_ARG_WITH([openacc-include],
            [AS_HELP_STRING([--with-openacc-include=<path-to-openacc.h>],
                            [If openacc.h is not installed in the default location, specify the directory where it can be found.])],
            [scorep_openacc_inc_dir="${withval}"],  # action-if-given.
            [scorep_openacc_inc_dir="${OPENACC_INCLUDE}"] # action-if-not-given
)

dnl get path to OpenACC Profiling header
AC_ARG_WITH([openacc-prof-include],
            [AS_HELP_STRING([--with-openacc-prof-include=<path-to-acc_prof.h>],
                            [If acc_prof.h is not installed in the default location, specify the directory where it can be found.])],
            [scorep_openacc_prof_inc_dir="${withval}"],  # action-if-given.
            [scorep_openacc_prof_inc_dir="${OPENACC_PROFILING_INCLUDE}"] # action-if-not-given
)

AS_IF([test "x$scorep_enable_openacc" = "xyes"],[

  dnl add include paths to CPPFLAGS
  AS_IF([test "x$scorep_openacc_inc_dir" != "x"],
        [with_openacc_cppflags="-I$scorep_openacc_inc_dir"])

  AS_IF([test "x$scorep_openacc_prof_inc_dir" != "x"],
        [with_openacc_cppflags="$with_openacc_cppflags -I$scorep_openacc_prof_inc_dir"])

  scorep_openacc_safe_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="$CPPFLAGS ${with_openacc_cppflags}"

  AC_LANG_PUSH([C])

  dnl check for openacc.h
  AC_CHECK_HEADER([openacc.h],[scorep_have_openacc="yes"])

  dnl check acc_prof.h for functions, types and fields the OpenACC adapter uses
  AS_IF([test "x${scorep_have_openacc}" = "xyes"],[
    AC_CHECK_HEADER([acc_prof.h],[scorep_have_openacc_prof="yes"],[],[
    dnl stdlib is needed for size_t, as a respective header is not included in acc_prof.h
    dnl acc_prof.h is included by openacc.h
# include <stdlib.h>
# include <openacc.h>
    ])
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
    #include <stdlib.h>
    #include <acc_prof.h>

    static void
    handle_event( acc_prof_info*  profInfo,
                  acc_event_info* eventInfo,
                  acc_api_info*   apiInfo )
    {
      // check used profiling information fields
      acc_event_t event_type  = profInfo->event_type;
      acc_device_t deviceType = profInfo->device_type;
      profInfo->device_number;
      profInfo->line_no;
      profInfo->src_file;

      // check for availability of processed events
      switch ( event_type )
      {
          case acc_ev_device_init_start:
          case acc_ev_device_shutdown_start:
          case acc_ev_compute_construct_start:
          case acc_ev_update_start:
          case acc_ev_enter_data_start:
          case acc_ev_exit_data_start:
          case acc_ev_enqueue_launch_start:
            eventInfo->launch_event.kernel_name;
            eventInfo->launch_event.num_gangs;
            eventInfo->launch_event.num_workers;
            eventInfo->launch_event.vector_length;
          case acc_ev_enqueue_upload_start:
          case acc_ev_enqueue_download_start:
            eventInfo->data_event.var_name;
            eventInfo->data_event.bytes;
            eventInfo->data_event.device_ptr;
          case acc_ev_wait_start:
            return;
      }

      // check for used other event fields
      eventInfo->other_event.tool_info;
      eventInfo->other_event.implicit;
    }

    void
    acc_register_library( acc_prof_reg reg, acc_prof_reg unreg, acc_prof_lookup lookup )
    {
        acc_register( acc_ev_device_init_start, handle_event, 0 );
    }
    ]])],
    [],
    [
      AC_MSG_NOTICE([Required OpenACC profiling features are missing in acc_prof.h.])
      scorep_have_openacc_prof="no"
    ])
  ])

  AC_LANG_POP([C])

  CPPFLAGS="${scorep_openacc_safe_CPPFLAGS}"
])

AS_IF([test "x${scorep_have_openacc_prof}" = xyes],
      [dnl check for OpenACC compiler flags
       AS_CASE([${ax_cv_c_compiler_vendor%/*}],
           [portland], [SCOREP_OPENACC_FLAG_TEST([-acc])],
           [gnu],      [],
           [cray],     [SCOREP_OPENACC_FLAG_TEST([-h pragma=acc])],
           [])dnl

       dnl print a notice on the used OpenACC compiler flags
       AS_IF([test "x${scorep_compiler_acc_flags}" != "x"],
             [AC_MSG_NOTICE([using compiler OpenACC flags: ${scorep_compiler_acc_flags}])])])

AC_SCOREP_COND_HAVE([OPENACC],
                    [test "x${scorep_have_openacc}" = "xyes"],
                    [Defined if openacc.h has been found and OpenACC enabled.])

AC_SCOREP_COND_HAVE([OPENACC_SUPPORT],
                    [test "x${scorep_have_openacc_prof}" = "xyes"],
                    [Defined if openacc.h and acc_prof.h have been found and OpenACC enabled],
                    [openacc_support_summary="yes, with compiler flag ${scorep_compiler_acc_flags}"
                     AC_SUBST([SCOREP_OPENACC_CPPFLAGS], [$with_openacc_cppflags])
                     AC_SUBST([SCOREP_COMPILER_ACC_FLAGS], [${scorep_compiler_acc_flags}])],
                    [openacc_support_summary="no"
                     AC_SUBST([SCOREP_OPENACC_CPPFLAGS], [""])])

AFS_SUMMARY([OpenACC support], [${openacc_support_summary}])
])
