AC_DEFUN([AC_SILC_UNCRUSTIFY],
[
AC_CHECK_PROG([ac_silc_have_uncrustify], [uncrustify], [yes], [no],,)
AM_CONDITIONAL([HAVE_UNCRUSTIFY], [test "x${ac_silc_have_uncrustify}" = "xyes"])
])
