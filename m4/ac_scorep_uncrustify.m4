AC_DEFUN([AC_SCOREP_UNCRUSTIFY],
[
AC_CHECK_PROG([ac_scorep_have_uncrustify], [uncrustify], [yes], [no],,)
AM_CONDITIONAL([HAVE_UNCRUSTIFY], [test "x${ac_scorep_have_uncrustify}" = "xyes"])
])
