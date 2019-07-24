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
 * @brief C interface wrappers for POSIX I/O routines
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include "scorep_posix_io.h"

#include <unistd.h>

#include <SCOREP_Events.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_IoManagement.h>

#define SCOREP_DEBUG_MODULE_NAME IO
#include <UTILS_Debug.h>

#include "scorep_posix_io_regions.h"
#include "scorep_posix_io_function_pointers.h"

/* *INDENT-OFF* */
#ifdef SCOREP_LIBWRAP_SHARED
#define INITIALIZE_FUNCTION_POINTER( func )                                \
    do                                                                     \
    {                                                                      \
        if ( !SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) )                      \
        {                                                                  \
            scorep_posix_io_early_init_function_pointers();                \
            UTILS_BUG_ON( SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) == NULL,   \
                          "Cannot obtain address of symbol: " #func "." ); \
        }                                                                  \
    } while ( 0 )
#else
#define INITIALIZE_FUNCTION_POINTER( func ) do { } while ( 0 )
#endif
/* *INDENT-ON* */

/*
 * Annex K of the C11 standard specifies bounds-checking interfaces.
 * Only a few implementations exist, e.g. Microsoft Visual Studio.
 * Neither BSD nor Linux distributions provide an implementation.
 * It's unlikely that the glibc will provide an implementation of
 * these APIs in the near future. We decided to do NOT wrap the
 * following functions as we can't test them yet:
 *
 * errno_t tmpfile_s( FILE * restrict * restrict streamptr );
 * errno_t tmpnam_s( char *s, rsize_t maxsize );
 * errno_t fopen_s( FILE * restrict * restrict streamptr,
 *                  const char * restrict filename,
 *                  const char * restrict mode );
 * errno_t freopen_s( FILE * restrict * restrict newstreamptr,
 *                    const char * restrict filename,
 *                    const char * restrict mode,
 *                    FILE * restrict stream );
 * int fprintf_s( FILE * restrict stream,
 *                const char * restrict format, ... );
 * int fscanf_s( FILE * restrict stream,
 *               const char * restrict format, ... );
 * int printf_s( const char * restrict format, ... );
 * int scanf_s(const char * restrict format, ...);
 * int snprintf_s( char * restrict s, rsize_t n,
 *                 const char * restrict format, ... );
 * int sprintf_s( char * restrict s, rsize_t n,
 *                const char * restrict format, ... );
 * int sscanf_s( const char * restrict s,
 *               const char * restrict format, ... );
 * int vfprintf_s( FILE * restrict stream,
 *                 const char * restrict format,
 *                 va_list arg );
 * int vfscanf_s( FILE * restrict stream,
 *                const char * restrict format,
 *                va_list arg );
 * int vprintf_s( const char * restrict format,
 *                va_list arg );
 * int vscanf_s( const char * restrict format,
 *               va_list arg );
 * int vsnprintf_s( char * restrict s, rsize_t n,
 *                  const char * restrict format,
 *                  va_list arg );
 * int vsprintf_s( char * restrict s, rsize_t n,
 *                 const char * restrict format,
 *                 va_list arg );
 * int vsscanf_s( const char * restrict s,
 *                const char * restrict format,
 *                va_list arg );
 * char *gets_s( char *s, rsize_t n );
 */


/* *******************************************************************
 * Translate ISO C I/O types to Score-P representative
 * ******************************************************************/

/**
 * Translate the POSIX mode of an open operation to its Score-P equivalent.
 * The mode is specified by a string starting with character(s) determining
 * the mode (e.g., 'r', 'r+', 'w', 'w+', 'a', 'a+').
 *
 * @param mode      String specifying the POSIX mode of the open operation as string.
 *
 * @return Score-P equivalent of the POSIX mode.
 */
static inline SCOREP_IoAccessMode
get_scorep_io_access_mode_from_string( const char* mode )
{
#define R_MODE             "r"
#define R_MODE_STRLEN      strlen( R_MODE )
#define R_PLUS_MODE        "r+"
#define R_PLUS_MODE_STRLEN strlen( R_PLUS_MODE )

#define W_MODE             "w"
#define W_MODE_STRLEN      strlen( W_MODE )
#define W_PLUS_MODE        "w+"
#define W_PLUS_MODE_STRLEN strlen( W_PLUS_MODE )

#define A_MODE             "a"
#define A_MODE_STRLEN      strlen( A_MODE )
#define A_PLUS_MODE        "a+"
#define A_PLUS_MODE_STRLEN strlen( A_PLUS_MODE )

    if ( strncmp( R_PLUS_MODE, mode, strlen( R_PLUS_MODE ) ) == 0 )
    {
        return SCOREP_IO_ACCESS_MODE_READ_WRITE;
    }
    else if ( strncmp( W_PLUS_MODE, mode, strlen( W_PLUS_MODE ) ) == 0 )
    {
        return SCOREP_IO_ACCESS_MODE_READ_WRITE;
    }
    else if ( strncmp( A_PLUS_MODE, mode, strlen( A_PLUS_MODE ) ) == 0 )
    {
        return SCOREP_IO_ACCESS_MODE_READ_WRITE;
    }
    else if ( strncmp( R_MODE, mode, strlen( R_MODE ) ) == 0 )
    {
        return SCOREP_IO_ACCESS_MODE_READ_ONLY;
    }
    else if ( strncmp( W_MODE, mode, strlen( W_MODE ) ) == 0 )
    {
        return SCOREP_IO_ACCESS_MODE_WRITE_ONLY;
    }
    else if ( strncmp( A_MODE, mode, strlen( A_MODE ) ) == 0 )
    {
        return SCOREP_IO_ACCESS_MODE_WRITE_ONLY;
    }
    else
    {
        UTILS_BUG( "Invalid IO open mode." );
        return SCOREP_IO_ACCESS_MODE_NONE;
    }

#undef R_MODE
#undef R_MODE_STRLEN
#undef R_PLUS_MODE
#undef R_PLUS_MODE_STRLEN

#undef W_MODE
#undef W_MODE_STRLEN
#undef W_PLUS_MODE
#undef W_PLUS_MODE_STRLEN

#undef A_MODE
#undef A_MODE_STRLEN
#undef A_PLUS_MODE
#undef A_PLUS_MODE_STRLEN
}

/* *******************************************************************
 * Function wrappers
 * ******************************************************************/

#if HAVE( POSIX_IO_SYMBOL_FCLOSE )
int
SCOREP_LIBWRAP_FUNC_NAME( fclose )( FILE* fp )
{
    /*
     * fclose manpage:
     *
     * int fclose(FILE *stream);
     *
     * The fclose() function flushes the stream pointed to by stream
     * (writing any buffered output data using fflush(3)) and closes
     * the underlying file descriptor.
     */
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fclose );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fclose );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_ISOC,
                                                                   &fp );
        #if HAVE_BACKEND( FILENO )
        int                   fd        = fileno( fp );
        SCOREP_IoHandleHandle fd_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
        #endif

        SCOREP_IoMgmt_PushHandle( handle );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fclose,
                                        ( fp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoMgmt_PopHandle( handle );
        /*
         * As any further access to the stream results in undefined behavior
         * destroy the corresponding handle in the Score-P internal I/O handle
         * management unconditionally.
         */
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( ret != 0 && errno != EBADF )
            {
                SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_ISOC, handle );

                #if HAVE_BACKEND( FILENO )
                if ( fd_handle != SCOREP_INVALID_IO_HANDLE )
                {
                    SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_POSIX, fd_handle );
                }
                #endif
            }
            else
            {
                #if HAVE_BACKEND( FILENO )
                if ( fd_handle != SCOREP_INVALID_IO_HANDLE )
                {
                    SCOREP_IoMgmt_DestroyHandle( fd_handle );
                    SCOREP_IoDestroyHandle( fd_handle );
                }
                #endif
                SCOREP_IoMgmt_DestroyHandle( handle );
            }
            SCOREP_IoDestroyHandle( handle );
        }

        SCOREP_ExitRegion( scorep_posix_io_region_fclose );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fclose,
                                        ( fp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FDOPEN )
FILE*
SCOREP_LIBWRAP_FUNC_NAME( fdopen )( int fd, const char* mode )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fdopen );
    FILE* ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fdopen );

        SCOREP_IoHandleHandle fd_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_ISOC,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fdopen,
                                        ( fd, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != NULL && fd_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoFileHandle   file          = SCOREP_IoHandleHandle_GetIoFile( fd_handle );
            SCOREP_IoHandleHandle stream_handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_ISOC, file, &ret );
            if ( stream_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreateHandle( stream_handle,                                  /* I/O handle */
                                       get_scorep_io_access_mode_from_string( mode ),  /* open mode */
                                       SCOREP_IO_CREATION_FLAG_NONE,                   /* additional flags */
                                       SCOREP_IO_STATUS_FLAG_NONE );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }


        SCOREP_ExitRegion( scorep_posix_io_region_fdopen );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fdopen,
                                        ( fd, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FFLUSH )
int
SCOREP_LIBWRAP_FUNC_NAME( fflush )( FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fflush );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fflush );

        SCOREP_IoHandleHandle io_handle;
        if ( stream == NULL )
        {
            io_handle = scorep_posix_io_flush_all_handle;
        }
        else
        {
            io_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        }
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_FLUSH,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PushHandle( io_handle );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fflush,
                                        ( stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( io_handle,
                                        SCOREP_IO_OPERATION_MODE_FLUSH,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fflush );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fflush,
                                        ( stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FGETC )
int
SCOREP_LIBWRAP_FUNC_NAME( fgetc )( FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fgetc );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fgetc );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( 1 /* one character == one byte */ ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fgetc,
                                        ( stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )( ret != EOF ? 1 : 0 ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fgetc );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fgetc,
                                        ( stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FGETPOS )
int
SCOREP_LIBWRAP_FUNC_NAME( fgetpos )( FILE* stream, fpos_t* pos )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fgetpos );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fgetpos );
        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fgetpos,
                                        ( stream, pos ) );
        SCOREP_EXIT_WRAPPED_REGION();
        SCOREP_IoMgmt_PopHandle( handle );
        SCOREP_ExitRegion( scorep_posix_io_region_fgetpos );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fgetpos,
                                        ( stream, pos ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FGETS )
char*
SCOREP_LIBWRAP_FUNC_NAME( fgets )( char* s, int size, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fgets );
    char* ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fgets );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( size ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fgets,
                                        ( s, size, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )( ret != NULL ? strlen( s ) + 1 /* terminating null byte */ : 0 ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fgets );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fgets,
                                        ( s, size, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FLOCKFILE )
void
SCOREP_LIBWRAP_FUNC_NAME( flockfile )( FILE* filehandle )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( flockfile );

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_flockfile );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &filehandle );

        SCOREP_ENTER_WRAPPED_REGION();
        SCOREP_LIBWRAP_FUNC_CALL( flockfile,
                                  ( filehandle ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoAcquireLock( handle, SCOREP_LOCK_EXCLUSIVE );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_flockfile );
    }
    else
    {
        SCOREP_LIBWRAP_FUNC_CALL( flockfile,
                                  ( filehandle ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

static inline void
create_posix_handle( int fd, const char* path, SCOREP_IoAccessMode access_mode )
{
    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0 /* do not unify */, "" );

    SCOREP_IoFileHandle   file      = SCOREP_IoMgmt_GetIoFileHandle( path );
    SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_CompleteHandleCreation(
        SCOREP_IO_PARADIGM_POSIX, file, &fd );

    if ( io_handle != SCOREP_INVALID_IO_HANDLE )
    {
        SCOREP_IoCreateHandle( io_handle,
                               access_mode,
                               SCOREP_IO_CREATION_FLAG_NONE,
                               SCOREP_IO_STATUS_FLAG_NONE );
    }
}

#if HAVE( POSIX_IO_SYMBOL_FOPEN )
FILE*
SCOREP_LIBWRAP_FUNC_NAME( fopen )( const char* path, const char* mode )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fopen );
    FILE* ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fopen );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_ISOC,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fopen,
                                        ( path, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        /*
         * Argument 'mode' is a string. It begins with one of the following sequences.
         *
         * r   Open file for reading.
         *     The stream is positioned at the beginning of the file.
         *
         * r+  Open file for reading and writing.
         *     The stream is positioned at the beginning of the file.
         *
         * w   Truncate file to zero length or create text file for writing.
         *     The stream is positioned at the beginning of the file.
         *
         * w+  Open for reading and writing.
         *     The file is created if it does not exist, otherwise it is truncated.
         *     The stream is positioned at the beginning of the file.
         *
         * a   Open for appending (writing at end of file).
         *     The file is created if it does not exist.
         *     The stream is positioned at the end of the file.
         *
         * a+  Open for reading and appending (writing at end of file).
         *     The file is created if it does not exist.
         *     The initial file position for reading is at the
         *     beginning of the file, but output is always appended to
         *     the end of the file.
         *
         * Additional characters may follow.
         *
         */

        if ( ret != NULL )
        {
            SCOREP_IoAccessMode access_mode = get_scorep_io_access_mode_from_string( mode );

            #if HAVE_BACKEND( FILENO )
            int fd = fileno( ret );

            SCOREP_IoHandleHandle fd_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );

            if ( fd_handle == SCOREP_INVALID_IO_HANDLE )
            {
                create_posix_handle( fd, path, access_mode );
            }
            #endif /* HAVE_BACKEND( FILENO ) */
            SCOREP_IoFileHandle   file      = SCOREP_IoMgmt_GetIoFileHandle( path );
            SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_ISOC, file, &ret );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreateHandle( io_handle,
                                       get_scorep_io_access_mode_from_string( mode ),
                                       SCOREP_IO_CREATION_FLAG_NONE,
                                       SCOREP_IO_STATUS_FLAG_NONE );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }


        SCOREP_ExitRegion( scorep_posix_io_region_fopen );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fopen,
                                        ( path, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FOPEN64 )
FILE*
SCOREP_LIBWRAP_FUNC_NAME( fopen64 )( const char* path, const char* mode )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fopen64 );
    FILE* ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fopen64 );

        SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_ISOC,
                                           SCOREP_IO_HANDLE_FLAG_NONE,
                                           SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                           0 /* do not unify */, "" );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fopen64,
                                        ( path, mode ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != NULL )
        {
            SCOREP_IoAccessMode access_mode = get_scorep_io_access_mode_from_string( mode );

            #if HAVE_BACKEND( FILENO )
            int fd = fileno( ret );

            SCOREP_IoHandleHandle fd_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );

            if ( fd_handle == SCOREP_INVALID_IO_HANDLE )
            {
                create_posix_handle( fd, path, access_mode );
            }
            #endif /* HAVE_BACKEND( FILENO ) */

            SCOREP_IoFileHandle file = SCOREP_IoMgmt_GetIoFileHandle( path );

            SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_CompleteHandleCreation(
                SCOREP_IO_PARADIGM_ISOC, file, &ret );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreateHandle( io_handle,
                                       access_mode,
                                       SCOREP_IO_CREATION_FLAG_NONE,
                                       SCOREP_IO_STATUS_FLAG_NONE );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }


        SCOREP_ExitRegion( scorep_posix_io_region_fopen64 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fopen64,
                                        ( path, mode ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FPUTC )
int
SCOREP_LIBWRAP_FUNC_NAME( fputc )( int c, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fputc );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fputc );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( 1 /* one character == one byte */ ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fputc,
                                        ( c, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )( 1 ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fputc );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fputc,
                                        ( c, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FPRINTF ) && HAVE( POSIX_IO_SYMBOL_VFPRINTF )
int
SCOREP_LIBWRAP_FUNC_NAME( fprintf )( FILE* stream, const char* format, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vfprintf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fprintf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        va_list args;
        va_start( args, format );
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfprintf,
                                        ( stream, format, args ) );
        SCOREP_EXIT_WRAPPED_REGION();
        va_end( args );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fprintf );
    }
    else
    {
        va_list args;
        va_start( args, format );
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfprintf,
                                        ( stream, format, args ) );
        va_end( args );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FPUTS )
int
SCOREP_LIBWRAP_FUNC_NAME( fputs )( const char* s, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fputs );
    /* fputs() writes the string s to stream, WITHOUT its terminating null byte ('\0'). */

    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fputs );

        uint64_t              length = ( uint64_t )strlen( s );
        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     length,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fputs,
                                        ( s, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        length,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fputs );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fputs,
                                        ( s, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FREAD )
size_t
SCOREP_LIBWRAP_FUNC_NAME( fread )( void* ptr, size_t size, size_t nmemb, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fread );
    size_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fread );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( size * nmemb ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fread,
                                        ( ptr, size, nmemb, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )( size * ret ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fread );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fread,
                                        ( ptr, size, nmemb, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FREOPEN )
FILE*
SCOREP_LIBWRAP_FUNC_NAME( freopen )( const char* path, const char* mode, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( freopen );
    FILE* ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_freopen );

        // Case: stream will be closed by freopen
        SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        #if HAVE_BACKEND( FILENO )
        int                   fd        = fileno( stream );
        SCOREP_IoHandleHandle fd_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
        #endif

        SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_ISOC,
                                              old_handle, 0 );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( freopen,
                                        ( path, mode, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( ret != NULL )
        {
            if ( old_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoDestroyHandle( old_handle );
                SCOREP_IoMgmt_DestroyHandle( old_handle );
            }
            SCOREP_IoAccessMode access_mode = get_scorep_io_access_mode_from_string( mode );

            fd = fileno( ret );
            #if HAVE_BACKEND( FILENO )
            if ( fd_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoDestroyHandle( fd_handle );
                SCOREP_IoMgmt_DestroyHandle( fd_handle );
            }
            create_posix_handle( fd, path, access_mode );
            #endif /* HAVE_BACKEND( FILENO ) */

            SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
                SCOREP_IO_PARADIGM_ISOC, SCOREP_IoMgmt_GetIoFileHandle( path ), &ret );
            if ( new_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoCreateHandle( new_handle,
                                       access_mode,
                                       SCOREP_IO_CREATION_FLAG_NONE,
                                       SCOREP_IO_STATUS_FLAG_NONE );
            }
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }



        SCOREP_ExitRegion( scorep_posix_io_region_freopen );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( freopen,
                                        ( path, mode, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}

#endif

#if HAVE( POSIX_IO_SYMBOL_FSCANF ) && HAVE( POSIX_IO_SYMBOL_VFSCANF )
int
SCOREP_LIBWRAP_FUNC_NAME( fscanf )( FILE* stream, const char* format, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vfscanf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fscanf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_ENTER_WRAPPED_REGION();

        va_list args;
        va_start( args, format );
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfscanf,
                                        ( stream, format, args ) );
        va_end( args );

        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fscanf );
    }
    else
    {
        va_list args;
        va_start( args, format );
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfscanf,
                                        ( stream, format, args ) );
        va_end( args );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FSEEK )
int
SCOREP_LIBWRAP_FUNC_NAME( fseek )( FILE* stream, long offset, int whence )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fseek );
    INITIALIZE_FUNCTION_POINTER( ftell );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fseek );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fseek,
                                        ( stream, offset, whence ) );
        SCOREP_EXIT_WRAPPED_REGION();

        long fp_offset = SCOREP_LIBWRAP_FUNC_CALL( ftell, ( stream ) );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           ( int64_t )offset,
                           scorep_posix_io_get_scorep_io_seek_option( whence ),
                           ( uint64_t )fp_offset );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fseek );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fseek,
                                        ( stream, offset, whence ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FSEEKO )
int
SCOREP_LIBWRAP_FUNC_NAME( fseeko )( FILE* stream, off_t offset, int whence )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fseeko );
    INITIALIZE_FUNCTION_POINTER( ftello );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fseeko );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fseeko,
                                        ( stream, offset, whence ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           ( int64_t )offset,
                           scorep_posix_io_get_scorep_io_seek_option( whence ),
                           ( uint64_t )( SCOREP_LIBWRAP_FUNC_CALL( ftello, ( stream ) ) ) );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fseeko );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fseeko,
                                        ( stream, offset, whence ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FSEEKO64 )
int
SCOREP_LIBWRAP_FUNC_NAME( fseeko64 )( FILE* stream, scorep_off64_t offset, int whence )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fseeko64 );
    INITIALIZE_FUNCTION_POINTER( ftello );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fseeko64 );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fseeko64,
                                        ( stream, offset, whence ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           ( int64_t )offset,
                           scorep_posix_io_get_scorep_io_seek_option( whence ),
                           ( uint64_t )( SCOREP_LIBWRAP_FUNC_CALL( ftello, ( stream ) ) ) );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fseeko64 );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fseeko64,
                                        ( stream, offset, whence ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FSETPOS )
int
SCOREP_LIBWRAP_FUNC_NAME( fsetpos )( FILE* stream, const fpos_t* pos )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fsetpos );
    INITIALIZE_FUNCTION_POINTER( ftell );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fsetpos );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fsetpos,
                                        ( stream, pos ) );
        SCOREP_EXIT_WRAPPED_REGION();

        long fp_offset = SCOREP_LIBWRAP_FUNC_CALL( ftell, ( stream ) );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           0,
                           scorep_posix_io_get_scorep_io_seek_option( SEEK_SET ),
                           fp_offset );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fsetpos );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fsetpos,
                                        ( stream, pos ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FTELL )
long
SCOREP_LIBWRAP_FUNC_NAME( ftell )( FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( ftell );
    long ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_ftell );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( ftell,
                                        ( stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_ftell );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( ftell,
                                        ( stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FTELLO )
off_t
SCOREP_LIBWRAP_FUNC_NAME( ftello )( FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( ftello );
    off_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_ftello );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( ftello,
                                        ( stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_ftello );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( ftello,
                                        ( stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FTRYLOCKFILE )
int
SCOREP_LIBWRAP_FUNC_NAME( ftrylockfile )( FILE* filehandle )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( ftrylockfile );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_ftrylockfile );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &filehandle );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( ftrylockfile,
                                        ( filehandle ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            if ( ret == 0 )
            {
                SCOREP_IoAcquireLock( handle, SCOREP_LOCK_EXCLUSIVE );
            }
            else
            {
                SCOREP_IoTryLock( handle, SCOREP_LOCK_EXCLUSIVE );
            }
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_ftrylockfile );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( ftrylockfile,
                                        ( filehandle ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FUNLOCKFILE )
void
SCOREP_LIBWRAP_FUNC_NAME( funlockfile )( FILE* filehandle )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( funlockfile );

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_funlockfile );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &filehandle );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoReleaseLock( handle, SCOREP_LOCK_EXCLUSIVE );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        SCOREP_LIBWRAP_FUNC_CALL( funlockfile,
                                  ( filehandle ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_funlockfile );
    }
    else
    {
        SCOREP_LIBWRAP_FUNC_CALL( funlockfile,
                                  ( filehandle ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( POSIX_IO_SYMBOL_FWRITE )
size_t
SCOREP_LIBWRAP_FUNC_NAME( fwrite )( const void* ptr, size_t size, size_t nmemb, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( fwrite );
    size_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_fwrite );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( size * nmemb ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( fwrite,
                                        ( ptr, size, nmemb, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )( size * ret ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_fwrite );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( fwrite,
                                        ( ptr, size, nmemb, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

/* getc() might be implemented as a macro */
#if HAVE( POSIX_IO_SYMBOL_GETC )  && !defined getc
int
SCOREP_LIBWRAP_FUNC_NAME( getc )( FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( getc );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_getc );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( 1 /* one character == one byte */ ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( getc,
                                        ( stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )( ( ret != EOF ? 1 : 0 ) ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_getc );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( getc,
                                        ( stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_GETCHAR )
int
SCOREP_LIBWRAP_FUNC_NAME( getchar )( void )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( getchar );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_getchar );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdin );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( 1 /* one character == one byte */ ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( getchar,
                                        ( ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )( ret != EOF ? 1 : 0 ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_getchar );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( getchar,
                                        ( ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_GETS )
char*
SCOREP_LIBWRAP_FUNC_NAME( gets )( char* s )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( gets );
    char* ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_gets );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdin );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( 0 ) /* What's the requested size of transferred bytes of a gets operation? */,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( gets,
                                        ( s ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        ( uint64_t )( ret != NULL ? strlen( s ) + 1 /* terminating null byte */ : 0 ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_gets );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( gets,
                                        ( s ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PRINTF ) && HAVE( POSIX_IO_SYMBOL_VPRINTF )
int
SCOREP_LIBWRAP_FUNC_NAME( printf )( const char* format, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vprintf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_printf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdout );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        va_list args;
        va_start( args, format );
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( vprintf,
                                        ( format, args ) );
        SCOREP_EXIT_WRAPPED_REGION();
        va_end( args );

        if ( handle != SCOREP_INVALID_IO_HANDLE && ret >= 0 )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_printf );
    }
    else
    {
        va_list args;
        va_start( args, format );
        ret = SCOREP_LIBWRAP_FUNC_CALL( vprintf,
                                        ( format, args ) );
        va_end( args );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PUTCHAR )
int
SCOREP_LIBWRAP_FUNC_NAME( putchar )( int c )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( putchar );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_putchar );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdout );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     ( uint64_t )( 1 /* one character == one byte */ ),
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( putchar,
                                        ( c ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ( uint64_t )( 1 ),
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_putchar );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( putchar,
                                        ( c ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_PUTS )
int
SCOREP_LIBWRAP_FUNC_NAME( puts )( const char* s )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( puts );
    /* puts() writes the string s and a trailing newline to stdout. */

    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_puts );

        uint64_t              length = ( uint64_t )strlen( s ) + 1 /* terminating null byte */;
        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdout );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     length,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( puts,
                                        ( s ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        length,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_puts );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( puts,
                                        ( s ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_REWIND )
void
SCOREP_LIBWRAP_FUNC_NAME( rewind )( FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( rewind );

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_rewind );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        SCOREP_LIBWRAP_FUNC_CALL( rewind,
                                  ( stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoSeek( handle,
                           ( int64_t )0,
                           scorep_posix_io_get_scorep_io_seek_option( SEEK_SET ),
                           ( uint64_t )0 );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_rewind );
    }
    else
    {
        SCOREP_LIBWRAP_FUNC_CALL( rewind,
                                  ( stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( POSIX_IO_SYMBOL_SCANF ) && HAVE( POSIX_IO_SYMBOL_VSCANF )
int
SCOREP_LIBWRAP_FUNC_NAME( scanf )( const char* format, ... )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vscanf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_scanf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, stdin );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_ENTER_WRAPPED_REGION();

        va_list args;
        va_start( args, format );
        ret = SCOREP_LIBWRAP_FUNC_CALL( vscanf,
                                        ( format, args ) );
        va_end( args );

        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_scanf );
    }
    else
    {
        va_list args;
        va_start( args, format );
        ret = SCOREP_LIBWRAP_FUNC_CALL( vscanf,
                                        ( format, args ) );
        va_end( args );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_UNGETC )
int
SCOREP_LIBWRAP_FUNC_NAME( ungetc )( int c, FILE* stream )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( ungetc );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_ungetc );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( ungetc,
                                        ( c, stream ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_ungetc );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( ungetc,
                                        ( c, stream ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_VFPRINTF )
int
SCOREP_LIBWRAP_FUNC_NAME( vfprintf )( FILE* stream, const char* format, va_list ap )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vfprintf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_vfprintf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfprintf,
                                        ( stream, format, ap ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_vfprintf );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfprintf,
                                        ( stream, format, ap ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_VFSCANF )
int
SCOREP_LIBWRAP_FUNC_NAME( vfscanf )( FILE* stream, const char* format, va_list ap )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vfscanf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_vfscanf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stream );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfscanf,
                                        ( stream, format, ap ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_vfscanf );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( vfscanf,
                                        ( stream, format, ap ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_VSCANF )
int
SCOREP_LIBWRAP_FUNC_NAME( vscanf )( const char* format, va_list ap )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vscanf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_vscanf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdin );

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( vscanf,
                                        ( format, ap ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_READ,
                                        SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_vscanf );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( vscanf,
                                        ( format, ap ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_VPRINTF )
int
SCOREP_LIBWRAP_FUNC_NAME( vprintf )( const char* format, va_list ap )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( vprintf );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_vprintf );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_ISOC, &stdout );
        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                     SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                     SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( vprintf,
                                        ( format, ap ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationComplete( handle,
                                        SCOREP_IO_OPERATION_MODE_WRITE,
                                        ret,
                                        SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC /* matching id */ );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_vprintf );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( vprintf,
                                        ( format, ap ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif
