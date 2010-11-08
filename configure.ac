AC_PREREQ([2.65])
AC_INIT([scorep],[0.1],[zih-silc-dev@groups.tu-dresden.de])

#echo "cross_compiling = $cross_compiling"

AC_SCOREP_VERSION_AND_REVISION

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
AC_CONFIG_MACRO_DIR([build-config/m4])

## Set the default installation prefix to /opt/scorep instead of /usr/local
AC_PREFIX_DEFAULT([/opt/scorep])

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign silent-rules color-tests 1.11 -Wall tar-ustar])


# don't detect and load defaults in nested configures (e.g. otf2)
if test ! -n "$ac_scorep_platform_data_provided" -o "x${ac_scorep_platform_data_provided}" = "xno"; then
    # $ac_scorep_cross_compiling, $ac_scorep_platform are set here
    AC_SCOREP_DETECT_PLATFORMS
    AC_SCOREP_WITH_COMPILER_SUITE
    AC_SCOREP_LOAD_PLATFORMS_DEFAULTS
    # determine arguments for subdir configures
    args="`$AWK -f $srcdir/build-config/process_arguments.awk \
           $srcdir/build-config/platforms/$ac_scorep_platform \
           $ac_scorep_compiler_suite_files                    \
           user_provided_configure_args`"
else
    AC_MSG_NOTICE([platform data provided by toplevel configure.])
    AC_MSG_NOTICE([debug: ac_scorep_cross_compiling: $ac_scorep_cross_compiling])
    # determine arguments for subdir configures
    args="`$AWK -f $srcdir/build-config/process_arguments.awk \
           user_provided_configure_args`"
fi
   


AC_SCOREP_SVN_CONTROLLED

AC_SCOREP_UNCRUSTIFY

AC_SCOREP_DOXYGEN

AC_SCOREP_DEBUG_OPTION

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])


# handle external package's subdir configures
AC_CONFIG_SUBDIR_CUSTOM([vendor/opari2], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/opari2])
AC_CONFIG_SUBDIR_CUSTOM([vendor/otf2],   [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/otf2])

# handle own package's subdir configures
AC_SCOREP_MPI_COMPILER
AC_CONFIG_SUBDIR_CUSTOM([build-backend/build-mpi], [${args} MPICC=${MPICC} MPICXX=${MPICXX} MPIF77=${MPIF77} MPIFC=${MPIFC} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])
AC_CONFIG_SUBDIR_CUSTOM([build-backend],           [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform}])
fi

AC_CONFIG_FILES([
	Makefile include/scorep/scorep_meets_tau_tmp.h 
])
AC_OUTPUT
