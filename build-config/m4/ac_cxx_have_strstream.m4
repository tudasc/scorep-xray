# ===========================================================================
#          http://autoconf-archive.cryp.to/ac_cxx_have_sstream.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_CXX_HAVE_SSTREAM
#
# DESCRIPTION
#
#   If the C++ library has a working stringstream, define HAVE_SSTREAM.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Ben Stanley <Ben.Stanley@exemail.com.au>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

# 2009-04-27 17:05:34 +0200 Adapted for strstream by 
# Christian Roessel <c.roessel@fz-juelich.de>

AC_DEFUN([AC_CXX_HAVE_STRSTREAM],
[AC_CACHE_CHECK(whether the compiler has strstream,
ac_cv_cxx_have_strstream,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <strstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[strstream message; message << "Hello"; return 0;],
 ac_cv_cxx_have_strstream=yes, ac_cv_cxx_have_strstream=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_strstream" = yes; then
  AC_DEFINE(HAVE_STRSTREAM,,[define if the compiler has strstream])
fi
])
