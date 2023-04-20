## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2023,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

# AFS_LT_RPATHS()
# ---------------
#
# Define filenames for a Makefile snippet and a config-header, both
# containing rpathing information. The Makefile snippet gets generated
# by config.status via AC_CONFIG_COMMANDS. The config-header contains
# AFS_PACKAGE_BUILD_name in it's name to allow usage of several
# config-headers at once (backend, backend-mpi, ...).
#
# Intended to be used in conjunction with Makefile-rpaths.inc.am.
#
AC_DEFUN_ONCE([AFS_LT_RPATHS], [
AC_REQUIRE([AFS_CPU_INSTRUCTION_SETS])
m4_define([GEN_RPATHS_MAKEFILE], [Makefile.rpaths])
m4_define([GEN_EXTERNAL_LIBS_HPP], AFS_PACKAGE_TO_TOP[src/config-external-libs-]AFS_PACKAGE_BUILD_name[.hpp])

AC_SUBST([RPATHS_MAKEFILE], GEN_RPATHS_MAKEFILE)
AC_SUBST([CONFIG_EXTERNAL_LIBS_HPP], GEN_EXTERNAL_LIBS_HPP)
AC_SUBST([CONFIG_EXTERNAL_LIBS_HPP_INCLUDE_GUARD], [CONFIG_EXTERNAL_LIBS_]AFS_PACKAGE_BUILD_NAME[_HPP])

# Use @INLCUDE_RPATHS_MAKEFILE@ in Makefile.am to prevent automake-time
# include but do make-time include instead. Already done in
# Makefile-rpaths.inc.am.
AC_SUBST([INLCUDE_RPATHS_MAKEFILE], "-include ./GEN_RPATHS_MAKEFILE")

AC_CONFIG_COMMANDS(GEN_RPATHS_MAKEFILE, [make ]GEN_RPATHS_MAKEFILE)

m4_undefine([GEN_RPATHS_MAKEFILE])
m4_undefine([GEN_EXTERNAL_LIBS_HPP])
]) # AFS_LT_RPATHS
