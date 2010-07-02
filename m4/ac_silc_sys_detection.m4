## -*- mode: autoconf -*-

AC_DEFUN([_AC_SILC_DETECT_LINUX_PLATFORMS],
[
    if test "x${ac_silc_platform}" = "xunknown"; then
        case ${build_os} in
            linux*)
                AS_IF([test "x${build_cpu}" = "xia64"      -a -f /etc/sgi-release], 
                          [ac_silc_platform="altix";    ac_silc_cross_compiling="no"],
                      [test "x${build_cpu}" = "xpowerpc64" -a -d /bgl/BlueLight],   
                          [ac_silc_platform="bgl";      ac_silc_cross_compiling="yes"],
                      [test "x${build_cpu}" = "xpowerpc64" -a -d /bgsys],           
                          [ac_silc_platform="bgp";      ac_silc_cross_compiling="yes"],
                      [test "x${build_cpu}" = "xx86_64"    -a -d /opt/xt-boot],     
                          [ac_silc_platform="crayxt";   ac_silc_cross_compiling="yes"],
                      [test "x${build_cpu}" = "xmips64"    -a -d /opt/sicortex],    
                          [ac_silc_platform="sicortex"; ac_silc_cross_compiling="yes"],
                      [ac_silc_platform=linux]
                )
            ;;
        esac
    fi
])


AC_DEFUN([_AC_SILC_DETECT_NON_LINUX_PLATFORMS],
[
    if test "x${ac_silc_platform}" = "xunknown"; then
        case ${build_os} in
            sunos* | solaris*)
                ac_silc_platform="sun"
                ac_silc_cross_compiling="no"
                ;;
            darwin*)
                ac_silc_platform="mac"
                ac_silc_cross_compiling="no"
                ;;
            aix*)
                ac_silc_platform="ibm"
                ac_silc_cross_compiling="no"
                ;;
            unicosmp*)
                ac_silc_platform="crayx1"
                ac_silc_cross_compiling="no"
                ;;
            superux*)
                ac_silc_platform="necsx"
                ac_silc_cross_compiling="yes"
                ;;
        esac
    fi
])

# The purpose of platform detection is to provide reasonable default
# compilers, mpi-implementations, OpenMP flags etc.  The user always has the
# possibility to override the defaults by setting environment variables, see
# section "Some influential environment variables" in configure --help.  On
# some systems there may be no reasonable defaults for the mpi-implementation,
# so specify them using --with-mpi=... I think we need to specify one or more
# paths too. Also, on some systems there are different compiler-suites available
# which can be choosen via --with-compiler=(gnu?|intel|sun|ibm|...)
# Have to think this over...

AC_DEFUN([AC_SILC_DETECT_PLATFORMS],
[
    AC_REQUIRE([AC_CANONICAL_BUILD])
    ac_silc_platform="unknown"
    ac_silc_cross_compiling="no"
    ac_silc_platform_detection=""
    ac_silc_platform_detection_given=""

    if test "x${host_alias}" != "x"; then
        AC_CANONICAL_HOST
        if test "x${build}" != "x${host}"; then
            ac_silc_cross_compiling="yes"
        fi
    fi

    AC_ARG_ENABLE([platform-detection],
                  [AS_HELP_STRING([--enable-platform-detection],
                                  [autodetect platform [yes]])],
                  [ac_silc_platform_detection_given="$enableval"],
                  [AS_IF([test "x${build_alias}" = "x" -a "x${host_alias}" = "x"],
                         [ac_silc_platform_detection="yes"],
                         [ac_silc_platform_detection="no"])])

    if test "x${ac_silc_platform_detection_given}" = "xyes"; then
        if test "x${build_alias}" != "x" -o "x${host_alias}" != "x"; then
            AC_MSG_ERROR([it makes no sense to request for platform detection while providing --host and/or --build.])
        fi
    fi
    if test "x${ac_silc_platform_detection_given}" != "x"; then
        ac_silc_platform_detection="$ac_silc_platform_detection_given"
    fi

    if test "x${ac_silc_platform_detection}" = "xyes"; then
        _AC_SILC_DETECT_LINUX_PLATFORMS
        _AC_SILC_DETECT_NON_LINUX_PLATFORMS        
        AC_MSG_CHECKING([for platform])
        if test "x${ac_silc_platform}" = "xunknown"; then
            AC_MSG_RESULT([$ac_silc_platform, please contact <AC_PACKAGE_BUGREPORT> if you encounter any problems.])
        else
            AC_MSG_RESULT([$ac_silc_platform])  
        fi
        AC_MSG_CHECKING([for cross compilation])
        AC_MSG_RESULT([$ac_silc_cross_compiling])
    elif test "x${ac_silc_platform_detection}" = "xno"; then
        AC_MSG_NOTICE([platform detection disabled.])
        AC_MSG_CHECKING([for cross compilation])
        AC_MSG_RESULT([$ac_silc_cross_compiling]) 
        ac_silc_platform="user_provided"
    else
        AC_MSG_ERROR([unknown value for ac_silc_platform_detection: $ac_silc_platform_detection])
    fi

    AM_CONDITIONAL([PLATFORM_ALTIX], [test "x${ac_silc_platform}" = "xaltix"])
])
