/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INFO_HPP
#define SCOREP_INFO_HPP

/**
 * @file
 *
 * Globals
 */

#include <string>

/**
   Contains the name of the tool for help output
 */
extern const std::string toolname;

/* *INDENT-OFF* */
/* see https://bixense.com/clicolors/ for the use of CLICOLOR */
#define PAGER_COMMAND \
    "( " \
        "test -t 1; " \
        "istty=$?; " \
        "if { { test \"${CLICOLOR-1}\" != \"0\" && test $istty -eq 0; } || test \"${CLICOLOR_FORCE:-0}\" != \"0\"; }; " \
        "then " \
           "cat; " \
        "else " \
           "sed -e 's/'\"$(printf \"\033\")\"'\\[[0-9;]*m//g'; " \
        "fi | if test $istty -eq 0 && test -n \"${PAGER:=$(which less || :)}\"; " \
        "then " \
           "LESS=${LESS:-FRX} ${PAGER}; " \
        "else " \
           "cat; " \
        "fi " \
    ")"
/* *INDENT-ON* */

#endif // SCOREP_INFO_HPP
