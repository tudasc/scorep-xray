# parts taken from scorep_llvm_plugin.m4

# SCOREP_XRAY_PLUGIN_SUMMARY()
# ----------------------------
# Reports summary about the XRAY plugin configuration.
# Checks if XRAY plugin is requested by the user
#
AC_DEFUN_ONCE([SCOREP_XRAY_PLUGIN_SUMMARY], [
AC_REQUIRE([SCOREP_XRAY_PLUGIN]) # Call SCOREP_XRAY_PLUGIN at least once

AC_SCOREP_COND_HAVE([XRAY_PLUGIN_SUPPORT],
    [test "x${have_xray_plugin_support}" = "xyes"],
    [Defined if XRAY plugin can be compiled and enabled for at least one language])

# If HAVE_XRAY_PLUGIN_SUPPORT then print summary, else: print error only if enabling was explicitly requested
AM_COND_IF([HAVE_XRAY_PLUGIN_SUPPORT],
    [AFS_PROG_CXX_SUMMARY AFS_COMMON_UTILS([build], [<SCOREP_ErrorCodes.h>], [../src/utils/include])],
    [AS_IF([test "x${enable_xray_plugin}" = "xyes"],
        [AC_MSG_ERROR([Request to enable the XRAY plug-in could not be fulfilled. Run configure with --disable-xray-plugin or run without --enable-xray-plugin. Reason: $xray_plugin_reason])])])

AFS_SUMMARY([XRAY plugin support], [$have_xray_plugin_support${xray_plugin_reason:+, $xray_plugin_reason}])
])


# SCOREP_XRAY_PLUGIN()
# -------------------------------------------------------------------
# Tests if XRAY plugin can be enabled and used for selected compiler
#
# These tests are conducted:
# - Are we in a cross-compile environment?
# - Is the compiler a LLVM based compiler?
#   (sets have_llvm_based_compiler)
# - Is llvm-config available?
# - Can a example plugin be compiled?
#   (sets can_compile_xray_plugin)
# - Can the plugin be enabled for C, C++ and Fortran
#   (writes xray_plugin_supported_[c|cxx|fc] if successful)
#
# If all tests are successful, required flags are
# written to SCOREP_XRAY_PLUGIN_[CXXFLAGS|CPPFLAGS|LDFLAGS|LIBS].
# If not, the reason can be extracted from xray_plugin_reason.
#
AC_DEFUN([SCOREP_XRAY_PLUGIN], [
    AC_REQUIRE([AC_PROG_SED])
    AC_ARG_ENABLE(
        [xray-plugin],
        [AS_HELP_STRING([--disable-xray-plugin],
        [Disable general support for the XRAY plug-in based instrumentation. Default is to determine support automatically.])],
        [enable_xray_plugin="${enableval}"],
        [enable_xray_plugin="not_given"]
    )
    have_llvm_based_compiler="no"
    have_xray_plugin_c_support="no"
    have_xray_plugin_cxx_support="no"
    have_xray_plugin_fc_support="no"
    have_xray_plugin_support="no"

    # TODO: As Xray is part of LLVM, requirements are based on LLVM plugin, check if cross compiling can be supported
    AS_IF(
        [test "x${cross_compiling}" = "xyes"],
        [AS_CASE([${ac_scorep_platform}],
            [crayx*],
            [], # Cray machines might report being a cross-compile environment even though they're not. Try to build the plug-in anyway.
            [enable_xray_plugin="no"
            xray_plugin_reason=${xray_plugin_reason:-"not yet supported in cross-compile mode"}]
        )]
    )

    # Check status of llvm plugin, as they are mutually exclusive. If the llvm plugin was successfully enabled for any
    # language, the xray plugin is disabled completely.
    AS_IF([test -f ../build-llvm-plugin/llvm_plugin_supported_c++ || test -f ../build-llvm-plugin/llvm_plugin_supported_c || test -f ../build-llvm-plugin/llvm_plugin_supported_fortran],
        [enable_xray_plugin="no"
        xray_plugin_reason="LLVM plugin was already enabled. Use --disable-llvm-plugin to enable XRay-plugin"],
    )

    AS_IF(
        [test "x${enable_xray_plugin}" != "xno"],
        [_CHECK_CXX_COMPILER_VENDOR
        AS_IF(
            [test "x${have_llvm_based_compiler}" = "xyes"],
            [SCOREP_LLVM_CONFIG
            AS_IF(
                [test "x${scorep_have_llvm_config}" = "xyes"],
                [XRAY_PLUGIN_TARGET_CFLAGS="${XRAY_PLUGIN_TARGET_CFLAGS} $(${scorep_llvm_config_bin} --cflags | ${SED} -E 's/ -std=[[a-zA-Z]]+.*[[0-9]]{2} //') -fxray-instrument -fxray-instruction-threshold=1 -fxray-enable-shared -fxray-attr-list=${srcdir}/../src/adapters/compiler/xray-plugin/scorep_xray_filter_no_instrumentation.txt"
                XRAY_PLUGIN_TARGET_CXXFLAGS="${XRAY_PLUGIN_TARGET_CXXFLAGS} $(${scorep_llvm_config_bin} --cxxflags | ${SED} -E 's/ -std=[[a-zA-Z]]+.*[[0-9]]{2} //') -fxray-instrument -fxray-instruction-threshold=1 -fxray-enable-shared -fxray-attr-list=${srcdir}/../src/adapters/compiler/xray-plugin/scorep_xray_filter_no_instrumentation.txt"
                XRAY_PLUGIN_TARGET_CPPFLAGS="${XRAY_PLUGIN_TARGET_CPPFLAGS} $(${scorep_llvm_config_bin} --cppflags)"
                # pass xray instrument flag to linker to link runtime libs
                XRAY_PLUGIN_TARGET_LDFLAGS="${XRAY_PLUGIN_TARGET_LDFLAGS} -fxray-instrument -fxray-enable-shared -lstdc++ $(${scorep_llvm_config_bin} --ldflags)"
                XRAY_PLUGIN_TARGET_LIBS="${XRAY_PLUGIN_TARGET_LIBS} $(${scorep_llvm_config_bin} --libs demangle support xray symbolize)"
                # System libs may not be empty if default include paths edited, therefore include them as target libs
                XRAY_PLUGIN_TARGET_LIBS="${XRAY_PLUGIN_TARGET_LIBS} $(${scorep_llvm_config_bin} --system-libs)"
                AC_LANG_PUSH([C++])
                _BUILD_XRAY_PLUGIN
                AC_LANG_POP([C++])
                AS_IF(
                    [test "x${can_compile_xray_plugin}" = "xyes"],
                    [AC_LANG_PUSH([C])
                    _TEST_ENABLE_XRAY_PLUGIN #TODO: Update TEST since plugin is compiled with c++ anyways
                    AC_LANG_POP([C])
                    AC_LANG_PUSH([C++])
                    _TEST_ENABLE_XRAY_PLUGIN
                    AC_LANG_POP([C++])
                    AC_LANG_PUSH([Fortran])
                    _TEST_ENABLE_XRAY_PLUGIN
                    AC_LANG_POP([Fortran])
                    AS_IF(
                        [test "x${have_xray_plugin_support}" = "xno"],
                        [xray_plugin_reason="could not enable plugin"]
                    )],
                    [xray_plugin_reason="could not build plugin"]
                )
                _CLEAN_LLVM_PLUGIN],
                [xray_plugin_reason="${scorep_llvm_config_reason}"]
            )],
            [xray_plugin_reason="only LLVM based compilers supported"]
        )],
        [xray_plugin_reason=${xray_plugin_reason:-"explicitly disabled via --disable-xray-plugin"}]
    )

    AS_IF(
        [test "x${have_xray_plugin_support}" = "xyes"],
        [AC_SUBST(SCOREP_XRAY_PLUGIN_CXXFLAGS, ["${XRAY_PLUGIN_TARGET_CXXFLAGS}"])
         AC_SUBST(SCOREP_XRAY_PLUGIN_CFLAGS, ["${XRAY_PLUGIN_TARGET_CFLAGS}"])
         AC_SUBST(SCOREP_XRAY_PLUGIN_CPPFLAGS, ["${XRAY_PLUGIN_TARGET_CPPFLAGS}"])
         AC_SUBST(SCOREP_XRAY_PLUGIN_LDFLAGS, ["${XRAY_PLUGIN_TARGET_LDFLAGS}"])
         AC_SUBST(SCOREP_XRAY_PLUGIN_LIBS, ["${XRAY_PLUGIN_TARGET_LIBS}"])]
    )
]) # </SCOREP_XRAY_PLUGIN>


# _CHECK_CXX_COMPILER_VENDOR
# ----------------------
# Checks for a given C++ compiler if the compiler vendor matches
# clang or intel/oneapi
#
m4_define(
    [_CHECK_CXX_COMPILER_VENDOR],
    [AS_UNSET([ax_cv_cxx_compiler_vendor])
    save_cxx=$CXX
    save_CXXFLAGS=$CXXFLAGS
    CXX=$XRAY_PLUGIN_TARGET_CXX
    CXXFLAGS=$XRAY_PLUGIN_TARGET_CXXFLAGS
    AC_LANG_PUSH([C++])
    AX_COMPILER_VENDOR
    AC_LANG_POP([C++])
    xray_plugin_target_vendor_cxx="${ax_cv_cxx_compiler_vendor}"
    CXX=$save_cxx
    CXXFLAGS=$save_CXXFLAGS
    AS_IF(
        [test "x${xray_plugin_target_vendor_cxx}" = "xclang" || test "x${xray_plugin_target_vendor_cxx}" = "xintel/oneapi"],
        [have_llvm_based_compiler="yes"])]
)

# _BUILD_XRAY_PLUGIN
# ------------------
#
m4_define(
    [_BUILD_XRAY_PLUGIN],
    [AC_REQUIRE([LT_OUTPUT])
    AC_LANG_PUSH([C++])
    plugin_compile='$SHELL ./libtool --mode=compile --tag=_AC_CC [$]_AC_CC $XRAY_PLUGIN_TARGET_CXXFLAGS [$]_AC_LANG_PREFIX[FLAGS] -c -o conftest.lo conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
    plugin_link='$SHELL ./libtool --mode=link --tag=_AC_CC [$]_AC_CC [$]_AC_LANG_PREFIX[FLAGS] -module -avoid-version $XRAY_PLUGIN_TARGET_LDFLAGS $XRAY_PLUGIN_TARGET_LIBS -rpath $PWD/lib -o confmodule.la conftest.lo >&AS_MESSAGE_LOG_FD'
    plugin_mkdir='$MKDIR_P lib >&AS_MESSAGE_LOG_FD'
    plugin_install='$SHELL ./libtool --mode=install $INSTALL confmodule.la $PWD/lib/confmodule.la >&AS_MESSAGE_LOG_FD'
    AC_MSG_CHECKING([whether XRAY example plugin can compile])
    AC_LANG_CONFTEST([AC_LANG_SOURCE(_INPUT_XRAY_PLUGIN)])
    AS_IF([_AC_DO_VAR([plugin_compile]) &&
           _AC_DO_VAR([plugin_link]) &&
           _AC_DO_VAR([plugin_mkdir]) &&
           _AC_DO_VAR([plugin_install])],
        [can_compile_xray_plugin="yes"],
        [can_compile_xray_plugin="no"])
    AC_MSG_RESULT([${can_compile_xray_plugin}])
    AC_LANG_POP([C++])

    AS_UNSET([plugin_compile])
    AS_UNSET([plugin_link])
    AS_UNSET([plugin_mkdir])
    AS_UNSET([plugin_install])
])

# _TEST_ENABLE_XRAY_PLUGIN
# ------------------------
#

# TODO!: Link plugin into basic executable or at least check for xray functionality (currently the file is written independent of xray plugin)
m4_define(
    [_TEST_ENABLE_XRAY_PLUGIN],
    [save_CC=$[]_AC_CC
    save_CFLAGS=$[]_AC_LANG_PREFIX[]FLAGS
    _AC_CC[]=$XRAY_PLUGIN_TARGET_[]_AC_CC
    AC_MSG_CHECKING([whether XRAY plugin for $[]_AC_CC can be loaded])
    for compiler_backend_flag_arg in "-Xclang" "-Xflang"; do
        #_AC_LANG_PREFIX[]FLAGS="-fpass-plugin=$PWD/lib/confmodule.so $compiler_backend_flag_arg -load $compiler_backend_flag_arg $PWD/lib/confmodule.so -mllvm -lang=[]_AC_LANG_ABBREV[]"
        #_AC_LANG_PREFIX[]FLAGS="$compiler_backend_flag_arg -load $compiler_backend_flag_arg $PWD/lib/confmodule.so -lang=[]_AC_LANG_ABBREV[]"
        # TODO: Run code or just compile?
        AC_RUN_IFELSE([AC_LANG_SOURCE(_INPUT_XRAY_TEST_[]_AC_LANG_PREFIX)],
            [AS_IF(
                [test -f xray_plugin_supported_[]_AC_LANG_ABBREV[]],
                [have_xray_plugin_support="yes"
                    have_xray_plugin_[]_AC_LANG_ABBREV[]_support="yes"
                    AC_SUBST(SCOREP_XRAY_PLUGIN_COMPILER_BACKEND_FLAG_[]_AC_CC[], ["${compiler_backend_flag_arg}"])
                    break],
                [have_xray_plugin_[]_AC_LANG_ABBREV[]_support="no"])],
            [have_xray_plugin_[]_AC_LANG_ABBREV[]_support="no"],
            [enable_xray_plugin="no"
                # TODO!: Check support for cross compiling, but leave this here to suppress configure warning
                AC_MSG_WARN([Testing XRay plugin led to cross compiling, which is not yet supported])
                xray_plugin_reason=${xray_plugin_reason:-"not yet supported in cross-compile mode"}]
        )
    done
    AC_MSG_RESULT([${have_xray_plugin_[]_AC_LANG_ABBREV[]_support}])

    _AC_CC[]=$save_CC
    _AC_LANG_PREFIX[]FLAGS=$save_CFLAGS
    AS_UNSET([save_CC])
    AS_UNSET([save_CFLAGS])
])

# _CLEAN_LLVM_PLUGIN
# ------------------
#
m4_define([_CLEAN_LLVM_PLUGIN], [
AC_LANG_PUSH([C++])
AS_IF([test "x${can_compile_xray_plugin}" = "xyes"],
    [plugin_uninstall='$SHELL ./libtool --mode=uninstall $RM $PWD/lib/confmodule.la >&AS_MESSAGE_LOG_FD'
     _AC_DO_VAR([plugin_uninstall])
     plugin_rmdir='rmdir lib >&AS_MESSAGE_LOG_FD'
     _AC_DO_VAR([plugin_rmdir])
     plugin_clean='$SHELL ./libtool --mode=clean $RM conftest.lo confmodule.la >&AS_MESSAGE_LOG_FD'
     _AC_DO_VAR([plugin_clean])])
$RM conftest.$ac_ext
AS_UNSET([plugin_clean])
AS_UNSET([plugin_uninstall])
AS_UNSET([plugin_rmdir])
AC_LANG_POP([C++])
])


# _INPUT_XRAY_PLUGIN
# ------------------
#
m4_define([_INPUT_XRAY_PLUGIN], [[
#include <iostream>
#include <stdio.h>
#include <xray/xray_interface.h>

int main() {
    __xray_init();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
]])

# _INPUT_XRAY_TEST_C
# ------------------
#
# Do not include xray header here as it causes problems with c compilers
# XRay plugin itself is built with clang++ / a cpp compiler anyways
m4_define([_INPUT_XRAY_TEST_C], [[
#include <stdio.h>
int main( void )
{
    FILE *file = fopen("xray_plugin_supported_c", "w");
    fprintf(file, "supported");
    fclose(file);
    return 0;
}
]])

# _INPUT_XRAY_TEST_CXX
# ------------------
#
m4_define([_INPUT_XRAY_TEST_CXX], [[
#include <stdio.h>
#include <xray/xray_interface.h>
int main( void )
{
    FILE *file = fopen("xray_plugin_supported_cxx", "w");
    fprintf(file, "supported");
    fclose(file);
    return 0;
}
]])

# _INPUT_XRAY_TEST_FC
# ------------------
#
m4_define([_INPUT_XRAY_TEST_FC], [[
      PROGRAM main
          open(10, file="xray_plugin_supported_fc")
          write(10, *) "hello world"
          close(10)
      END
]])
