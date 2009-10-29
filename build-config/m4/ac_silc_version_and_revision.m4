## -*- mode: autoconf -*-

AC_DEFUN([AC_SILC_VERSION_AND_REVISION],
[
    echo $PACKAGE_VERSION > $srcdir/build-config/VERSION

    silc_revision=`svnversion $srcdir`
    which svnversion > /dev/null; \
    if test $? -eq 0 -a "x$silc_revision" != "xexported"; then
        echo $silc_revision > $srcdir/build-config/REVISION
    fi

    if grep -E [[A-Z]] $srcdir/build-config/REVISION > /dev/null || \
       grep ":" $srcdir/build-config/REVISION > /dev/null; then                                      
        AC_MSG_WARN([distribution does not match a single, unmodified revision, but $silc_revision.])
    fi
])
