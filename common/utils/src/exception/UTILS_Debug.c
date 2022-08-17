/*
 * This file is part of the Score-P software ecosystem (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2021-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * @ingroup         UTILS_Exception_module
 *
 * @brief           Implementation of debug output handling in SCOREP.
 *
 * @created         2009-05-08
 */


#include <config.h>
#include <UTILS_Debug.h>

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>


#include <UTILS_CStr.h>
#include <UTILS_Mutex.h>


#include "normalize_file.h"


bool     utils_debug_initialized = false;
uint64_t utils_debug_level       = 0;

static UTILS_Mutex debug_mutex  = UTILS_MUTEX_INIT;
static FILE*       debug_stream = NULL;

static THREAD_LOCAL_STORAGE_SPECIFIER int32_t thread_id = -1;


static bool
is_base_digit( int        c,
               int        base,
               int* const out )
{
    switch ( c )
    {
        case 'f':
        case 'e':
        case 'd':
        case 'c':
        case 'b':
        case 'a':
            if ( base < 16 )
            {
                return false;
            }
            c = c - 'a' + '9' + 1;
        case '9':
        case '8':
            if ( base <= 8 )
            {
                return false;
            }
        case '7':
        case '6':
        case '5':
        case '4':
        case '3':
        case '2':
            if ( base <= 2 )
            {
                return false;
            }
        case '1':
        case '0':
            *out = c - '0';
            return true;
    }

    return false;
}

static int
parse_number( const char*     numberString,
              uint64_t* const numberReference )
{
    uint64_t number = 0;

    assert( numberString );
    assert( numberReference );

    /* determine base of number */
    int base = 10;
    if ( numberString[ 0 ] == '0' && tolower( ( unsigned char )numberString[ 1 ] ) == 'x' )
    {
        base          = 16;
        numberString += 2;
    }
    else if ( numberString[ 0 ] == '0' && tolower( ( unsigned char )numberString[ 1 ] ) == 'b' )
    {
        base          = 2;
        numberString += 2;
    }
    else if ( numberString[ 0 ] == '0' )
    {
        base = 8;
        /* keep the 0, "0" is also valid */
    }
    const char* value_iterator = numberString;
    int         digit;
    while ( is_base_digit( tolower( ( unsigned char )*value_iterator ), base, &digit ) )
    {
        uint64_t new_number = base * number + digit;

        /* Check for overflow */
        if ( new_number < number )
        {
            return ERANGE;
        }

        number = new_number;
        value_iterator++;
    }

    /* Have we consumed at least one digit? */
    if ( value_iterator == numberString )
    {
        return EINVAL;
    }
    numberString = value_iterator;

    /* Have we consumed the complete string */
    if ( *numberString != '\0' )
    {
        return EINVAL;
    }

    *numberReference = number;

    return 0;
}

static const char* debug_module_names[] =
{
    #define UTILS_DEFINE_DEBUG_MODULE( name, bit ) \
    UTILS_STRINGIFY( name )
    UTILS_DEBUG_MODULES,
    #undef UTILS_DEFINE_DEBUG_MODULE
    NULL
};

static bool
string_equal_icase( const char* s1,
                    const char* s2 )
{
    while ( *s1 && *s2 )
    {
        if ( toupper( ( unsigned char )*s1 ) != toupper( ( unsigned char )*s2 ) )
        {
            return false;
        }
        s1++;
        s2++;
    }
    if ( *s1 || *s2 )
    {
        return false;
    }
    return true;
}

static int
parse_debug_level( const char*     levelString,
                   uint64_t* const levelReference )
{
    char*    levelStringCopy;
    uint64_t level = 0;

    assert( levelString );
    assert( levelReference );

    levelStringCopy = UTILS_CStr_dup( levelString );
    if ( !levelStringCopy )
    {
        return ENOMEM;
    }

    char* token;
    for ( token = strtok( levelStringCopy, " ,;" );
          token; token = strtok( NULL, " ,;" ) )
    {
        /* some common constants */
        if ( string_equal_icase( token, "all" )
             || strcmp( token, "~0" ) == 0
             || strcmp( token, "-1" ) == 0 )
        {
            level |= UINT64_MAX;
            continue;
        }

        bool     invert    = false;
        uint64_t level_mod = 0;
        if ( *token == '~' )
        {
            invert = true;
            token++;
        }

        /* try module names */
        level_mod = 1;
        const char** module_name = debug_module_names;
        while ( *module_name )
        {
            if ( string_equal_icase( *module_name, token ) )
            {
                break;
            }
            module_name++;
            level_mod <<= 1;
        }
        if ( *module_name )
        {
            goto modify_level;
        }

        /* parse it as a number */
        level_mod = 0;
        if ( 0 == parse_number( token, &level_mod ) )
        {
            goto modify_level;
        }

        free( levelStringCopy );
        return 1;

modify_level:
        if ( invert )
        {
            level &= ~level_mod;
        }
        else
        {
            level |= level_mod;
        }
    }
    free( levelStringCopy );

    *levelReference = level;

    return 0;
}

/**
 * Initialize the debug system.  Retrieve the enabled debug modules from the
 * @c <PACKAGE>_DEBUG environment variable and print a corresponding status
 * message to @c stderr.
 */
void
utils_debug_init( void )
{
    UTILS_MutexLock( &debug_mutex );
    if ( utils_debug_initialized )
    {
        UTILS_MutexUnlock( &debug_mutex );
        return;
    }

    const char* env_name           = UTILS_STRINGIFY( PACKAGE_MANGLE_NAME( DEBUG ) );
    const char* debug_level_string = getenv( env_name );

    utils_debug_level = 0;
    if ( debug_level_string )
    {
        int ret = parse_debug_level( debug_level_string, &utils_debug_level );
        if ( ret )
        {
            fprintf( stderr, "[%s] Invalid value for %s: %s\n",
                     PACKAGE_NAME, env_name, debug_level_string );
        }
    }

    utils_debug_level &= ~( UTILS_DEBUG_FUNCTION_ENTRY | UTILS_DEBUG_FUNCTION_EXIT );
    if ( utils_debug_level )
    {
        fprintf( stderr, "[%s] Active debug module(s):", PACKAGE_NAME );
        uint64_t     level_mod   = 1;
        const char** module_name = debug_module_names;
        while ( *module_name )
        {
            if ( utils_debug_level & level_mod )
            {
                fprintf( stderr, " %s", *module_name );
            }
            level_mod <<= 1;
            module_name++;
        }
        fprintf( stderr, "\n" );
    }

    utils_debug_initialized = true;
    UTILS_MutexUnlock( &debug_mutex );
}

void
UTILS_Debug_SetLogStream( FILE* stream )
{
    UTILS_MutexLock( &debug_mutex );
    debug_stream = stream;
    UTILS_MutexUnlock( &debug_mutex );
}

void
UTILS_Debug_Printf( uint64_t    kind,
                    const char* srcdir,
                    const char* file,
                    uint64_t    line,
                    const char* function,
                    const char* msgFormatString,
                    ... )
{
    /* Extract the "kind" bits and ensure they are not all set simultaneously */
    kind &= ( UTILS_DEBUG_FUNCTION_ENTRY | UTILS_DEBUG_FUNCTION_EXIT );
    assert( kind != ( UTILS_DEBUG_FUNCTION_ENTRY | UTILS_DEBUG_FUNCTION_EXIT ) );

    size_t msg_format_string_length = msgFormatString ?
                                      strlen( msgFormatString ) : 0;

    const char* normalized_file = normalize_file( srcdir, file );

    UTILS_MutexLock( &debug_mutex );
    if ( !debug_stream )
    {
        debug_stream = stdout;
    }
    if ( thread_id == -1 )
    {
        static int32_t thread_count = 0;

        thread_id = thread_count++;
    }
    if ( kind )
    {
        const char* kind_str = "Entering";
        if ( kind & UTILS_DEBUG_FUNCTION_EXIT )
        {
            kind_str = "Leaving";
        }

        fprintf( debug_stream,
                 "[%s - %" PRId32 "] %s:%" PRIu64 ": %s function '%s'%s",
                 PACKAGE_NAME,
                 thread_id,
                 normalized_file,
                 line,
                 kind_str,
                 function,
                 msg_format_string_length ? ": " : "\n" );
    }
    else
    {
        fprintf( debug_stream,
                 "[%s - %" PRId32 "] %s:%" PRIu64 "%s",
                 PACKAGE_NAME,
                 thread_id,
                 normalized_file,
                 line,
                 msg_format_string_length ? ": " : "\n" );
    }

    if ( msg_format_string_length )
    {
        va_list va;
        va_start( va, msgFormatString );
        vfprintf( debug_stream, msgFormatString, va );
        fprintf( debug_stream, "\n" );
        va_end( va );
    }
    UTILS_MutexUnlock( &debug_mutex );
}
