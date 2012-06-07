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

#ifndef SCOREP_ERROR_CODES_H
#define SCOREP_ERROR_CODES_H


/**
 * @file            MANGLE_NAME( Error_Codes.h )
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status          ALPHA
 * @ingroup         MANGLE_NAME( Exception_module )
 *
 * @brief           Error codes and error handling.
 *
 * @author          Dominic Eschweiler <d.eschweiler@fz-juelich.de>
 */

#include <errno.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t MANGLE_NAME ( Error_Code );

/** This offset is used for indexing purposes. See enum @ref MANGLE_NAME( Error_Type ) */
enum { MANGLE_NAME( ERROR_CODE_OFFSET ) = -512 };


/** Operation successful */
enum { MANGLE_NAME( SUCCESS ) = 0 };

/** Invalid error code
 *
 * Should only be used internally and not as an actual error code.
 *
 * This is by intention not inside the enum @a MANGLE_NAME( Error_Type ), to make it
 * more clear, that this is not an error code.
 */
enum { MANGLE_NAME( ERROR_INVALID ) = 1 };

/** Special marker for error messages which are only warnings. */
enum { MANGLE_NAME( WARNING ) = 2 };

/**
 * This is the list of error codes in SCOREP. If you want to add a new error code,
 * please also edit error_reverse_lookup in MANGLE_NAME( Error.c )
 */
typedef enum
{
    /* This is the SCOREP internal implementation of posix error codes. */
    MANGLE_NAME( ERROR_E2BIG ) = MANGLE_NAME( ERROR_CODE_OFFSET ),
    MANGLE_NAME( ERROR_EACCES ),
    MANGLE_NAME( ERROR_EADDRNOTAVAIL ),
    MANGLE_NAME( ERROR_EAFNOSUPPORT ),
    MANGLE_NAME( ERROR_EAGAIN ),
    MANGLE_NAME( ERROR_EALREADY ),
    MANGLE_NAME( ERROR_EBADF ),
    MANGLE_NAME( ERROR_EBADMSG ),
    MANGLE_NAME( ERROR_EBUSY ),
    MANGLE_NAME( ERROR_ECANCELED ),
    MANGLE_NAME( ERROR_ECHILD ),
    MANGLE_NAME( ERROR_ECONNREFUSED ),
    MANGLE_NAME( ERROR_ECONNRESET ),
    MANGLE_NAME( ERROR_EDEADLK ),
    MANGLE_NAME( ERROR_EDESTADDRREQ ),
    MANGLE_NAME( ERROR_EDOM ),
    MANGLE_NAME( ERROR_EDQUOT ),
    MANGLE_NAME( ERROR_EEXIST ),
    MANGLE_NAME( ERROR_EFAULT ),
    MANGLE_NAME( ERROR_EFBIG ),
    MANGLE_NAME( ERROR_EINPROGRESS ),
    MANGLE_NAME( ERROR_EINTR ),
    MANGLE_NAME( ERROR_EINVAL ),
    MANGLE_NAME( ERROR_EIO ),
    MANGLE_NAME( ERROR_EISCONN ),
    MANGLE_NAME( ERROR_EISDIR ),
    MANGLE_NAME( ERROR_ELOOP ),
    MANGLE_NAME( ERROR_EMFILE ),
    MANGLE_NAME( ERROR_EMLINK ),
    MANGLE_NAME( ERROR_EMSGSIZE ),
    MANGLE_NAME( ERROR_EMULTIHOP ),
    MANGLE_NAME( ERROR_ENAMETOOLONG ),
    MANGLE_NAME( ERROR_ENETDOWN ),
    MANGLE_NAME( ERROR_ENETRESET ),
    MANGLE_NAME( ERROR_ENETUNREACH ),
    MANGLE_NAME( ERROR_ENFILE ),
    MANGLE_NAME( ERROR_ENOBUFS ),
    MANGLE_NAME( ERROR_ENODATA ),
    MANGLE_NAME( ERROR_ENODEV ),
    MANGLE_NAME( ERROR_ENOENT ),
    MANGLE_NAME( ERROR_ENOEXEC ),
    MANGLE_NAME( ERROR_ENOLCK ),
    MANGLE_NAME( ERROR_ENOLINK ),
    MANGLE_NAME( ERROR_ENOMEM ),
    MANGLE_NAME( ERROR_ENOMSG ),
    MANGLE_NAME( ERROR_ENOPROTOOPT ),
    MANGLE_NAME( ERROR_ENOSPC ),
    MANGLE_NAME( ERROR_ENOSR ),
    MANGLE_NAME( ERROR_ENOSTR ),
    MANGLE_NAME( ERROR_ENOSYS ),
    MANGLE_NAME( ERROR_ENOTCONN ),
    MANGLE_NAME( ERROR_ENOTDIR ),
    MANGLE_NAME( ERROR_ENOTEMPTY ),
    MANGLE_NAME( ERROR_ENOTSOCK ),
    MANGLE_NAME( ERROR_ENOTSUP ),
    MANGLE_NAME( ERROR_ENOTTY ),
    MANGLE_NAME( ERROR_ENXIO ),
    MANGLE_NAME( ERROR_EOPNOTSUPP ),
    MANGLE_NAME( ERROR_EOVERFLOW ),
    MANGLE_NAME( ERROR_EPERM ),
    MANGLE_NAME( ERROR_EPIPE ),
    MANGLE_NAME( ERROR_EPROTO ),
    MANGLE_NAME( ERROR_EPROTONOSUPPORT ),
    MANGLE_NAME( ERROR_EPROTOTYPE ),
    MANGLE_NAME( ERROR_ERANGE ),
    MANGLE_NAME( ERROR_EROFS ),
    MANGLE_NAME( ERROR_ESPIPE ),
    MANGLE_NAME( ERROR_ESRCH ),
    MANGLE_NAME( ERROR_ESTALE ),
    MANGLE_NAME( ERROR_ETIME ),
    MANGLE_NAME( ERROR_ETIMEDOUT ),
    MANGLE_NAME( ERROR_ETXTBSY ),
    MANGLE_NAME( ERROR_EWOULDBLOCK ),
    MANGLE_NAME( ERROR_EXDEV ),

    MANGLE_NAME( ERROR_END_OF_FUNCTION ),
    MANGLE_NAME( ERROR_INVALID_CALL ),
    MANGLE_NAME( ERROR_INVALID_ARGUMENT ),
    MANGLE_NAME( ERROR_INVALID_RECORD ),
    MANGLE_NAME( ERROR_INVALID_DATA ),
    MANGLE_NAME( ERROR_CANT_HANDLE_EVENT ),
    MANGLE_NAME( ERROR_INVALID_SIZE_GIVEN ),
    MANGLE_NAME( ERROR_UNKNOWN_TYPE ),
    MANGLE_NAME( ERROR_INVALID_POINTER ),
    MANGLE_NAME( ERROR_COULD_NOT_BE_FREED ),
    MANGLE_NAME( ERROR_INTEGRITY_FAULT ),
    MANGLE_NAME( ERROR_MEM_FAULT ),
    MANGLE_NAME( ERROR_MEM_ALLOC_FAILED ),
    MANGLE_NAME( ERROR_PROCESSED_WITH_FAULTS ),
    MANGLE_NAME( ERROR_ASSERTION_FAILED ),
    MANGLE_NAME( ERROR_INDEX_OUT_OF_BOUNDS ),
    MANGLE_NAME( ERROR_F2C_INT_SIZE_MISMATCH ),
    MANGLE_NAME( ERROR_UNKNOWN_REGION_TYPE ),
    MANGLE_NAME( ERROR_INVALID_LINENO ),
    MANGLE_NAME( ERROR_END_OF_BUFFER ),

    MANGLE_NAME( ERROR_MPI_NO_WINDOW ),
    MANGLE_NAME( ERROR_MPI_NO_COMM ),
    MANGLE_NAME( ERROR_MPI_TOO_MANY_WINDOWS ),
    MANGLE_NAME( ERROR_MPI_TOO_MANY_COMMS ),
    MANGLE_NAME( ERROR_MPI_TOO_MANY_GROUPS ),
    MANGLE_NAME( ERROR_MPI_NO_GROUP ),
    MANGLE_NAME( ERROR_MPI_TOO_MANY_WINACCS ),
    MANGLE_NAME( ERROR_MPI_NO_WINACC ),
    MANGLE_NAME( ERROR_MPI_NO_LAST_REQUEST ),

    MANGLE_NAME( ERROR_USER_INVALID_REGION ),
    MANGLE_NAME( ERROR_USER_INVALID_MGROUP ),

    MANGLE_NAME( ERROR_FILE_INTERACTION ),
    MANGLE_NAME( ERROR_FILE_CAN_NOT_OPEN ),

    MANGLE_NAME( ERROR_PARSE_UNKNOWN_TOKEN ),
    MANGLE_NAME( ERROR_PARSE_UNEXPECTED_END ),
    MANGLE_NAME( ERROR_PARSE_NO_SEPARATOR ),
    MANGLE_NAME( ERROR_PARSE_NO_KEY ),
    MANGLE_NAME( ERROR_PARSE_NO_VALUE ),
    MANGLE_NAME( ERROR_PARSE_INVALID_VALUE ),
    MANGLE_NAME( ERROR_PARSE_SYNTAX ),

    MANGLE_NAME( ERROR_POMP_SCL_BROKEN ),
    MANGLE_NAME( ERROR_POMP_UNKNOWN_SCHEDULE ),
    MANGLE_NAME( ERROR_POMP_INVALID_SECNUM ),
    MANGLE_NAME( ERROR_POMP_NO_NAME ),

    MANGLE_NAME( ERROR_PROFILE_INCONSISTENT ),
    MANGLE_NAME( ERROR_PROFILE_UNINITIALIZED_THREAD ),

    MANGLE_NAME( ERROR_PAPI_INIT ),

    MANGLE_NAME( ERROR_MEMORY_OUT_OF_PAGES ),
    MANGLE_NAME( ERROR_SWITCH_IN_PARALLEL ),
    MANGLE_NAME( ERROR_OA_PARSE_MRI ),
    MANGLE_NAME( ERROR_ON_SYSTEM_CALL ),

    MANGLE_NAME( ERROR_POMP_UNKNOWN_REGION_TYPE ),
    // If you want to add a new error code, please
    // also edit error_reverse_lookup in MANGLE_NAME( Error.c )
} MANGLE_NAME ( Error_Type );


/**
 * Signature of error handler callback functions. The error handler can be set
 * with @ref MANGLE_NAME( Error_RegisterCallback ).
 *
 * @param function        : Name of the function where the error appeared
 * @param file            : Name of the source-code file where the error appeared
 * @param line            : Line number in the source-code where the error appeared
 * @param errorCode       : Error Code
 * @param msgFormatString : Format string like it is used at the printf family.
 * @param va              : Variable argument list
 *
 * @returns Should return the errorCode
 * @ingroup MANGLE_NAME( Exception_module )
 */
typedef int64_t ( *MANGLE_NAME( Error_CallbackPointer ) )( const char*                    package,
                                                           const char*                    file,
                                                           const uint64_t                 line,
                                                           const char*                    function,
                                                           const MANGLE_NAME( Error_Code )errorCode,
                                                           const char*                    msgFormatString,
                                                           va_list                        va );

/**
 * Returns the name of an error code.
 *
 * @param errorCode : Error Code
 *
 * @returns Returns the name of a known error code, and "INVALID_ERROR" for
 *          invalid or unknown error IDs.
 * @ingroup MANGLE_NAME( Exception_module )
 */
const char*
MANGLE_NAME( Error_GetName ) ( const MANGLE_NAME( Error_Code ) errorCode );

/**
 * Returns the description of an error code.
 *
 * @param errorCode : Error Code
 *
 * @returns Returns the description of a known error code.
 * @ingroup MANGLE_NAME( Exception_module )
 */
const char*
MANGLE_NAME( Error_GetDescription ) ( const MANGLE_NAME( Error_Code ) errorCode );


/**
 * Register a programmers callback function for error handling.
 *
 * @param errorCallbackIn : Function pointer which points to a callback
 *                          function (type is defined in scorep_error.h). If it is
 *                          NULL the default error handler is set.
 *
 * @returns Function pointer to the former error handling function.
 * @ingroup MANGLE_NAME( Exception_module )
 *
 */
MANGLE_NAME( Error_CallbackPointer )
MANGLE_NAME( Error_RegisterCallback ) ( const MANGLE_NAME( Error_CallbackPointer ) errorCallbackIn );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCOREP_ERROR_CODES_H */
