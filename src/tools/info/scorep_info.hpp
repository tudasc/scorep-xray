/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
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
#define PAGER_COMMAND \
    "if test -t 1; " \
    "then " \
        "${PAGER-$(type less >/dev/null 2>&1 && echo less || echo cat)}; " \
    "else " \
        "cat; " \
    "fi"
/* *INDENT-ON* */

#endif // SCOREP_INFO_HPP
