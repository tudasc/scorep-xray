AC_PREREQ([2.68])
AC_INIT([Score-P], m4_esyscmd([vendor/common/build-config/generate-package-version.sh build-config/VERSION]), [scorep-bugs@groups.tu-dresden.de], [scorep])

AC_SCOREP_SUMMARY_INIT

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
AC_CONFIG_MACRO_DIR([vendor/common/build-config/m4])

## Set the default installation prefix to /opt/scorep instead of /usr/local
AC_PREFIX_DEFAULT([/opt/scorep])

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

AC_SCOREP_COMMON_PACKAGE([scorep], [subst-only])

# don't detect and load defaults in nested configures (e.g. otf2)
if test ! -n "$ac_scorep_platform_data_provided" -o "x${ac_scorep_platform_data_provided}" = "xno"; then
    # $ac_scorep_cross_compiling, $ac_scorep_platform are set here
    AC_SCOREP_DETECT_PLATFORMS
    AC_SCOREP_WITH_COMPILER_SUITE
    AC_SCOREP_WITH_MPI_COMPILER_SUITE
    # determine arguments for subdir configures
    args="`$AWK -f $srcdir/vendor/common/build-config/process_arguments.awk \
           mpi_compiler_suite                                               \
           $ac_scorep_compilers_frontend                                    \
           $ac_scorep_compilers_backend                                     \
           user_provided_configure_args`"
else
    AC_MSG_NOTICE([platform data provided by toplevel configure.])
    AC_SCOREP_SUMMARY([Platform], [$ac_scorep_platform (provided)])
    # determine arguments for subdir configures
    args="`$AWK -f $srcdir/vendor/common/build-config/process_arguments.awk \
           user_provided_configure_args`"
fi

AC_SCOREP_SVN_CONTROLLED

AC_SCOREP_UNCRUSTIFY

AC_SCOREP_DOXYGEN
AC_SCOREP_DOXYGEN_CONFIG_FILES

AC_SCOREP_DEBUG_OPTION

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])

AC_SCOREP_HAVE_CONFIG_TOOL([otf2],   [2], [0])
AC_SCOREP_HAVE_CONFIG_TOOL([opari2], [1], [0])
AC_SCOREP_HAVE_CONFIG_TOOL([cube],   [4], [1])

# handle external package's subdir configures
AS_IF([test "x${scorep_have_opari2_config}" = "xno"],
      [AC_CONFIG_SUBDIR_CUSTOM([vendor/opari2], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/opari2])])

AS_IF([test "x${scorep_have_otf2_config}" = "xno"],
      [AC_CONFIG_SUBDIR_CUSTOM([vendor/otf2],   [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/otf2])])

# handle own package's subdir configures
scorep_have_score_build="scorep_have_score_build=no"
AS_IF([test "x${scorep_have_cube_config}" = "xyes"],
      [CUBE_READER_CXX="`${scorep_cube_config_bin} --reader-cxx | awk '{printf $1}'`"
       # Source files to get CXX and CXX_FOR_BUILD. Fragile, does not
       # consider user-provided arguments. We can't get CXX from the
       # backend because the following conditional is evaluated before
       # the backend configure is run.
       AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
             [. ${ac_scorep_compilers_frontend}
              FRONTEND_CXX="`echo ${CXX_FOR_BUILD} | awk '{printf $1}'`"],
             [. ${ac_scorep_compilers_backend}
              FRONTEND_CXX="`echo ${CXX}           | awk '{printf $1}'`"])
       AS_IF([test "x${FRONTEND_CXX}" != "x${CUBE_READER_CXX}"],
             [AC_CONFIG_SUBDIR_CUSTOM([build-score], [CXX="${CUBE_READER_CXX}" ${scorep_cube_config_arg}])
              scorep_have_score_build="scorep_have_score_build=yes"])])
AM_CONDITIONAL([HAVE_SCORE_BUILD], [test "x${scorep_have_score_build}" = "xscorep_have_score_build=yes"])

AC_CONFIG_SUBDIR_CUSTOM([build-mpi],     [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ${scorep_otf2_config_arg}])
AC_CONFIG_SUBDIR_CUSTOM([build-backend], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ${scorep_otf2_config_arg} ${scorep_opari2_config_arg} ${scorep_cube_config_arg} ${scorep_have_score_build}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform} ${scorep_opari2_config_arg} ${scorep_cube_config_arg}  ${scorep_have_score_build}])
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
    AC_SCOREP_SUMMARY_COLLECT
])
