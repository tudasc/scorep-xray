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


AC_DEFUN([_AC_SCOREP_LIBPHMPP_LIB_CHECK],[

with_libphmpp_lib_checks_successful="yes"
with_libphmpp_libs="-lphmpp -lffi"

phmpp_ldflags_save=${LDFLAGS}
LDFLAGS="${LDFLAGS} -L${with_libphmpp_lib}/../slib"
AC_CHECK_LIB([phmpp],
             [phmpp_get_code_property],
             [with_libphmpp_ldflags="${with_libphmpp_ldflags} -L${with_libphmpp_lib}/../slib"],
             [with_libphmpp_lib_checks_successful="no"
              with_libphmpp_libs=""],
             [-lffi])
LDFLAGS=${phmpp_ldflags_save}

])

AC_DEFUN([AC_SCOREP_HMPP], [
AC_SCOREP_BACKEND_LIB([libphmpp], [phmpp/phmpp.h])


dnl phmpp_code_t*
dnl phmpp_hardware_t*
dnl phmpp_context_t*
dnl phmpp_event_t*
dnl phmpp_event_state_t
dnl phmpp_event_kind_t

dnl phmpp_get_code_property
dnl phmpp_get_hardware_property
dnl phmpp_get_error_message
dnl phmpp_get_event_code
dnl phmpp_get_code_file_name
dnl phmpp_get_code_line_number
dnl phmpp_get_parent_hardware
dnl phmpp_get_event_hardware
dnl phmpp_get_hardware_name
dnl phmpp_get_first_child_hardware
dnl phmpp_get_next_sibling_hardware
dnl phmpp_get_first_child_context
dnl phmpp_get_next_sibling_context
dnl phmpp_get_parent_context
dnl phmpp_get_event_context
dnl phmpp_get_context_name
dnl phmpp_get_context_property
dnl phmpp_get_event_state
dnl phmpp_get_event_state_name
dnl phmpp_get_event_property
dnl phmpp_get_event_kind
])
