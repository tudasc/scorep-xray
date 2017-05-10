/*
 * This file is part of the Score-P software (http://www.score-p.org)
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
 * Copyright (c) 2009-2012, 2017,
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

#ifndef @AFS_PACKAGE_NAME@_ERROR_CODES_H
#define @AFS_PACKAGE_NAME@_ERROR_CODES_H


/**
 * @file            @AFS_PACKAGE_NAME@_ErrorCodes.h
 * @ingroup         @AFS_PACKAGE_NAME@_Exception_module
 *
 * @brief           Error codes and error handling.
 *
 */

#include <errno.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is the list of error codes for @AFS_PACKAGE_NAME@.
 */
typedef enum
{
    /** Special marker for error messages which indicates a deprecation. */
    @AFS_PACKAGE_NAME@_DEPRECATED    = -3,

    /** Special marker when the application will be aborted. */
    @AFS_PACKAGE_NAME@_ABORT         = -2,

    /** Special marker for error messages which are only warnings. */
    @AFS_PACKAGE_NAME@_WARNING       = -1,

    /** Operation successful */
    @AFS_PACKAGE_NAME@_SUCCESS       = 0,

    /** Invalid error code
     *
     * Should only be used internally and not as an actual error code.
     */
    @AFS_PACKAGE_NAME@_ERROR_INVALID = 1,

    /* These are the internal implementation of POSIX error codes. */
    /** The list of arguments is too long */
    @AFS_PACKAGE_NAME@_ERROR_E2BIG,
    /** Not enough rights */
    @AFS_PACKAGE_NAME@_ERROR_EACCES,
    /** Address is not available */
    @AFS_PACKAGE_NAME@_ERROR_EADDRNOTAVAIL,
    /** Address family is not supported */
    @AFS_PACKAGE_NAME@_ERROR_EAFNOSUPPORT,
    /** Resource temporarily not available */
    @AFS_PACKAGE_NAME@_ERROR_EAGAIN,
    /** Connection is already processed */
    @AFS_PACKAGE_NAME@_ERROR_EALREADY,
    /** Invalid file pointer */
    @AFS_PACKAGE_NAME@_ERROR_EBADF,
    /** Invalid message */
    @AFS_PACKAGE_NAME@_ERROR_EBADMSG,
    /** Resource or device is busy */
    @AFS_PACKAGE_NAME@_ERROR_EBUSY,
    /** Operation was aborted */
    @AFS_PACKAGE_NAME@_ERROR_ECANCELED,
    /** No child process available */
    @AFS_PACKAGE_NAME@_ERROR_ECHILD,
    /** Connection was refused */
    @AFS_PACKAGE_NAME@_ERROR_ECONNREFUSED,
    /** Connection was reset */
    @AFS_PACKAGE_NAME@_ERROR_ECONNRESET,
    /** Resolved deadlock */
    @AFS_PACKAGE_NAME@_ERROR_EDEADLK,
    /** Destination address was expected */
    @AFS_PACKAGE_NAME@_ERROR_EDESTADDRREQ,
    /** Domain error */
    @AFS_PACKAGE_NAME@_ERROR_EDOM,
    /** Reserved */
    @AFS_PACKAGE_NAME@_ERROR_EDQUOT,
    /** File does already exist */
    @AFS_PACKAGE_NAME@_ERROR_EEXIST,
    /** Invalid address */
    @AFS_PACKAGE_NAME@_ERROR_EFAULT,
    /** File is too large */
    @AFS_PACKAGE_NAME@_ERROR_EFBIG,
    /** Operation is work in progress */
    @AFS_PACKAGE_NAME@_ERROR_EINPROGRESS,
    /** Interuption of an operating system call */
    @AFS_PACKAGE_NAME@_ERROR_EINTR,
    /** Invalid argument */
    @AFS_PACKAGE_NAME@_ERROR_EINVAL,
    /** Generic I/O error */
    @AFS_PACKAGE_NAME@_ERROR_EIO,
    /** Socket is already connected */
    @AFS_PACKAGE_NAME@_ERROR_EISCONN,
    /** Target is a directory */
    @AFS_PACKAGE_NAME@_ERROR_EISDIR,
    /** Too many layers of symbolic links */
    @AFS_PACKAGE_NAME@_ERROR_ELOOP,
    /** Too many opened files */
    @AFS_PACKAGE_NAME@_ERROR_EMFILE,
    /** Too many links */
    @AFS_PACKAGE_NAME@_ERROR_EMLINK,
    /** Message buffer is too small */
    @AFS_PACKAGE_NAME@_ERROR_EMSGSIZE,
    /** Reserved */
    @AFS_PACKAGE_NAME@_ERROR_EMULTIHOP,
    /** Filename is too long */
    @AFS_PACKAGE_NAME@_ERROR_ENAMETOOLONG,
    /** Network is down */
    @AFS_PACKAGE_NAME@_ERROR_ENETDOWN,
    /** Connection was reset from the network */
    @AFS_PACKAGE_NAME@_ERROR_ENETRESET,
    /** Network is not reachable */
    @AFS_PACKAGE_NAME@_ERROR_ENETUNREACH,
    /** Too many opened files */
    @AFS_PACKAGE_NAME@_ERROR_ENFILE,
    /** No buffer space available */
    @AFS_PACKAGE_NAME@_ERROR_ENOBUFS,
    /** No more data left in the queue */
    @AFS_PACKAGE_NAME@_ERROR_ENODATA,
    /** This device does not support this function */
    @AFS_PACKAGE_NAME@_ERROR_ENODEV,
    /** File or directory does not exist */
    @AFS_PACKAGE_NAME@_ERROR_ENOENT,
    /** Cannot execute binary */
    @AFS_PACKAGE_NAME@_ERROR_ENOEXEC,
    /** Locking failed */
    @AFS_PACKAGE_NAME@_ERROR_ENOLCK,
    /** Reserved */
    @AFS_PACKAGE_NAME@_ERROR_ENOLINK,
    /** Not enough main memory available */
    @AFS_PACKAGE_NAME@_ERROR_ENOMEM,
    /** Message has not the expected type */
    @AFS_PACKAGE_NAME@_ERROR_ENOMSG,
    /** This protocol is not available */
    @AFS_PACKAGE_NAME@_ERROR_ENOPROTOOPT,
    /** No space left on device */
    @AFS_PACKAGE_NAME@_ERROR_ENOSPC,
    /** No stream available */
    @AFS_PACKAGE_NAME@_ERROR_ENOSR,
    /** This is not a stream */
    @AFS_PACKAGE_NAME@_ERROR_ENOSTR,
    /** Requested function is not implemented */
    @AFS_PACKAGE_NAME@_ERROR_ENOSYS,
    /** Socket is not connected */
    @AFS_PACKAGE_NAME@_ERROR_ENOTCONN,
    /** This is not a directory */
    @AFS_PACKAGE_NAME@_ERROR_ENOTDIR,
    /** This directory is not empty */
    @AFS_PACKAGE_NAME@_ERROR_ENOTEMPTY,
    /** No socket */
    @AFS_PACKAGE_NAME@_ERROR_ENOTSOCK,
    /** This operation is not supported */
    @AFS_PACKAGE_NAME@_ERROR_ENOTSUP,
    /** This IOCTL is not supported by the device */
    @AFS_PACKAGE_NAME@_ERROR_ENOTTY,
    /** Device is not yet configured */
    @AFS_PACKAGE_NAME@_ERROR_ENXIO,
    /** Operation is not supported by this socket */
    @AFS_PACKAGE_NAME@_ERROR_EOPNOTSUPP,
    /** Value is to long for the datatype */
    @AFS_PACKAGE_NAME@_ERROR_EOVERFLOW,
    /** Operation is not permitted */
    @AFS_PACKAGE_NAME@_ERROR_EPERM,
    /** Broken pipe */
    @AFS_PACKAGE_NAME@_ERROR_EPIPE,
    /** Protocol error */
    @AFS_PACKAGE_NAME@_ERROR_EPROTO,
    /** Protocol is not supported */
    @AFS_PACKAGE_NAME@_ERROR_EPROTONOSUPPORT,
    /** Wrong protocol type for this socket */
    @AFS_PACKAGE_NAME@_ERROR_EPROTOTYPE,
    /** Value is out of range */
    @AFS_PACKAGE_NAME@_ERROR_ERANGE,
    /** Filesystem is read only */
    @AFS_PACKAGE_NAME@_ERROR_EROFS,
    /** This seek is not allowed */
    @AFS_PACKAGE_NAME@_ERROR_ESPIPE,
    /** No matching process found */
    @AFS_PACKAGE_NAME@_ERROR_ESRCH,
    /** Reserved */
    @AFS_PACKAGE_NAME@_ERROR_ESTALE,
    /** Timout in file stream or IOCTL */
    @AFS_PACKAGE_NAME@_ERROR_ETIME,
    /** Connection timed out */
    @AFS_PACKAGE_NAME@_ERROR_ETIMEDOUT,
    /** File could not be executed while it is opened */
    @AFS_PACKAGE_NAME@_ERROR_ETXTBSY,
    /** Operation would be blocking */
    @AFS_PACKAGE_NAME@_ERROR_EWOULDBLOCK,
    /** Invalid link between devices */
    @AFS_PACKAGE_NAME@_ERROR_EXDEV,

    /* These are the error codes specific to the @PACKAGE_NAME@ package */
    @PACKAGE_SPECIFIC_ERROR_CODES@
} @AFS_PACKAGE_NAME@_ErrorCode;


/**
 * Returns the name of an error code.
 *
 * @param errorCode : Error Code
 *
 * @returns Returns the name of a known error code, and "INVALID_ERROR" for
 *          invalid or unknown error IDs.
 * @ingroup @AFS_PACKAGE_NAME@_Exception_module
 */
const char*
@AFS_PACKAGE_NAME@_Error_GetName( @AFS_PACKAGE_NAME@_ErrorCode errorCode );


/**
 * Returns the description of an error code.
 *
 * @param errorCode : Error Code
 *
 * @returns Returns the description of a known error code.
 * @ingroup @AFS_PACKAGE_NAME@_Exception_module
 */
const char*
@AFS_PACKAGE_NAME@_Error_GetDescription( @AFS_PACKAGE_NAME@_ErrorCode errorCode );


/**
 * Signature of error handler callback functions. The error handler can be set
 * with @ref @AFS_PACKAGE_NAME@_Error_RegisterCallback.
 *
 * @param userData        : Data passed to this function as given by the registration call.
 * @param file            : Name of the source-code file where the error appeared
 * @param line            : Line number in the source code where the error appeared
 * @param function        : Name of the function where the error appeared
 * @param errorCode       : Error code
 * @param msgFormatString : Format string like it is used for the printf family.
 * @param va              : Variable argument list
 *
 * @returns Should return the errorCode
 */
typedef @AFS_PACKAGE_NAME@_ErrorCode
( *@AFS_PACKAGE_NAME@_ErrorCallback )( void*          userData,
                         const char*    file,
                         uint64_t       line,
                         const char*    function,
                         @AFS_PACKAGE_NAME@_ErrorCode errorCode,
                         const char*    msgFormatString,
                         va_list        va );


/**
 * Register a programmers callback function for error handling.
 *
 * @param errorCallbackIn : Function which will be called instead of printing a
 *                          default message to standard error.
 * @param userData :        Data pointer passed to the callback.
 *
 * @returns Function pointer to the former error handling function.
 * @ingroup @AFS_PACKAGE_NAME@_Exception_module
 *
 */
@AFS_PACKAGE_NAME@_ErrorCallback
@AFS_PACKAGE_NAME@_Error_RegisterCallback( @AFS_PACKAGE_NAME@_ErrorCallback errorCallbackIn,
                             void*              userData );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* @AFS_PACKAGE_NAME@_ERROR_CODES_H */
