
AC_DEFUN([AC_SCOREP_LINK_FLAGS_CHECK], [
 _AC_SCOREP_LINK_FLAG_CHECK([-Bstatic], [HAVE_LINK_FLAG_BSTATIC])
 _AC_SCOREP_LINK_FLAG_CHECK([-Bdynamic], [HAVE_LINK_FLAG_BDYNAMIC])
 _AC_SCOREP_LINK_FLAG_CHECK([-shared], [HAVE_LINK_FLAG_SHARED])
 _AC_SCOREP_LINK_FLAG_CHECK([-dynamiclib], [HAVE_LINK_FLAG_DYNAMICLIB])
 _AC_SCOREP_LINK_FLAG_CHECK([-qmkshrobj], [HAVE_LINK_FLAG_QMKSHROBJ])
 _AC_SCOREP_LINK_FLAG_CHECK([-G], [HAVE_LINK_FLAG_G_FOR_SHARED])
])

dnl ---------------------------------------------------------------------------


# _AC_SCOREP_LINK_FLAG_CHECK([compiler flag],[variable name])
#________________________________________________________________________
AC_DEFUN([_AC_SCOREP_LINK_FLAG_CHECK], [

my_save_cflags="$CFLAGS"
CFLAGS="$1"
AC_MSG_CHECKING([whether CC supports $1])
AC_LINK_IFELSE([_AC_SCOREP_HELLO_WORLD_PROGRAM],
    [AC_MSG_RESULT([yes])
     result="1"],
    [AC_MSG_RESULT([no])
     result="0"]
)
AC_SUBST([$2],[$result])
CFLAGS="$my_save_cflags"
])

dnl ---------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_HELLO_WORLD_PROGRAM], [
AC_LANG_PROGRAM([[
#include <stdio.h>
]], [[
int main()
{
    printf("hello world!\n");
}
]])
])
