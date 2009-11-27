## -*- mode: autoconf -*-

AC_DEFUN([AC_SILC_VERSION_AND_REVISION],
[
    # When working with a svn checkout, write a VERSION and a REVISION
    # file. The REVISION file is updated during each configure call and also
    # at make doxygen-user and make dist. 

    # When working with a make-dist-generated tarball, VERSION and REVISION
    # are already there.

    if test ! -f $srcdir/build-config/VERSION; then
        echo $PACKAGE_VERSION > $srcdir/build-config/VERSION
    fi 

    silc_revision="invalid"
    which svnversion > /dev/null; \
    if test $? -eq 0; then
        silc_revision=`svnversion $srcdir`
        if test "x$silc_revision" != "xexported"; then
            echo $silc_revision > $srcdir/build-config/REVISION
        fi
    fi

    if grep -E [[A-Z]] $srcdir/build-config/REVISION > /dev/null || \
       grep ":" $srcdir/build-config/REVISION > /dev/null; then
        silc_revision=`cat $srcdir/build-config/REVISION`
        AC_MSG_WARN([distribution does not match a single, unmodified revision, but $silc_revision.])
    fi
])
