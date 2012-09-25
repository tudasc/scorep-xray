/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_FILTER_MATCHING_H
#define SCOREP_FILTER_MATCHING_H

/**
 * @file       scorep_filter_matching.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Defines the types for the internal representation of the filter rules and its
 * manipulation functions.
 */

#include <stdbool.h>
#include <UTILS_Error.h>

/**
 * Adds a filter rule to the file filter rule list.
 * @param rule       The rule's string pattern.
 * @param is_exclude True if it is an exclude rule, false otherwise.
 */
SCOREP_ErrorCode
scorep_filter_add_file_rule( const char* rule,
                             bool        is_exclude );

/**
 * Adds a filter rule to the function filter rule list.
 * @param rule       The rule's string pattern.
 * @param is_exclude True if it is an exclude rule, false otherwise.
 * @param is_fortran True if Fortran mangling should be applied to the rule.
 */
SCOREP_ErrorCode
scorep_filter_add_function_rule( const char* rule,
                                 bool        is_exclude,
                                 bool        is_fortran );

/**
 * Frees memory allocations for filter rules.
 */
void
scorep_filter_free_rules();

#endif /* SCOREP_FILTER_MATCHING_H */
