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


/**
 * @file       scorep_filter_matching.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Maintains the internal filter rules and performs the pattern machcing requests.
 * We maintain two rule lists: One for the function name rules and one for the
 * file name rules. Due to the possible include/exclude
 * combinations, the rules must be evaluated in sequential order. Thus, the use of a
 * single linked list it sufficient.
 */

#include <config.h>
#include <fnmatch.h>

#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Filter.h>
#include <scorep_filter_matching.h>

/* **************************************************************************************
   Variable and type definitions
****************************************************************************************/

/**
 * Type which is used to store the filter rules.
 */
typedef struct scorep_filter_rule_struct scorep_filter_rule_t;

/**
 * The struct definition for the rule list. Each record contains one rule.
 * The @ref scorep_filter_rule_t defines a type for this struct.
 */
struct scorep_filter_rule_struct
{
    const char*           pattern;    /**< Pointer to the pattern string */
    bool                  is_exclude; /**< True if it is a exclude rule, false else */
    bool                  is_fortran; /**< True if the rule might be Fortran mangled */
    scorep_filter_rule_t* next;       /**< Next filter rule */
};

/**
 * Pointer to the head of the filter rules for source file names.
 */
scorep_filter_rule_t* scorep_filter_file_rules_head = NULL;

/**
 * Pointer to the tail of the filter rules for source file names.
 */
scorep_filter_rule_t* scorep_filter_file_rules_tail = NULL;

/**
 * Pointer to the head of the filter rules for function names.
 */
scorep_filter_rule_t* scorep_filter_function_rules_head = NULL;

/**
 * Pointer to the tail of the filter rules for function names.
 */
scorep_filter_rule_t* scorep_filter_function_rules_tail = NULL;

bool                  scorep_filter_is_enabled = false;

/* **************************************************************************************
   Rule representation manipulation functions
****************************************************************************************/

SCOREP_Error_Code
scorep_filter_add_file_rule( const char* rule, bool is_exclude )
{
    assert( rule );
    assert( *rule != '\0' );

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = ( scorep_filter_rule_t* )
                                     SCOREP_Memory_AllocForMisc( sizeof( scorep_filter_rule_t ) );

    if ( new_rule == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern    = SCOREP_CStr_dup( rule );
    new_rule->is_exclude = is_exclude;
    new_rule->is_fortran = false;
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


SCOREP_Error_Code
scorep_filter_add_function_rule( const char* rule, bool is_exclude, bool is_fortran )
{
    assert( rule );
    assert( *rule != '\0' );

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = ( scorep_filter_rule_t* )
                                     SCOREP_Memory_AllocForMisc( sizeof( scorep_filter_rule_t ) );

    if ( new_rule == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern    = SCOREP_CStr_dup( rule );
    new_rule->is_exclude = is_exclude;
    new_rule->is_fortran = is_fortran;
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
scorep_filter_free_rules()
{
    /* Free function rules.
       Only the strings are allocated via malloc.
       The struct itself is free if the memory management is finalized. */
    scorep_filter_rule_t* current_rule = scorep_filter_function_rules_head;
    while ( current_rule != NULL )
    {
        free( current_rule->pattern );
        current_rule = current_rule->next;
    }

    /* Free file rules.
       Only the strings are allocated via malloc.
       The struct itself is free if the memory management is finalized. */
    current_rule = scorep_filter_file_rules_head;
    while ( current_rule != NULL )
    {
        free( current_rule->pattern );
        current_rule = current_rule->next;
    }
}

/* **************************************************************************************
   Matching requests
****************************************************************************************/
bool
SCOREP_Filter_Match( const char* file_name, const char* function_name, bool use_fortran )
{
    scorep_filter_rule_t* current_rule = scorep_filter_file_rules_head;
    bool                  excluded     = false; /* Start with all included */
    int                   error_value;

    if ( !SCOREP_Filter_IsEnabled() )
    {
        return false;
    }

    if ( file_name != NULL )
    {
        while ( current_rule != NULL )
        {
            /* If included so far and we have an exclude rule */
            if ( ( !excluded ) && current_rule->is_exclude )
            {
                error_value = fnmatch( current_rule->pattern, file_name, FNM_PERIOD );
                if ( error_value == 0 )
                {
                    excluded = true;
                }
                else if ( error_value != FNM_NOMATCH )
                {
                    SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                  "Error in pattern matching during evaluation of filter rules"
                                  "with file '%s' and pattern '%s'. Disable filtering",
                                  file_name, current_rule->pattern );
                    SCOREP_Filter_Disable();
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !current_rule->is_exclude ) )
            {
                error_value = fnmatch( current_rule->pattern, file_name, FNM_PERIOD );
                if ( error_value == 0 )
                {
                    excluded = false;
                }
                else if ( error_value != FNM_NOMATCH )
                {
                    SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                  "Error in pattern matching during evaluation of filter rules"
                                  "with file '%s' and pattern '%s'. Disable filtering",
                                  file_name, current_rule->pattern );
                    SCOREP_Filter_Disable();
                    return false;
                }
            }

            current_rule = current_rule->next;
        }
    }

    /* If file is excluded, function can no longer be included. Thus, return. */
    if ( excluded )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FILTEING,
                             "Filtered file %s\n", file_name );
        return true;
    }

    excluded     = false; /* Start with all functions included */
    current_rule = scorep_filter_function_rules_head;
    if ( function_name != NULL )
    {
        while ( current_rule != NULL )
        {
            /* If included so far and we have an exclude rule */
            if ( ( !excluded ) && current_rule->is_exclude )
            {
                error_value = fnmatch( current_rule->pattern, function_name, FNM_PERIOD );
                if ( error_value == 0 )
                {
                    excluded = true;
                }
                else if ( error_value != FNM_NOMATCH )
                {
                    SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                  "Error in pattern matching during evaluation of filter rules"
                                  "with file '%s' and pattern '%s'. Disable filtering",
                                  file_name, current_rule->pattern );
                    SCOREP_Filter_Disable();
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !current_rule->is_exclude ) )
            {
                error_value = fnmatch( current_rule->pattern, function_name, FNM_PERIOD );
                if ( error_value == 0 )
                {
                    excluded = false;
                }
                else if ( error_value != FNM_NOMATCH )
                {
                    SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                                  "Error in pattern matching during evaluation of filter rules"
                                  "with file '%s' and pattern '%s'. Disable filtering",
                                  file_name, current_rule->pattern );
                    SCOREP_Filter_Disable();
                    return false;
                }
            }

            current_rule = current_rule->next;
        }
    }

    if ( excluded )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                             "Filtered function %s\n", function_name );
    }

    return excluded;
}
