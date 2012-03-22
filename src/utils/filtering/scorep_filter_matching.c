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
#include <ctype.h>
#include <malloc.h>

#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Filter.h>
#include <scorep_filter_matching.h>

#define STR( x ) STR_( x )
#define STR_( x ) #x

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
    char*                 pattern;    /**< Pointer to the pattern string */
    const char*           pattern2;   /**< Pointer to the mangled pattern string */
    bool                  is_exclude; /**< True if it is a exclude rule, false else */
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

bool scorep_filter_is_enabled = false;

/* **************************************************************************************
   Rule representation manipulation functions
****************************************************************************************/

/**
 * Creates a string which contains the content of @a pattern which is mangled according
 * to Fortran rules. Currently, upper or lower case are considered and up to one
 * underscore appended.
 * @param pattern The pattern that is mangled
 * @returns A pointer to the mangled pattern string. The new string is allocated in
 *          a Score-P Misc page. Thus it is automatically freed at memory management
            finalization.
 */
static const char*
scorep_filter_mangle_pattern( const char* pattern )
{
    size_t i;
    size_t len    = strlen( pattern );
    char*  result = SCOREP_CStr_dup( pattern );

    /* Put everything to lower or upper case */
    char* test_case = STR( FC_FUNC( x, X ) );

    if ( *test_case == 'x' )
    {
        for ( i = 0; i < len; i++ )
        {
            result[ i ] = ( char )tolower( result[ i ] );
        }
    }
    else
    {
        for ( i = 0; i < len; i++ )
        {
            result[ i ] = ( char )toupper( result[ i ] );
        }
    }

    /* Append underscore */
    if ( test_case[ 1 ] == '_' )
    {
        result[ len ]     = '_';
        result[ len + 1 ] = '\0';
    }
    return result;
}

SCOREP_Error_Code
scorep_filter_add_file_rule( const char* rule, bool is_exclude )
{
    assert( rule );
    assert( *rule != '\0' );

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = ( scorep_filter_rule_t* )
                                     malloc( sizeof( scorep_filter_rule_t ) );

    if ( new_rule == NULL )
    {
        SCOREP_ERROR_POSIX( "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern    = SCOREP_CStr_dup( rule );
    new_rule->pattern2   = NULL;
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


SCOREP_Error_Code
scorep_filter_add_function_rule( const char* rule, bool is_exclude, bool is_fortran )
{
    assert( rule );
    assert( *rule != '\0' );

    /* Create new rule entry */
    scorep_filter_rule_t* new_rule = ( scorep_filter_rule_t* )
                                     malloc( sizeof( scorep_filter_rule_t ) );

    if ( new_rule == NULL )
    {
        SCOREP_ERROR_POSIX( "Failed to allocate memory for filter rule." );
        return SCOREP_ERROR_MEM_ALLOC_FAILED;
    }

    new_rule->pattern = SCOREP_CStr_dup( rule );
    if ( is_fortran )
    {
        new_rule->pattern2 = scorep_filter_mangle_pattern( new_rule->pattern );
    }
    else
    {
        new_rule->pattern2 = NULL;
    }
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
SCOREP_Filter_FreeRules()
{
    /* Free function rules. */
    scorep_filter_rule_t* current_rule = scorep_filter_function_rules_head;
    scorep_filter_rule_t* deleted_rule;
    while ( current_rule != NULL )
    {
        free( current_rule->pattern );
        free( current_rule->pattern2 );
        deleted_rule = current_rule;
        current_rule = current_rule->next;
        free( deleted_rule );
    }

    /* Free file rules. */
    current_rule = scorep_filter_file_rules_head;
    while ( current_rule != NULL )
    {
        free( current_rule->pattern );
        deleted_rule = current_rule;
        current_rule = current_rule->next;
        free( deleted_rule );
    }
}

/* **************************************************************************************
   Matching requests
****************************************************************************************/

static bool
scorep_filter_match_file( const char*           with_path,
                          scorep_filter_rule_t* rule,
                          SCOREP_Error_Code*    error_code )
{
    int error_value = fnmatch( rule->pattern, with_path, 0 );

    if ( error_value == 0 )
    {
        *error_code = SCOREP_SUCCESS;
        return true;
    }
    else if ( error_value != FNM_NOMATCH )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
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
scorep_filter_match_function( const char*           function_name,
                              scorep_filter_rule_t* rule,
                              bool                  use_fortran,
                              SCOREP_Error_Code*    error_code )
{
    int error_value = 0;
    if ( use_fortran && ( rule->pattern2 != NULL ) )
    {
        error_value = fnmatch( rule->pattern2, function_name, 0 );
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
        SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                      "Error in pattern matching during evaluation of filter rules"
                      "with file '%s' and pattern '%s'. Disable filtering",
                      function_name, rule->pattern );
        SCOREP_Filter_Disable();
        return false;
    }
    return false;
}

bool
SCOREP_Filter_Match( const char* file_name, const char* function_name, bool use_fortran )
{
    scorep_filter_rule_t* current_rule = scorep_filter_file_rules_head;
    bool                  excluded     = false; /* Start with all included */
    int                   error_value;
    SCOREP_Error_Code     error_code = SCOREP_SUCCESS;

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
                excluded = scorep_filter_match_file( file_name,
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
                excluded = !scorep_filter_match_file( file_name,
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

    /* If file is excluded, function can no longer be included. Thus, return. */
    if ( excluded )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
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
                excluded = scorep_filter_match_function( function_name,
                                                         current_rule,
                                                         use_fortran,
                                                         &error_code );

                if ( error_code != SCOREP_SUCCESS )
                {
                    return false;
                }
            }
            /* If excluded so far and we have an include rule */
            else if ( excluded && ( !current_rule->is_exclude ) )
            {
                excluded = !scorep_filter_match_function( function_name,
                                                          current_rule,
                                                          use_fortran,
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
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FILTERING,
                             "Filtered function %s\n", function_name );
    }

    return excluded;
}
