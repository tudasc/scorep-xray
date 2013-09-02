dnl 
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.68])
AC_INIT([Score-P], m4_esyscmd([vendor/common/build-config/generate-package-version.sh build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_INIT
AFS_SUMMARY_INIT

AC_SCOREP_REVISION

AC_SCOREP_TOPLEVEL_ARGS

## Packages that do manual configuration or use the install program might need
## to tell configure where to find some other shell scripts by calling
## AC_CONFIG_AUX_DIR, though the default places it looks are correct for most
## cases.
## Need to be called early on configure.ac because other macros search tools
## in this directory.
AC_CONFIG_AUX_DIR([build-config])

## Packages that use aclocal should declare where local macros can be found
## using AC_CONFIG_MACRO_DIR.
AC_CONFIG_MACRO_DIR([vendor/common/build-config/m4
                     build-config/m4])

## Set the default installation prefix to /opt/scorep instead of /usr/local
AC_PREFIX_DEFAULT([/opt/scorep])

AC_SCOREP_SVN_CONTROLLED

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

AC_SCOREP_DETECT_PLATFORMS
AFS_CROSSCOMPILING
AFS_SCOREP_MACHINE_NAME
AFS_SCOREP_PLATFORM_NAME

# quote $afs_scorep_platform_name and $afs_scorep_default_machine_name for use as arguments to sub-configures
afs_scorep_default_machine_name_sq=$afs_scorep_default_machine_name
case $afs_scorep_default_machine_name_sq in
*\'*) afs_scorep_default_machine_name_sq="`AS_ECHO(["$afs_scorep_default_machine_name_sq"]) | sed "s/'/'\\\\\\\\''/g"`" ;;
esac
afs_scorep_platform_name_sq=$afs_scorep_platform_name
case $afs_scorep_platform_name_sq in
*\'*) afs_scorep_platform_name_sq="`AS_ECHO(["$afs_scorep_platform_name_sq"]) | sed "s/'/'\\\\\\\\''/g"`" ;;
esac

# don't detect and load defaults in nested configures (e.g. otf2)
AS_IF([test "x$ac_scorep_platform_data_provided" = "x" || \
       test "${ac_scorep_platform_data_provided}" = "no"],
      [dnl AFS_COMPILER_BACKEND([CC, CXX, F77, FC])
       AC_SCOREP_WITH_COMPILER_SUITE
       AS_CASE([${afs_custom_compilers_given},${ac_scorep_platform}],
           [no,mac|no,mingw],
               [AC_MSG_ERROR([have no suitable defaults to build Score-P on $afs_scorep_platform_name yet. Please use --with-custom-compilers and provide settings to <AC_PACKAGE_BUGREPORT> on success. Thanks.])],
           [no,unknown],
               [AC_MSG_ERROR([cannot detect platform to set suitable defaults. Please use --with-custom-compilers and provide settings and platform to <AC_PACKAGE_BUGREPORT> on success. Thanks.])
           ])
       AC_SCOREP_WITH_MPI_COMPILER_SUITE
       # determine arguments for subdir configures
       args="`${AWK} -f ${srcdir}/vendor/common/build-config/process_arguments.awk \
              ${ac_scorep_compilers_mpi}                                       \
              ${ac_scorep_compilers_frontend}                                  \
              ${ac_scorep_compilers_backend}                                   \
              user_provided_configure_args`"],
      [AC_MSG_NOTICE([platform data provided by toplevel configure.])
       # determine arguments for subdir configures
       args="`${AWK} -f $srcdir/vendor/common/build-config/process_arguments.awk \
              user_provided_configure_args`"])

AC_SCOREP_UNCRUSTIFY

AC_SCOREP_DOXYGEN
AC_SCOREP_DOXYGEN_CONFIG_FILES

AC_SCOREP_DEBUG_OPTION

# Assume that all lockfile-progs are available if lockfile-create is.
AC_CHECK_PROG([ac_scorep_have_lockfile_progs], [lockfile-create], [yes], [no])
AM_CONDITIONAL([HAVE_LOCKFILE_PROGS],
               [test "x${ac_scorep_have_lockfile_progs}" = "xyes"])

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])

AC_SCOREP_HAVE_CONFIG_TOOL([otf2],   [3])
AC_SCOREP_HAVE_CONFIG_TOOL([opari2], [2])
AC_SCOREP_HAVE_CONFIG_TOOL([cube],   [4])

# handle external package's subdir configures
AS_IF([test "x${scorep_have_opari2_config}" = "xno"],
      [AC_CONFIG_SUBDIR_CUSTOM([vendor/opari2], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/opari2])])

AS_IF([test "x${scorep_have_otf2_config}" = "xno"],
      [AC_CONFIG_SUBDIR_CUSTOM([vendor/otf2],   [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/otf2])])

# handle own package's subdir configures
scorep_have_score_build="scorep_have_score_build=no"
AS_IF([test "x${scorep_have_cube_config}" = "xyes"],
      [CUBE_READER_CXX="`${scorep_cube_config_bin} --reader-cxx | ${AWK} '{print $1}'`"
       # Source files to get CXX and CXX_FOR_BUILD. Fragile, does not
       # consider user-provided arguments. We can't get CXX from the
       # backend because the following conditional is evaluated before
       # the backend configure is run.
       AC_REQUIRE([AC_PROG_GREP])
       AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
             [frontend_compilers=${ac_scorep_compilers_frontend}],
             [frontend_compilers=${ac_scorep_compilers_backend}])
       FRONTEND_CXX=`${GREP} CXX_FOR_BUILD ${frontend_compilers} | ${AWK} -F "=" '{print $2}' | ${AWK} '{print $1}'`
       AS_IF([test "x${FRONTEND_CXX}" != "x${CUBE_READER_CXX}"],
             [AC_CONFIG_SUBDIR_CUSTOM([build-score], [CXX="${CUBE_READER_CXX}" ${scorep_cube_config_arg}])
              scorep_have_score_build="scorep_have_score_build=yes"])],
      [AS_IF([test "x${ac_scorep_svn_controlled}" = "xno" && test "x${triggered_by_make_distcheck}" != "xyes"],
             [AC_MSG_ERROR([cube installation required but not found. Please reconfigure using the --with-cube option or make sure that cube-config is in PATH.])],
             [AC_MSG_WARN([cube installation required but not found. scorep-score wont be build. Please reconfigure using the --with-cube option or make sure that cube-config is in PATH.])])])
AM_CONDITIONAL([HAVE_SCORE_BUILD], [test "x${scorep_have_score_build}" = "xscorep_have_score_build=yes"])

AM_CONDITIONAL([HAVE_SCOREP_MPI], [test "x${scorep_mpi_user_disabled}" != "xyes"])
AS_IF([test "x${scorep_mpi_user_disabled}" != "xyes"],
    [AC_CONFIG_SUBDIR_CUSTOM([build-mpi],     [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg}])
    ],
    [AFS_SUMMARY([MPI support], [no, explicitly disabled via --without-mpi])
     AC_CONFIG_FILES([src/scorep_config_tool_mpi.h:src/tools/config/scorep_config_tool_without_mpi.h.in])
    ])
AC_CONFIG_SUBDIR_CUSTOM([build-backend], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg} ${scorep_opari2_config_arg} ${scorep_cube_config_arg} ${scorep_have_score_build}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_opari2_config_arg} ${scorep_cube_config_arg}  ${scorep_have_score_build}])
fi

AC_CONFIG_FILES([
    Makefile
    src/config-common.h:vendor/common/build-config/config-common.h
])

# generate the Score-P error codes header
AC_CONFIG_FILES(
    [src/utils/include/SCOREP_ErrorCodes.h:vendor/common/src/utils/exception/ErrorCodes.tmpl.h],
    [$srcdir/vendor/common/src/utils/exception/finalize_error_codes.sh SCOREP $srcdir/share/scorep.errors src/utils/include/SCOREP_ErrorCodes.h src/utils/include/scorep_error_decls.gen.h])

AC_OUTPUT

AS_IF([test "x$ac_scorep_platform_data_provided" != "xyes"], [
    AFS_SUMMARY_COLLECT
])
