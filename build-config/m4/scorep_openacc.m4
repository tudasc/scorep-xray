## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015-2016, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2021-2022, 2024,
## Forschungszentrum Juelich GmbH, Germany
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
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl

scorep_enable_openacc="yes"
scorep_have_openacc="no"
scorep_have_openacc_prof="no"
scorep_compiler_acc_flags=""

AC_ARG_ENABLE([openacc],
              [AS_HELP_STRING([--enable-openacc],
                              [Enable or disable support for OpenACC. (defaults to yes)])],
              [AS_IF([test "x$enableval" = "xyes"], [scorep_enable_openacc="yes"], [scorep_enable_openacc="no"])])

AS_IF([test "x$scorep_enable_openacc" = "xyes"],[

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
acc_register_library( acc_prof_reg accRegister,
                      acc_prof_reg accUnregister,
                      acc_prof_lookup_func lookup )
{
    accRegister( acc_ev_device_init_start, handle_event, 0 );
}
    ]])],
    [],
    [
      AC_MSG_NOTICE([Required OpenACC profiling features are missing in acc_prof.h.])
      scorep_have_openacc_prof="no"
    ])
  ])

  AC_LANG_POP([C])
])

AS_IF([test "x${scorep_have_openacc_prof}" = xyes],
      [dnl check for OpenACC compiler flags
       AS_CASE([${ax_cv_c_compiler_vendor%/*}],
           [nvhpc],    [SCOREP_OPENACC_FLAG_TEST([-acc])],
           [portland], [SCOREP_OPENACC_FLAG_TEST([-acc])],
           [gnu],      [],
           [cray],     [SCOREP_OPENACC_FLAG_TEST([-h pragma=acc])],
           [])dnl

       dnl print a notice on the used OpenACC compiler flags
       AS_IF([test "x${scorep_compiler_acc_flags}" != "x"],
             [AC_MSG_NOTICE([using compiler OpenACC flags: ${scorep_compiler_acc_flags}])])])

AC_SCOREP_COND_HAVE([OPENACC_PROFILING_SUPPORT],
                    [test "x${scorep_have_openacc_prof}" = "xyes"],
                    [Defined if an OpenACC profiling tool can be compiled],
                    [openacc_support_summary="yes, with compiler flag ${scorep_compiler_acc_flags}"
                     AC_SUBST([OPENACC_CFLAGS], [${scorep_compiler_acc_flags}])],
                    [openacc_support_summary="no"])

AFS_SUMMARY([OpenACC support], [${openacc_support_summary}])
])
