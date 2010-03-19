AC_PREREQ([2.65])
AC_INIT([silc-frontend], [0])

## Declare variable as precious variable and include its description in the
## variable section of `./configure --help'.
AC_ARG_VAR([CC_FOR_BUILD], [CC used for the frontend build, usually set automatically])
AC_ARG_VAR([CXX_FOR_BUILD], [CXX used for the frontend build, usually set automatically])
AC_ARG_VAR([CFLAGS_FOR_BUILD],   [CFLAGS used for the frontend build])
AC_ARG_VAR([CXXFLAGS_FOR_BUILD], [CXXFLAGS used for the frontend build])
AC_ARG_VAR([CPPFLAGS_FOR_BUILD], [CPPFLAGS used for the frontend build])

AC_SILC_CONVERT_FLAGS([CFLAGS])
AC_SILC_CONVERT_FLAGS([CXXFLAGS])
AC_SILC_CONVERT_FLAGS([CPPFLAGS])

AC_SILC_CONVERT_COMPILER([CC])
AC_SILC_CONVERT_COMPILER([CXX])

AC_SILC_SVN_CONTROLLED
AH_SILC_CONFIG_HEADER_CUSTOM
AC_CONFIG_HEADERS([../src/config-frontend.h])




AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../build-config/m4])

AM_INIT_AUTOMAKE([foreign silent-rules color-tests 1.11 -Wall])
AM_CONDITIONAL([BUILD_TESTS], [test 1 -eq 1])


LT_PREREQ([2.2.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_SILC_COMMON_CHECKS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
