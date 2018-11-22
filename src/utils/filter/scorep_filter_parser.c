/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 * Parses the filter file and creates a filter.
 */

#include <config.h>

#include <SCOREP_Filter.h>

#include <UTILS_IO.h>

#include "scorep_filter_matching.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

/* **************************************************************************************
   Variable and type definitions
****************************************************************************************/

/**
 * Type for the possible states of the parser. The following states are possible:
 * <dl>
 * <dt>SCOREP_FILTER_PARSE_START
 * <dd> Inital state or state outside of the file rule block and function rule block.
 * <dt> SCOREP_FILTER_PARSE_FILES
 * <dd> State after a file rule block began with SCOREP_FILE_NAMES_BEGIN.
 * <dt> SCOREP_FILTER_PARSE_FILES_EXCLUDE
 * <dd> Inside an exclude rule in the file rule block
 * <dt> SCOREP_FILTER_PARSE_FILES_INCLUDE
 * <dd> Inside an include rule in the file rule block
 * <dt> SCOREP_FILTER_PARSE_REGIONS
 * <dd> State after a function rule block began with SCOREP_REGION_NAMES_BEGIN.
 * <dt> SCOREP_FILTER_PARSE_REGIONS_EXCLUDE
 * <dd> Inside an exclude rule in the function rule block
 * <dt> SCOREP_FILTER_PARSE_REGIONS_INCLUDE
 * <dd> Inside an include rule in the function rule block
 * </dl>
 * Beside this basic states two further values are defined with special purpose:
 * <dl>
 * <dt> SCOREP_FILTER_PARSE_MANGLED
 * <dd> Can be combined with SCOREP_FILTER_PARSE_REGIONS_EXCLUDE or
 *      SCOREP_FILTER_PARSE_REGIONS_INCLUDE to indicate that the rules should be
 *      applied to the mangled name.
 * <dt> SCOREP_FILTER_PARSE_BASE
 * <dd> Is used to extract the base state without the fortran option from the
 *      state variable.
 * </dl>
 *
 * If you add further states for any reason make sure that
 * SCOREP_FILTER_PARSE_FORTRAN is a single bit that contains in no other
 * state else the bitwise combination will not work anymore.
 * Furthermore, SCOREP_FILTER_PARSE_BASE must have set all bits used in other
 * states except the bit set by SCOREP_FILTER_PARSE_FORTRAN.
 */
typedef int scorep_filter_parse_modes;

#define SCOREP_FILTER_PARSE_START             0
#define SCOREP_FILTER_PARSE_FILES             1
#define SCOREP_FILTER_PARSE_FILES_EXCLUDE     2
#define SCOREP_FILTER_PARSE_FILES_INCLUDE     3
#define SCOREP_FILTER_PARSE_REGIONS           4
#define SCOREP_FILTER_PARSE_REGIONS_EXCLUDE   5
#define SCOREP_FILTER_PARSE_REGIONS_INCLUDE   6
#define SCOREP_FILTER_PARSE_BASE              7
#define SCOREP_FILTER_PARSE_MANGLED           8

/**
 * @def SCOREP_FILTER_MODE_IS_FORTRAN
 * Expands to a non-zero value if @mode has not the fortran mode set.
 */
#define SCOREP_FILTER_MODE_IS_MANGLED( mode ) \
    ( mode & SCOREP_FILTER_PARSE_MANGLED )

/**
 * @def SCOREP_FILTER_MODE_BASE
 * Gives the mode without the fortran option set.
 */
#define SCOREP_FILTER_MODE_BASE( mode ) \
    ( mode & SCOREP_FILTER_PARSE_BASE )


/* **************************************************************************************
   Local helper functions
****************************************************************************************/

struct parse_state
{
    scorep_filter_rule_t***   file_rule_tail;
    scorep_filter_rule_t***   function_rule_tail;
    scorep_filter_parse_modes mode;
};

/**
 * Processes a token of the filter parser. A token may be a key word or an expression
 * for name matching.
 * @param token   The string that contains the token.
 * @param mode    The current mode of the parser. This value may be changed when
 *                the token is evaluated.
 * @returns SCOREP_SUCCESS if the token was processed succesfully. Else an error code
 *          is returned. The only possible error code is SCOREP_ERROR_PARSE_SYNTAX.
 */
static SCOREP_ErrorCode
process_token( struct parse_state* state,
               const char*         token )
{
    assert( state );
    assert( token );
    if ( *token == '\0' )
    {
        return SCOREP_SUCCESS;
    }

    /* ------------------------------ SCOREP_FILE_NAMES_BEGIN */
    if ( strcmp( token, "SCOREP_FILE_NAMES_BEGIN" ) == 0 )
    {
        if ( state->mode == SCOREP_FILTER_PARSE_START )
        {
            state->mode = SCOREP_FILTER_PARSE_FILES;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                         "Unexpected token 'SCOREP_FILE_NAMES_BEGIN'" );
            return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ SCOREP_FILE_NAMES_END */
    else if ( strcmp( token, "SCOREP_FILE_NAMES_END" ) == 0 )
    {
        if ( ( state->mode >= SCOREP_FILTER_PARSE_FILES ) &&
             ( state->mode <= SCOREP_FILTER_PARSE_FILES_INCLUDE ) )
        {
            state->mode = SCOREP_FILTER_PARSE_START;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                         "Unexpected token 'SCOREP_FILE_NAMES_END'" );
            return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ SCOREP_REGION_NAMES_BEGIN */
    else if ( strcmp( token, "SCOREP_REGION_NAMES_BEGIN" ) == 0 )
    {
        if ( state->mode == SCOREP_FILTER_PARSE_START )
        {
            state->mode = SCOREP_FILTER_PARSE_REGIONS;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                         "Unexpected token 'SCOREP_REGION_NAMES_BEGIN'" );
            return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ SCOREP_REGION_NAMES_END */
    else if ( strcmp( token, "SCOREP_REGION_NAMES_END" ) == 0 )
    {
        if ( ( SCOREP_FILTER_MODE_BASE( state->mode ) >= SCOREP_FILTER_PARSE_REGIONS ) &&
             ( SCOREP_FILTER_MODE_BASE( state->mode ) <= SCOREP_FILTER_PARSE_REGIONS_INCLUDE ) )
        {
            state->mode = SCOREP_FILTER_PARSE_START;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                         "Unexpected token 'SCOREP_REGION_NAMES_END'" );
            return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ EXCLUDE */
    else if ( strcmp( token, "EXCLUDE" ) == 0 )
    {
        switch ( SCOREP_FILTER_MODE_BASE( state->mode ) )
        {
            case SCOREP_FILTER_PARSE_FILES:
            case SCOREP_FILTER_PARSE_FILES_EXCLUDE:
            case SCOREP_FILTER_PARSE_FILES_INCLUDE:
                state->mode = SCOREP_FILTER_PARSE_FILES_EXCLUDE;
                break;
            case SCOREP_FILTER_PARSE_REGIONS:
            case SCOREP_FILTER_PARSE_REGIONS_EXCLUDE:
            case SCOREP_FILTER_PARSE_REGIONS_INCLUDE:
                state->mode = SCOREP_FILTER_PARSE_REGIONS_EXCLUDE;
                break;
            default:
                UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                             "Unexpected token 'EXCLUDE'" );
                return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ INCLUDE */
    else if ( strcmp( token, "INCLUDE" ) == 0 )
    {
        switch ( SCOREP_FILTER_MODE_BASE( state->mode ) )
        {
            case SCOREP_FILTER_PARSE_FILES:
            case SCOREP_FILTER_PARSE_FILES_EXCLUDE:
            case SCOREP_FILTER_PARSE_FILES_INCLUDE:
                state->mode = SCOREP_FILTER_PARSE_FILES_INCLUDE;
                break;
            case SCOREP_FILTER_PARSE_REGIONS:
            case SCOREP_FILTER_PARSE_REGIONS_EXCLUDE:
            case SCOREP_FILTER_PARSE_REGIONS_INCLUDE:
                state->mode = SCOREP_FILTER_PARSE_REGIONS_INCLUDE;
                break;
            default:
                UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                             "Unexpected token 'INCLUDE'" );
                return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ MANGLED */
    else if ( strcmp( token, "MANGLED" ) == 0 )
    {
        switch ( SCOREP_FILTER_MODE_BASE( state->mode ) )
        {
            case SCOREP_FILTER_PARSE_REGIONS_EXCLUDE:
            case SCOREP_FILTER_PARSE_REGIONS_INCLUDE:
                state->mode |= SCOREP_FILTER_PARSE_MANGLED;
                break;
            default:
                UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                             "Unexpected token 'MANGLED'" );
                return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    /* ------------------------------ Default */
    else
    {
        bool exclude = false;
        switch ( SCOREP_FILTER_MODE_BASE( state->mode ) )
        {
            case SCOREP_FILTER_PARSE_FILES_EXCLUDE:
                exclude = true;
            case SCOREP_FILTER_PARSE_FILES_INCLUDE:
                scorep_filter_add_rule( state->file_rule_tail,
                                        token,
                                        exclude,
                                        false );
                break;

            case SCOREP_FILTER_PARSE_REGIONS_EXCLUDE:
                exclude = true;
            case SCOREP_FILTER_PARSE_REGIONS_INCLUDE:
                scorep_filter_add_rule( state->function_rule_tail,
                                        token,
                                        exclude,
                                        SCOREP_FILTER_MODE_IS_MANGLED( state->mode ) );
                break;

            default:
                UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                             "Unexpected token '%s'", token );
                return SCOREP_ERROR_PARSE_SYNTAX;
        }
    }

    return SCOREP_SUCCESS;
}

SCOREP_Filter*
SCOREP_Filter_New( void )
{
    SCOREP_Filter* new_filter = calloc( 1, sizeof( *new_filter ) );
    if ( !new_filter )
    {
        return NULL;
    }
    new_filter->file_rules_tail     = &new_filter->file_rules;
    new_filter->function_rules_tail = &new_filter->function_rules;
    return new_filter;
}

void
SCOREP_Filter_Delete( SCOREP_Filter* filter )
{
    if ( filter )
    {
        scorep_filter_free_rules( filter->file_rules );
        scorep_filter_free_rules( filter->function_rules );
        free( filter );
    }
}

/**
 * Parses the given filter file @a file.
 * @param file  On already opened filter file that is parsed.
 * @returns SCOREP_SUCCESS if the file was successfully parsed. Else an error code is
 *          returned.
 */
SCOREP_ErrorCode
SCOREP_Filter_ParseFile( SCOREP_Filter* filter,
                         const char*    fileName )
{
    if ( !filter || !fileName )
    {
        return SCOREP_ERROR_INVALID_ARGUMENT;
    }

    FILE*              filter_file = NULL;
    size_t             buffer_size = 0;
    char*              buffer      = NULL;
    size_t             pos         = 0;
    size_t             length      = 0;
    size_t             token_start = 0;
    struct parse_state state;
    SCOREP_ErrorCode   err = SCOREP_SUCCESS;

    /* Open configuration file */
    filter_file = fopen( fileName, "r" );
    if ( filter_file == NULL )
    {
        UTILS_ERROR_POSIX(  "Unable to open filter specification file '%s'",
                            fileName );
        err = SCOREP_ERROR_FILE_CAN_NOT_OPEN;
        goto cleanup;
    }

    state.file_rule_tail     = &filter->file_rules_tail;
    state.function_rule_tail = &filter->function_rules_tail;
    state.mode               = SCOREP_FILTER_PARSE_START;

    /* Read file line by line */
    while ( !feof( filter_file ) )
    {
        err = UTILS_IO_GetLine( &buffer, &buffer_size, filter_file );
        if ( ( err != SCOREP_SUCCESS ) && ( err != SCOREP_ERROR_END_OF_BUFFER ) )
        {
            goto cleanup;
        }
        length = strlen( buffer );
        if ( length == 0 )
        {
            continue;
        }

        /* Cut away comments:
           Find first '#' that is not escaped by a backslash
         */
        pos = 0;
        do
        {
            pos += strcspn( &buffer[ pos ], "#" );
            if ( ( pos < length ) &&                                    // Whether '#' appears
                 ( ( ( pos == 0 ) && ( *buffer == '#' ) ) ||            // Cannot be escaped if first
                   ( ( pos > 0 ) && ( buffer[ pos - 1 ] != '\\' ) ) ) ) // Check if '#' is escaped
            {
                buffer[ pos ] = '\0';
                length        = pos;
            }
            pos++;
        }
        while ( pos < length );

        /* Escaping linebreaks is not allowed */
        if ( ( length >= 2 ) && ( buffer[ length - 2 ] == '\\' ) )
        {
            UTILS_ERROR( SCOREP_ERROR_PARSE_SYNTAX,
                         "Escaping line breaks is not supported." );
            err = SCOREP_ERROR_PARSE_SYNTAX;
            goto cleanup;
        }

        /* Tokenize:
           Find next whitespace that is not escaped and terminate there
         */
        pos         = 0;
        token_start = 0;
        while ( pos < length )
        {
            pos += strcspn( &buffer[ pos ], " \t\n\0" );

            if ( ( pos <= length ) &&                                   // Whether whitespace appears
                 ( ( pos == 0 ) ||                                      // Cannot be escaped if first
                   ( ( pos > 0 ) && ( buffer[ pos - 1 ] != '\\' ) ) ) ) // Check if whitespace is escaped
            {
                buffer[ pos ] = '\0';
                err           = process_token( &state, &buffer[ token_start ] );
                if ( err != SCOREP_SUCCESS )
                {
                    goto cleanup;
                }
                token_start = pos + 1;
            }
            pos++;
        }
    }

    return SCOREP_SUCCESS;

cleanup:
    if ( filter_file )
    {
        fclose( filter_file );
    }
    free( buffer );

    return err;
}

SCOREP_ErrorCode
SCOREP_Filter_MatchFile( const SCOREP_Filter* filter,
                         const char*          fileName,
                         int*                 result )
{
    if ( !filter || !result )
    {
        return SCOREP_ERROR_INVALID_ARGUMENT;
    }

    SCOREP_ErrorCode err;

    *result = scorep_filter_match_file( filter->file_rules, fileName, &err );

    return err;
}

SCOREP_ErrorCode
SCOREP_Filter_MatchFunction( const SCOREP_Filter* filter,
                             const char*          functionName,
                             const char*          mangledName,
                             int*                 result )
{
    if ( !filter || !result )
    {
        return SCOREP_ERROR_INVALID_ARGUMENT;
    }

    SCOREP_ErrorCode err;

    *result = scorep_filter_match_function( filter->function_rules,
                                            functionName,
                                            mangledName,
                                            &err );

    return err;
}

SCOREP_ErrorCode
SCOREP_Filter_IncludeFunction( const SCOREP_Filter* filter,
                               const char*          functionName,
                               const char*          mangledName,
                               int*                 result )
{
    if ( !filter || !result )
    {
        return SCOREP_ERROR_INVALID_ARGUMENT;
    }

    SCOREP_ErrorCode err;

    *result = scorep_filter_include_function( filter->function_rules,
                                              functionName,
                                              mangledName,
                                              &err );

    return err;
}

SCOREP_ErrorCode
SCOREP_Filter_Match( const SCOREP_Filter* filter,
                     const char*          fileName,
                     const char*          functionName,
                     const char*          mangledName,
                     int*                 result )
{
    if ( !filter || !result )
    {
        return SCOREP_ERROR_INVALID_ARGUMENT;
    }

    SCOREP_ErrorCode err;

    *result = scorep_filter_match_file( filter->file_rules, fileName, &err ) ||
              scorep_filter_match_function( filter->function_rules, functionName, mangledName, &err );

    return err;
}
