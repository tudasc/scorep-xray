## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_libsion.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

# output of sionconfig on jugene:
# sionconfig --be --ser --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -D_SION_BGP
# sionconfig --be --ser --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionser_32
# sionconfig --be --ser --path
# /usr/local/sionlib/v1.2p2
# sionconfig --be --mpi --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -DSION_MPI -D_SION_BGP
# sionconfig --be --mpi --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsion_32 -lsionser_32
# sionconfig --be --mpi --path
# /usr/local/sionlib/v1.2p2
# sionconfig --fe --ser --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -D_SION_BGP
# sionconfig --fe --ser --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionserfe_32
# sionconfig --fe --ser --path
# /usr/local/sionlib/v1.2p2
# sionconfig --fe --mpi --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -DSION_MPI -D_SION_BGP
# sionconfig --fe --mpi --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionfe_32 -lsionserfe_32
# sionconfig --fe --mpi --path
# /usr/local/sionlib/v1.2p2


AC_DEFUN([AC_SCOREP_LIBSION],
[
# make SIONCONFIG precious as we use it in AC_CHECK_PROG
AC_ARG_VAR([SIONCONFIG], [Absolute path to sionconfig, including "sionconfig".])

AC_ARG_WITH([sionconfig],
            [AS_HELP_STRING([--with-sionconfig=(yes|no|<path-to-sionconfig>)], 
                            [Whether to use sionconfig and where to find it. "yes" assumes it is in PATH [yes].])],
            # action-if-given
            [AS_CASE([$withval],
                     ["yes"], [scorep_with_sionconfig="yes"],
                     ["no"],  [scorep_with_sionconfig="no"],
                     [scorep_with_sionconfig="$withval"])],
            # action-if-not-given
            [scorep_with_sionconfig="yes"])

#echo "debug: scorep_with_sionconfig=${scorep_with_sionconfig}"

# macro-internal variables
scorep_sion_cppflags=""
scorep_sion_ldflags=""
scorep_sion_libs=""
scorep_have_sion="no"

if test "x${scorep_with_sionconfig}" != "xno"; then
    if test "x${scorep_with_sionconfig}" = "xyes"; then
        AC_CHECK_PROG([SIONCONFIG], [sionconfig], [`which sionconfig`], ["no"])
    else
        AC_CHECK_PROG([SIONCONFIG], [sionconfig], [${scorep_with_sionconfig}/sionconfig], ["no"], [${scorep_with_sionconfig}])
    fi

    if test "x${SIONCONFIG}" != "xno"; then
        scorep_have_sion="yes"

        sionconfig_febe_flag=""
        if test "x${ac_scorep_backend}" = "xyes"; then
            sionconfig_febe_flag="--be"
        elif test "x${ac_scorep_frontend}" = "xyes"; then
            sionconfig_febe_flag="--fe"
        fi

        # this needs to be discussed as it add a MPI dependency to OTF2
        sionconfig_sermpi_flag="--mpi"

        scorep_sion_cppflags=`$SIONCONFIG $sionconfig_febe_flag $sionconfig_sermpi_flag --cflags`
        scorep_sion_ldflags=`$SIONCONFIG $sionconfig_febe_flag $sionconfig_sermpi_flag --libs | \
                             awk '{for (i=1; i<=NF; i++) {if (index($i, "-L") == 1){ldflags = ldflags " " $i}}}END{print ldflags}'`
        scorep_sion_libs=`$SIONCONFIG $sionconfig_febe_flag $sionconfig_sermpi_flag --libs | \
                          awk '{for (i=1; i<=NF; i++) {if (index($i, "-l") == 1){libs = libs " " $i}}}END{print libs}'`

        #echo "debug: scorep_sion_cppflags=$scorep_sion_cppflags"
        #echo "debug: scorep_sion_ldflags=$scorep_sion_ldflags"
        #echo "debug: scorep_sion_libs=$scorep_sion_libs"

        # maybe add links checks, but how to do this with MPI?
    
    fi
fi

# The output of this macro
AC_SUBST([SCOREP_SION_CPPFLAGS], [$scorep_sion_cppflags])
AC_SUBST([SCOREP_SION_LDFLAGS],  [$scorep_sion_ldflags])
AC_SUBST([SCOREP_SION_LIBS],     [$scorep_sion_libs])
if test "x${scorep_have_sion}" = "xyes"; then
    AC_DEFINE([HAVE_SION], [1], [Defined if libsion is available.])
else
    AC_DEFINE([HAVE_SION], [0], [Defined if libsion is available.])
fi
AM_CONDITIONAL([HAVE_SION], [test "x${scorep_have_sion}" = "xyes"])

]) 
