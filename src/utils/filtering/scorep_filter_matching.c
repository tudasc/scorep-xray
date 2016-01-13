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
 * @file       scorep_filter_matching.c
 *
 *
 * Maintains the internal filter rules and performs the pattern machcing requests.
 * We maintain two rule lists: One for the function name rules and one for the
 * file name rules. Due to the possible include/exclude
 * combinations, the rules must be evaluated in sequential order. Thus, the use of a
 * single linked list it sufficient.
 */

#include <config.h>
#include <scorep_filter_matching.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <SCOREP_Filter.h>

#include <fnmatch.h>
#include <stdlib.h>
#include <assert.h>


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

/**
 * Pointer to the head of the filter rules for source file names.
 */
static scorep_filter_rule_t* scorep_filter_file_rules_head = NULL;

/**
 * Pointer to the tail of the filter rules for source file names.
 */
static scorep_filter_rule_t* scorep_filter_file_rules_tail = NULL;

/**
 * Pointer to the head of the filter rules for function names.
 */
static scorep_filter_rule_t* scorep_filter_function_rules_head = NULL;

/**
 * Pointer to the tail of the filter rules for function names.
 */
static scorep_filter_rule_t* scorep_filter_function_rules_tail = NULL;

/* **************************************************************************************
   Rule representation manipulation functions
****************************************************************************************/

void
scorep_filter_start_user_rules( scorep_filter_rule_t** function_rule_head,
                                scorep_filter_rule_t** function_rule_tail )
{
    *function_rule_head               = scorep_filter_function_rules_head;
    *function_rule_tail               = scorep_filter_function_rules_tail;
    scorep_filter_function_rules_head = NULL;
    scorep_filter_function_rules_tail = NULL;
}

void
scorep_filter_end_user_rules( scorep_filter_rule_t* function_rule_head,
                              scorep_filter_rule_t* function_rule_tail )
{
    if ( function_rule_tail == NULL )
    {
        return;
    }

    /* Append list */
    if ( scorep_filter_function_rules_tail == NULL )  /* First entry */
    {
        scorep_filter_function_rules_head = function_rule_head;
    }
    else
    {
        scorep_filter_function_rules_tail->next = function_rule_head;
    }
    scorep_filter_function_rules_tail = function_rule_tail;
}

SCOREP_ErrorCode
scorep_filter_add_file_rule( const char* rule, bool is_exclude )
{
    assert( rule );
    assert( *rule != '\0' );

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = ( scorep_filter_rule_t* )
                                     malloc( sizeof( scorep_filter_rule_t ) );

    if ( new_rule == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern    = UTILS_CStr_dup( rule );
    new_rule->is_mangled = false;
    new_rule->is_exclude = is_exclude;
    new_rule->next       = NULL;

    /* Insert entry in list */
    if ( scorep_filter_file_rules_tail == NULL )  /* First entry */
    {
        scorep_filter_file_rules_head = new_rule;
        scorep_filter_file_rules_tail = new_rule;
    }
    else /* Append at the end */
    {
        scorep_filter_file_rules_tail->next = new_rule;
        scorep_filter_file_rules_tail       = new_rule;
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
SCOREP_Filter_AddFunctionRule( const char* rule, bool is_exclude, bool is_mangled )
{
    assert( rule );
    assert( *rule != '\0' );

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = ( scorep_filter_rule_t* )
                                     malloc( sizeof( scorep_filter_rule_t ) );

    if ( new_rule == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern    = UTILS_CStr_dup( rule );
    new_rule->is_mangled = is_mangled;
    new_rule->is_exclude = is_exclude;
    new_rule->next       = NULL;

    /* Insert entry in list */
    if ( scorep_filter_function_rules_tail == NULL )  /* First entry */
    {
        scorep_filter_function_rules_head = new_rule;
        scorep_filter_function_rules_tail = new_rule;
    }
    else /* Append at the end */
    {
        scorep_filter_function_rules_tail->next = new_rule;
        scorep_filter_function_rules_tail       = new_rule;
    }
    return SCOREP_SUCCESS;
}

void
SCOREP_Filter_FreeRules( void )
{
    /* Free function rules. */
    while ( scorep_filter_function_rules_head )
    {
        scorep_filter_rule_t* current_rule = scorep_filter_function_rules_head;
        scorep_filter_function_rules_head = current_rule->next;

        free( current_rule->pattern );
        free( current_rule );
    }
    scorep_filter_function_rules_tail = NULL;

    /* Free file rules. */
    while ( scorep_filter_file_rules_head )
    {
        scorep_filter_rule_t* current_rule = scorep_filter_file_rules_head;
        scorep_filter_file_rules_head = current_rule->next;

        free( current_rule->pattern );
        free( current_rule );
    }
    scorep_filter_file_rules_tail = NULL;
}



/* **************************************************************************************
   Matching requests
****************************************************************************************/

static bool
scorep_filter_match_file_rule( const char*           with_path,
                               scorep_filter_rule_t* rule,
                               SCOREP_ErrorCode*     error_code )
{
    int error_value = fnmatch( rule->pattern, with_path, 0 );

    if ( error_value == 0 )
    {
        *error_code = SCOREP_SUCCESS;
        return true;
    }
    else if ( error_value != FNM_NOMATCH )
    {
        UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                     "Error in pattern matching during evaluation of filter rules"
                     "with file '%s' and pattern '%s'. Disable filtering",
                     with_path, rule->pattern );
        SCOREP_Filter_Disable();
        *error_code = SCOREP_ERROR_PROCESSED_WITH_FAULTS;
        return false;
    }
    return false;
}

static bool
scorep_filter_match_function_rule( const char*           function_name,
                                   const char*           mangled_name,
                                   scorep_filter_rule_t* rule,
                                   SCOREP_ErrorCode*     error_code )
{
    int error_value = 0;
    if ( rule->is_mangled && ( mangled_name != NULL ) )
    {
        error_value = fnmatch( rule->pattern, mangled_name, 0 );
    }
    else
    {
        error_value = fnmatch( rule->pattern, function_name, 0 );
    }

    if ( error_value == 0 )
    {
        *error_code = SCOREP_SUCCESS;
        return true;
    }
    else if ( error_value != FNM_NOMATCH )
    {
        UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                     "Error in pattern matching during evaluation of filter rules"
                     "with file '%s' and pattern '%s'. Disable filtering",
                     function_name, rule->pattern );
        SCOREP_Filter_Disable();
        return false;
    }
    return false;
}

static inline bool
scorep_filter_match_file( const char* file_name )
{
    scorep_filter_rule_t* current_rule = scorep_filter_file_rules_head;
    bool                  excluded     = false; /* Start with all included */
    SCOREP_ErrorCode      error_code   = SCOREP_SUCCESS;

    if ( file_name != NULL )
    {
        while ( current_rule != NULL )
        {
            /* If included so far and we have an exclude rule */
            if ( ( !excluded ) && current_rule->is_exclude )
            {
                excluded = scorep_filter_match_file_rule( file_name,
                                                          current_rule,
                                                          &error_code );
                if ( error_code != SCOREP_SUCCESS )
                {
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !current_rule->is_exclude ) )
            {
                excluded = !scorep_filter_match_file_rule( file_name,
                                                           current_rule,
                                                           &error_code );

                if ( error_code != SCOREP_SUCCESS )
                {
                    return false;
                }
            }

            current_rule = current_rule->next;
        }
    }

    if ( excluded )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                            "Filtered file %s\n", file_name );
    }

    return excluded;
}

static inline bool
scorep_filter_match_function(  const char* function_name,
                               const char* mangled_name )
{
    scorep_filter_rule_t* current_rule = scorep_filter_function_rules_head;
    bool                  excluded     = false; /* Start with all included */
    SCOREP_ErrorCode      error_code   = SCOREP_SUCCESS;

    if ( function_name != NULL )
    {
        while ( current_rule != NULL )
        {
            /* If included so far and we have an exclude rule */
            if ( ( !excluded ) && current_rule->is_exclude )
            {
                excluded = scorep_filter_match_function_rule( function_name,
                                                              mangled_name,
                                                              current_rule,
                                                              &error_code );

                if ( error_code != SCOREP_SUCCESS )
                {
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !current_rule->is_exclude ) )
            {
                excluded = !scorep_filter_match_function_rule( function_name,
                                                               mangled_name,
                                                               current_rule,
                                                               &error_code );

                if ( error_code != SCOREP_SUCCESS )
                {
                    return false;
                }
            }

            current_rule = current_rule->next;
        }
    }

    if ( excluded )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                            "Filtered function %s\n", function_name );
    }

    return excluded;
}

bool
SCOREP_Filter_MatchFile( const char* file_name )
{
    if ( !SCOREP_Filter_IsEnabled() )
    {
        return false;
    }

    return scorep_filter_match_file( file_name );
}
bool
SCOREP_Filter_MatchFunction( const char* function_name,
                             const char* mangled_name )
{
    if ( !SCOREP_Filter_IsEnabled() )
    {
        return false;
    }

    return scorep_filter_match_function( function_name,
                                         mangled_name );
}
bool
SCOREP_Filter_Match( const char* file_name,
                     const char* function_name,
                     const char* mangled_name )
{
    if ( !SCOREP_Filter_IsEnabled() )
    {
        return false;
    }

    /* If file is excluded, function can no longer be included. */
    return scorep_filter_match_file( file_name ) ||
           scorep_filter_match_function( function_name, mangled_name );
}
