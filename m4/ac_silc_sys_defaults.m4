## -*- mode: autoconf -*-

# require AC_SILC_DETECT_PLATFORMS?
AC_DEFUN([AC_SILC_LOAD_PLATFORMS_DEFAULTS],
[
    AC_ARG_ENABLE([platform-defaults],
                  [AS_HELP_STRING([--enable-platform-defaults],
                                  [load platform defaults if available [yes]])],
                  [ac_silc_platform_defaults=$enableval],
                  [ac_silc_platform_defaults="yes"])

    if  test "x${ac_silc_platform_defaults}" = "xyes"; then
        if test "x${ac_silc_platform}" = "xunknown"; then
            AC_MSG_NOTICE([platform unknown, not loading defaults.])
        elif test "x${ac_silc_platform}" = "xuser_provided"; then
            if test ! -e ./build-config/platforms/${ac_silc_platform}; then
                mkdir -p ./build-config/platforms
                cp ${srcdir}/build-config/platforms/${ac_silc_platform} ./build-config/platforms/${ac_silc_platform}
            fi
            . ./build-config/platforms/${ac_silc_platform}
            AC_MSG_NOTICE([loading defaults for platform $ac_silc_platform; you may want to alter the file ./build-config/m4/platforms/$ac_silc_platform and rerun configure.])
        else
            . $srcdir/build-config/platforms/${ac_silc_platform}
            AC_MSG_NOTICE([loading defaults for platform $ac_silc_platform])
        fi
    else
        AC_MSG_NOTICE([loading platform defaults disabled.])
    fi
])
