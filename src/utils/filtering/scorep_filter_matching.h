/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_FILTER_MATCHING_H
#define SCOREP_FILTER_MATCHING_H

/**
 * @file       scorep_filter_matching.h
 *
 *
 * Defines the types for the internal representation of the filter rules and its
 * manipulation functions.
 */

#include <stdbool.h>
#include <SCOREP_ErrorCodes.h>

/**
 * Type which is used to store the filter rules.
 */
typedef struct scorep_filter_rule_struct scorep_filter_rule_t;

/**
 * Adds a filter rule to the file filter rule list.
 * @param rule       The rule's string pattern.
 * @param is_exclude True if it is an exclude rule, false otherwise.
 */
SCOREP_ErrorCode
scorep_filter_add_file_rule( const char* rule,
                             bool        is_exclude );

/**
 * Frees memory allocations for filter rules.
 */
void
scorep_filter_free_rules( void );

/**
 * If the adapters or the system wants to exclude some functions, this exclusion
 * should not be affected by user rules, e.g. an INCLUDE * (include everything) at
 * the beginning. Thus, we must append system rules at the end. On the other hand,
 * The filter file is read during initialization, while the filter rules should be
 * added during registration. Thus, when parsing the user filter file, the parser
 * should store the old list and append it after adding the files again.
 * This function returns and removes the currently stored functions rules. They should
 * be reenabled afterwars with scorep_filter_end_user_rules.
 * @param function_rule_head Memory location where we can store the pointer to the list
 *                           head.
 * @param function_rule_tail Memory location where we can store the pointer to the list
 *                           tail.
 */
void
scorep_filter_start_user_rules( scorep_filter_rule_t** function_rule_head,
                                scorep_filter_rule_t** function_rule_tail );

/**
 * Restores the system filter rules after the user rules were added.
 * @param function_rule_head Pointer to the head of the system rule list.
 * @param function_rule_tail Pointer to the tail of the system rule list.
 */
void
scorep_filter_end_user_rules( scorep_filter_rule_t* function_rule_head,
                              scorep_filter_rule_t* function_rule_tail );


#endif /* SCOREP_FILTER_MATCHING_H */
