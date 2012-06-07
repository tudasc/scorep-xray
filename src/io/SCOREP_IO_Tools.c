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
 * @status     alpha
 * @file       SCOREP_IO_Tools.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Functions for file parsing in C.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <SCOREP_Error.h>
#include <SCOREP_IO.h>
#include <SCOREP_CStr.h>

#define BUFFER_SIZE 1024

MANGLE_NAME( Error_Code )
MANGLE_NAME( IO_GetLine ) ( char** buffer, size_t * buffer_size, FILE * file )
{
    assert( buffer );

    if ( *buffer == NULL || *buffer_size == 0 )
    {
        *buffer = ( char* )realloc( *buffer, BUFFER_SIZE );
        if ( *buffer == NULL )
        {
            *buffer_size = 0;
            MANGLE_NAME( ERROR_POSIX ) ( "" );
            return MANGLE_NAME( ERROR_MEM_ALLOC_FAILED );
        }
        *buffer_size = BUFFER_SIZE;
    }

    **buffer = '\0';

    if ( !fgets( *buffer, *buffer_size, file ) )
    {
        if (  feof( file ) )
        {
            return MANGLE_NAME( ERROR_END_OF_BUFFER );
        }
        MANGLE_NAME( ERROR_POSIX ) ( "Error while reading from file" );
        return MANGLE_NAME( ERROR_FILE_INTERACTION );
    }

    while ( strlen( *buffer ) == *buffer_size - 1 )
    {
        *buffer_size += BUFFER_SIZE;
        *buffer       = ( char* )realloc( *buffer, *buffer_size );
        if ( *buffer == NULL )
        {
            *buffer_size = 0;
            MANGLE_NAME( ERROR_POSIX ) ( "Failed to increase memory for string buffer" );
            return MANGLE_NAME( ERROR_MEM_ALLOC_FAILED );
        }
        if ( !fgets( &( *buffer )[ *buffer_size - BUFFER_SIZE - 1 ],
                     BUFFER_SIZE + 1, file ) )
        {
            if (  feof( file ) )
            {
                return MANGLE_NAME( ERROR_END_OF_BUFFER );
            }
            MANGLE_NAME( ERROR_POSIX ) ( "Error while reading from file" );
            return MANGLE_NAME( ERROR_FILE_INTERACTION );
        }
    }

    return MANGLE_NAME( SUCCESS );
}

bool
MANGLE_NAME( IO_HasPath ) ( const char* path )
{
    assert( path );
    return strcspn( path, "/" ) < strlen( path );
}

const char*
MANGLE_NAME( IO_GetWithoutPath ) ( const char* path )
{
    assert( path );

    int len = strlen( path );
    int pos;

    for ( pos = len - 1; pos >= 0; pos-- )
    {
        if ( path[ pos ] == '/' )
        {
            return &( path[ pos + 1 ] );
        }
    }
    return path;
}

void
MANGLE_NAME( IO_SimplifyPath ) ( char* path )
{
    assert( path );

    int  len                = 0;
    int  pos                = 0;
    int  cut_end            = 0;
    int  level              = 0;
    bool has_slashes        = false;
    bool has_starting_slash = ( path[ 0 ] == '/' );
    bool has_trailing_slash = false;

    /* Remove double slashes and '/./' sequences */
    for ( len = 0; path[ len ] != '\0'; len++ )
    {
        path[ len - level ] = path[ len ];
        if ( path[ len ] == '/' )
        {
            has_slashes = true;
        }
        if ( ( path[ len ] == '/' ) && ( path[ len + 1 ] == '/' ) )
        {
            level++;
        }
        if ( ( path[ len ] == '/' ) &&
             ( path[ len + 1 ] == '.' )  &&
             ( path[ len + 2 ] == '/' ) )
        {
            level += 2;
            len++;
        }
    }

    /* determine string length and terminate string */
    len -= level;
    if ( ( len > 0 ) && ( path[ len - 1 ] == '/' ) )
    {
        len--;
        has_trailing_slash = true;
    }
    path[ len ] = '\0';

    /* Remove /.. structures */
    level   = 0;
    cut_end = len;
    for ( pos = len - 1; pos >= 0; pos-- )
    {
        /* Check whether we encountered a /../ or /.. at the end of the path */
        if ( ( pos >= 3 ) &&
             ( ( path[ pos + 1 ] == '/' ) || ( path[ pos + 1 ] == '\0' ) ) &&
             ( path[ pos ] == '.' ) &&
             ( path[ pos - 1 ] == '.' ) &&
             ( path[ pos - 2 ] == '/' ) )
        {
            if ( level == 0 )
            {
                cut_end = pos + 1;
            }
            level++;
            pos = pos - 2;
        }

        /* If we find a slash */
        else if ( path[ pos ] == '/' )
        {
            if ( level > 0 )
            {
                level--;
                /* Cut piece out */
                if ( level == 0 )
                {
                    size_t diff = cut_end - pos;
                    size_t copy = pos;
                    len -= diff;
                    for ( copy = pos; copy <= len; copy++ )
                    {
                        path[ copy ] = path[ copy + diff ];
                    }
                    cut_end = pos;
                }
            }
            else
            {
                cut_end = pos;
            }
        }
    }

    /* If string does not start with a slash, process first path item */
    if ( ( path[ 0 ] != '/' ) && ( level > 0 ) )
    {
        /* Sepcial handling for path starting with '../' */
        if ( ( path[ 0 ] == '.' ) &&
             ( path[ 1 ] == '.' ) &&
             ( path[ 2 ] == '/' ) )
        {
            level++;
        }
        /* Ignore leading './' */
        else if ( ( path[ 0 ] != '.' ) ||
                  ( path[ 1 ] != '/' ) )
        {
            level--;
        }

        /* start with leading ../ seqence for each remaining level */
        pos = 0;
        while ( pos < level * 3 )
        {
            path[ pos ]     = '.';
            path[ pos + 1 ] = '.';
            path[ pos + 2 ] = '/';
            pos            += 3;
        }

        /* Cut piece out */
        cut_end++;  /* omit leading slash */
        size_t  diff = cut_end - pos;
        int64_t copy;
        len -= diff;
        for ( copy = pos; copy <= len; copy++ )
        {
            path[ copy ] = path[ copy + diff ];
        }
        if ( len < 0 )
        {
            path[ 0 ] = '\0';
        }
    }

    /* Add trailing slash again */
    if ( has_trailing_slash )
    {
        path[ len ]     = '/';
        path[ len + 1 ] = '\0';
    }

    /* Add starting slashed if string empty */
    if ( has_starting_slash && ( path[ 0 ] == '\0' ) )
    {
        path[ 0 ] = '/';
        path[ 1 ] = '\0';
    }

    /* Ensure it still has slashes if it had some before */
    if ( has_slashes && !MANGLE_NAME( IO_HasPath ) ( path ) && ( path[ 0 ] != '\0' ) )
    {
        for ( pos = len; pos >= 0; pos-- )
        {
            path[ pos + 2 ] = path[ pos ];
        }
        path[ 0 ] = '.';
        path[ 1 ] = '/';
    }
}

char*
MANGLE_NAME( IO_JoinPath ) ( int nPaths, ... )
{
    char*       result_path;
    size_t      total_length  = 0;
    const char* sep           = "";
    int         last_absolute = 0;
    int         i;

    va_list paths;

    va_start( paths, nPaths );
    for ( int i = 0; i < nPaths; i++ )
    {
        const char* path = va_arg( paths, const char* );
        if ( !path )
        {
            return NULL;
        }

        /* ignore empty path elements */
        size_t path_len = strlen( path );
        if ( 0 == path_len )
        {
            continue;
        }

        /* check if path is absolute */
        if ( path[ 0 ] == '/' )
        {
            /* Yes, reset counters, and remeber the position */
            total_length  = 0;
            sep           = "";
            last_absolute = i;
        }

        total_length += strlen( sep ) + path_len;
        sep           = "/";
    }
    va_end( paths );

    /* include the terminating \0 */
    total_length += 1;

    result_path = malloc( total_length * sizeof( char ) );
    if ( !result_path )
    {
        return NULL;
    }

    va_start( paths, nPaths );
    total_length = 0;
    sep          = "";
    for ( int i = 0; i < nPaths; i++ )
    {
        const char* path = va_arg( paths, const char* );

        /* ignore path element, if an absolute path comes after */
        if ( i < last_absolute )
        {
            continue;
        }

        size_t path_len = strlen( path );
        if ( 0 == path_len )
        {
            continue;
        }

        strcpy( result_path + total_length, sep );
        total_length += strlen( sep );
        sep           = "/";

        strcpy( result_path + total_length, path );
        total_length += path_len;
    }
    va_end( paths );

    /* always terminate result string */
    result_path[ total_length ] = 0;

    return result_path;
}

/* Declare gethostname if we can link against it but it is not declared */
#if ( !HAVE( DECL_GETHOSTNAME ) ) && HAVE( GETHOSTNAME )
int
gethostname( char*  name,
             size_t namelen );

#endif

int
MANGLE_NAME( IO_GetHostname ) ( char* name, size_t namelen )
{
#if HAVE( DECL_GETHOSTNAME ) || HAVE( GETHOSTNAME )
    return gethostname( name, namelen );
#else

    char* hostname = getenv( "HOST" );
    if ( ( hostname == NULL ) || ( *hostname == '\0' ) )
    {
        return -1;
    }

    size_t len = strlen( hostname ) + 1; /* For terminating zero */
    if ( len > namelen )
    {
        len = namelen;
    }
    strncpy( name, hostname, len );
    return 0;
#endif
}

/* Declare getcwd if we can link against it but it is not declared */
#if ( !HAVE( DECL_GETCWD ) ) && HAVE( GETCWD )
char*
getcwd( char*  buf,
        size_t size );

#endif


char*
MANGLE_NAME( IO_GetCwd ) ( char* buf, size_t size )
{
#if HAVE( DECL_GETCWD ) || HAVE( GETCWD )
    return getcwd( buf, size );
#else
    char* cwd = getenv( "PWD" );
    if ( cwd == NULL )
    {
        errno = EACCES;
        return NULL;
    }
    if ( buf == NULL )
    {
        return MANGLE_NAME( CStr_dup ) ( cwd );
    }

    if ( size == 0 )
    {
        errno = EINVAL;
        return NULL;
    }

    size_t len = strlen( cwd );
    if ( len + 1 < size )
    {
        errno = ERANGE;
        return NULL;
    }

    strcpy( buf, cwd );
    return buf;
#endif
}
