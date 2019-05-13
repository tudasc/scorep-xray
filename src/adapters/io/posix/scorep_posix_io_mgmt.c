/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    POSIX_IO_Wrapper
 *
 * @brief MGMT for the POSIX I/O adapter
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include "scorep_posix_io.h"

#include <unistd.h>

#if HAVE( GETRLIMIT )
#include <sys/resource.h>
#endif

#include <SCOREP_IoManagement.h>

#define SCOREP_DEBUG_MODULE_NAME IO
#include <UTILS_Debug.h>

SCOREP_IoHandleHandle scorep_posix_io_sync_all_handle = SCOREP_INVALID_IO_HANDLE;

#if HAVE( POSIX_AIO_SUPPORT )

/** Hash table for tracking asynchronous I/O requests */
#define AIO_REQUEST_TABLE_SIZE 16

SCOREP_Hashtab* scorep_posix_io_aio_request_table       = NULL;
SCOREP_Mutex    scorep_posix_io_aio_request_table_mutex = SCOREP_INVALID_MUTEX;

#endif

/* *******************************************************************
 * Translate POSIX I/O types to Score-P representative
 * ******************************************************************/

SCOREP_IoAccessMode
scorep_posix_io_get_scorep_io_access_mode( int mode )
{
    switch ( mode & ( O_RDONLY | O_WRONLY | O_RDWR ) )
    {
        case O_RDONLY:
            return SCOREP_IO_ACCESS_MODE_READ_ONLY;
        case O_WRONLY:
            return SCOREP_IO_ACCESS_MODE_WRITE_ONLY;
        case O_RDWR:
            return SCOREP_IO_ACCESS_MODE_READ_WRITE;
    }

    return SCOREP_IO_ACCESS_MODE_NONE;
}

void
scorep_posix_io_get_scorep_io_flags( int                    flags,
                                     SCOREP_IoCreationFlag* creationFlags,
                                     SCOREP_IoStatusFlag*   statusFlags )
{
    if ( creationFlags != NULL )
    {
        *creationFlags = SCOREP_IO_CREATION_FLAG_NONE;
        /******************************************************************************
         * Handle creation flags
         *****************************************************************************/
        if ( flags & O_CREAT )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_CREATE;
        }
        /**
         * 'O_DIRECTORY' specified in POSIX.1-2008
         */
#ifdef O_DIRECTORY
        if ( flags & O_DIRECTORY )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_DIRECTORY;
        }
#endif
        if ( flags & O_EXCL )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_EXCLUSIVE;
        }
        /**
         *  'O_LARGEFILE' depends on definition of '__USE_LARGEFILE64'
         */
#ifdef O_LARGEFILE
        if ( flags & O_LARGEFILE )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_LARGEFILE;
        }
#endif
        if ( flags & O_NOCTTY )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_NO_CONTROLLING_TERMINAL;
        }
        /**
         * 'O_NOFOLLOW' specified in POSIX.1-2008
         */
#ifdef O_NOFOLLOW
        if ( flags & O_NOFOLLOW )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_NO_FOLLOW;
        }
#endif
        /**
         * 'O_PATH' depends on definition of '__USE_GNU'
         */
#ifdef O_PATH
        if ( flags & O_PATH )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_PATH;
        }
#endif
        /**
         * 'O_TMPFILE' depends on definition of '__USE_GNU'
         */
#ifdef O_TMPFILE
        if ( flags & O_TMPFILE )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_TEMPORARY_FILE;
        }
#endif
        if ( flags & O_TRUNC )
        {
            *creationFlags |= SCOREP_IO_CREATION_FLAG_TRUNCATE;
        }
    }
    if ( statusFlags != NULL )
    {
        *statusFlags = SCOREP_IO_STATUS_FLAG_NONE;
        /******************************************************************************
         * Handle status flags
         *****************************************************************************/
        if ( flags & O_APPEND )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_APPEND;
        }
        if ( flags & O_ASYNC )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_ASYNC;
        }
        /**
         * O_CLOEXEC specified in POSIX.1-2008
         */
#ifdef O_CLOEXEC
        if ( flags & O_CLOEXEC )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_CLOSE_ON_EXEC;
        }
#endif
        /**
         * 'O_DIRECT' depends on definition of '__USE_GNU'
         */
#ifdef O_DIRECT
        if ( flags & O_DIRECT )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_AVOID_CACHING;
        }
#endif
        if ( flags & O_DSYNC )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_DATA_SYNC;
        }
        /**
         * 'O_NOATIME' depends on definition of '__USE_GNU'
         */
#ifdef O_NOATIME
        if ( flags & O_NOATIME )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_NO_ACCESS_TIME;
        }
#endif
        if ( flags & O_NONBLOCK )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_NON_BLOCKING;
        }
        if ( flags & O_NDELAY )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_NON_BLOCKING;
        }
        if ( flags & O_SYNC )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_SYNC;
        }
    }
}

bool
scorep_posix_io_get_scorep_io_flags_from_fd( int                    fd,
                                             SCOREP_IoAccessMode*   accessMode,
                                             SCOREP_IoCreationFlag* creationFlags,
                                             SCOREP_IoStatusFlag*   statusFlags )
{
    if ( accessMode || creationFlags || statusFlags )
    {
        int flags = SCOREP_LIBWRAP_FUNC_CALL( fcntl, ( fd, F_GETFL ) );
        if ( flags == -1 )
        {
            return false;
        }
        scorep_posix_io_get_scorep_io_flags( flags, creationFlags, statusFlags );

    #if defined( F_GETFD ) && defined( FD_CLOEXEC )
        flags = SCOREP_LIBWRAP_FUNC_CALL( fcntl, ( fd, F_GETFD ) );
        if ( flags != -1 && flags & FD_CLOEXEC )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_CLOSE_ON_EXEC;
        }
    #endif
        if ( accessMode != NULL )
        {
            *accessMode = scorep_posix_io_get_scorep_io_access_mode( flags );
        }
        return true;
    }
    return false;
}

SCOREP_IoSeekOption
scorep_posix_io_get_scorep_io_seek_option( int whence )
{
    SCOREP_IoSeekOption scorep_seek_option = SCOREP_IO_SEEK_INVALID;

    switch ( whence )
    {
        case SEEK_SET:
            scorep_seek_option = SCOREP_IO_SEEK_FROM_START;
            break;
        case SEEK_CUR:
            scorep_seek_option = SCOREP_IO_SEEK_FROM_CURRENT;
            break;
        case SEEK_END:
            scorep_seek_option = SCOREP_IO_SEEK_FROM_END;
            break;

#ifdef SEEK_DATA
        case SEEK_DATA:
            scorep_seek_option = SCOREP_IO_SEEK_DATA;
            break;
#endif
#ifdef SEEK_HOLE
        case SEEK_HOLE:
            scorep_seek_option = SCOREP_IO_SEEK_HOLE;
            break;
#endif

        default:
            UTILS_BUG( "Unsupported seek option (%d) in POSIX I/O call.", whence );
    }
    return scorep_seek_option;
}

static inline int
get_fd_name( int fd, char* buf, size_t buflen )
{
    int ret = 0;
    #define STR( v ) #v

    UTILS_BUG_ON( strlen( STR( STDERR_FILENO ) ) > buflen,
                  "Provided buffer is too small" );

    switch ( fd )
    {
        case STDIN_FILENO:
            strcpy( buf, STR( STDIN_FILENO ) );
            break;
        case STDOUT_FILENO:
            strcpy( buf, STR( STDOUT_FILENO ) );
            break;
        case STDERR_FILENO:
            strcpy( buf, STR( STDERR_FILENO ) );
            break;
        default:
            if ( !isatty( fd ) )
            {
                return 1;
            }
            ret = ttyname_r( fd, buf, buflen );
            if ( ret != 0 )
            {
                if ( errno == ERANGE )
                {
                    UTILS_BUG( "Provided buffer is too small" );
                }
                else
                {
                    UTILS_WARNING( "Could not determine name of fd %d", fd );
                }
            }
    }

    #undef STR

    return ret;
}

/* *******************************************************************
 * Internal management routine
 * ******************************************************************/

/**
 * Create definition handles for default stdin/stdout/stderr streams.
 */
void
scorep_posix_io_init( void )
{
    SCOREP_IoMgmt_RegisterParadigm( SCOREP_IO_PARADIGM_POSIX,
                                    SCOREP_IO_PARADIGM_CLASS_SERIAL,
                                    "POSIX I/O",
                                    SCOREP_IO_PARADIGM_FLAG_OS,
                                    sizeof( int ),
                                    SCOREP_INVALID_IO_PARADIGM_PROPERTY );

    int nofile = 1024;
#if HAVE( GETRLIMIT )
    struct rlimit res_nofile;
    int           res = getrlimit( RLIMIT_NOFILE, &res_nofile );
    if ( 0 == res )
    {
        nofile = res_nofile.rlim_cur;
    }
#endif
    for ( int fd = 0; fd < nofile; fd++ )
    {
        SCOREP_IoHandleHandle handle = SCOREP_INVALID_IO_HANDLE;
        SCOREP_IoAccessMode   access_mode;
        SCOREP_IoCreationFlag creation_flags;
        SCOREP_IoStatusFlag   status_flags;

        if ( !scorep_posix_io_get_scorep_io_flags_from_fd( fd, &access_mode, &creation_flags, &status_flags ) )
        {
            continue;
        }

        char fd_name[ 256 ];
        if ( get_fd_name( fd, fd_name, 256 ) )
        {
            fd_name[ 0 ] = '\0';
        }

        SCOREP_IoMgmt_CreatePreCreatedHandle( SCOREP_IO_PARADIGM_POSIX,
                                              SCOREP_INVALID_IO_FILE,
                                              SCOREP_IO_HANDLE_FLAG_PRE_CREATED,
                                              access_mode,
                                              status_flags,
                                              SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                              fd + 1 /* avoid zero as value */,
                                              fd_name,
                                              &fd );
    }

    scorep_posix_io_sync_all_handle =
        SCOREP_Definitions_NewIoHandle( "sync - commit buffer cache to disk",
                                        SCOREP_INVALID_IO_FILE,
                                        SCOREP_IO_PARADIGM_POSIX,
                                        SCOREP_IO_HANDLE_FLAG_PRE_CREATED | SCOREP_IO_HANDLE_FLAG_ALL_PROXY,
                                        SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                        SCOREP_INVALID_IO_HANDLE,
                                        1 /* unify all I/O handles into one */,
                                        true,
                                        0,
                                        NULL,
                                        SCOREP_IO_ACCESS_MODE_READ_WRITE,
                                        SCOREP_IO_STATUS_FLAG_NONE );

#if HAVE( POSIX_AIO_SUPPORT )
    scorep_posix_io_aio_request_table =
        SCOREP_Hashtab_CreateSize( AIO_REQUEST_TABLE_SIZE,
                                   SCOREP_Hashtab_HashPointer,
                                   SCOREP_Hashtab_ComparePointer );

    SCOREP_ErrorCode err = SCOREP_MutexCreate( &scorep_posix_io_aio_request_table_mutex );
    UTILS_BUG_ON( err != SCOREP_SUCCESS,
                  "Mutex could not be created for asynchronous I/O requests" );
#endif
}

void
scorep_posix_io_fini( void )
{
    SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IO_PARADIGM_POSIX );

#if HAVE( POSIX_AIO_SUPPORT )
    SCOREP_Hashtab_Free( scorep_posix_io_aio_request_table );
    SCOREP_MutexDestroy( &scorep_posix_io_aio_request_table_mutex );
#endif
}
