AC_INIT([silc],[0.1],[zih-silc@groups.tu-dresden.de])
AC_PREREQ(2.63)

#echo "cross_compiling = $cross_compiling"

AC_SILC_VERSION_AND_REVISION

# <original arguments>
# Export original configure arguments for use by sub-configures.
# Quote arguments with shell meta charatcers.
TOPLEVEL_CONFIGURE_ARGUMENTS=
set -- "$progname" "$@"
for ac_arg
do
  case "$ac_arg" in
  *" "*|*"	"*|*[[\[\]\~\#\$\^\&\*\(\)\{\}\\\|\;\<\>\?\']]*)
    ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"`
    # if the argument is of the form -foo=baz, quote the baz part only
    ac_arg=`echo "'$ac_arg'" | sed "s/^'\([[-a-zA-Z0-9]]*=\)/\\1'/"` ;;
  *) ;;
  esac
  # Add the quoted argument to the list.
  TOPLEVEL_CONFIGURE_ARGUMENTS="$TOPLEVEL_CONFIGURE_ARGUMENTS  
$ac_arg"
done
if test "$silent" = yes; then
  TOPLEVEL_CONFIGURE_ARGUMENTS="$TOPLEVEL_CONFIGURE_ARGUMENTS --silent"
fi
# Remove the initial space we just introduced and, as these will be
# expanded by make, quote '$'.
TOPLEVEL_CONFIGURE_ARGUMENTS=`echo "x$TOPLEVEL_CONFIGURE_ARGUMENTS" | sed -e 's/^x *//' -e 's,\\$,$$,g'`
#AC_SUBST(TOPLEVEL_CONFIGURE_ARGUMENTS)
#echo "debug: TOPLEVEL_CONFIGURE_ARGUMENTS: $TOPLEVEL_CONFIGURE_ARGUMENTS"
# </original arguments>

echo "$TOPLEVEL_CONFIGURE_ARGUMENTS" > ./user_provided_configure_args 


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
AM_INIT_AUTOMAKE([foreign -Wall 1.10.1])


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
#AC_CHECK_PROG([have_svnversion], [svnversion], [yes], [no],,)
AC_SILC_DOXYGEN

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_silc_cross_compiling}" = "xyes"])

# handle external package's subdir configures
AC_CONFIG_SUBDIR_CUSTOM([vendor/otf2], [$args ac_silc_platform_data_provided=yes ac_silc_cross_compiling=$ac_silc_cross_compiling])

# handle own package's subdir configures
AC_CONFIG_SUBDIR_CUSTOM([build-backend], [$args ac_silc_cross_compiling=${ac_silc_cross_compiling}])

if test "x${ac_silc_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [$args])
fi


#AC_CONFIG_SUBDIR_CUSTOM([bar], [ CC=gcc ])
#AC_CONFIG_SUBDIR_CUSTOM([ ], [ CC=gcc ])

#AC_CONFIG_SUBDIR_CUSTOM([build-frontend ], [ CC=icc --host=powerpc-bgp-linux --build=powerpc64-unknown-linux-gnu], [no])
#AC_CONFIG_SUBDIR_CUSTOM([ build-backend], [ CC=gcc ])

#if test "x${cross_compiling}" = xyes; then
#   echo ${build}
#   echo ${host}
#   AC_CONFIG_SUBDIRS([build-cross-frontend build-cross-backend])
#   SUBDIRS_TO_USE="build-cross-frontend build-cross-backend"
#else
#   echo ${build}
#   echo ${host}
#   AC_CONFIG_SUBDIRS([build-nocross])
#   SUBDIRS_TO_USE="build-nocross"
#fi

AC_CONFIG_FILES([
	Makefile
])
AC_OUTPUT
