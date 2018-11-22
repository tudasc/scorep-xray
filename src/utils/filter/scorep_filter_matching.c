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
 * Copyright (c) 2009-2013, 2016,
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


/**
 * @file
 *
 *
 * Maintains the internal filter rules and performs the pattern matching requests.
 * We maintain two rule lists: One for the function name rules and one for the
 * file name rules. Due to the possible include/exclude
 * combinations, the rules must be evaluated in sequential order. Thus, the use of a
 * single linked list it sufficient.
 */

#include <config.h>
#include "scorep_filter_matching.h"

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <SCOREP_Filter.h>

#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>


/* **************************************************************************************
   Variable and type definitions
****************************************************************************************/

/**
 * The struct definition for the rule list. Each record contains one rule.
 * The @ref scorep_filter_rule_t defines a type for this struct.
 */
struct scorep_filter_rule_struct
{
    char*                 pattern;    /**< Pointer to the pattern string */
    bool                  is_mangled; /**< Apply this rule on the mangled name */
    bool                  is_exclude; /**< True if it is a exclude rule, false else */
    scorep_filter_rule_t* next;       /**< Next filter rule */
};

/* **************************************************************************************
   Rule representation manipulation functions
****************************************************************************************/

SCOREP_ErrorCode
scorep_filter_add_rule( scorep_filter_rule_t*** tail,
                        const char*             rule,
                        bool                    is_exclude,
                        bool                    is_mangled )
{
    if ( !rule || !rule[ 0 ] )
    {
        return SCOREP_ERROR_INVALID_ARGUMENT;
    }

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = calloc( 1, sizeof( *new_rule ) );
    if ( new_rule == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern    = UTILS_CStr_dup( rule );
    new_rule->is_mangled = is_mangled;
    new_rule->is_exclude = is_exclude;
    new_rule->next       = NULL;

    **tail = new_rule;
    *tail  = &new_rule->next;

    return SCOREP_SUCCESS;
}

void
scorep_filter_free_rules( scorep_filter_rule_t* head )
{
    /* Free function rules. */
    while ( head )
    {
        scorep_filter_rule_t* next = head->next;

        free( head->pattern );
        free( head );
        head = next;
    }
}



/* **************************************************************************************
   Matching requests
****************************************************************************************/

static bool
scorep_filter_match_file_rule( const char*                 with_path,
                               const scorep_filter_rule_t* rule,
                               SCOREP_ErrorCode*           err )
{
    int error_value = fnmatch( rule->pattern, with_path, 0 );

    *err = SCOREP_SUCCESS;
    if ( error_value == 0 )
    {
        return true;
    }
    else if ( error_value != FNM_NOMATCH )
    {
        *err = UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Error in pattern matching during evaluation of filter rules"
                            "with file '%s' and pattern '%s'. Disable filtering",
                            with_path, rule->pattern );
        return false;
    }
    return false;
}

bool
scorep_filter_match_file( const scorep_filter_rule_t* fileRules,
                          const char*                 fileName,
                          SCOREP_ErrorCode*           err )
{
    bool excluded = false;                      /* Start with all included */
    *err = SCOREP_SUCCESS;

    if ( fileName )
    {
        while ( fileRules )
        {
            /* If included so far and we have an exclude rule */
            if ( ( !excluded ) && fileRules->is_exclude )
            {
                excluded = scorep_filter_match_file_rule( fileName,
                                                          fileRules,
                                                          err );
                if ( *err != SCOREP_SUCCESS )
                {
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !fileRules->is_exclude ) )
            {
                excluded = !scorep_filter_match_file_rule( fileName,
                                                           fileRules,
                                                           err );

                if ( *err != SCOREP_SUCCESS )
                {
                    return false;
                }
            }

            fileRules = fileRules->next;
        }
    }

    if ( excluded )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                            "Filtered file %s\n", fileName );
    }

    return excluded;
}

static bool
scorep_filter_match_function_rule( const char*                 functionName,
                                   const char*                 mangledName,
                                   const scorep_filter_rule_t* rule,
                                   SCOREP_ErrorCode*           err )
{
    int error_value = 0;
    if ( rule->is_mangled && ( mangledName != NULL ) )
    {
        error_value = fnmatch( rule->pattern, mangledName, 0 );
    }
    else
    {
        error_value = fnmatch( rule->pattern, functionName, 0 );
    }

    *err = SCOREP_SUCCESS;
    if ( error_value == 0 )
    {
        return true;
    }
    else if ( error_value != FNM_NOMATCH )
    {
        *err = UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Error in pattern matching during evaluation of filter rules"
                            "with file '%s' and pattern '%s'. Disable filtering",
                            functionName, rule->pattern );
        return false;
    }
    return false;
}

bool
scorep_filter_match_function( const scorep_filter_rule_t* functionRules,
                              const char*                 functionName,
                              const char*                 mangledName,
                              SCOREP_ErrorCode*           err )
{
    bool excluded = false;                      /* Start with all included */
    *err = SCOREP_SUCCESS;

    if ( functionName )
    {
        while ( functionRules )
        {
            /* If included so far and we have an exclude rule */
            if ( ( !excluded ) && functionRules->is_exclude )
            {
                excluded = scorep_filter_match_function_rule( functionName,
                                                              mangledName,
                                                              functionRules,
                                                              err );

                if ( *err != SCOREP_SUCCESS )
                {
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !functionRules->is_exclude ) )
            {
                excluded = !scorep_filter_match_function_rule( functionName,
                                                               mangledName,
                                                               functionRules,
                                                               err );

                if ( *err != SCOREP_SUCCESS )
                {
                    return false;
                }
            }

            functionRules = functionRules->next;
        }
    }

    if ( excluded )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                            "Filtered function %s\n", functionName );
    }

    return excluded;
}

bool
scorep_filter_include_function( const scorep_filter_rule_t* functionRules,
                                const char*                 functionName,
                                const char*                 mangledName,
                                SCOREP_ErrorCode*           err )
{
    bool excluded            = false;           /* Start with all included */
    bool explicitly_included = false;
    *err = SCOREP_SUCCESS;

    if ( !functionName )
    {
        return true;
    }

    while ( functionRules )
    {
        bool matched = scorep_filter_match_function_rule( functionName,
                                                          mangledName,
                                                          functionRules,
                                                          err );

        if ( matched )
        {
            if ( functionRules->is_exclude )
            {
                explicitly_included = false;
            }
            else if ( 0 != strcmp( functionRules->pattern, "*" ) )
            {
                explicitly_included = true;
            }
        }

        /* If included so far and we have an exclude rule */
        if ( ( !excluded ) && functionRules->is_exclude )
        {
            excluded = matched;

            if ( *err != SCOREP_SUCCESS )
            {
                return true;
            }
        }
        /* If excluded so far and we have an include rule */
        else if ( excluded && ( !functionRules->is_exclude ) )
        {
            excluded = !matched;

            if ( *err != SCOREP_SUCCESS )
            {
                return true;
            }
        }

        functionRules = functionRules->next;
    }

    if ( excluded )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                            "Filtered function %s\n", functionName );
    }

    return !excluded && explicitly_included;
}
