dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2022, 2024,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl


# |-separated list of valid options, i.e., experimental features,
# to `--enable-default`. So far, just used for help- and debug-
# output. Corresponding scorep_enable_default_<foo> variable(s)
# to be created manually (tried m4 magic to automate but failed).
m4_define([_DEFAULTS_LIST], [ompt])


# SCOREP_ENABLE_DEFAULT()
# -----------------------
# Switch defaults for experimental features. In case established
# alternatives exist, experimental features should not be
# default. Although, switching defaults to experimental features should
# be possible for eager users or project partners.
#
# Set variable scorep_enable_default_<experimental_feature> from `no`
# to `yes` if given in comma-separated list to
# `--enable-default=`. Abort if `--enable-default` was given with
# invalid value(s).
#
# Intended to be AC_REQUIREd. Handling of
# scorep_enable_default_<feature> to be done in requiring macro.
#
# Note that the list of valid 'experimental features' might change
# without deprecation notice.
#
AC_DEFUN_ONCE([SCOREP_ENABLE_DEFAULT], [

scorep_enable_default_ompt=no

AC_ARG_ENABLE([default],
    [AS_HELP_STRING([--enable-default=<comma-separated list>],
         [Make experimental feature(s) default. Valid value(s): ]_DEFAULTS_LIST[. List of valid values may change without deprecation notice.])],
    [AS_IF([test "x${enableval}" = xyes || test "x${enableval}" = xno],
         [AC_MSG_ERROR([Comma-separated list to --enable-default expected, but got ${enableval}.])],
         [save_IFS=$IFS
          IFS=","
          for i in ${enableval}; do
              AS_CASE([${i}],
                  []_DEFAULTS_LIST[], [eval scorep_enable_default_${i}=yes],
                  [# We need to reset IFS here, since as_echo or the resulting printf seems to
                   # use IFS for separating something, or ${i} gets an "," added during evaluation in as_echo.
                   # Not resetting IFS will result in errors where the shell is trying to execute
                   # non existing commands.
                   IFS=$save_IFS
                   AC_MSG_ERROR([One of the valid list elements (]_DEFAULTS_LIST[) to --enable-default expected, but got ${i}])])
          done
          IFS=$save_IFS
         ])
    ])
])dnl SCOREP_ENABLE_DEFAULT
