dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2024,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

# SCOREP_LLVM_CONFIG
# ------------------
# Checks if `llvm-config` exists via the following approaches.
# If --with-llvm=<path> is passed to configure, the following approaches are used:
# 1. Check if `llvm-config` is available in the directory given by the --with-llvm=<path> argument
# 2. Check if `llvm-config` is available in the bin subdirectory in the directory given by the --with-llvm=<path> argument
# If --with-llvm is not passed or is set to "yes", the following approach is used:
# 1. Check if `llvm-config` is available in $PATH
# If `llvm-config` is found and working, the following variables are set:
#  - scorep_have_llvm_config: "yes"
#  - scorep_llvm_config_bin: The path to the `llvm-config` binary
# If it is not found, not working or explicitly disabled via --without-llvm, the following variables are set:
#  - scorep_have_llvm_config: "no"
#  - scorep_llvm_config_reason: The reason why `llvm-config` is not available or not working
#
AC_DEFUN([SCOREP_LLVM_CONFIG], [
AC_ARG_WITH([llvm],
    [AS_HELP_STRING([--with-llvm[[=<llvm-bindir>]]],
        [Use an already installed LLVM, in particular llvm-config. Auto-detected if already in $PATH.])],
    [with_llvm="${with_llvm%/}"], dnl yes, no, or <path>
    [with_llvm="not_given"])

scorep_have_llvm_config="no"
scorep_llvm_config_with_llvm="${with_llvm}"

# Check if llvm-config is available
AS_CASE([${scorep_llvm_config_with_llvm}],
    [yes|not_given], [AC_PATH_PROG([scorep_have_llvm_config], [llvm-config], [no])],
    [no],  [scorep_llvm_config_reason="explicitly disabled via --without-llvm"],
    [# --with-llvm=<path>
     AS_IF([test ! -d "${scorep_llvm_config_with_llvm}"],
         [AC_MSG_WARN([Provided LLVM directory ${scorep_llvm_config_with_llvm} does not exist.])],
         [AC_PATH_PROG([scorep_have_llvm_config], [llvm-config], [no], ["${scorep_llvm_config_with_llvm}/bin"])
          AS_IF([test "x${scorep_have_llvm_config}" == "xno"],
              [AS_UNSET([ac_cv_path_scorep_have_llvm_config])
               AS_UNSET([scorep_have_llvm_config])
               AC_PATH_PROG([scorep_have_llvm_config], [llvm-config], [no], ["${scorep_llvm_config_with_llvm}"])])])])

# Check if llvm-config is working
AS_IF([test "x${scorep_have_llvm_config}" != "xno"],
    [scorep_llvm_config_bin="${scorep_have_llvm_config}"
     # try executing llvm-config to rule out that it doesn't work due to not finding linked libs e.g.
     AC_MSG_CHECKING([whether llvm-config is functional])
     llvm_config_version='${scorep_llvm_config_bin} --version >&AS_MESSAGE_LOG_FD'
     AS_IF([_AC_DO_VAR([llvm_config_version])],
         [scorep_have_llvm_config="yes"],
         [scorep_have_llvm_config="no"
          scorep_llvm_config_reason="${scorep_llvm_config_bin} crashes when executed"])
     AC_MSG_RESULT([${scorep_have_llvm_config}${scorep_llvm_config_reason:+, scorep_llvm_config_reason}])],
    [scorep_llvm_config_reason="${scorep_llvm_config_reason:-llvm-config not found}"])
])
