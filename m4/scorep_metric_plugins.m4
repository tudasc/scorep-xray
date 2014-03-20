## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_sampling.m4


AC_DEFUN([AC_SCOREP_METRIC_PLUGINS], [

dnl Don't check for prerequisite of metric plugins on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for metric plugins on frontend.])])

AC_LANG_PUSH([C])

has_metric_plugins_headers="yes"
AC_CHECK_HEADERS([dlfcn.h],
                 [],
                 [has_metric_plugins_headers=no])

libs_save="$LIBS"

has_metric_plugins_functions="yes"
AC_CHECK_LIB([dl],[dlopen],[],[has_metric_plugins_functions="no"])
AC_CHECK_LIB([dl],[dlerror],[],[has_metric_plugins_functions="no"])
AC_CHECK_LIB([dl],[dlsym],[],[has_metric_plugins_functions="no"])
AC_CHECK_LIB([dl],[dlclose],[],[has_metric_plugins_functions="no"])

AS_IF([test "x${has_metric_plugins_functions}" = "xyes"],
      [ac_scorep_with_metric_plugins_libs="dl"])
LIBS="$libs_save"

AC_LANG_POP([C])

# generating output
AS_IF([   test "x${has_metric_plugins_headers}" = "xyes" \
       && test "x${has_metric_plugins_functions}" = "xyes"],
      [has_metric_plugins="yes"
       metric_plugins_summary="yes, using -l${ac_scorep_with_metric_plugins_libs}"],
      [has_metric_plugins="no"
       metric_plugins_summary="no"])

AC_SUBST([SCOREP_PLUGIN_METRICS_LIBS], [$ac_scorep_with_metric_plugins_libs])

AC_SCOREP_COND_HAVE([METRIC_PLUGINS],
                    [test "x${has_metric_plugins}" = "xyes"],
                    [Defined if metric plugins support is available.],
                    [AC_SUBST([SCOREP_METRIC_PLUGINS_LIBS], ["-l${ac_scorep_with_metric_plugins_libs}"])],
                    [AC_SUBST([SCOREP_METRIC_PLUGINS_LIBS], [""])])

AFS_SUMMARY([metric plugins support], [${metric_plugins_summary}])
])
