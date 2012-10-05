## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


AC_DEFUN([AC_SCOREP_PDT], [

## Evalute parameters
AC_ARG_WITH([pdt], 
            [AS_HELP_STRING([--with-pdt=<path-to-binaries>], 
                            [Specifies the path to the program database toolkit (PDT) binaries, e.g., cparse.])],

            [provided_pdt_path=$withval$PATH_SEPARATOR$PATH],
            [provided_pdt_path=$PATH])

## Check for pdt programs
for pdt_prog in cparse cxxparse gfparse tau_instrumentor; do
    AC_PATH_PROG([have_pdt],
                 [${pdt_prog}],
                 ["no"],
                 [${provided_pdt_path}])
    AS_IF([test "x${have_pdt}" = "xno"] , [real_pdt_path=""; break])
    # Assume that if all pdt_progs are found, they are located in the same directory
    real_pdt_path=`AS_DIRNAME([${have_pdt}])`
    AS_UNSET([have_pdt])
    AS_UNSET([ac_cv_path_have_pdt])
done

## Temporarily disable PDT on BlueGene systems until PDT bug is fixed,
## see scorep:ticket:508
AS_IF([(test "x${ac_scorep_platform}" = "xbgp") || (test "x${ac_scorep_platform}" = "xbgq") || (test "x${ac_scorep_platform}" = "xbgl")], 
      [have_pdt="no"
       AC_MSG_WARN([PDT on BlueGene platforms currently not supported.])
       AC_SCOREP_SUMMARY([PDT support], [on BlueGene platforms currently not available.])])

## Create output
AC_SUBST([PDT_PATH], ["${real_pdt_path}"])
AM_CONDITIONAL([HAVE_PDT], [test "x${have_pdt}" != "xno"])
AS_IF([test "x${have_pdt}" != "xno"],
      [AC_SCOREP_SUMMARY([PDT support], [yes, using binaries in ${real_pdt_path}])],
      [AC_SCOREP_SUMMARY([PDT support], [no])])
])
