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
 * @file
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

struct SCOREP_Filter
{
    scorep_filter_rule_t*  file_rules;
    scorep_filter_rule_t** file_rules_tail;
    scorep_filter_rule_t*  function_rules;
    scorep_filter_rule_t** function_rules_tail;
};

/**
 * Adds a filter rule to the file filter rule list.
 * @param rule       The rule's string pattern.
 * @param is_exclude True if it is an exclude rule, false otherwise.
 */
SCOREP_ErrorCode
scorep_filter_add_rule( scorep_filter_rule_t*** tail,
                        const char*             rule,
                        bool                    isExclude,
                        bool                    isMangled );

/**
 * Frees memory allocations for filter rules.
 */
void
scorep_filter_free_rules( scorep_filter_rule_t* head );

bool
scorep_filter_match_file( const scorep_filter_rule_t* fileRules,
                          const char*                 fileName,
                          SCOREP_ErrorCode*           err );

bool
scorep_filter_match_function( const scorep_filter_rule_t* functionRules,
                              const char*                 functionName,
                              const char*                 mangledName,
                              SCOREP_ErrorCode*           err );

bool
scorep_filter_include_function( const scorep_filter_rule_t* functionRules,
                                const char*                 functionName,
                                const char*                 mangledName,
                                SCOREP_ErrorCode*           err );

#endif /* SCOREP_FILTER_MATCHING_H */
