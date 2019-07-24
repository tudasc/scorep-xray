
AC_DEFUN([AC_SCOREP_LINK_FLAGS_CHECK], [
 _AC_SCOREP_LINK_FLAG_CHECK([-Bstatic], [HAVE_LINK_FLAG_BSTATIC])
 _AC_SCOREP_LINK_FLAG_CHECK([-Bdynamic], [HAVE_LINK_FLAG_BDYNAMIC])
 _AC_SCOREP_LINK_FLAG_CHECK([-Wl,-start-group -Wl,-end-group], [HAVE_LINKER_START_END_GROUP])
])

dnl ---------------------------------------------------------------------------


# _AC_SCOREP_LINK_FLAG_CHECK([compiler flag],[variable name])
#________________________________________________________________________
AC_DEFUN([_AC_SCOREP_LINK_FLAG_CHECK], [

my_save_ldflags="$LDFLAGS"
LDFLAGS="$LDFLAGS $1"
AC_MSG_CHECKING([whether LD supports $1])
AC_LINK_IFELSE([_AC_SCOREP_HELLO_WORLD_PROGRAM],
    [AC_MSG_RESULT([yes])
     result="1"],
    [AC_MSG_RESULT([no])
     result="0"]
)
AC_SUBST([$2],[$result])
LDFLAGS="$my_save_ldflags"
])

dnl ---------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_HELLO_WORLD_PROGRAM], [
AC_LANG_PROGRAM([[
#include <stdio.h>
]], [[
    printf("hello world!\n");
]])
])
