/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_TYPES_HPP
#define SCOREP_CONFIG_TYPES_HPP

/**
 * @file
 *
 * Defines types that are used in multiple classes in the config tool.
 */

typedef enum
{
    SCOREP_CONFIG_LANGUAGE_C,
    SCOREP_CONFIG_LANGUAGE_CXX,
    SCOREP_CONFIG_LANGUAGE_FORTRAN
} SCOREP_Config_Language;

#endif // SCOREP_CONFIG_TYPES_HPP
