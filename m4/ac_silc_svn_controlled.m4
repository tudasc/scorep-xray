AC_DEFUN([AC_SILC_SVN_CONTROLLED],
[
    ac_silc_svn_controlled="no"
    if test -d $srcdir/.svn; then
        ac_silc_svn_controlled="yes"
    fi
    AM_CONDITIONAL(SVN_CONTROLLED, test "x${ac_silc_svn_controlled}" = xyes)
])
