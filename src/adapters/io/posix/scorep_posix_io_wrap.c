/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
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
 * @brief C interface wrappers for POSIX I/O routines
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

/* We need some defines to wrap 'fcntl' */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <SCOREP_Events.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_IoManagement.h>
#include <SCOREP_Mutex.h>

#define SCOREP_DEBUG_MODULE_NAME IO
#include <UTILS_Debug.h>

#include "scorep_posix_io_regions.h"
#include "scorep_posix_io_function_pointers.h"

/* *INDENT-OFF* */
#ifdef SCOREP_LIBWRAP_SHARED
#define INITIALIZE_FUNCTION_POINTER( func )                                \
    do                                                                     \
    {                                                                      \
        if ( !SCOREP_LIBWRAP_FUNC_REAL_NAME( func ))                       \
        {                                                                  \
            scorep_posix_io_early_init_function_pointers();                \
            UTILS_BUG_ON( SCOREP_LIBWRAP_FUNC_REAL_NAME( func )== NULL,    \
                          "Cannot obtain address of symbol: " #func "." ); \
        }                                                                  \
    } while ( 0 )
#else
#define INITIALIZE_FUNCTION_POINTER( func ) do { } while ( 0 )
#endif
/* *INDENT-ON* */

/** Artificial I/O handle representing all currently active I/O handles */
static SCOREP_IoHandleHandle io_sync_all_handle = SCOREP_INVALID_IO_HANDLE;


#if HAVE( POSIX_AIO_SUPPORT )

/** Hash table for tracking asynchronous I/O requests */
#define AIO_REQUEST_TABLE_SIZE 16

static SCOREP_Hashtab* aio_request_table;
SCOREP_Mutex           aio_request_table_mutex;

#endif

/* *******************************************************************
 * Translate POSIX types to Score-P representative
 * ******************************************************************/

/**
 * Translate the POSIX mode of an open operation to its Score-P equivalent.
 *
 * @param mode      POSIX mode of the open operation (e.g., O_RDONLY, O_WRONLY, O_RDWR).
 *
 * @return Score-P equivalent of the POSIX mode.
 */
static inline SCOREP_IoAccessMode
get_scorep_io_access_mode( int mode )
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

/**
 * Translate the POSIX flags of an I/O operation to their Score-P equivalents.
 *
 * @param[in]  flags            POSIX flags of the I/O operation (e.g., O_APPEND).
 * @param[out] creationFlags    Score-P equivalents of the POSIX flags set at file creation.
 * @param[out] statusFlags      Score-P equivalents of the POSIX flags which might be changed during lifetime of the I/O handle.
 */
static inline void
get_scorep_io_flags( int                    flags,
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

static inline bool
get_scorep_io_flags_from_fd( int                    fd,
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
        get_scorep_io_flags( flags, creationFlags, statusFlags );

    #if defined( F_GETFD ) && defined( FD_CLOEXEC )
        flags = SCOREP_LIBWRAP_FUNC_CALL( fcntl, ( fd, F_GETFD ) );
        if ( flags != -1 && flags & FD_CLOEXEC )
        {
            *statusFlags |= SCOREP_IO_STATUS_FLAG_CLOSE_ON_EXEC;
        }
    #endif
        if ( accessMode != NULL )
        {
            *accessMode = get_scorep_io_access_mode( flags );
        }
        return true;
    }
    return false;
}

/**
 * Translate the POSIX seek option to its Score-P equivalent.
 *
 * @param whence        Option of a POSIX seek operation.
 *
 * @return Score-P equivalent of the POSIX mode.
 */
static inline SCOREP_IoSeekOption
get_scorep_io_seek_option( int whence )
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

static inline
int
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

        if ( !get_scorep_io_flags_from_fd( fd, &access_mode, &creation_flags, &status_flags ) )
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

    io_sync_all_handle = SCOREP_Definitions_NewIoHandle( "sync - commit buffer cache to disk",
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
    aio_request_table = SCOREP_Hashtab_CreateSize( AIO_REQUEST_TABLE_SIZE,
                                                   SCOREP_Hashtab_HashPointer,
                                                   SCOREP_Hashtab_ComparePointer );

    SCOREP_ErrorCode err = SCOREP_MutexCreate( &aio_request_table_mutex );
    UTILS_BUG_ON( err != SCOREP_SUCCESS,
                  "Mutex could not be created for asynchronous I/O requests" );
#endif
}

void
scorep_posix_io_fini( void )
{
    SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IO_PARADIGM_POSIX );

#if HAVE( POSIX_AIO_SUPPORT )
    SCOREP_Hashtab_Free( aio_request_table );
    SCOREP_MutexDestroy( &aio_request_table_mutex );
#endif
}

/* *******************************************************************
 * Function wrappers
 * ******************************************************************/

/*
 * Synchronous I/O
 */

#if HAVE( POSIX_IO_SYMBOL_CLOSE )
int
SCOREP_LIBWRAP_FUNC_NAME( close )( int fd )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( close );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_close );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                       &fd );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX,
                                        &fd );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( close,
                                        ( fd ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoMgmt_PopHandle( handle );

            if ( ret != 0 )
            {
                SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_POSIX, handle );
            }
            else
            {
                SCOREP_IoMgmt_DestroyHandle( handle );
            }
            SCOREP_IoDestroyHandle( handle );
        }

        SCOREP_ExitRegion( scorep_posix_io_region_close );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( close,
                                        ( fd ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_CLOSEDIR )
int
SCOREP_LIBWRAP_FUNC_NAME( closedir )( DIR* dirp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( closedir );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_closedir );

        SCOREP_IoHandleHandle handle = SCOREP_INVALID_IO_HANDLE;
        int                   fd     = dirfd( dirp );

        if ( fd != -1 )
        {
            handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                     &fd );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX,
                                            &fd );
            }
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( closedir,
                                        ( dirp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoMgmt_PopHandle( handle );

            if ( ret != 0 )
            {
                SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_POSIX, handle );
            }
            else
            {
                SCOREP_IoMgmt_DestroyHandle( handle );
            }
            SCOREP_IoDestroyHandle( handle );
        }

        SCOREP_ExitRegion( scorep_posix_io_region_closedir );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( closedir,
                                        ( dirp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_CREAT )
int
SCOREP_LIBWRAP_FUNC_NAME( creat )( const char* pathname, mode_t mode )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( creat );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_creat );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( creat,
                                        ( pathname, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != -1 )
        {
            SCOREP_IoFileHandle   file   = SCOREP_IoMgmt_GetIoFileHandle( pathname );
            SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_POSIX, file, &ret );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                int                   flags = O_CREAT | O_WRONLY | O_TRUNC;
                SCOREP_IoCreationFlag creation_flags;
                SCOREP_IoStatusFlag   status_flags;
                get_scorep_io_flags( flags, &creation_flags, &status_flags );

                SCOREP_IoCreateHandle( handle,
                                       get_scorep_io_access_mode( flags ),
                                       creation_flags,
                                       status_flags );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }


        SCOREP_ExitRegion( scorep_posix_io_region_creat );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( creat,
                                        ( pathname, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_CREAT64 )
int
SCOREP_LIBWRAP_FUNC_NAME( creat64 )( const char* pathname, mode_t mode )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( creat64 );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_creat64 );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( creat64,
                                        ( pathname, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != -1 )
        {
            SCOREP_IoFileHandle   file   = SCOREP_IoMgmt_GetIoFileHandle( pathname );
            SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_POSIX, file, &ret );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                int                   flags = O_CREAT | O_WRONLY | O_TRUNC;
                SCOREP_IoCreationFlag creation_flags;
                SCOREP_IoStatusFlag   status_flags;
                get_scorep_io_flags( flags, &creation_flags, &status_flags );

                SCOREP_IoCreateHandle( handle,
                                       get_scorep_io_access_mode( flags ),
                                       creation_flags,
                                       status_flags );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }

        SCOREP_ExitRegion( scorep_posix_io_region_creat64 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( creat64,
                                        ( pathname, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_DUP )
int
SCOREP_LIBWRAP_FUNC_NAME( dup )( int oldfd )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( dup );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_dup );

        SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &oldfd );

        if ( old_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX,
                                                  old_handle, 0 );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( dup,
                                        ( oldfd ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( old_handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( ret != -1 )
            {
                SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
                    SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &ret );
                if ( new_handle != SCOREP_INVALID_IO_HANDLE )
                {
                    SCOREP_IoStatusFlag flags = SCOREP_IO_STATUS_FLAG_NONE;
                    if ( !get_scorep_io_flags_from_fd( oldfd, NULL, NULL, &flags ) )
                    {
                        UTILS_WARNING( "Flags for oldfd are not determined" );
                    }

                    SCOREP_IoDuplicateHandle( old_handle,
                                              new_handle,
                                              flags );
                }
            }
            else
            {
                SCOREP_IoMgmt_DropIncompleteHandle();
            }
        }


        SCOREP_ExitRegion( scorep_posix_io_region_dup );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( dup,
                                        ( oldfd ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}

#endif

#if HAVE( POSIX_IO_SYMBOL_DUP2 )
int
SCOREP_LIBWRAP_FUNC_NAME( dup2 )( int oldfd, int newfd )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( dup2 );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_dup2 );

        SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &newfd );
        SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &oldfd );

        if ( old_handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( new_handle != SCOREP_INVALID_IO_HANDLE )
            {
                // newfd is still open
                if ( oldfd != newfd )
                {
                    // newfd will be closed
                    SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &newfd );
                    SCOREP_IoMgmt_DestroyHandle( new_handle );
                    SCOREP_IoDestroyHandle( new_handle );
                    SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
                }
                // else
                // oldfd == newfd dup2 will return newfd and does nothing
                // the FD_CLOEXEC file descriptor flag is cleared for newfd
            }
            else
            {
                SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
            }
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( dup2,
                                        ( oldfd, newfd ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( oldfd != newfd && old_handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( ret != -1 )
            {
                new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
                    SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &newfd );
                if ( new_handle != SCOREP_INVALID_IO_HANDLE )
                {
                    SCOREP_IoStatusFlag flags       = SCOREP_IO_STATUS_FLAG_NONE;
                    int                 posix_flags = SCOREP_LIBWRAP_FUNC_CALL( fcntl, ( ret, F_GETFL, 0 ) );

                    if ( posix_flags >= 0 )
                    {
                        get_scorep_io_flags( posix_flags, NULL, &flags );
                    }
                    else
                    {
                        UTILS_WARNING( "Flags for oldfd are not determined" );
                    }
                    SCOREP_IoDuplicateHandle( old_handle,
                                              new_handle,
                                              flags );
                }
            }
            else
            {
                SCOREP_IoMgmt_DropIncompleteHandle();
            }
        }
        SCOREP_ExitRegion( scorep_posix_io_region_dup2 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( dup2,
                                        ( oldfd, newfd ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_DUP3 )
int
SCOREP_LIBWRAP_FUNC_NAME( dup3 )( int oldfd, int newfd, int flags )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( dup3 );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_dup3 );

        SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &newfd );
        SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &oldfd );

        if ( old_handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( new_handle != SCOREP_INVALID_IO_HANDLE )
            {
                // If oldfd equals newfd, then dup3() fails with the error EINVAL.
                SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &newfd );
                SCOREP_IoMgmt_DestroyHandle( new_handle );
                SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
            }
            else
            {
                SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
            }
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( dup3,
                                        ( oldfd, newfd, flags ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( old_handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( ret != -1 )
            {
                new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
                    SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &newfd );
                if ( new_handle != SCOREP_INVALID_IO_HANDLE )
                {
                    SCOREP_IoStatusFlag status_flags = SCOREP_IO_STATUS_FLAG_NONE;

                    get_scorep_io_flags( flags, NULL, &status_flags );

                    SCOREP_IoDuplicateHandle( old_handle,
                                              new_handle,
                                              status_flags );
                }
            }
            else
            {
                SCOREP_IoMgmt_DropIncompleteHandle();
            }
        }


        SCOREP_ExitRegion( scorep_posix_io_region_dup3 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( dup3,
                                        ( oldfd, newfd, flags ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FCNTL )
int
SCOREP_LIBWRAP_FUNC_NAME( fcntl )( int fd, int cmd, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fcntl );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fcntl );
    }

    int                   int_arg;
    void*                 pointer_arg;
    SCOREP_IoHandleHandle new_handle;
    SCOREP_IoFileHandle   file_handle;
    SCOREP_IoHandleHandle old_handle = SCOREP_INVALID_IO_HANDLE;

    va_list args;
    va_start( args, cmd );
    switch ( cmd )
    {
        /*
         * Calls to fcntl without var arg parameter
         */
        case F_GETFD:      /* (void) */
        case F_GETFL:      /* (void) */
        case F_GETOWN:     /* (void) */
#ifdef F_GETSIG
        case F_GETSIG:     /* (void) */
#endif
#ifdef F_GETLEASE
        case F_GETLEASE:   /* (void) */
#endif
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ: /* (void) (since Linux 2.6.35) */
#endif                     /* F_GETPIPE_SZ */
#ifdef F_GET_SEALS
        case F_GET_SEALS:  /* (void) (since Linux 3.17) */
#endif                     /* F_GET_SEALS */
            {
                if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
                {
                    old_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
                    SCOREP_ENTER_WRAPPED_REGION();
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd /* no var arg */ ) );
                    SCOREP_EXIT_WRAPPED_REGION();
                    SCOREP_IoMgmt_PopHandle( old_handle );
                }
                else
                {
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd /* no var arg */ ) );
                }
                break;
            }

        /*
         * Calls to fcntl with var arg parameter of type int
         */
        case F_DUPFD:         /* (int) */
#ifdef F_DUPFD_CLOEXEC
        case F_DUPFD_CLOEXEC: /* (int) (since Linux 2.6.24) */
#endif                        /* F_DUPFD_CLOEXEC */
            {
                int_arg = va_arg( args, int );
                if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
                {
                    old_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
                    if ( fd != int_arg )
                    {
                        SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
                    }
                    SCOREP_ENTER_WRAPPED_REGION();
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd, int_arg ) );
                    SCOREP_EXIT_WRAPPED_REGION();
                }
                else
                {
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd, int_arg ) );
                }
                break;
            }
        case F_SETFD:      /* (int) */
        case F_SETFL:      /* (int) */
        case F_SETOWN:     /* (int) */
#ifdef F_SETSIG
        case F_SETSIG:     /* (int) */
#endif
#ifdef F_SETLEASE
        case F_SETLEASE:   /* (int) */
#endif
#ifdef F_NOTIFY
        case F_NOTIFY:     /* (int) */
#endif
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ: /* (int) (since Linux 2.6.35) */
#endif                     /* F_SETPIPE_SZ */
#ifdef F_ADD_SEALS
        case F_ADD_SEALS:  /* (int) (since Linux 3.17) */
#endif                     /* F_ADD_SEALS */
            {
                int_arg = va_arg( args, int );
                if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
                {
                    old_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
                    SCOREP_ENTER_WRAPPED_REGION();
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd, int_arg ) );
                    SCOREP_EXIT_WRAPPED_REGION();
                    SCOREP_IoMgmt_PopHandle( old_handle );
                }
                else
                {
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd, int_arg ) );
                }
                break;
            }

        /*
         * Calls to fcntl with var arg parameter of type (struct flock *)
         */
        case F_SETLK:      /* (struct flock *) */
        case F_SETLKW:     /* (struct flock *) */
        case F_GETLK:      /* (struct flock *) */
#ifdef F_OFD_SETLK
        case F_OFD_SETLK:  /* (struct flock *) */
#endif                     /* F_OFD_SETLK */
#ifdef F_OFD_SETLKW
        case F_OFD_SETLKW: /* (struct flock *) */
#endif                     /* F_OFD_SETLKW */
#ifdef F_OFD_GETLK
        case F_OFD_GETLK:  /* (struct flock *) */
#endif                     /* F_OFD_GETLK */

            /*
             * Calls to fcntl with var arg parameter of type (struct flock64 *)
             */
#if defined( F_SETLK64 ) && F_SETLK64 != F_SETLK
        case F_SETLK64:  /* (struct flock64 *) */
#endif                   /* defined( F_SETLK64 ) && F_SETLK64 != F_SETLK */
#if defined( F_SETLKW64 ) && F_SETLKW64 != F_SETLKW
        case F_SETLKW64: /* (struct flock64 *) */
#endif                   /* defined( F_SETLKW64 ) && F_SETLKW64 != F_SETLKW */
#if defined( F_GETLK64 ) && F_GETLK64 != F_GETLK
        case F_GETLK64:  /* (struct flock64 *) */
#endif                   /* defined( F_GETLK64 ) && F_GETLK64 != F_GETLK */

            /*
             * Calls to fcntl with var arg parameter of type (struct f_owner_ex *)
             */
#ifdef F_GETOWN_EX
        case F_GETOWN_EX: /* (struct f_owner_ex *) (since Linux 2.6.32) */
#endif                    /* F_GETOWN_EX */
#ifdef F_SETOWN_EX
        case F_SETOWN_EX: /* (struct f_owner_ex *) (since Linux 2.6.32) */
#endif                    /* F_SETOWN_EX */

#ifdef F_GETOWNER_UIDS
        case F_GETOWNER_UIDS: /* (uid_t *) (since Linux 3.6) */
#endif                        /* F_GETOWNER_UIDS */
            {
                pointer_arg = va_arg( args, void* );
                if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
                {
                    old_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
                    SCOREP_ENTER_WRAPPED_REGION();
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd, pointer_arg ) );
                    SCOREP_EXIT_WRAPPED_REGION();
                    SCOREP_IoMgmt_PopHandle( old_handle );
                }
                else
                {
                    ret = SCOREP_LIBWRAP_FUNC_CALL( fcntl,
                                                    ( fd, cmd, pointer_arg ) );
                }
                break;
            }

        default:
            UTILS_BUG( "Unhandled command in fcntl wrapper." );
    }

    va_end( args );

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( cmd == F_DUPFD
#ifdef F_DUPFD_CLOEXEC
             || cmd == F_DUPFD_CLOEXEC
#endif  /* F_DUPFD_CLOEXEC */
             )
        {
            if ( fd != int_arg && old_handle != SCOREP_INVALID_IO_HANDLE )
            {
                if ( ret != -1 )
                {
                    new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
                        SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &ret );
                    if ( new_handle != SCOREP_INVALID_IO_HANDLE )
                    {
                        SCOREP_IoStatusFlag flags     = SCOREP_IO_STATUS_FLAG_NONE;
                        int                 old_flags = SCOREP_LIBWRAP_FUNC_CALL( fcntl, ( fd, F_GETFL, 0 ) );

                        if ( old_flags >= 0 )
                        {
                            get_scorep_io_flags( old_flags, NULL, &flags );
                        }
                        else
                        {
                            UTILS_WARNING( "Flags for oldfd are not determined" );
                        }
                        SCOREP_IoDuplicateHandle( old_handle,
                                                  new_handle,
                                                  flags );
                    }
                }
                else
                {
                    SCOREP_IoMgmt_DropIncompleteHandle();
                }
            }
        }
        SCOREP_ExitRegion( scorep_posix_io_region_fcntl );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FDATASYNC )
int
SCOREP_LIBWRAP_FUNC_NAME( fdatasync )( int fd )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fdatasync );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fdatasync );
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_FLUSH,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fdatasync,
                                        ( fd ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_FLUSH,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fdatasync );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fdatasync,
                                        ( fd ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FSYNC )
int
SCOREP_LIBWRAP_FUNC_NAME( fsync )( int fd )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fsync );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fsync );

        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_FLUSH,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fsync,
                                        ( fd ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_FLUSH,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fsync );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fsync,
                                        ( fd ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_LOCKF )
int
SCOREP_LIBWRAP_FUNC_NAME( lockf )( int fd, int cmd, off_t len )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( lockf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_lockf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
        if ( handle != SCOREP_INVALID_IO_HANDLE && cmd == F_ULOCK )
        {
            SCOREP_IoReleaseLock( handle, SCOREP_LOCK_EXCLUSIVE );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( lockf,
                                        ( fd, cmd, len ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            switch ( cmd )
            {
                case F_LOCK:
                    SCOREP_IoAcquireLock( handle, SCOREP_LOCK_EXCLUSIVE );
                    break;
                case F_TLOCK:
                    if ( ret == 0 )
                    {
                        SCOREP_IoAcquireLock( handle, SCOREP_LOCK_EXCLUSIVE );
                    }
                    else
                    {
                        SCOREP_IoTryLock( handle, SCOREP_LOCK_EXCLUSIVE );
                    }
                    break;
            }
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_lockf );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( lockf,
                                        ( fd, cmd, len ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_LSEEK )
off_t
SCOREP_LIBWRAP_FUNC_NAME( lseek )( int fd, off_t offset, int whence )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( lseek );
    off_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_lseek );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( lseek,
                                        ( fd, offset, whence ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           ( int64_t )offset,
                           get_scorep_io_seek_option( whence ),
                           ( uint64_t )ret );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_lseek );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( lseek,
                                        ( fd, offset, whence ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_LSEEK64 )
scorep_off64_t
SCOREP_LIBWRAP_FUNC_NAME( lseek64 )( int fd, scorep_off64_t offset, int whence )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( lseek64 );
    scorep_off64_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_lseek64 );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX, &fd );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( lseek64,
                                        ( fd, offset, whence ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           ( int64_t )offset,
                           get_scorep_io_seek_option( whence ),
                           ( uint64_t )ret );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_lseek64 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( lseek64,
                                        ( fd, offset, whence ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_OPEN )
int
SCOREP_LIBWRAP_FUNC_NAME( open )( const char* pathname, int flags, ... /* mode_t mode */ )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( open );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_open );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );
        mode_t mode = 0;
        if ( ( flags & O_CREAT )
#ifdef O_TMPFILE
             || ( flags & O_TMPFILE )
#endif
             )
        {
            va_list va;
            va_start( va, flags );
            mode = va_arg( va, mode_t );
            va_end( va );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( open, ( pathname, flags, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != -1 )
        {
            SCOREP_IoFileHandle   file   = SCOREP_IoMgmt_GetIoFileHandle( pathname );
            SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_POSIX, file, &ret );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreationFlag creation_flags;
                SCOREP_IoStatusFlag   status_flags;
                get_scorep_io_flags( flags, &creation_flags, &status_flags );
                /* @todo handle mode, write as attribute? */
                SCOREP_IoCreateHandle( handle,
                                       get_scorep_io_access_mode( flags ),
                                       creation_flags,
                                       status_flags );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }


        SCOREP_ExitRegion( scorep_posix_io_region_open );
    }
    else
    {
        mode_t mode = 0;
        if ( ( flags & O_CREAT )
#ifdef O_TMPFILE
             || ( flags & O_TMPFILE )
#endif
             )
        {
            va_list va;
            va_start( va, flags );
            mode = va_arg( va, mode_t );
            va_end( va );
        }
        ret = SCOREP_LIBWRAP_FUNC_CALL( open, ( pathname, flags, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_OPEN64 )
int
SCOREP_LIBWRAP_FUNC_NAME( open64 )( const char* pathname, int flags, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( open64 );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_open64 );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );
        mode_t mode = 0;
        if ( ( flags & O_CREAT )
#ifdef O_TMPFILE
             || ( flags & O_TMPFILE )
#endif
             )
        {
            va_list va;
            va_start( va, flags );
            mode = va_arg( va, mode_t );
            va_end( va );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( open64, ( pathname, flags, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != -1 )
        {
            SCOREP_IoFileHandle   file   = SCOREP_IoMgmt_GetIoFileHandle( pathname );
            SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_POSIX, file, &ret );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreationFlag creation_flags;
                SCOREP_IoStatusFlag   status_flags;
                get_scorep_io_flags( flags, &creation_flags, &status_flags );

                SCOREP_IoCreateHandle( handle,
                                       get_scorep_io_access_mode( flags ),
                                       creation_flags,
                                       status_flags );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }
        SCOREP_ExitRegion( scorep_posix_io_region_open64 );
    }
    else
    {
        mode_t mode = 0;
        if ( ( flags & O_CREAT )
#ifdef O_TMPFILE
             || ( flags & O_TMPFILE )
#endif
             )
        {
            va_list va;
            va_start( va, flags );
            mode = va_arg( va, mode_t );
            va_end( va );
        }
        ret = SCOREP_LIBWRAP_FUNC_CALL( open64, ( pathname, flags, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_OPENAT )
int
SCOREP_LIBWRAP_FUNC_NAME( openat )( int dirfd, const char* pathname, int flags, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( openat );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_openat );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );
        mode_t mode = 0;
        if ( ( flags & O_CREAT )
#ifdef O_TMPFILE
             || ( flags & O_TMPFILE )
#endif
             )
        {
            va_list va;
            va_start( va, flags );
            mode = va_arg( va, mode_t );
            va_end( va );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( openat, ( dirfd, pathname, flags, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != -1 )
        {
            char        fd_path[ PATH_MAX ];
            char        file_path[ PATH_MAX ];
            const char* proc_path = "/proc/self/fd/%d";

            int len = snprintf( fd_path, PATH_MAX, proc_path, ret );

            UTILS_BUG_ON( len < 0, "Executing snprintf to generate proc path failed" );

            len = readlink( fd_path, file_path, PATH_MAX );

            UTILS_BUG_ON( len < 0, "Cannot resolve link %s", fd_path );

            file_path[ len ] = '\0';

            SCOREP_IoFileHandle   file   = SCOREP_IoMgmt_GetIoFileHandle( file_path );
            SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_POSIX, file, &ret );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreationFlag creation_flags;
                SCOREP_IoStatusFlag   status_flags;
                get_scorep_io_flags( flags, &creation_flags, &status_flags );

                SCOREP_IoCreateHandle( handle,
                                       get_scorep_io_access_mode( flags ),
                                       creation_flags,
                                       status_flags );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }

        SCOREP_ExitRegion( scorep_posix_io_region_openat );
    }
    else
    {
        mode_t mode = 0;
        if ( ( flags & O_CREAT )
#ifdef O_TMPFILE
             || ( flags & O_TMPFILE )
#endif
             )
        {
            va_list va;
            va_start( va, flags );
            mode = va_arg( va, mode_t );
            va_end( va );
        }
        ret = SCOREP_LIBWRAP_FUNC_CALL( openat, ( dirfd, pathname, flags, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PREAD )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( pread )( int fd, void* buf, size_t count, off_t offset )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( pread );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_pread );

        /*
         * Reads up to 'count' bytes from file descriptor 'fd' at offset 'offset' (from the start of the file) into the buffer starting at 'buf'.
         * The file offset is not changed.
         *
         * @todo: how to represent offset?
         */
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( pread,
                                        ( fd, buf, count, offset ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_pread );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( pread,
                                        ( fd, buf, count, offset ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PREAD64 )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( pread64 )( int fd, void* buf, size_t count, scorep_off64_t offset )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( pread64 );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_pread64 );

        /*
         * Reads up to 'count' bytes from file descriptor 'fd' at offset 'offset' (from the start of the file) into the buffer starting at 'buf'.
         * The file offset is not changed.
         *
         * @todo: how to represent offset?
         */
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( pread64,
                                        ( fd, buf, count, offset ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_pread64 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( pread64,
                                        ( fd, buf, count, offset ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PSELECT )
int
SCOREP_LIBWRAP_FUNC_NAME( pselect )( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timespec* timeout, const sigset_t* sigmask )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( pselect );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_pselect );
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( pselect,
                                        ( nfds, readfds, writefds, exceptfds, timeout, sigmask ) );
        SCOREP_EXIT_WRAPPED_REGION();
        SCOREP_ExitRegion( scorep_posix_io_region_pselect );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( pselect,
                                        ( nfds, readfds, writefds, exceptfds, timeout, sigmask ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PWRITE )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( pwrite )( int fd, const void* buf, size_t count, off_t offset )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( pwrite );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_pwrite );

        /*
         * Writes up to 'count' bytes from the buffer starting at 'buf' to the file descriptor 'fd' at offset 'offset'.
         * The file offset is not changed.
         *
         * @todo: how to represent offset?
         */
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( pwrite,
                                        ( fd, buf, count, offset ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_pwrite );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( pwrite,
                                        ( fd, buf, count, offset ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PWRITE64 )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( pwrite64 )( int fd, const void* buf, size_t count, scorep_off64_t offset )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( pwrite64 );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_pwrite64 );

        /*
         * Writes up to 'count' bytes from the buffer starting at 'buf' to the file descriptor 'fd' at offset 'offset'.
         * The file offset is not changed.
         *
         * @todo: how to represent offset?
         */
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( pwrite64,
                                        ( fd, buf, count, offset ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_pwrite64 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( pwrite64,
                                        ( fd, buf, count, offset ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_READ )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( read )( int fd, void* buf, size_t count )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( read );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_read );

        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( read,
                                        ( fd, buf, count ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_read );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( read,
                                        ( fd, buf, count ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_SELECT )
int
SCOREP_LIBWRAP_FUNC_NAME( select )( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( select );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_select );
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( select,
                                        ( nfds, readfds, writefds, exceptfds, timeout ) );
        SCOREP_EXIT_WRAPPED_REGION();
        SCOREP_ExitRegion( scorep_posix_io_region_select );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( select,
                                        ( nfds, readfds, writefds, exceptfds, timeout ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_SYNC )
void
SCOREP_LIBWRAP_FUNC_NAME( sync )( void )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( sync );

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_sync );

        SCOREP_IoMgmt_PushHandle( io_sync_all_handle );

        SCOREP_IoOperationBegin( io_sync_all_handle,
                                 SCOREP_IO_OPERATION_MODE_FLUSH,
                                 SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                 SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                 SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );

        SCOREP_ENTER_WRAPPED_REGION();
        SCOREP_LIBWRAP_FUNC_CALL( sync,
                                  ( ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoOperationComplete( io_sync_all_handle,
                                    SCOREP_IO_OPERATION_MODE_FLUSH,
                                    SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                    SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );

        SCOREP_IoMgmt_PopHandle( io_sync_all_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_sync );
    }
    else
    {
        SCOREP_LIBWRAP_FUNC_CALL( sync,
                                  ( ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( POSIX_IO_SYMBOL_SYNCFS )
int
SCOREP_LIBWRAP_FUNC_NAME( syncfs )( int fd )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( syncfs );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_syncfs );

        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_FLUSH,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( syncfs,
                                        ( fd ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_FLUSH,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_syncfs );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( syncfs,
                                        ( fd ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_UNLINK )
int
SCOREP_LIBWRAP_FUNC_NAME( unlink )( const char* pathname )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( unlink );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_unlink );

        SCOREP_IoFileHandle file_handle = SCOREP_IoMgmt_GetIoFileHandle( pathname );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( unlink,
                                        ( pathname ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( file_handle != SCOREP_INVALID_IO_FILE )
        {
            SCOREP_IoDeleteFile( SCOREP_IO_PARADIGM_POSIX,
                                 file_handle );
        }

        SCOREP_ExitRegion( scorep_posix_io_region_unlink );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( unlink,
                                        ( pathname ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_UNLINKAT )
int
SCOREP_LIBWRAP_FUNC_NAME( unlinkat )( int dirfd, const char* pathname, int flags )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( unlinkat );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_unlinkat );

        char                  file_path[ PATH_MAX ];
        SCOREP_IoFileHandle   file_handle      = SCOREP_INVALID_IO_FILE;
        SCOREP_IoHandleHandle handle           = SCOREP_INVALID_IO_HANDLE;
        bool                  path_is_absolute = pathname[ 0 ] == '/';

        if ( dirfd == AT_FDCWD || path_is_absolute )
        {
            file_handle = SCOREP_IoMgmt_GetIoFileHandle( pathname );
        }
        else
        {
            handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                     &dirfd );
            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                const char* dir_path = SCOREP_IoMgmt_GetIoFile( handle );

                int len = snprintf( file_path, PATH_MAX, "%s/%s", dir_path, pathname );
                UTILS_BUG_ON( len < 0, "Executing snprintf to generate file path failed" );

                file_handle = SCOREP_IoMgmt_GetIoFileHandle( file_path );
            }
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( unlinkat,
                                        ( dirfd, pathname, flags ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( file_handle != SCOREP_INVALID_IO_FILE )
        {
            SCOREP_IoDeleteFile( SCOREP_IO_PARADIGM_POSIX, file_handle );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_unlinkat );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( unlinkat,
                                        ( dirfd, pathname, flags ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_WRITE )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( write )( int fd, const void* buf, size_t count )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( write );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_write );

        SCOREP_IoFileHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                     &fd );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( write,
                                        ( fd, buf, count ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_write );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( write,
                                        ( fd, buf, count ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PREADV )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( preadv )( int fd, const struct iovec* iov, int iovcnt, off_t offset )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( preadv );
    ssize_t  ret;
    uint64_t count = 0;
    for ( int i = 0; i < iovcnt; i++ )
    {
        count += iov[ i ].iov_len;
    }

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_preadv );

        /*
         * Reads up to 'count' bytes from file descriptor 'fd' at offset 'offset' (from the start of the file) into the buffer starting at 'buf'.
         * The file offset is not changed.
         *
         * @todo: how to represent offset?
         */
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( preadv,
                                        ( fd, iov, iovcnt, offset ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_preadv );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( preadv,
                                        ( fd, iov, iovcnt, offset ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PWRITEV )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( pwritev )( int fd, const struct iovec* iov, int iovcnt, off_t offset )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( pwritev );
    ssize_t  ret;
    uint64_t count = 0;
    for ( int i = 0; i < iovcnt; i++ )
    {
        count += iov[ i ].iov_len;
    }

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_pwritev );

        /*
         * Writes up to 'count' bytes from the buffer starting at 'buf' to the file descriptor 'fd' at offset 'offset'.
         * The file offset is not changed.
         *
         * @todo: how to represent offset?
         */
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( pwritev,
                                        ( fd, iov, iovcnt, offset ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_pwritev );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( pwritev,
                                        ( fd, iov, iovcnt, offset ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_READV )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( readv )( int fd, const struct iovec* iov, int iovcnt )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( readv );
    ssize_t  ret;
    uint64_t count = 0;
    for ( int i = 0; i < iovcnt; i++ )
    {
        count += iov[ i ].iov_len;
    }

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_readv );

        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &fd );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( readv,
                                        ( fd, iov, iovcnt ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_readv );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( readv,
                                        ( fd, iov, iovcnt ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_WRITEV )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( writev )( int fd, const struct iovec* iov, int iovcnt )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( writev );
    ssize_t  ret;
    uint64_t count = 0;
    for ( int i = 0; i < iovcnt; i++ )
    {
        count += iov[ i ].iov_len;
    }

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_writev );

        SCOREP_IoFileHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                     &fd );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     count,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( writev,
                                        ( fd, iov, iovcnt ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_writev );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( writev,
                                        ( fd, iov, iovcnt ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif


#if HAVE( POSIX_AIO_SUPPORT )
#include "scorep_posix_io_wrap_aio.inc.c"
#endif /* HAVE( POSIX_AIO_SUPPORT ) */
