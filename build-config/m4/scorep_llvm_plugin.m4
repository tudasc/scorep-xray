dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2017,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2023,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

# SCOREP_LLVM_PLUGIN_SUMMARY()
# ----------------------------
# Reports summary about the LLVM plugin configuration.
# Checks if LLVM plugin is requested by the user and calls
# SCOREP_LLVM_PLUGIN the value is not set to no.
#
# Reported features:
# - Support for LLVM plugin
# - If demangling from LLVM is used for plugin
#
AC_DEFUN_ONCE([SCOREP_LLVM_PLUGIN_SUMMARY], [
AC_REQUIRE([SCOREP_LLVM_PLUGIN])

AC_SCOREP_COND_HAVE([LLVM_PLUGIN_SUPPORT],
    [test "x${have_llvm_plugin_support}" = "xyes"],
    [Defined if LLVM plugin can be compiled and enabled for at least one language])
AC_SCOREP_COND_HAVE([LLVM_DEMANGLE],
    [test "x${have_llvm_demangle}" = "xyes"],
    [Defined if LLVM demangle headers are found])
AM_COND_IF([HAVE_LLVM_PLUGIN_SUPPORT],
    [AFS_PROG_CXX_SUMMARY
     AFS_COMMON_UTILS([build], [<SCOREP_ErrorCodes.h>], [../src/utils/include])
     AFS_SUMMARY([LLVM demangle], [$have_llvm_demangle])],
    [AS_IF([test "x${enable_llvm_plugin}" = "xyes"],
        [AC_MSG_ERROR([Request to enable the LLVM plug-in could not be fulfilled. Run configure with --disable-llvm-plugin or run without --enable-llvm-plugin. Reason: $llvm_plugin_reason])])])
AFS_SUMMARY([LLVM plugin support], [$have_llvm_plugin_support${llvm_plugin_reason:+, $llvm_plugin_reason}])
])


# SCOREP_LLVM_PLUGIN()
# -------------------------------------------------------------------
# Tests if LLVM plugin can be enabled and used for selected compiler
#
# These tests are conducted:
# - Are we in a cross-compile environment?
# - Is the compiler a Clang based compiler?
#   (sets have_llvm_compiler)
# - Is llvm-config available?
# - Are required LLVM IR headers present?
#   (sets have_llvm_plugin_header)
# - Can a example plugin be compiled?
#   (sets have_llvm_plugin_library)
# - Can the plugin be enabled for C, C++ and Fortran
#   (writes llvm_plugin_supported_[c|cxx|fc] if successful)
#
# If all tests are successful, required flags are
# written to SCOREP_LLVM_PLUGIN_[CXXFLAGS|CPPFLAGS|LDFLAGS|LIBS].
# If not, the reason can be extracted from llvm_plugin_reason.
#
AC_DEFUN([SCOREP_LLVM_PLUGIN], [
AC_REQUIRE([AC_PROG_SED])
AC_ARG_ENABLE([llvm-plugin],
    [AS_HELP_STRING([--disable-llvm-plugin],
        [Disable support for the LLVM plug-in instrumentation. Default is to determine support automatically. This disables it by request and fails if support cannot be satisfied but was requested.])],
    [enable_llvm_plugin="${enableval}"],
    [enable_llvm_plugin="not_given"])

have_llvm_compiler="no"
have_llvm_plugin_header="no"
have_llvm_plugin_library="no"

have_llvm_plugin_c_support="no"
have_llvm_plugin_cxx_support="no"
have_llvm_plugin_fc_support="no"

have_llvm_plugin_support="no"
have_llvm_demangle="no"

AS_IF([test "x${cross_compiling}" = "xyes"],
    [AS_CASE([${ac_scorep_platform}],
         [crayx*],
             [], # Cray machines might report being a cross-compile environment even though they're not. Try to build the plug-in anyway.
         [enable_llvm_plugin="no"
          llvm_plugin_reason=${llvm_plugin_reason:-"not supported in cross-compile mode"}])])

AS_IF([test "x${enable_llvm_plugin}" != "xno"],
     [_CHECK_CXX_COMPILER_VENDOR
      AS_IF([test "x${have_llvm_compiler}" = "xyes"],
          [SCOREP_LLVM_CONFIG
           AS_IF([test "x${scorep_have_llvm_config}" = "xyes"],
               [LLVM_PLUGIN_TARGET_CXXFLAGS="${LLVM_PLUGIN_TARGET_CXXFLAGS} $(${scorep_llvm_config_bin} --cxxflags | ${SED} -E 's/ -std=[[a-zA-Z]]+.*[[0-9]]{2} //')"
                LLVM_PLUGIN_TARGET_CPPFLAGS="${LLVM_PLUGIN_TARGET_CPPFLAGS} $(${scorep_llvm_config_bin} --cppflags)"
                LLVM_PLUGIN_TARGET_LDFLAGS="${LLVM_PLUGIN_TARGET_LDFLAGS} $(${scorep_llvm_config_bin} --ldflags)"
                LLVM_PLUGIN_TARGET_LIBS="${LLVM_PLUGIN_TARGET_LIBS} $(${scorep_llvm_config_bin} --libs demangle support)"

                _CHECK_LLVM_PLUGIN_HEADERS
                AS_IF([test "x${have_llvm_plugin_header}" = "xyes"],
                    [_CHECK_LLVM_DEMANGLE_SUPPORT
                     AC_LANG_PUSH([C++])
                     _BUILD_LLVM_PLUGIN
                     AC_LANG_POP([C++])
                     AS_IF([test "x${have_llvm_plugin_library}" = "xyes"],
                         [AC_LANG_PUSH([C])
                          _TEST_ENABLE_LLVM_PLUGIN
                          AC_LANG_POP([C])
                          AC_LANG_PUSH([C++])
                          _TEST_ENABLE_LLVM_PLUGIN
                          AC_LANG_POP([C++])
                          AC_LANG_PUSH([Fortran])
                          _TEST_ENABLE_LLVM_PLUGIN
                          AC_LANG_POP([Fortran])
                          AS_IF([test "x${have_llvm_plugin_support}" = "xno"], [llvm_plugin_reason="could not enable plugin"])],
                          [llvm_plugin_reason="could not build plugin"])
                     _CLEAN_LLVM_PLUGIN],
                    [llvm_plugin_reason="required headers missing"])],
                [llvm_plugin_reason="${scorep_llvm_config_reason}"])],
           [llvm_plugin_reason="only LLVM based compilers supported"])],
     [llvm_plugin_reason=${llvm_plugin_reason:-"explicitly disabled via --disable-llvm-plugin"}])

    AS_IF([test "x${have_llvm_plugin_support}" = "xyes"],
        [AC_SUBST(SCOREP_LLVM_PLUGIN_CXXFLAGS, ["${LLVM_PLUGIN_TARGET_CXXFLAGS}"])
         AC_SUBST(SCOREP_LLVM_PLUGIN_CPPFLAGS, ["${LLVM_PLUGIN_TARGET_CPPFLAGS}"])
         AC_SUBST(SCOREP_LLVM_PLUGIN_LDFLAGS, ["${LLVM_PLUGIN_TARGET_LDFLAGS}"])
         AC_SUBST(SCOREP_LLVM_PLUGIN_LIBS, ["${LLVM_PLUGIN_TARGET_LIBS}"])])
dnl
]) dnl SCOREP_LLVM_PLUGIN


# _CHECK_CXX_COMPILER_VENDOR
# ----------------------
# Checks for a given C++ compiler if the compiler vendor matches
# clang or intel/oneapi
#
m4_define([_CHECK_CXX_COMPILER_VENDOR], [
AS_UNSET([ax_cv_cxx_compiler_vendor])
save_cxx=$CXX
save_CXXFLAGS=$CXXFLAGS
CXX=$LLVM_PLUGIN_TARGET_CXX
CXXFLAGS=$LLVM_PLUGIN_TARGET_CXXFLAGS
AC_LANG_PUSH([C++])
AX_COMPILER_VENDOR
AC_LANG_POP([C++])
llvm_plugin_target_vendor_cxx="${ax_cv_cxx_compiler_vendor}"
CXX=$save_cxx
CXXFLAGS=$save_CXXFLAGS

AS_IF([test "x${llvm_plugin_target_vendor_cxx}" = "xclang" ||
       test "x${llvm_plugin_target_vendor_cxx}" = "xintel/oneapi"],
    [have_llvm_compiler="yes"])
])

# _CHECK_LLVM_PLUGIN_HEADERS
# --------------------------
#
m4_define([_CHECK_LLVM_PLUGIN_HEADERS], [
have_llvm_plugin_header="yes"
AC_LANG_PUSH([C++])
save_CXXFLAGS=$CXXFLAGS
save_CPPFLAGS=$CPPFLAGS
CXXFLAGS="$LLVM_PLUGIN_TARGET_CXXFLAGS"
CPPFLAGS="$LLVM_PLUGIN_TARGET_CPPFLAGS"
AC_CHECK_HEADERS([llvm/IR/IRBuilder.h llvm/IR/InstIterator.h llvm/Pass.h llvm/Passes/PassBuilder.h llvm/Passes/PassPlugin.h llvm/Support/CommandLine.h llvm/Support/ErrorHandling.h llvm/Transforms/Utils/BasicBlockUtils.h],
                 [],
                 [have_llvm_plugin_header="no"])
CXXFLAGS=$save_CXXFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_LANG_POP([C++])
])

# _CHECK_LLVM_DEMANGLE_SUPPORT
# ----------------------------
#
m4_define([_CHECK_LLVM_DEMANGLE_SUPPORT], [
have_llvm_demangle="yes"
AC_LANG_PUSH([C++])
save_CXXFLAGS=$CXXFLAGS
save_CPPFLAGS=$CPPFLAGS
CXXFLAGS="$LLVM_PLUGIN_TARGET_CXXFLAGS"
CPPFLAGS="$LLVM_PLUGIN_TARGET_CPPFLAGS"
AC_CHECK_HEADER([llvm/Demangle/Demangle.h],
    [],
    [have_llvm_demangle="no, compile-time filtering only works with mangled names"])
AS_IF([test "x${have_llvm_demangle}" = "xyes"],
    [AC_MSG_CHECKING([whether llvm::demangle can be linked])
     cross_compiling_save="${cross_compiling}"
     AS_IF([test "x${afs_platform_cray}" = "xyes"],
         [cross_compiling="no"])
     save_LDFLAGS=$LDFLAGS
     LDFLAGS="$LLVM_PLUGIN_TARGET_LDFLAGS $LLVM_PLUGIN_TARGET_LIBS"
     AC_RUN_IFELSE([AC_LANG_SOURCE(_INPUT_LLVM_DEMANGLE)],
         [have_llvm_demangle="yes"],
         [have_llvm_demangle="no, compile-time filtering only works with mangled names"],
         [# In cross compiling environments, assume no.
          # The LLVM plug-in will use Score-P demangling instead.
          have_llvm_demangle="no, compile-time filtering only works with mangled names"])
     LDFLAGS=$save_LDFLAGS
     cross_compiling="${cross_compiling_save}"
     AC_MSG_RESULT([$have_llvm_demangle])])
CXXFLAGS=$save_CXXFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_LANG_POP([C++])
])


# _BUILD_LLVM_PLUGIN
# ------------------
#
m4_define([_BUILD_LLVM_PLUGIN], [
AC_REQUIRE([LT_OUTPUT])

AC_LANG_PUSH([C++])
plugin_compile='$SHELL ./libtool --mode=compile --tag=_AC_CC [$]_AC_CC $LLVM_PLUGIN_TARGET_CXXFLAGS [$]_AC_LANG_PREFIX[FLAGS] -c -o conftest.lo conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
plugin_link='$SHELL ./libtool --mode=link --tag=_AC_CC [$]_AC_CC [$]_AC_LANG_PREFIX[FLAGS] -module -avoid-version $LLVM_PLUGIN_TARGET_LDFLAGS $LLVM_PLUGIN_TARGET_LIBS -rpath $PWD/lib -o confmodule.la conftest.lo >&AS_MESSAGE_LOG_FD'
plugin_mkdir='$MKDIR_P lib >&AS_MESSAGE_LOG_FD'
plugin_install='$SHELL ./libtool --mode=install $INSTALL confmodule.la $PWD/lib/confmodule.la >&AS_MESSAGE_LOG_FD'

AC_MSG_CHECKING([whether LLVM example plugin can compile])
AC_LANG_CONFTEST([AC_LANG_SOURCE(_INPUT_LLVM_PLUGIN)])
AS_IF([_AC_DO_VAR([plugin_compile]) &&
       _AC_DO_VAR([plugin_link]) &&
       _AC_DO_VAR([plugin_mkdir]) &&
       _AC_DO_VAR([plugin_install])],
    [have_llvm_plugin_library="yes"],
    [have_llvm_plugin_library="no"])
AC_MSG_RESULT([${have_llvm_plugin_library}])
AC_LANG_POP([C++])

AS_UNSET([plugin_compile])
AS_UNSET([plugin_link])
AS_UNSET([plugin_mkdir])
AS_UNSET([plugin_install])
])

# _TEST_ENABLE_LLVM_PLUGIN
# ------------------------
#
m4_define([_TEST_ENABLE_LLVM_PLUGIN], [
save_CC=$[]_AC_CC
save_CFLAGS=$[]_AC_LANG_PREFIX[]FLAGS
_AC_CC[]=$LLVM_PLUGIN_TARGET_[]_AC_CC

AC_MSG_CHECKING([whether $[]_AC_CC compiler plug-in can be loaded])
for compiler_backend_flag_arg in "-Xclang" "-Xflang"; do
    _AC_LANG_PREFIX[]FLAGS="-fpass-plugin=$PWD/lib/confmodule.so $compiler_backend_flag_arg -load $compiler_backend_flag_arg $PWD/lib/confmodule.so -mllvm -lang=[]_AC_LANG_ABBREV[]"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE(_INPUT_LLVM_TEST_[]_AC_LANG_PREFIX)],
        [AS_IF([test -f llvm_plugin_supported_[]_AC_LANG_ABBREV[]],
            [have_llvm_plugin_support="yes"
             have_llvm_plugin_[]_AC_LANG_ABBREV[]_support="yes"
             AC_SUBST(SCOREP_LLVM_PLUGIN_COMPILER_BACKEND_FLAG_[]_AC_CC[], ["${compiler_backend_flag_arg}"])
             break],
            [have_llvm_plugin_[]_AC_LANG_ABBREV[]_support="no"])],
        [have_llvm_plugin_[]_AC_LANG_ABBREV[]_support="no"])
done
AC_MSG_RESULT([${have_llvm_plugin_[]_AC_LANG_ABBREV[]_support}])

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
AS_IF([test "x${have_llvm_plugin_library}" = "xyes"],
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

# _INPUT_LLVM_DEMANGLE
# --------------------
#
m4_define([_INPUT_LLVM_DEMANGLE], [[
#undef PIC
#include <llvm/Demangle/Demangle.h>

int main( int argc, char** argv )
{
    llvm::demangle( "main" );
    return 0;
}
]])

# _INPUT_LLVM_PLUGIN
# ------------------
#
m4_define([_INPUT_LLVM_PLUGIN], [[
#undef PIC
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <fstream>

using namespace llvm;

static cl::opt<std::string> Language( "lang", cl::init( "" ),
                           cl::desc( "language to create file for if plugin ran" ) );

namespace
{
bool
runFunction( Function& F )
{
    errs().write_escaped( F.getName() ) << '\n';
    return false;
}

struct ConfigureTest : PassInfoMixin<ConfigureTest>
{
    PreservedAnalyses
    run( Module& M, ModuleAnalysisManager& MAM )
    {
        std::ofstream file( "llvm_plugin_supported_" + Language );
        file.close();
        bool preserved = true;
        for ( auto& F : M )
        {
            if( !runFunction( F ) )
            {
                preserved = false;
            }
        }
        return preserved ? PreservedAnalyses::all() : PreservedAnalyses::none();
    }
};
} // namespace

llvm::PassPluginLibraryInfo
getConfigureTestPluginInfo()
{
    return { LLVM_PLUGIN_API_VERSION, "ConfigureTest", LLVM_VERSION_STRING,
             @<:@ @:>@( PassBuilder& PB ) {
#if not __has_include( "llvm/Passes/OptimizationLevel.h" )
                 using OptimizationLevel = PassBuilder::OptimizationLevel;
#endif
                 PB.registerOptimizerLastEPCallback(
                     @<:@ @:>@( llvm::ModulePassManager& PM, OptimizationLevel Level ) {
                PM.addPass( ConfigureTest() );
            } );
                 PB.registerPipelineParsingCallback(
                     @<:@ @:>@( StringRef Name, llvm::ModulePassManager& PM,
                          ArrayRef<llvm::PassBuilder::PipelineElement>) {
                if ( Name == "configure" )
                {
                    PM.addPass( ConfigureTest() );
                    return true;
                }
                return false;
            } );
             } };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return getConfigureTestPluginInfo();
}

]])

# _INPUT_LLVM_TEST_C
# ------------------
#
m4_define([_INPUT_LLVM_TEST_C], [[
int main( void )
{
    return 0;
}
]])

# _INPUT_LLVM_TEST_CXX
# ------------------
#
m4_copy([_INPUT_LLVM_TEST_C], [_INPUT_LLVM_TEST_CXX])

# _INPUT_LLVM_TEST_FC
# ------------------
#
m4_define([_INPUT_LLVM_TEST_FC], [[
      PROGRAM main

      END
]])
