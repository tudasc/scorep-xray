AC_PREREQ([2.65])
AC_INIT([silc],[0.1],[zih-silc@groups.tu-dresden.de])

#echo "cross_compiling = $cross_compiling"

AC_SILC_VERSION_AND_REVISION

AC_SILC_TOPLEVEL_ARGS

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

## Set the default installation prefix to /opt/silc instead of /usr/local
AC_PREFIX_DEFAULT([/opt/silc])

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign silent-rules color-tests 1.11 -Wall])


# don't detect and load defaults in nested configures (e.g. otf2)
if test ! -n "$ac_silc_platform_data_provided" -o "x${ac_silc_platform_data_provided}" = "xno"; then
    # $ac_silc_cross_compiling, $ac_silc_platform are set here
    AC_SILC_DETECT_PLATFORMS
    AC_SILC_LOAD_PLATFORMS_DEFAULTS
    # determine arguments for subdir configures
    args="`$AWK -f $srcdir/build-config/process_arguments.awk \
           $srcdir/build-config/platforms/$ac_silc_platform   \
           user_provided_configure_args`"
else
    AC_MSG_NOTICE([platform data provided by toplevel configure.])
    AC_MSG_NOTICE([debug: ac_silc_cross_compiling: $ac_silc_cross_compiling])
    # determine arguments for subdir configures
    args="`$AWK -f $srcdir/build-config/process_arguments.awk \
           user_provided_configure_args`"
fi
   


AC_SILC_SVN_CONTROLLED

AC_SILC_UNCRUSTIFY

AC_SILC_DOXYGEN

AC_SILC_DEBUG_OPTION

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_silc_cross_compiling}" = "xyes"])


# handle external package's subdir configures
AC_CONFIG_SUBDIR_CUSTOM([vendor/opari2], [${args} ac_silc_platform=${ac_silc_platform} ac_silc_platform_data_provided=yes ac_silc_cross_compiling=${ac_silc_cross_compiling} ac_silc_doxygen_distdir=${ac_silc_doxygen_distdir}/vendor/opari2])
AC_CONFIG_SUBDIR_CUSTOM([vendor/otf2],   [${args} ac_silc_platform=${ac_silc_platform} ac_silc_platform_data_provided=yes ac_silc_cross_compiling=${ac_silc_cross_compiling} ac_silc_doxygen_distdir=${ac_silc_doxygen_distdir}/vendor/otf2])

# handle own package's subdir configures
AC_SILC_MPI_COMPILER
AC_CONFIG_SUBDIR_CUSTOM([build-backend/build-mpi], [${args} MPICC=${MPICC} MPICXX=${MPICXX} MPIF77=${MPIF77} MPIFC=${MPIFC} ac_silc_platform=${ac_silc_platform} ac_silc_cross_compiling=${ac_silc_cross_compiling}])
AC_CONFIG_SUBDIR_CUSTOM([build-backend],           [${args} ac_silc_platform=${ac_silc_platform} ac_silc_cross_compiling=${ac_silc_cross_compiling}])

if test "x${ac_silc_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [${args} ac_silc_platform=${ac_silc_platform}])
fi

AC_CONFIG_FILES([
	Makefile include/silc/scorep_meets_tau_tmp.h 
])
AC_OUTPUT
