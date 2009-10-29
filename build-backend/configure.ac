AC_INIT([silc-backend], [0])

## require $ac_silc_cross_compiling to be set to 'yes' on 'no'
if test "x${ac_silc_cross_compiling}" != "xyes" -a "x${ac_silc_cross_compiling}" != "xno"; then
    AC_MSG_ERROR([invalid value '$ac_silc_cross_compiling' for variable \$ac_silc_cross_compiling.], [1])
fi








AC_SILC_CHECK_COMPILER_VAR_SET([CC])
AC_SILC_CHECK_COMPILER_VAR_SET([CXX])

AC_CONFIG_HEADERS([../src/config-backend.h])
# As the config.h.in is generated from this configure.ac by autoheader, there 
# is no need for a separate config-frontend.h
#if test "x${ac_silc_cross_compiling}" = "xno"; then
#    AC_CONFIG_HEADERS([../src/config-frontend.h])
#fi

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../build-config/m4])

AM_INIT_AUTOMAKE([foreign -Wall])
AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_silc_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_silc_cross_compiling}" = "xyes"])

LT_PREREQ([2.2.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

# don't use ${exec_prefix} here, evaluates to 'NONE' but use it in Makefile.am
backend_bin_dir="/bin"
backend_lib_dir="/lib"
if test "x${ac_silc_cross_compiling}" = "xyes"; then
    backend_bin_dir="/bin/backend"
    backend_lib_dir="/lib/backend"
fi
AC_SUBST(backend_bin_dir)
AC_SUBST(backend_lib_dir)

AC_SILC_COMMON_CHECKS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
