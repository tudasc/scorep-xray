## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2021,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_addr2line.m4


# SCOREP_ADDR2LINE()
# ------------------
# Check for required and optional features to be able to implement
# addr2line lookup. Required features are the availability of libbfd
# and the ability to iterate over shared objects (using
# dl_iterate_phdr). Being able to honor dlopened/dlcosed shared
# objects for addr2line lookup is considered optional.
#
AC_DEFUN_ONCE([SCOREP_ADDR2LINE_SUMMARY], [
AC_REQUIRE([SCOREP_ADDR2LINE])
AFS_SUMMARY_PUSH
AFS_SUMMARY([libbfd found], [$libbfd_summary])
AFS_SUMMARY([dl_iterate_phdr], [$scorep_have_dl_iterate_phdr])
AS_IF([test "x${scorep_have_ldaudit_api}" = xno],
     [AS_IF([test "x${enable_shared}" = xyes],
          [rtld_audit_summary="no, API not supported"],
          [rtld_audit_summary="no, needs --enable-shared"])],
     [test "x${scorep_have_ldaudit_needs_env_var}" = xyes],
     [rtld_audit_summary="yes, via LD_AUDIT enviroment variable"],
     [rtld_audit_summary="yes, via --audit instrumentation or LD_AUDIT environment variable"])
AFS_SUMMARY([rtld-audit], [$rtld_audit_summary])
AFS_SUMMARY_POP([addr2line support], [$scorep_have_addr2line])
]) # SCOREP_ADDR2LINE_SUMMARY


# SCOREP_ADDR2LINE()
# ------------------------
# Delegate of SCOREP_ADDR2LINE to be able to AC_REQUIRE it without
# forcing summary output.
#
AC_DEFUN([SCOREP_ADDR2LINE], [
AC_REQUIRE([_SCOREP_LIBBFD])dnl
AC_REQUIRE([_SCOREP_ITERATE_SHARED_OBJECTS])dnl
AS_IF([test "x${CPU_INSTRUCTION_SET}" = xunknown],
    [AC_MSG_ERROR([Addr2line requires instruction set te be either ppc64, x86_64, or aarch64, but 'unknown' detected.])])
# libbfd is already a hard requirement for configure to
# succeed. dl_iterate_phdr is essential for addr2line support,
# thus communicate via (allow override for testing):
scorep_have_addr2line="${scorep_have_addr2line:-$scorep_have_dl_iterate_phdr}"
AFS_AM_CONDITIONAL([HAVE_SCOREP_ADDR2LINE], [test "x${scorep_have_addr2line}" = xyes], [false])
AC_DEFINE_UNQUOTED([HAVE_SCOREP_ADDR2LINE],
    $(if test "x${scorep_have_addr2line}" = xyes; then echo 1; else echo 0; fi),
    [If addr2line is supported])
AC_REQUIRE([_SCOREP_LDAUDIT_API])dnl
AC_REQUIRE([_SCOREP_LDAUDIT_ENV_VAR])dnl
]) # SCOREP_ADDR2LINE


# _SCOREP_ITERATE_SHARED_OBJECTS()
# --------------------------------
# Check whether it is possible to iterate at runtime over the shared
# objects of an application. This is a hard requirement for the
# addr2line feature.
#
# This macro requires link.h to be available. According to
# https://www.gnu.org/software/gnulib/manual/html_node/link_002eh.html,
# this is not the case for macOS 11.1, AIX 7.1, HP-UX 11.31, IRIX 6.5,
# Cygwin 2.9.0, mingw, MSVC 14. It is also not available on macOS
# 11.6.
# The same requirement and portabilty issues apply to
# _SCOREP_LDAUDIT_API.
#
AC_DEFUN([_SCOREP_ITERATE_SHARED_OBJECTS], [
AC_LANG_PUSH([C])
scorep_have_dl_iterate_phdr=no
libs_save="$LIBS"
LIBS=""
cppflags_save="$CPPFLAGS"
CPPFLAGS=""
for scorep_dl_iterate_phdr_cppflags in "" "-D_GNU_SOURCE"; do
    CPPFLAGS="${scorep_dl_iterate_phdr_cppflags}"
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([[
/* fails for docker_pgi-17.9
 * PGC-S-0040-Illegal use of symbol, __int128_t (/usr/include/x86_64-linux-gnu/bits/link.h: 99) ...
 */
#include <link.h>
#include <stdint.h>
int relevant( const char* name, uintptr_t addr )
{
    return addr > 42;
}
int cb( struct dl_phdr_info* info, size_t unused, void* cnt )
{
    int i = info->dlpi_phnum - 1;
    uintptr_t addr = ( uintptr_t )( info->dlpi_addr + info->dlpi_phdr[ i ].p_vaddr + info->dlpi_phdr[ i ].p_memsz );
    if ( !relevant( info->dlpi_name, addr ) && info->dlpi_phdr[ i ].p_type != PT_LOAD )
    {
        return info->dlpi_phdr[ i ].p_flags == ( PF_X | PF_R );
    }
    ++*( int* )cnt;
    return 0;
}
int main()
{
    int cnt;
    dl_iterate_phdr( cb, &cnt );
    return cnt;
}
]])],
        [scorep_have_dl_iterate_phdr=yes])
    AS_IF([test "x${scorep_have_dl_iterate_phdr}" = xyes ], [break])
done
LIBS="$libs_save"
CPPFLAGS="$cppflags_save"
AC_LANG_POP([C])
AC_MSG_CHECKING([whether dl_iterate_phdr is supported])
AS_IF([test "x${scorep_have_dl_iterate_phdr}" = xno],
    [AC_MSG_RESULT([${scorep_have_dl_iterate_phdr}])
     AC_MSG_WARN([dl_iterate_phdr required for addr2line lookup, but not available])],
    [needs=""
     AS_IF([test "x${scorep_dl_iterate_phdr_cppflags}" != x],
         [needs=", needs ${scorep_dl_iterate_phdr_cppflags}"])
     AC_MSG_RESULT([${scorep_have_dl_iterate_phdr}${needs}])
     AC_SUBST([SCOREP_DL_ITERATE_PHDR_CPPFLAGS], [${scorep_dl_iterate_phdr_cppflags}])])
]) # _SCOREP_ITERATE_SHARED_OBJECTS


# _SCOREP_LDAUDIT_API()
# ---------------------
# Check whether the rtdl-audit API can be used and if it is possible
# to communicate with the audited application by opening a
# corresponding shared object in the application's namespace. This
# feature allows for taking dlopened shared objects into account for
# addr2line lookup.
#
# See _SCOREP_ITERATE_SHARED_OBJECTS for requirements and portability
# issues of link.h.
#
AC_DEFUN([_SCOREP_LDAUDIT_API], [
AC_REQUIRE([SCOREP_CHECK_DLFCN])dnl
AC_REQUIRE([_SCOREP_ITERATE_SHARED_OBJECTS])dnl
scorep_have_ldaudit_api=no
scorep_ldaudit_cppflags=""
scorep_ldaudit_libs=""
AS_IF([test "x${scorep_have_dl_iterate_phdr}" = xyes && test "x${enable_shared}" = xyes],
    [# audit library needs to be shared
     AC_LANG_PUSH([C])
     AS_IF([test "x${scorep_have_dlfcn_support}" = xyes], [
         libs_save="$LIBS"
         scorep_ldaudit_libs="-ldl"
         LIBS="$scorep_ldaudit_libs"
         cppflags_save="$CPPFLAGS"
         CPPFLAGS=""
         for scorep_ldaudit_cppflags in "" "-D_GNU_SOURCE"; do
             CPPFLAGS="${scorep_ldaudit_cppflags}"
             AC_LINK_IFELSE(
                 [AC_LANG_SOURCE([[
#include <dlfcn.h>
#include <link.h>
unsigned int la_version(unsigned int version)
{
    return LAV_CURRENT;
}
static void *scorep_so;
static void (*scorep_la_preinit)( uintptr_t* cookie );
static void (*scorep_la_objopen)( const char* name, uintptr_t* cookie );
static void (*scorep_la_objclose)( uintptr_t* cookie );
void la_preinit( uintptr_t* cookie )
{
    scorep_so = dlmopen( LM_ID_BASE, "./libscorep_measurement.so", RTLD_LAZY );
    scorep_la_preinit = dlsym( scorep_so, "scorep_la_preinit" );
    scorep_la_objopen = dlsym( scorep_so, "scorep_la_objopen" );
    scorep_la_objclose = dlsym( scorep_so, "scorep_la_objclose" );
    scorep_la_preinit( cookie );
}
unsigned int la_objopen( struct link_map *map, Lmid_t lmid, uintptr_t *cookie )
{
    scorep_la_objopen( map->l_name, cookie );
    return 0;
}
unsigned int la_objclose( uintptr_t* cookie )
{
    scorep_la_objclose( cookie );
    return 0;
}
int main()
{
    la_version( 42 );
    uintptr_t cookie = 42;
    la_preinit( &cookie );
    struct link_map map;
    la_objopen( &map, LM_ID_BASE, &cookie );
    la_objclose( &cookie );
    return 0;
}]])],
                 [scorep_have_ldaudit_api=yes])
             AS_IF([test "x${scorep_have_ldaudit_api}" = xyes ], [break])
         done
         LIBS="$libs_save"
         CPPFLAGS="$cppflags_save"])
     AC_LANG_POP([C])])
AC_MSG_CHECKING([whether rtld-audit API is supported])
needs=""
AS_IF([test "x${scorep_ldaudit_cppflags}" != x],
    [needs=", needs ${scorep_ldaudit_cppflags}"])
AC_MSG_RESULT([${scorep_have_ldaudit_api}${needs}])
AC_SUBST([SCOREP_LDAUDIT_CPPFLAGS], [${scorep_ldaudit_cppflags}])
AC_SUBST([SCOREP_LDAUDIT_LIBS], [${scorep_ldaudit_libs}])
AFS_AM_CONDITIONAL([HAVE_SCOREP_LDAUDIT_API], [test "x${scorep_have_ldaudit_api}" = xyes], [false])
]) # _SCOREP_LDAUDIT_API


# _SCOREP_LDAUDIT_ENV_VAR()
# -------------------------
# Determine if linker auditing can be activated at link time (i.e.,
# instrumentation time) via the link flags --audit or --depaudit or if
# the environment variable LD_AUDIT is required to activate auditing
# at load time (i.e., in the job script).
#
AC_DEFUN([_SCOREP_LDAUDIT_ENV_VAR], [
AC_REQUIRE([_SCOREP_GLIBC_VERSION])dnl
# glibc >= 2.32 honors AUDIT/DEPAUDIT entries inserted via the linkers
# --audit/--depaudit option. If --audit is used at
# libscorep_measurement link time or --depaudit at instrumentation
# link time, an audit library gets loaded at program startup. For
# older glibc versions, the audit library needs to be provided via the
# LD_AUDIT environment variable.
scorep_have_ldaudit_needs_env_var=yes
AS_IF([test "x${scorep_have_ldaudit_api}" = xyes],
    [AS_IF([test "$scorep_glibc_major" -ge 2 && test "$scorep_glibc_minor" -ge 32],
         [scorep_have_ldaudit_needs_env_var=no])
     AC_MSG_CHECKING([whether rtld-audit needs LD_AUDIT environment variable])
     AC_MSG_RESULT([$scorep_have_ldaudit_needs_env_var (glibc ${scorep_glibc_major}.${scorep_glibc_minor})])],
    [scorep_have_ldaudit_needs_env_var="does not apply"])
AC_SUBST([SCOREP_HAVE_INSTRUMENTATION_TIME_LDAUDIT],
    $(if test "x${scorep_have_ldaudit_needs_env_var}" = xno; then echo 1; else echo 0; fi))
]) # _SCOREP_LDAUDIR_ENV_VAR


# _SCOREP_GLIBC_VERSION()
# -----------------------
# Set scorep_glibc_major and scorep_glibc_minor to glibc's major and
# minor version number or to 'unknown', if version can't be
# determined.
#
AC_DEFUN([_SCOREP_GLIBC_VERSION], [
dnl see https://sourceforge.net/p/predef/wiki/Libraries/
AC_COMPUTE_INT([scorep_glibc_major], [__GLIBC__], [#include <limits.h>], [scorep_glibc_major=unknown])
AS_IF([test "xscorep_glibc_major" != xunknown],
    [AC_COMPUTE_INT([scorep_glibc_minor], [__GLIBC_MINOR__], [#include <limits.h>], [scorep_glibc_minor=unknown])],
    [scorep_glibc_minor=unknown])
]) # _SCOREP_GLIBC_VERSION
