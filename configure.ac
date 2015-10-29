dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2015,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2014,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
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
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([build-config/m4 vendor/common/build-config/m4])

## Set the default installation prefix to /opt/scorep instead of /usr/local
AC_PREFIX_DEFAULT([/opt/scorep])

AC_SCOREP_SVN_CONTROLLED

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])

AC_SCOREP_DETECT_PLATFORMS
AFS_CROSSCOMPILING
AFS_SCOREP_MACHINE_NAME
AFS_SCOREP_PLATFORM_NAME
AC_SCOREP_PLATFORM_SETTINGS

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
      [AFS_CUSTOM_COMPILERS
       AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
           [dnl AFS_COMPILER_BACKEND([CC, CXX, F77, FC])
            AC_SCOREP_WITH_COMPILER_SUITE
            AS_CASE([${ac_scorep_platform}],
                [mac|mingw],
                    [AC_MSG_ERROR([have no suitable defaults to build Score-P on $afs_scorep_platform_name yet. Please use --with-custom-compilers and provide settings to <AC_PACKAGE_BUGREPORT> on success. Thanks.])],
                [unknown],
                    [AC_MSG_ERROR([cannot detect platform to set suitable defaults. Please use --with-custom-compilers and provide settings and platform to <AC_PACKAGE_BUGREPORT> on success. Thanks.])
                ])
            AC_SCOREP_WITH_MPI_COMPILER_SUITE
            AFS_WITH_SHMEM_COMPILER_SUITE
           ],
           [afs_compiler_mpi_save=${ac_scorep_compilers_mpi}
            AC_SCOREP_WITH_MPI_COMPILER_SUITE
            AS_IF([test "x${scorep_mpi_user_disabled}" = xno],
                [ac_scorep_compilers_mpi=${afs_compiler_mpi_save}])
            afs_compiler_shmem_save=${ac_scorep_compilers_shmem}
            AFS_WITH_SHMEM_COMPILER_SUITE
            AS_IF([test "x${scorep_shmem_user_disabled}" = xno],
                [ac_scorep_compilers_shmem=${afs_compiler_shmem_save}])
           ])
       AS_IF([test -f ${ac_scorep_compilers_shmem}],
             [# determine arguments for subdir configures with handling SHMEM compiler
              args="`${AWK} -f ${srcdir}/vendor/common/build-config/process_arguments.awk \
                     ${ac_scorep_compilers_mpi}                                       \
                     ${ac_scorep_compilers_shmem}                                     \
                     ${ac_scorep_compilers_frontend}                                  \
                     ${ac_scorep_compilers_backend}                                   \
                     user_provided_configure_args`"],
             [# determine arguments for subdir configures without handling SHMEM compiler
              args="`${AWK} -f ${srcdir}/vendor/common/build-config/process_arguments.awk \
                     ${ac_scorep_compilers_mpi}                                       \
                     ${ac_scorep_compilers_frontend}                                  \
                     ${ac_scorep_compilers_backend}                                   \
                     user_provided_configure_args`"])
       AS_IF([test $? -ne 0], [AC_MSG_ERROR([cannot process provided and/or autodetected arguments. Please contact <AC_PACKAGE_BUGREPORT> and provide the above output. Thanks.])])
      ],
      [AC_MSG_NOTICE([platform data provided by toplevel configure.])
       # determine arguments for subdir configures
       args="`${AWK} -f $srcdir/vendor/common/build-config/process_arguments.awk \
              user_provided_configure_args`"
       AS_IF([test $? -ne 0], [AC_MSG_ERROR([cannot process provided arguments. Please contact <AC_PACKAGE_BUGREPORT> and provide the above output. Thanks.])])
      ])

AC_SCOREP_UNCRUSTIFY

AC_SCOREP_DOXYGEN
AS_IF([test "x${ac_scorep_svn_controlled}" = xyes],
    [AC_CONFIG_FILES([doc/doxygen-user.cfg:doc/doxygen-user/doxygen-user.cfg.in])
     AC_CONFIG_FILES([doc/doxygen-dev.cfg:doc/doxygen-dev/doxygen-dev.cfg.in])])

AFS_DEBUG_OPTION

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

AC_SCOREP_HAVE_CONFIG_TOOL([otf2],   [7])
AS_IF([test -n "${scorep_otf2_config_bin}" && \
       test "x${ac_scorep_platform}" = "xmic"],
      [scorep_otf2_have_mic=no
       for target in `${scorep_otf2_config_bin} --features=targets`
       do
           AS_CASE([${target}],
               [mic], [scorep_otf2_have_mic=yes])
       done
       AS_IF([test "x${scorep_otf2_have_mic}" = "xno"],
             [AC_MSG_ERROR([cannot detect MIC support for external OTF2 installation. Either provide a suitable OTF2 library or reconfigure ]AC_PACKAGE_NAME[ with the '--without-otf2' option to use the included OTF2 copy.])])
      ])
AC_SCOREP_HAVE_CONFIG_TOOL([opari2], [3])
AC_SCOREP_HAVE_CONFIG_TOOL([cube],   [7])
AS_IF([test -n "${scorep_cube_config_bin}" && \
       test "x${ac_scorep_platform}" = "xmic"],
      [scorep_cube_have_mic=no
       for target in `${scorep_cube_config_bin} --features=targets`
       do
           AS_CASE([${target}],
               [mic], [scorep_cube_have_mic=yes])
       done
       AS_IF([test "x${scorep_cube_have_mic}" = "xno"],
             [AC_MSG_ERROR([cannot detect MIC support for external Cube installation. Either provide a suitable Cube library or reconfigure ]AC_PACKAGE_NAME[ with the '--without-cube' option to use the included Cube copy.])])
      ])

# handle external package's subdir configures

# Cube's java-reader breaks the parallel build and is used by TAU only
# which ships its own, so disable it.
cube_args="`${AWK} -f ${srcdir}/vendor/common/build-config/process_arguments.awk user_provided_configure_args` --without-java-reader"
AS_IF([test "x${scorep_have_cube_config}" = xno],
    [AC_CONFIG_SUBDIR_CUSTOM([vendor/cube], [${cube_args} afs_externally_triggered=yes ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/cube])])

AS_IF([test "x${scorep_have_opari2_config}" = "xno"],
      [AC_CONFIG_SUBDIR_CUSTOM([vendor/opari2], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/opari2])])

AS_IF([test "x${scorep_have_otf2_config}" = "xno"],
      [AC_CONFIG_SUBDIR_CUSTOM([vendor/otf2],   [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/otf2])])

# handle own package's subdir configures

# Always build scorep-score as frontend tool. scorep-score uses cube
# libraries and compiler information, either from internal or external
# cube. Don't pass the usual ${args} to the score configure, it
# figures it out by querying an installed cube or by searching the
# internal cube's configure output. If --bindir was used we need to
# pass it down though (--bindir was used if $bindir = $BINDIR).
scorep_score_extra_arg=
adl_RECURSIVE_EVAL([${bindir}], [BINDIR_EVALUATED])
AS_IF([test "x${bindir}" = "x${BINDIR_EVALUATED}"],
    [scorep_score_extra_arg="--bindir=${BINDIR_EVALUATED}"])
AC_CONFIG_SUBDIR_CUSTOM([build-score], [${scorep_score_extra_arg} ${scorep_cube_config_arg}  ${scorep_otf2_config_arg} afs_cross_compiling=${ac_scorep_cross_compiling}])

AC_CONFIG_SUBDIR_CUSTOM([build-gcc-plugin], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])

AM_CONDITIONAL([HAVE_SCOREP_MPI], [test "x${scorep_mpi_user_disabled}" != "xyes"])
AS_IF([test "x${scorep_mpi_user_disabled}" != "xyes"],
    [AC_CONFIG_SUBDIR_CUSTOM([build-mpi],     [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg}])
    ],
    [AFS_SUMMARY([MPI support], [no, explicitly disabled via --without-mpi])
     AC_CONFIG_FILES([src/scorep_config_tool_mpi.h:src/tools/config/scorep_config_tool_without_mpi.h.in])
    ])

AM_CONDITIONAL([HAVE_SCOREP_SHMEM],
               [test "x${scorep_shmem_user_disabled}" != "xyes" && \
                ( test "x${ac_scorep_platform}" = "xlinux" || \
                  test -f "${srcdir}/vendor/common/build-config/platforms/platform-shmem-${ac_scorep_platform}" )])
AM_COND_IF([HAVE_SCOREP_SHMEM],
    [AC_CONFIG_SUBDIR_CUSTOM([build-shmem], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg}])
    ],
    [AS_IF([test "x${scorep_shmem_user_disabled}" = "xyes"],
        [AFS_SUMMARY([SHMEM support], [no, explicitly disabled via --without-shmem])],
        [AFS_SUMMARY([SHMEM support], [no, platform not supported])])
     AC_CONFIG_FILES([src/scorep_config_tool_shmem.h:src/tools/config/scorep_config_tool_without_shmem.h.in])
    ])

AC_CONFIG_SUBDIR_CUSTOM([build-backend], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg} ${scorep_opari2_config_arg} ${scorep_cube_config_arg}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_opari2_config_arg} ${scorep_cube_config_arg} ${scorep_otf2_config_arg}])
fi

AC_CONFIG_FILES([
    Makefile
    src/config-common.h:vendor/common/build-config/config-common.h
])

# generate the Score-P error codes header
AC_CONFIG_FILES(
    [src/utils/include/SCOREP_ErrorCodes.h:vendor/common/src/utils/exception/ErrorCodes.tmpl.h],
    [$srcdir/vendor/common/src/utils/exception/finalize_error_codes.sh SCOREP $srcdir/share/scorep.errors src/utils/include/SCOREP_ErrorCodes.h src/utils/include/scorep_error_decls.gen.h])

# generate remapper specification file
AC_CONFIG_FILES([share/scorep.spec])

AC_OUTPUT

AS_IF([test "x$ac_scorep_platform_data_provided" != "xyes"], [
    AFS_SUMMARY_COLLECT
])
