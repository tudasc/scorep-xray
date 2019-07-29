dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014, 2017, 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2019,
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

SCOREP_TOPLEVEL_ARGS

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
AFS_MAINTAINER_MODE

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

AC_ARG_ENABLE([experimental-platform],
              [AS_HELP_STRING([--enable-experimental-platform],
                              [Enables builds on platforms that are not officially supported (currently macOS and MinGW).])],
              [scorep_enable_experimental_platform=$enableval],
              [scorep_enable_experimental_platform=no])

# don't detect and load defaults in nested configures (e.g. otf2)
AS_IF([test "x$ac_scorep_platform_data_provided" = "x" || \
       test "${ac_scorep_platform_data_provided}" = "no"],
      [AFS_CUSTOM_COMPILERS
       AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
           [dnl AFS_COMPILER_BACKEND([CC, CXX, F77, FC])
            AC_SCOREP_WITH_COMPILER_SUITE
            AS_CASE([${ac_scorep_platform},${scorep_enable_experimental_platform}],
                [*,yes],
                    [AC_MSG_WARN([Attempting an experimental build for platform '${afs_scorep_platform_name}'. Feel free to report bugs or issues to <AC_PACKAGE_BUGREPORT>, but note that this is not a supported platform yet. Namaste.])],
                [mac,no|mingw,no],
                    [AC_MSG_ERROR([Platform '${afs_scorep_platform_name}' is not officially supported by ]AC_PACKAGE_NAME[. You can specify "--enable-experimental-platform" to enable an experimental build and report success to <AC_PACKAGE_BUGREPORT>. Thanks.])],
                [unknown],
                    [AC_MSG_ERROR([Cannot detect platform to set suitable defaults. Please use --with-custom-compilers and provide settings and platform to <AC_PACKAGE_BUGREPORT> on success. Thanks.])
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

       # temporary workaround: prevent using libunwind together with
       # Intel MPI as application results may show erroneous behavior.
       # bt-mz's verification fails for some configurations, cp2k does
       # not converge. Reason so far unclear but problem vanishes when
       # not linking with libunwind.
       AC_REQUIRE([AC_PROG_GREP])
       AC_REQUIRE([AC_PROG_SED])
       AS_CASE([${ac_scorep_compilers_mpi}],
           [*intel|*intel[[23]]],
               [GREP_OPTIONS= ${GREP} '^--with-libunwind' user_provided_configure_args > /dev/null
                AS_IF([test $? -eq 0],
                   [# Remove any given --with-libunwind option
                    ${SED} '/^--with-libunwind/d' user_provided_configure_args > user_provided_configure_args.nounw
                    mv user_provided_configure_args.nounw user_provided_configure_args])
                echo "--without-libunwind" >> user_provided_configure_args
                AFS_SUMMARY([Unwinding/Sampling support], [Disabled when used with Intel MPI as this combination shows erroneous behavior])])
       # end of workaround

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

AC_SCOREP_HAVE_CONFIG_TOOL([otf2],   [9])
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
AC_SCOREP_HAVE_CONFIG_TOOL([cubew],   [7])
AS_IF([test -n "${scorep_cubew_config_bin}" && \
       test "x${ac_scorep_platform}" = "xmic"],
      [scorep_cubew_have_mic=no
       for target in `${scorep_cubew_config_bin} --features=targets`
       do
           AS_CASE([${target}],
               [mic], [scorep_cubew_have_mic=yes])
       done
       AS_IF([test "x${scorep_cubew_have_mic}" = "xno"],
             [AC_MSG_ERROR([cannot detect MIC support for external Cube Writer installation. Either provide a suitable Cube library or reconfigure ]AC_PACKAGE_NAME[ with the '--without-cubew' option to use the included Cube Writer copy.])])
      ])

# handle external package's subdir configures

user_args="`${AWK} -f ${srcdir}/vendor/common/build-config/process_arguments.awk user_provided_configure_args`"
AS_IF([test "x${scorep_have_cubew_config}" = xno],
    [AX_CONFIG_SUBDIR([vendor/cubew], [${user_args} afs_externally_triggered=yes ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/cubew])])


AC_SCOREP_HAVE_CONFIG_TOOL([cubelib],   [7])

user_args="`${AWK} -f ${srcdir}/vendor/common/build-config/process_arguments.awk user_provided_configure_args`"
AS_IF([test "x${scorep_have_cubelib_config}" = xno],
    [AX_CONFIG_SUBDIR([vendor/cubelib], [${user_args} afs_externally_triggered=yes ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/cubelib])])

AS_IF([test "x${scorep_have_opari2_config}" = "xno"],
      [AX_CONFIG_SUBDIR([vendor/opari2], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/opari2])])

AS_IF([test "x${scorep_have_otf2_config}" = "xno"],
      [AX_CONFIG_SUBDIR([vendor/otf2],   [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/otf2])])

# handle own package's subdir configures

# Always build scorep-score as frontend tool. scorep-score uses cube
# libraries and compiler information, either from internal or external
# cube. Don't pass the usual ${args}; the compiler might get changed
# and then potentially chokes on FLAGS (see r5771, #849).  But users
# expect that shared/static and --bindir and --libdir are passed, if
# given.
# We detect if --bin/libdir was given by comparing against the default
# values.
AC_PROG_EGREP
scorep_score_extra_arg="$(GREP_OPTIONS= ${EGREP} -e 'able-shared' -e 'able-static' user_provided_configure_args | tr "\n" " ")"
AS_IF([test "x${bindir}" != 'x${exec_prefix}/bin'],
    [adl_RECURSIVE_EVAL([${bindir}], [BINDIR_EVALUATED])
     scorep_score_extra_arg="--bindir=${BINDIR_EVALUATED} ${scorep_score_extra_arg}"])
AS_IF([test "x${libdir}" != 'x${exec_prefix}/lib'],
    [adl_RECURSIVE_EVAL([${libdir}], [LIBDIR_EVALUATED])
     scorep_score_extra_arg="--libdir=${LIBDIR_EVALUATED} ${scorep_score_extra_arg}"])
AX_CONFIG_SUBDIR([build-score], [${scorep_score_extra_arg} ${scorep_cubelib_config_arg} ${scorep_otf2_config_arg} afs_cross_compiling=${ac_scorep_cross_compiling}])

AX_CONFIG_SUBDIR([build-gcc-plugin], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])

AX_CONFIG_SUBDIR([build-libwrap], [${args} afs_cross_compiling=${ac_scorep_cross_compiling}])

AM_CONDITIONAL([HAVE_SCOREP_MPI], [test "x${scorep_mpi_user_disabled}" != "xyes"])
AS_IF([test "x${scorep_mpi_user_disabled}" != "xyes"],
    [AX_CONFIG_SUBDIR([build-mpi],     [${args} ${backend_args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg} ${scorep_opari2_config_arg}])
    ],
    [AFS_SUMMARY([MPI support], [no, explicitly disabled via --without-mpi])
     AC_CONFIG_FILES([src/scorep_config_tool_mpi.h:src/tools/config/scorep_config_tool_without_mpi.h.in])
    ])

AM_CONDITIONAL([HAVE_SCOREP_SHMEM],
               [test "x${scorep_shmem_user_disabled}" != "xyes" && \
                ( test "x${ac_scorep_platform}" = "xlinux" || \
                  test -f "${srcdir}/vendor/common/build-config/platforms/platform-shmem-${ac_scorep_platform}" )])
AM_COND_IF([HAVE_SCOREP_SHMEM],
    [AX_CONFIG_SUBDIR([build-shmem], [${args} ${backend_args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg} ${scorep_opari2_config_arg}])
    ],
    [AS_IF([test "x${scorep_shmem_user_disabled}" = "xyes"],
        [AFS_SUMMARY([SHMEM support], [no, explicitly disabled via --without-shmem])],
        [AFS_SUMMARY([SHMEM support], [no, platform not supported])])
     AC_CONFIG_FILES([src/scorep_config_tool_shmem.h:src/tools/config/scorep_config_tool_without_shmem.h.in])
    ])

AX_CONFIG_SUBDIR([build-backend], [${args} ${backend_args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_otf2_config_arg} ${scorep_opari2_config_arg} ${scorep_cubew_config_arg} ${scorep_cubelib_config_arg}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AX_CONFIG_SUBDIR([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform} afs_scorep_platform_name='${afs_scorep_platform_name_sq}' afs_scorep_default_machine_name='${afs_scorep_default_machine_name_sq}' ${scorep_opari2_config_arg} ${scorep_cubew_config_arg} ${scorep_otf2_config_arg}])
fi

adl_RECURSIVE_EVAL([${docdir}], [DOCDIR])
AC_SUBST([DOCDIR])

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
