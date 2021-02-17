## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2021,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_fortran.m4

dnl ------------------------------------------------------------------

# Workaround for GCC 10 argument mismatch errors:
# GCC 10 now rejects argument mismatches occurring in the same source
# file, see #9. This affects jacobi MPI tests that use MPI_GET_ADDRESS on
# Cray EX (does it affect all MPICH-based MPIs? Open MPI seems to work
# OK). Porting the jacobi code would be a solution. Replacing it with
# a conforming code would be another. As both can't be implemented in
# time for 7.0, downgrade the error to a warning via
# -fallow-argument-mismatch.
AC_DEFUN([SCOREP_FORTRAN_ARGUMENT_MISMATCH], [
for fcflag in "" "-fallow-argument-mismatch"; do
    FCFLAGS_save="$FCFLAGS"
    FCFLAGS="$fcflag $FCFLAGS"
    _FORTRAN_HAVE_ARGUMENT_MISMATCH
    FCFLAGS="$FCFLAGS_save"
    AS_IF([test "x${have_argument_mismatch_error}" = xno],
        [AC_SUBST([SCOREP_FORTRAN_ALLOW_ARGUMENT_MISMATCH], ["$fcflag"])
         break])
done
]) # SCOREP_FORTRAN_ARGUMENT_MISMATCH

AC_DEFUN([_FORTRAN_HAVE_ARGUMENT_MISMATCH], [
# see https://gcc.gnu.org/gcc-10/porting_to.html#argument-mismatch
AC_LANG_PUSH([Fortran])
AC_COMPILE_IFELSE([
    AC_LANG_SOURCE(
[[
      subroutine sub_assumed(array, n)
        real array(*)
        integer n
      end

      program arg_mismatch
        real var
        call sub_assumed(var, 1)
      end
]])],
    [have_argument_mismatch_error=no],
    [have_argument_mismatch_error=yes])
AC_LANG_POP([Fortran])
]) # _FORTRAN_HAVE_ARGUMENT_MISMATCH
