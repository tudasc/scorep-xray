dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2023,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

# SCOREP_CHECK_SECTION_ATTRIBUTE()
# --------------------------------
# Checks if C compiler can compile source code with section attribute and if the sections shows up in the
# compiled object. The result is saved in a variable called scorep_section_attribute_support.
# Requires objdump to be present. Uses the libtool check to know if objdump is present.
#
AC_DEFUN([SCOREP_CHECK_SECTION_ATTRIBUTE], [
AC_CHECK_TOOL([OBJDUMP], [objdump], [false])
AC_REQUIRE([AC_PROG_GREP])
scorep_section_attribute_support=no

AS_CASE([$host_os],
    [darwin*], [SCOREP_CHECK_SECTION_ATTRIBUTE_MACOS],
    [linux*], [SCOREP_CHECK_SECTION_ATTRIBUTE_LINUX],
    [SCOREP_CHECK_SECTION_ATTRIBUTE_LINUX])

AC_MSG_CHECKING([if compiler supports section attribute])
AC_MSG_RESULT([${scorep_section_attribute_support}])
])

# SCOREP_CHECK_SECTION_ATTRIBUTE_MACOS()
# --------------------------------
# macOS version for the section attribute check. Will get called
# by the generic version. Here, we check two additional tools available
# either through LLVM or Xcode. While we cannot check the variable name,
# we are able to check the existence of the section.
#
m4_define([SCOREP_CHECK_SECTION_ATTRIBUTE_MACOS], [
AC_CHECK_TOOL([OTOOL], [otool], [false])
AC_CHECK_TOOL([OTOOL64], [otool64], [false])

tools="$OBJDUMP $OTOOL $OTOOL64"
for tool in $tools; do
    AS_IF([test "x${tool}" != "xfalse"],
        [AC_LANG_PUSH([C])
         AC_COMPILE_IFELSE([AC_LANG_SOURCE(_INPUT_SCOREP_CHECK_SECTION_ATTRIBUTE)],
         [AS_CASE(["$tool"],
              ["$OBJDUMP"], [$tool --disassemble --section __scorep conftest.o | GREP_OPTIONS= ${GREP} -q scorep_test_variable],
              [$tool -s __SCOREP __scorep conftest.o | GREP_OPTIONS= ${GREP} -q "Contents of (__SCOREP,__scorep) section"])
          AS_IF([test $? = 0],
              [scorep_section_attribute_support=yes
               break])])
         AC_LANG_POP([C])])
done
])

# SCOREP_CHECK_SECTION_ATTRIBUTE_LINUX()
# --------------------------------
# Linux version for the section attribute check.
# Checks with objdump if the section and variable exists
#
m4_define([SCOREP_CHECK_SECTION_ATTRIBUTE_LINUX], [
AS_IF([test "x${OBJDUMP}" != "xfalse"],
    [AC_LANG_PUSH([C])
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([_INPUT_SCOREP_CHECK_SECTION_ATTRIBUTE])],
     [AS_IF([${OBJDUMP} --disassemble --section scorep.test.section conftest.o | GREP_OPTIONS= ${GREP} -q scorep_test_variable],
          [scorep_section_attribute_support=yes])])
     AC_LANG_POP([C])])
])

# _INPUT_SCOREP_CHECK_SECTION_ATTRIBUTE
# --------------------------------
# Input C source code for _CHECK_SECTION_ATTRIBUTE
#
m4_define([_INPUT_SCOREP_CHECK_SECTION_ATTRIBUTE], [[
#ifdef __APPLE__
int scorep_test_variable __attribute__ ((section("__SCOREP,__scorep"))) = 0;
#else
int scorep_test_variable __attribute__ ((section("scorep.test.section"))) = 0;
#endif

int main( void )
{
    return scorep_test_variable;
}
]])
