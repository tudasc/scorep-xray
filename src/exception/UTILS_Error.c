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
 * Copyright (c) 2009-2012, 2017, 2021-2022,
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
 * @brief           Module for error handling in PACKAGE
 */


#include <config.h>
#include <UTILS_Error.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

#include <utils_package.h>

#include "normalize_file.h"


/*--- Internal functions -------------------------------------------*/

/**
 * Default callback function for error handling, which is used if the programmer
 * doesn't register an own callback with function PACKAGE_Error_RegisterCallback().
 *
 * @param function        : Name of the function where the error appeared
 * @param file            : Name of the source-code file where the error appeared
 * @param line            : Line number in the source code where the error appeared
 * @param errorCode       : Error code
 * @param msgFormatString : Format string like it is used for the printf family.
 * @param va              : Variable argument list
 *
 * @returns Should return the errorCode
 */

/**
 * This is a pointer to a function which handles errors if UTILS_ERROR(a) is
 * called.
 */
static PACKAGE_ErrorCallback utils_error_callback;
static void*                 utils_error_callback_user_data;


/*--- External visible functions ----------------------------------*/

static PACKAGE_ErrorCode
utils_error_handler_va( const char*       srcdir,
                        const char*       file,
                        const uint64_t    line,
                        const char*       function,
                        PACKAGE_ErrorCode errorCode,
                        const char*       msgFormatString,
                        va_list           va )
{
    const char* normalized_file = normalize_file( srcdir, file );

    if ( utils_error_callback )
    {
        errorCode = utils_error_callback( utils_error_callback_user_data,
                                          normalized_file,
                                          line,
                                          function,
                                          errorCode,
                                          msgFormatString,
                                          va );
    }
    else
    {
        size_t msg_format_string_length = msgFormatString ?
                                          strlen( msgFormatString ) : 0;
        const char* type               = "error";
        const char* description        = "";
        const char* description_prefix = "";

        if ( errorCode == PACKAGE_WARNING )
        {
            type = "warning";
        }
        else if ( errorCode == PACKAGE_DEPRECATED )
        {
            type = "deprecated";
        }
        else if ( errorCode == PACKAGE_ABORT )
        {
            type = "abort";
        }
        else
        {
            description        = PACKAGE_Error_GetDescription( errorCode );
            description_prefix = ": ";
        }

        fprintf( stderr, "[%s] %s:%" PRIu64 ": %s%s%s%s",
                 PACKAGE_NAME, normalized_file, line,
                 type, description_prefix, description,
                 msg_format_string_length ? ": " : "\n" );

        if ( msg_format_string_length )
        {
            vfprintf( stderr, msgFormatString, va );
            fprintf( stderr, "\n" );
        }
    }

    return errorCode;
}

PACKAGE_ErrorCode
UTILS_Error_Handler( const char*       srcdir,
                     const char*       file,
                     const uint64_t    line,
                     const char*       function,
                     PACKAGE_ErrorCode errorCode,
                     const char*       msgFormatString,
                     ... )
{
    if ( errorCode == PACKAGE_SUCCESS )
    {
        return errorCode;
    }

    va_list va;
    va_start( va, msgFormatString );

    errorCode = utils_error_handler_va( srcdir,
                                        file,
                                        line,
                                        function,
                                        errorCode,
                                        msgFormatString,
                                        va );


    va_end( va );

    return errorCode;
}

struct utils_error_decl
{
    const char*       errorName;
    const char*       errorDescription;
    PACKAGE_ErrorCode errorCode;
};


/* *INDENT-OFF* */
static const struct utils_error_decl none_error_decls[] =
{
    { "SUCCESS",    "Success",    PACKAGE_SUCCESS    },
    { "WARNING",    "Warning",    PACKAGE_WARNING    },
    { "ABORT",      "Aborting",   PACKAGE_ABORT      },
    { "DEPRECATED", "Deprecated", PACKAGE_DEPRECATED }
};
/* *INDENT-ON* */
static const size_t none_error_decls_size =
    sizeof( none_error_decls ) / sizeof( none_error_decls[ 0 ] );


/* *INDENT-OFF* */
static const struct utils_error_decl error_decls[] =
{
    #define UTILS_ERROR_DECL( code, description ) \
    { \
        #code, \
        description, \
        PACKAGE_MANGLE_NAME( ERROR_ ## code ) \
    }

    /* This is the internal implementation of POSIX error code descriptions. */
    UTILS_ERROR_DECL( E2BIG,           "The list of arguments is too long" ),
    UTILS_ERROR_DECL( EACCES,          "Not enough rights" ),
    UTILS_ERROR_DECL( EADDRNOTAVAIL,   "Address is not available" ),
    UTILS_ERROR_DECL( EAFNOSUPPORT,    "Address family is not supported" ),
    UTILS_ERROR_DECL( EAGAIN,          "Resource temporarily not available" ),
    UTILS_ERROR_DECL( EALREADY,        "Connection is already processed" ),
    UTILS_ERROR_DECL( EBADF,           "Invalid file pointer" ),
    UTILS_ERROR_DECL( EBADMSG,         "Invalid message" ),
    UTILS_ERROR_DECL( EBUSY,           "Resource or device is busy" ),
    UTILS_ERROR_DECL( ECANCELED,       "Operation was aborted" ),
    UTILS_ERROR_DECL( ECHILD,          "No child process available" ),
    UTILS_ERROR_DECL( ECONNREFUSED,    "Connection was refused" ),
    UTILS_ERROR_DECL( ECONNRESET,      "Connection was reset" ),
    UTILS_ERROR_DECL( EDEADLK,         "Resolved deadlock" ),
    UTILS_ERROR_DECL( EDESTADDRREQ,    "Destination address was expected" ),
    UTILS_ERROR_DECL( EDOM,            "Domain error" ),
    UTILS_ERROR_DECL( EDQUOT,          "Reserved" ),
    UTILS_ERROR_DECL( EEXIST,          "File does already exist" ),
    UTILS_ERROR_DECL( EFAULT,          "Invalid address" ),
    UTILS_ERROR_DECL( EFBIG,           "File is too large" ),
    UTILS_ERROR_DECL( EINPROGRESS,     "Operation is work in progress" ),
    UTILS_ERROR_DECL( EINTR,           "Interruption of an operating system call" ),
    UTILS_ERROR_DECL( EINVAL,          "Invalid argument" ),
    UTILS_ERROR_DECL( EIO,             "Generic I/O error" ),
    UTILS_ERROR_DECL( EISCONN,         "Socket is already connected" ),
    UTILS_ERROR_DECL( EISDIR,          "Target is a directory" ),
    UTILS_ERROR_DECL( ELOOP,           "Too many layers of symbolic links" ),
    UTILS_ERROR_DECL( EMFILE,          "Too many opened files" ),
    UTILS_ERROR_DECL( EMLINK,          "Too many links" ),
    UTILS_ERROR_DECL( EMSGSIZE,        "Message buffer is too small" ),
    UTILS_ERROR_DECL( EMULTIHOP,       "Reserved" ),
    UTILS_ERROR_DECL( ENAMETOOLONG,    "Filename is too long" ),
    UTILS_ERROR_DECL( ENETDOWN,        "Network is down" ),
    UTILS_ERROR_DECL( ENETRESET,       "Connection was reset from the network" ),
    UTILS_ERROR_DECL( ENETUNREACH,     "Network is not reachable" ),
    UTILS_ERROR_DECL( ENFILE,          "Too many opened files" ),
    UTILS_ERROR_DECL( ENOBUFS,         "No buffer space available" ),
    UTILS_ERROR_DECL( ENODATA,         "No more data left in the queue" ),
    UTILS_ERROR_DECL( ENODEV,          "This device does not support this function" ),
    UTILS_ERROR_DECL( ENOENT,          "File or directory does not exist" ),
    UTILS_ERROR_DECL( ENOEXEC,         "Cannot execute binary" ),
    UTILS_ERROR_DECL( ENOLCK,          "Locking failed" ),
    UTILS_ERROR_DECL( ENOLINK,         "Reserved" ),
    UTILS_ERROR_DECL( ENOMEM,          "Not enough main memory available" ),
    UTILS_ERROR_DECL( ENOMSG,          "Message has not the expected type" ),
    UTILS_ERROR_DECL( ENOPROTOOPT,     "This protocol is not available" ),
    UTILS_ERROR_DECL( ENOSPC,          "No space left on device" ),
    UTILS_ERROR_DECL( ENOSR,           "No stream available" ),
    UTILS_ERROR_DECL( ENOSTR,          "This is not a stream" ),
    UTILS_ERROR_DECL( ENOSYS,          "Requested function is not implemented" ),
    UTILS_ERROR_DECL( ENOTCONN,        "Socket is not connected" ),
    UTILS_ERROR_DECL( ENOTDIR,         "This is not a directory" ),
    UTILS_ERROR_DECL( ENOTEMPTY,       "This directory is not empty" ),
    UTILS_ERROR_DECL( ENOTSOCK,        "No socket" ),
    UTILS_ERROR_DECL( ENOTSUP,         "This operation is not supported" ),
    UTILS_ERROR_DECL( ENOTTY,          "This IOCTL is not supported by the device" ),
    UTILS_ERROR_DECL( ENXIO,           "Device is not yet configured" ),
    UTILS_ERROR_DECL( EOPNOTSUPP,      "Operation is not supported by this socket" ),
    UTILS_ERROR_DECL( EOVERFLOW,       "Value is to long for the datatype" ),
    UTILS_ERROR_DECL( EPERM,           "Operation is not permitted" ),
    UTILS_ERROR_DECL( EPIPE,           "Broken pipe" ),
    UTILS_ERROR_DECL( EPROTO,          "Protocol error" ),
    UTILS_ERROR_DECL( EPROTONOSUPPORT, "Protocol is not supported" ),
    UTILS_ERROR_DECL( EPROTOTYPE,      "Wrong protocol type for this socket" ),
    UTILS_ERROR_DECL( ERANGE,          "Value is out of range" ),
    UTILS_ERROR_DECL( EROFS,           "Filesystem is read only" ),
    UTILS_ERROR_DECL( ESPIPE,          "This seek is not allowed" ),
    UTILS_ERROR_DECL( ESRCH,           "No matching process found" ),
    UTILS_ERROR_DECL( ESTALE,          "Reserved" ),
    UTILS_ERROR_DECL( ETIME,           "Timeout in file stream or IOCTL" ),
    UTILS_ERROR_DECL( ETIMEDOUT,       "Connection timed out" ),
    UTILS_ERROR_DECL( ETXTBSY,         "File could not be executed while it is opened" ),
    UTILS_ERROR_DECL( EWOULDBLOCK,     "Operation would be blocking" ),
    UTILS_ERROR_DECL( EXDEV,           "Invalid link between devices" ),

    #include PACKAGE_INCLUDE( error_decls.gen.h )

    #undef UTILS_ERROR_DECL
};
/* *INDENT-ON* */
static size_t error_decls_size =
    sizeof( error_decls ) / sizeof( error_decls[ 0 ] );


/* *INDENT-OFF* */
static const struct
{
    PACKAGE_ErrorCode errorCode;
    int                posixErrno;
} posix_errno_delcs[] =
{
    #define UTILS_ERRNO_DECL( code ) \
    { \
        PACKAGE_MANGLE_NAME( ERROR_ ## code ), \
        code \
    }

#ifdef EACCES
    UTILS_ERRNO_DECL( EACCES ),           //  0
#endif
#ifdef EADDRNOTAVAIL
    UTILS_ERRNO_DECL( EADDRNOTAVAIL ),    //  1
#endif
#ifdef EAFNOSUPPORT
    UTILS_ERRNO_DECL( EAFNOSUPPORT ),     //  2
#endif
#ifdef EAGAIN
    UTILS_ERRNO_DECL( EAGAIN ),           //  3
#endif
#ifdef EALREADY
    UTILS_ERRNO_DECL( EALREADY ),         //  4
#endif
#ifdef EBADF
    UTILS_ERRNO_DECL( EBADF ),            //  5
#endif
#ifdef EBADMSG
    UTILS_ERRNO_DECL( EBADMSG ),          //  6
#endif
#ifdef EBUSY
    UTILS_ERRNO_DECL( EBUSY ),            //  7
#endif
#ifdef ECANCELED
    UTILS_ERRNO_DECL( ECANCELED ),        //  8
#endif
#ifdef ECHILD
    UTILS_ERRNO_DECL( ECHILD ),           //  9
#endif
#ifdef ECONNREFUSED
    UTILS_ERRNO_DECL( ECONNREFUSED ),     // 10
#endif
#ifdef ECONNRESET
    UTILS_ERRNO_DECL( ECONNRESET ),       // 11
#endif
#ifdef EDEADLK
    UTILS_ERRNO_DECL( EDEADLK ),          // 12
#endif
#ifdef EDESTADDRREQ
    UTILS_ERRNO_DECL( EDESTADDRREQ ),     // 13
#endif
#ifdef EDOM
    UTILS_ERRNO_DECL( EDOM ),             // 14
#endif
#ifdef EDQUOT
    UTILS_ERRNO_DECL( EDQUOT ),           // 15
#endif
#ifdef EEXIST
    UTILS_ERRNO_DECL( EEXIST ),           // 16
#endif
#ifdef EFAULT
    UTILS_ERRNO_DECL( EFAULT ),           // 17
#endif
#ifdef EFBIG
    UTILS_ERRNO_DECL( EFBIG ),            // 18
#endif
#ifdef EINPROGRESS
    UTILS_ERRNO_DECL( EINPROGRESS ),      // 19
#endif
#ifdef EINTR
    UTILS_ERRNO_DECL( EINTR ),            // 20
#endif
#ifdef EINVAL
    UTILS_ERRNO_DECL( EINVAL ),           // 21
#endif
#ifdef EIO
    UTILS_ERRNO_DECL( EIO ),              // 22
#endif
#ifdef EISCONN
    UTILS_ERRNO_DECL( EISCONN ),          // 23
#endif
#ifdef EISDIR
    UTILS_ERRNO_DECL( EISDIR ),           // 24
#endif
#ifdef ELOOP
    UTILS_ERRNO_DECL( ELOOP ),            // 25
#endif
#ifdef EMFILE
    UTILS_ERRNO_DECL( EMFILE ),           // 26
#endif
#ifdef EMLINK
    UTILS_ERRNO_DECL( EMLINK ),           // 27
#endif
#ifdef EMSGSIZE
    UTILS_ERRNO_DECL( EMSGSIZE ),         // 28
#endif
#ifdef EMULTIHOP
    UTILS_ERRNO_DECL( EMULTIHOP ),        // 29
#endif
#ifdef ENAMETOOLONG
    UTILS_ERRNO_DECL( ENAMETOOLONG ),     // 30
#endif
#ifdef ENETDOWN
    UTILS_ERRNO_DECL( ENETDOWN ),         // 31
#endif
#ifdef ENETRESET
    UTILS_ERRNO_DECL( ENETRESET ),        // 32
#endif
#ifdef ENETUNREACH
    UTILS_ERRNO_DECL( ENETUNREACH ),      // 33
#endif
#ifdef ENFILE
    UTILS_ERRNO_DECL( ENFILE ),           // 34
#endif
#ifdef ENOBUFS
    UTILS_ERRNO_DECL( ENOBUFS ),          // 35
#endif
#ifdef ENODATA
    UTILS_ERRNO_DECL( ENODATA ),          // 36
#endif
#ifdef ENODEV
    UTILS_ERRNO_DECL( ENODEV ),           // 37
#endif
#ifdef ENOENT
    UTILS_ERRNO_DECL( ENOENT ),           // 38
#endif
#ifdef ENOEXEC
    UTILS_ERRNO_DECL( ENOEXEC ),          // 39
#endif
#ifdef ENOLCK
    UTILS_ERRNO_DECL( ENOLCK ),           // 40
#endif
#ifdef ENOLINK
    UTILS_ERRNO_DECL( ENOLINK ),          // 41
#endif
#ifdef ENOMEM
    UTILS_ERRNO_DECL( ENOMEM ),           // 42
#endif
#ifdef ENOMSG
    UTILS_ERRNO_DECL( ENOMSG ),           // 43
#endif
#ifdef ENOPROTOOPT
    UTILS_ERRNO_DECL( ENOPROTOOPT ),      // 44
#endif
#ifdef ENOSPC
    UTILS_ERRNO_DECL( ENOSPC ),           // 45
#endif
#ifdef ENOSR
    UTILS_ERRNO_DECL( ENOSR ),            // 46
#endif
#ifdef ENOSTR
    UTILS_ERRNO_DECL( ENOSTR ),           // 47
#endif
#ifdef ENOSYS
    UTILS_ERRNO_DECL( ENOSYS ),           // 48
#endif
#ifdef ENOTCONN
    UTILS_ERRNO_DECL( ENOTCONN ),         // 49
#endif
#ifdef ENOTDIR
    UTILS_ERRNO_DECL( ENOTDIR ),          // 50
#endif
#ifdef ENOTEMPTY
    UTILS_ERRNO_DECL( ENOTEMPTY ),        // 51
#endif
#ifdef ENOTSOCK
    UTILS_ERRNO_DECL( ENOTSOCK ),         // 52
#endif
#ifdef ENOTSUP
    UTILS_ERRNO_DECL( ENOTSUP ),          // 53
#endif
#ifdef ENOTTY
    UTILS_ERRNO_DECL( ENOTTY ),           // 54
#endif
#ifdef ENXIO
    UTILS_ERRNO_DECL( ENXIO ),            // 55
#endif
#ifdef EOPNOTSUPP
    UTILS_ERRNO_DECL( EOPNOTSUPP ),       // 56
#endif
#ifdef EOVERFLOW
    UTILS_ERRNO_DECL( EOVERFLOW ),        // 57
#endif
#ifdef EPERM
    UTILS_ERRNO_DECL( EPERM ),            // 58
#endif
#ifdef EPIPE
    UTILS_ERRNO_DECL( EPIPE ),            // 59
#endif
#ifdef EPROTO
    UTILS_ERRNO_DECL( EPROTO ),           // 60
#endif
#ifdef EPROTONOSUPPORT
    UTILS_ERRNO_DECL( EPROTONOSUPPORT ),  // 61
#endif
#ifdef EPROTOTYPE
    UTILS_ERRNO_DECL( EPROTOTYPE ),       // 62
#endif
#ifdef ERANGE
    UTILS_ERRNO_DECL( ERANGE ),           // 63
#endif
#ifdef EROFS
    UTILS_ERRNO_DECL( EROFS ),            // 64
#endif
#ifdef ESPIPE
    UTILS_ERRNO_DECL( ESPIPE ),           // 65
#endif
#ifdef ESRCH
    UTILS_ERRNO_DECL( ESRCH ),            // 66
#endif
#ifdef ESTALE
    UTILS_ERRNO_DECL( ESTALE ),           // 67
#endif
#ifdef ETIME
    UTILS_ERRNO_DECL( ETIME ),            // 68
#endif
#ifdef ETIMEDOUT
    UTILS_ERRNO_DECL( ETIMEDOUT ),        // 69
#endif
#ifdef ETXTBSY
    UTILS_ERRNO_DECL( ETXTBSY ),          // 70
#endif
#ifdef EWOULDBLOCK
    UTILS_ERRNO_DECL( EWOULDBLOCK ),      // 71
#endif
#ifdef EXDEV
    UTILS_ERRNO_DECL( EXDEV ),            // 72
#endif

    #undef UTILS_ERRNO_DECL
};
/* *INDENT-ON* */
static size_t posix_errno_delcs_size =
    sizeof( posix_errno_delcs ) / sizeof( posix_errno_delcs[ 0 ] );

static const struct utils_error_decl*
utils_get_error_decl( PACKAGE_ErrorCode errorCode )
{
    if ( errorCode <= PACKAGE_SUCCESS )
    {
        if ( -errorCode >= ( PACKAGE_ErrorCode )none_error_decls_size )
        {
            return NULL;
        }
        return &none_error_decls[ -errorCode ];
    }

    /* real error codes start 1 behind PACKAGE_ERROR_INVALID */
    size_t index = errorCode - PACKAGE_ERROR_INVALID - 1;

    if ( errorCode == PACKAGE_ERROR_INVALID ||
         index >= error_decls_size )
    {
        return NULL;
    }
    return &error_decls[ index ];
}

const char*
PACKAGE_Error_GetName( const PACKAGE_ErrorCode errorCode )
{
    const struct utils_error_decl* decl = utils_get_error_decl( errorCode );
    if ( !decl )
    {
        return "INVALID";
    }

    /* See scorep_error_codes.h for the definition of this array */
    return decl->errorName;
}


const char*
PACKAGE_Error_GetDescription( const PACKAGE_ErrorCode errorCode )
{
    const struct utils_error_decl* decl = utils_get_error_decl( errorCode );
    if ( !decl )
    {
        return "Unknown error code";
    }

    /* See scorep_error_codes.h for the definition of this array */
    return decl->errorDescription;
}


PACKAGE_ErrorCode
UTILS_Error_FromPosix( const int posixErrno )
{
    uint64_t i;

    if ( posixErrno == 0 )
    {
        return PACKAGE_SUCCESS;
    }

    for ( i = 0; i < posix_errno_delcs_size; i++ )
    {
        if ( posix_errno_delcs[ i ].posixErrno == posixErrno )
        {
            return posix_errno_delcs[ i ].errorCode;
        }
    }

    return PACKAGE_ERROR_INVALID;
}

void
UTILS_Error_Abort( const char* srcdir,
                   const char* fileName,
                   uint64_t    line,
                   const char* functionName,
                   const char* messageFormatString,
                   ... )
{
    va_list va;
    va_start( va, messageFormatString );

    utils_error_handler_va( srcdir,
                            fileName,
                            line,
                            functionName,
                            PACKAGE_ABORT,
                            messageFormatString,
                            va );

    va_end( va );

    abort();
}


PACKAGE_ErrorCallback
PACKAGE_Error_RegisterCallback( PACKAGE_ErrorCallback errorCallbackIn,
                                void*                 userData )
{
    PACKAGE_ErrorCallback temp_pointer = utils_error_callback;
    utils_error_callback           = errorCallbackIn;
    utils_error_callback_user_data = userData;
    return temp_pointer;
}
