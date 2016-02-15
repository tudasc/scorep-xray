/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_FILTER_H
#define SCOREP_FILTER_H

/**
 * @file       SCOREP_Filter.h
 *
 *
 * The interface to the filtering system.  The file that contains the filter
 * definitions can be set via the envirionment variable
 * SCOREP_FILTERING_FILE. If no file is specified or the given file is not
 * found, filtering is disabled.
 */

#include <SCOREP_ErrorCodes.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

UTILS_BEGIN_C_DECLS

SCOREP_ErrorCode
SCOREP_Filter_ParseFile( const char* file_name );

/**
 * Frees the memory allocated to store filter rules. After calling this
 * function, no filter rules are available anymore.
 */
void
SCOREP_Filter_FreeRules( void );

/**
 * Enables filtering.
 */
void
SCOREP_Filter_Enable( void );

/**
 * Disables filtering.
 */
void
SCOREP_Filter_Disable( void );

/**
 * Returns true if filtering is enabled.
 */
bool
SCOREP_Filter_IsEnabled( void );

/**
 * Checks whether a function is filtered.
 * @param filename      The file name of the file that contain the considered function.
 * @param function_name The function name as it will be displayed in profile and
 *                      trace definitions.
 * @param mangled_name  A mangled name of the function is available that differs
 *                      from @a function_name. If no different mangled name is
 *                      available you may pass NULL here. In this case the function_name
 *                      will be used to compare against patterns that are prepended
 *                      by the MANGLED keyword. In particular, passing NULL or the
 *                      same string as for @a function_name leads to the same results.
 * @returns True, if the tested function should be excluded from measurement.
 */
bool
SCOREP_Filter_Match( const char* file_name,
                     const char* function_name,
                     const char* mangled_name );

/**
 * Checks whether a file is filtered.
 * @param filename      The file name of the file that is checked.
 * @returns True, if functions of the tested file should be excluded from measurement.
 */
bool
SCOREP_Filter_MatchFile( const char* file_name );

/**
 * Checks whether a function is filtered.
 * @param function_name The function name as it will be displayed in profile and
 *                      trace definitions.
 * @param mangled_name  A mangled name of the function is available that differs
 *                      from @a function_name. If no different mangled name is
 *                      available you may pass NULL here. In this case the function_name
 *                      will be used to compare against patterns that are prepended
 *                      by the MANGLED keyword. In praticular passing NULL or the
 *                      same string as for @a function_name leads to the same results.
 * @returns True, if the tested function should be excluded from measurement.
 */
bool
SCOREP_Filter_MatchFunction( const char* function_name,
                             const char* mangled_name );

/**
 * Adds a filter rule to the function filter rule list. Because some adapters apply
 * filter restrictions during initilization, the rules should be added during
 * subsystem registration.
 * @param rule       The rule's string pattern.
 * @param is_exclude True if it is an exclude rule, false if it is an include rule.
 * @param is_mangled True if the mangled name should be used instead of the displayed
 *                   name. If a match request does not provide a mangled name, the
 *                   rule is matched against the displayed name. If you pass false,
 *                   Then it uses always the displayed name.
 */
SCOREP_ErrorCode
SCOREP_Filter_AddFunctionRule( const char* rule,
                               bool        is_exclude,
                               bool        is_mangled );


UTILS_END_C_DECLS

#endif /* SCOREP_FILTER_H */
