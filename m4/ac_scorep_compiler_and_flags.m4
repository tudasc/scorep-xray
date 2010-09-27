AC_DEFUN([AC_SCOREP_CONVERT_FLAGS],
[
if test "x${ac_cv_env_[$1]_FOR_BUILD_set}" != "xset"; then
   # don't use the backend flags if nothing is specified for the frontend
   unset [$1]
else
   # use the frontend flags 
   [$1]="$ac_cv_env_[$1]_FOR_BUILD_value"
fi
## ac_substituting done automagically 
##AC_SUBST([$1])
])


AC_DEFUN([AC_SCOREP_CHECK_COMPILER_VAR_SET],
[
if test "x${ac_cv_env_[$1]_set}" != "xset"; then
    AC_MSG_ERROR([argument $1 not provided in configure call.], [1])
fi
])


AC_DEFUN([AC_SCOREP_CONVERT_COMPILER],
[
if test "x${ac_cv_env_[$1]_FOR_BUILD_set}" != "xset"; then
    AC_MSG_ERROR([argument $1_FOR_BUILD not provided in configure call.], [1])    
fi
[$1]="$ac_cv_env_[$1]_FOR_BUILD_value"
])
