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
 * @status          alpha
 * @file            MANGLE_NAME( Error.c )
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @ingroup         MANGLE_NAME( Exception_module )
 *
 * @brief           Module for error handling in SCOREP
 */


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>


#include <SCOREP_Error.h>


#include "scorep_normalize_file.h"


/*--- Internal functions -------------------------------------------*/

/**
 * Default callback function for error handling, which is used if the programmer
 * doesn't register an own callback with function MANGLE_NAME( Error_RegisterCallback )().
 *
 * @param function        : Name of the function where the error appeared
 * @param file            : Name of the source-code file where the error appeared
 * @param line            : Line number in the source-code where the error appeared
 * @param errorCode       : Error Code
 * @param msgFormatString : Format string like it is used at the printf family.
 * @param va              : Variable argument list
 *
 * @returns Should return the errorCode
 */

static MANGLE_NAME( Error_Code )
scorep_error_default_callback( const char*             package,
                               const char*             file,
                               const uint64_t line,
                               const char*             function,
                               const MANGLE_NAME( Error_Code ) errorCode,
                               const char*             msgFormatString,
                               va_list va )
{
    size_t msg_format_string_length = msgFormatString ?
                                      strlen( msgFormatString ) : 0;
    const char* type        = "error";
    const char* description = MANGLE_NAME( Error_GetDescription ) ( errorCode );
    if ( errorCode == MANGLE_NAME( WARNING ) )
    {
        type        = "warning";
        description = "";
    }

    fprintf( stderr, "[%s] %s:%" PRIu64 ": %s: %s%s",
             package, file, line,
             type, description,
             msg_format_string_length ? ": " : "\n" );

    if ( msg_format_string_length )
    {
        vfprintf( stderr, msgFormatString, va );
        fprintf( stderr, "\n" );
    }

    return errorCode;
}


/**
 * This is a pointer to a function which handles errors if SCOREP_ERROR(a) is
 * called.
 */
static MANGLE_NAME( Error_CallbackPointer ) scorep_error_callback = scorep_error_default_callback;

/*--- External visible functions ----------------------------------*/


MANGLE_NAME( Error_Code )
MANGLE_NAME( Error_Handler ) ( const char*             package,
                               const char*             srcdir,
                               const char*             builddir,
                               const char*             file,
                               const uint64_t line,
                               const char*             function,
                               const MANGLE_NAME( Error_Code ) errorCode,
                               const char*             msgFormatString,
                               ... )
{
    if ( errorCode == MANGLE_NAME( SUCCESS ) )
    {
        return errorCode;
    }

    va_list va;
    MANGLE_NAME( Error_Code ) rc;
    char* normalized_file = scorep_normalize_file( srcdir, builddir, file );

    va_start( va, msgFormatString );
    rc = scorep_error_callback( package,
                                normalized_file,
                                line,
                                function,
                                errorCode,
                                msgFormatString,
                                va );
    va_end( va );

    free( normalized_file );

    return rc;
}

MANGLE_NAME( Error_CallbackPointer )
MANGLE_NAME( Error_RegisterCallback ) ( const MANGLE_NAME( Error_CallbackPointer ) errorCallbackIn )
{
    MANGLE_NAME( Error_CallbackPointer ) temp_pointer = scorep_error_callback;

    if ( errorCallbackIn != NULL )
    {
        scorep_error_callback = errorCallbackIn;
    }
    else
    {
        scorep_error_callback = scorep_error_default_callback;
    }
    return temp_pointer;
}


/* *INDENT-OFF* */

static const struct
{
    const char*     errorName;
    const char*     errorDescription;
    MANGLE_NAME( Error_Code ) errorCode;
} error_reverse_lookup[] =
{
    #define _strx( x ) #x
    #define _str( x ) _strx( x )
    #if defined( PACKAGE_SCOREP )
    #define _e( code, description ) { _str( SCOREP_ERROR_ ## code ),  \
                                      description, \
                                      SCOREP_ERROR_ ## code }
    #elif defined( PACKAGE_OTF2 )
    #define _e( code, description ) { _str( OTF2_ERROR_ ## code ), \
                                      description, \
                                      OTF2_ERROR_ ## code }
    #else
    #error Unsupported package.
    #endif

    /* This is the SCOREP internal implementation of posix error code descriptions. */
    _e( E2BIG,                 "The list of arguments is to long" ),
    _e( EACCES,                "Not enough rights" ),
    _e( EADDRNOTAVAIL,         "Address is not available" ),
    _e( EAFNOSUPPORT,          "Address family is not supported" ),
    _e( EAGAIN,                "Resource temporaly not available" ),
    _e( EALREADY,              "Connection is already processed" ),
    _e( EBADF,                 "Invalid file pointer" ),
    _e( EBADMSG,               "Invalid message" ),
    _e( EBUSY,                 "Resource or device is busy" ),
    _e( ECANCELED,             "Operation was aborted" ),
    _e( ECHILD,                "No child process available" ),
    _e( ECONNREFUSED,          "Connection was refused" ),
    _e( ECONNRESET,            "Connection was reset" ),
    _e( EDEADLK,               "Resolved deadlock" ),
    _e( EDESTADDRREQ,          "Destination address was expected" ),
    _e( EDOM,                  "Domain error" ),
    _e( EDQUOT,                "Reserved" ),
    _e( EEXIST,                "File does already exist" ),
    _e( EFAULT,                "Invalid Address" ),
    _e( EFBIG,                 "File is to big" ),
    _e( EINPROGRESS,           "Operation is work in progress" ),
    _e( EINTR,                 "Interuption of an operating system call" ),
    _e( EINVAL,                "Invalid argument" ),
    _e( EIO,                   "Generic I/O error" ),
    _e( EISCONN,               "Socket is already connected" ),
    _e( EISDIR,                "Target is a directory" ),
    _e( ELOOP,                 "To many layers of symbolic links" ),
    _e( EMFILE,                "To many opened files" ),
    _e( EMLINK,                "To many links" ),
    _e( EMSGSIZE,              "Message buffer is to small" ),
    _e( EMULTIHOP,             "Reserved" ),
    _e( ENAMETOOLONG,          "Filename is to long" ),
    _e( ENETDOWN,              "Network is down" ),
    _e( ENETRESET,             "Connection was reset from the network" ),
    _e( ENETUNREACH,           "Network is not reachable" ),
    _e( ENFILE,                "To much opened files" ),
    _e( ENOBUFS,               "No buffer space available" ),
    _e( ENODATA,               "No more data left in the queue" ),
    _e( ENODEV,                "This device does not support this function" ),
    _e( ENOENT,                "File or Directory does not exist" ),
    _e( ENOEXEC,               "Can not execute binary" ),
    _e( ENOLCK,                "Locking failed" ),
    _e( ENOLINK,               "Reserved" ),
    _e( ENOMEM,                "Not enough main memory available" ),
    _e( ENOMSG,                "Message has not the expected type" ),
    _e( ENOPROTOOPT,           "This protocol is not available" ),
    _e( ENOSPC,                "No space left on device" ),
    _e( ENOSR,                 "No stream available" ),
    _e( ENOSTR,                "This is not a stream" ),
    _e( ENOSYS,                "Requested function is not implemented" ),
    _e( ENOTCONN,              "Socket is not connected" ),
    _e( ENOTDIR,               "This is not an directory" ),
    _e( ENOTEMPTY,             "This directory is not empty" ),
    _e( ENOTSOCK,              "No socket" ),
    _e( ENOTSUP,               "This operation is not supported" ),
    _e( ENOTTY,                "This IOCTL is not supported by the device" ),
    _e( ENXIO,                 "Device is not yet configured" ),
    _e( EOPNOTSUPP,            "Operation is not supported by this socket" ),
    _e( EOVERFLOW,             "Value is to long for the datatype" ),
    _e( EPERM,                 "Operation is not permitted" ),
    _e( EPIPE,                 "Broken pipe" ),
    _e( EPROTO,                "Protocoll error" ),
    _e( EPROTONOSUPPORT,       "Protocoll is not supported" ),
    _e( EPROTOTYPE,            "Wrong protocoll type for this socket" ),
    _e( ERANGE,                "Value is out of range" ),
    _e( EROFS,                 "Filesystem is read only" ),
    _e( ESPIPE,                "This seek is not allowed" ),
    _e( ESRCH,                 "No matching process found" ),
    _e( ESTALE,                "Reserved" ),
    _e( ETIME,                 "Timout in file stream or IOCTL" ),
    _e( ETIMEDOUT,             "Connection timed out" ),
    _e( ETXTBSY,               "File couldn't be executed while it is opened" ),
    _e( EWOULDBLOCK,           "Operation would be blocking" ),
    _e( EXDEV,                 "Invalid link between devices" ),

    _e( END_OF_FUNCTION,       "Unintentional reached end of function" ),
    _e( INVALID_CALL,          "Function call not allowed in current state" ),
    _e( INVALID_ARGUMENT,      "Parameter value out of range" ),
    _e( INVALID_RECORD,        "Invalid definition or event record" ),
    _e( INVALID_DATA,          "Invalid or inconsistent record data" ),
    _e( CANT_HANDLE_EVENT,     "Unhandable event" ),
    _e( INVALID_SIZE_GIVEN,    "The given size can not be used" ),
    _e( UNKNOWN_TYPE,          "The given type is not known" ),
    _e( INVALID_POINTER,       "This pointer is invalid" ),
    _e( COULD_NOT_BE_FREED,    "This pointer memory could not be freed" ),
    _e( INTEGRITY_FAULT,       "The structural integrity is not given" ),
    _e( MEM_FAULT,             "This could not be done with the given memory" ),
    _e( MEM_ALLOC_FAILED,      "Memory allocation failed" ),

    _e( PROCESSED_WITH_FAULTS, "An error appeared when data was processed" ),
    _e( ASSERTION_FAILED,      "An assertion failed" ),
    _e( INDEX_OUT_OF_BOUNDS,   "Index out of bounds" ),
    _e( F2C_INT_SIZE_MISMATCH, "Fortran and C integer size mismatch" ),
    _e( UNKNOWN_REGION_TYPE,   "Unknown region type" ),
    _e( INVALID_LINENO,        "Invalid source code line number" ),
    _e( END_OF_BUFFER,         "End of buffer/file reached" ),

    _e( MPI_NO_WINDOW,         "Cannot find MPI window" ),
    _e( MPI_NO_COMM,           "Cannot find MPI communicator" ),
    _e( MPI_TOO_MANY_WINDOWS,  "Too many MPI windows are created" ),
    _e( MPI_TOO_MANY_COMMS,    "Too many MPI communicators are created" ),
    _e( MPI_TOO_MANY_GROUPS,   "Too many MPI groups are created" ),
    _e( MPI_NO_GROUP,          "Cannot find MPI group" ),
    _e( MPI_TOO_MANY_WINACCS,  "Too many concurrent window accesses" ),
    _e( MPI_NO_WINACC,         "Cannot find window access entry" ),
    _e( MPI_NO_LAST_REQUEST,   "Missing termination in tracking data structure." ),

    _e( USER_INVALID_REGION,   "Usage of an invalid region handle" ),
    _e( USER_INVALID_MGROUP,   "Usage of an invalid metric group handle" ),
    _e( FILE_INTERACTION,      "Invalid file operation" ),
    _e( FILE_CAN_NOT_OPEN,     "Unable to open file" ),

    _e( PARSE_UNKNOWN_TOKEN,   "Error while parsing: Unknown token" ),
    _e( PARSE_UNEXPECTED_END,  "Error while parsing: Unexpected end of string" ),
    _e( PARSE_NO_SEPARATOR,    "Error while parsing: Missing seperator" ),
    _e( PARSE_NO_KEY,          "Error while parsing: Missing key" ),
    _e( PARSE_NO_VALUE,        "Error while parsing: Missing value" ),
    _e( PARSE_INVALID_VALUE,   "Error while parsing: Invalid value" ),
    _e( PARSE_SYNTAX,          "Error while parsing: Syntax error" ),

    _e( POMP_SCL_BROKEN,       "Expect location format \"filename:lineNo1:lineNo2\"" ),
    _e( POMP_UNKNOWN_SCHEDULE, "Unknown schedule specified" ),
    _e( POMP_INVALID_SECNUM,   "Invalid number of sections. Exspected > 0 sections" ),
    _e( POMP_NO_NAME,          "Name of user region missing" ),

    _e( PROFILE_INCONSISTENT,  "Inconsistent profile. Stop profiling" ),
    _e( PROFILE_UNINITIALIZED_THREAD, "Profiling accessed uninitialized thread" ),

    _e( PAPI_INIT,             "Could not initialize PAPI library" ),

    _e( MEMORY_OUT_OF_PAGES,   "No free memory page available" ),
    _e( SWITCH_IN_PARALLEL,    "Switching recording on or off in a parallel region is not allowed. Please consult the manual for information, where recording can be switched on/off" ),
    _e( OA_PARSE_MRI,          "Could not parse MRI request" ),
    _e( ON_SYSTEM_CALL,        "Error in execution of system command" ),


    _e( POMP_UNKNOWN_REGION_TYPE, "Unknown POMP region type specified in CTC string" ),
    #undef _strx
    #undef _str
    #undef _e
};

uint64_t MANGLE_NAME( error_reverse_lookup_size ) =
    sizeof( error_reverse_lookup ) /
    sizeof( error_reverse_lookup[ 0 ] );


static const struct
{
    MANGLE_NAME( Error_Code ) scorepErrorCode;
    int             posixErrno;
} posix_errno_to_scorep_error_translation_table[] =
{
    #if defined( PACKAGE_SCOREP )
    #define _e( code ) { SCOREP_ERROR_ ## code, code }
    #elif defined( PACKAGE_OTF2 )
    #define _e( code ) { OTF2_ERROR_ ## code, code }
    #else
    #error Unsupported package.
    #endif

#ifdef EACCES
    _e( EACCES ),           //  0
#endif
#ifdef EADDRNOTAVAIL
    _e( EADDRNOTAVAIL ),    //  1
#endif
#ifdef EAFNOSUPPORT
    _e( EAFNOSUPPORT ),     //  2
#endif
#ifdef EAGAIN
    _e( EAGAIN ),           //  3
#endif
#ifdef EALREADY
    _e( EALREADY ),         //  4
#endif
#ifdef EBADF
    _e( EBADF ),            //  5
#endif
#ifdef EBADMSG
    _e( EBADMSG ),          //  6
#endif
#ifdef EBUSY
    _e( EBUSY ),            //  7
#endif
#ifdef ECANCELED
    _e( ECANCELED ),        //  8
#endif
#ifdef ECHILD
    _e( ECHILD ),           //  9
#endif
#ifdef ECONNREFUSED
    _e( ECONNREFUSED ),     // 10
#endif
#ifdef ECONNRESET
    _e( ECONNRESET ),       // 11
#endif
#ifdef EDEADLK
    _e( EDEADLK ),          // 12
#endif
#ifdef EDESTADDRREQ
    _e( EDESTADDRREQ ),     // 13
#endif
#ifdef EDOM
    _e( EDOM ),             // 14
#endif
#ifdef EDQUOT
    _e( EDQUOT ),           // 15
#endif
#ifdef EEXIST
    _e( EEXIST ),           // 16
#endif
#ifdef EFAULT
    _e( EFAULT ),           // 17
#endif
#ifdef EFBIG
    _e( EFBIG ),            // 18
#endif
#ifdef EINPROGRESS
    _e( EINPROGRESS ),      // 19
#endif
#ifdef EINTR
    _e( EINTR ),            // 20
#endif
#ifdef EINVAL
    _e( EINVAL ),           // 21
#endif
#ifdef EIO
    _e( EIO ),              // 22
#endif
#ifdef EISCONN
    _e( EISCONN ),          // 23
#endif
#ifdef EISDIR
    _e( EISDIR ),           // 24
#endif
#ifdef ELOOP
    _e( ELOOP ),            // 25
#endif
#ifdef EMFILE
    _e( EMFILE ),           // 26
#endif
#ifdef EMLINK
    _e( EMLINK ),           // 27
#endif
#ifdef EMSGSIZE
    _e( EMSGSIZE ),         // 28
#endif
#ifdef EMULTIHOP
    _e( EMULTIHOP ),        // 29
#endif
#ifdef ENAMETOOLONG
    _e( ENAMETOOLONG ),     // 30
#endif
#ifdef ENETDOWN
    _e( ENETDOWN ),         // 31
#endif
#ifdef ENETRESET
    _e( ENETRESET ),        // 32
#endif
#ifdef ENETUNREACH
    _e( ENETUNREACH ),      // 33
#endif
#ifdef ENFILE
    _e( ENFILE ),           // 34
#endif
#ifdef ENOBUFS
    _e( ENOBUFS ),          // 35
#endif
#ifdef ENODATA
    _e( ENODATA ),          // 36
#endif
#ifdef ENODEV
    _e( ENODEV ),           // 37
#endif
#ifdef ENOENT
    _e( ENOENT ),           // 38
#endif
#ifdef ENOEXEC
    _e( ENOEXEC ),          // 39
#endif
#ifdef ENOLCK
    _e( ENOLCK ),           // 40
#endif
#ifdef ENOLINK
    _e( ENOLINK ),          // 41
#endif
#ifdef ENOMEM
    _e( ENOMEM ),           // 42
#endif
#ifdef ENOMSG
    _e( ENOMSG ),           // 43
#endif
#ifdef ENOPROTOOPT
    _e( ENOPROTOOPT ),      // 44
#endif
#ifdef ENOSPC
    _e( ENOSPC ),           // 45
#endif
#ifdef ENOSR
    _e( ENOSR ),            // 46
#endif
#ifdef ENOSTR
    _e( ENOSTR ),           // 47
#endif
#ifdef ENOSYS
    _e( ENOSYS ),           // 48
#endif
#ifdef ENOTCONN
    _e( ENOTCONN ),         // 49
#endif
#ifdef ENOTDIR
    _e( ENOTDIR ),          // 50
#endif
#ifdef ENOTEMPTY
    _e( ENOTEMPTY ),        // 51
#endif
#ifdef ENOTSOCK
    _e( ENOTSOCK ),         // 52
#endif
#ifdef ENOTSUP
    _e( ENOTSUP ),          // 53
#endif
#ifdef ENOTTY
    _e( ENOTTY ),           // 54
#endif
#ifdef ENXIO
    _e( ENXIO ),            // 55
#endif
#ifdef EOPNOTSUPP
    _e( EOPNOTSUPP ),       // 56
#endif
#ifdef EOVERFLOW
    _e( EOVERFLOW ),        // 57
#endif
#ifdef EPERM
    _e( EPERM ),            // 58
#endif
#ifdef EPIPE
    _e( EPIPE ),            // 59
#endif
#ifdef EPROTO
    _e( EPROTO ),           // 60
#endif
#ifdef EPROTONOSUPPORT
    _e( EPROTONOSUPPORT ),  // 61
#endif
#ifdef EPROTOTYPE
    _e( EPROTOTYPE ),       // 62
#endif
#ifdef ERANGE
    _e( ERANGE ),           // 63
#endif
#ifdef EROFS
    _e( EROFS ),            // 64
#endif
#ifdef ESPIPE
    _e( ESPIPE ),           // 65
#endif
#ifdef ESRCH
    _e( ESRCH ),            // 66
#endif
#ifdef ESTALE
    _e( ESTALE ),           // 67
#endif
#ifdef ETIME
    _e( ETIME ),            // 68
#endif
#ifdef ETIMEDOUT
    _e( ETIMEDOUT ),        // 69
#endif
#ifdef ETXTBSY
    _e( ETXTBSY ),          // 70
#endif
#ifdef EWOULDBLOCK
    _e( EWOULDBLOCK ),      // 71
#endif
#ifdef EXDEV
    _e( EXDEV ),            // 72
#endif

    #undef _e
};
/* *INDENT-ON* */

static uint64_t posix_errno_to_scorep_error_translation_table_size =
    sizeof( posix_errno_to_scorep_error_translation_table ) /
    sizeof( posix_errno_to_scorep_error_translation_table[ 0 ] );


const char*
MANGLE_NAME( Error_GetName ) ( const MANGLE_NAME( Error_Code ) errorCode )
{
    if ( MANGLE_NAME( SUCCESS ) == errorCode )
    {
        return "MANGLE_NAME( SUCCESS )";
    }

    int64_t index = errorCode - MANGLE_NAME( ERROR_CODE_OFFSET );

    if ( ( errorCode >= 0 ) || ( errorCode < MANGLE_NAME( ERROR_CODE_OFFSET ) ) ||
         ( index < 0 ) || ( index >= MANGLE_NAME( error_reverse_lookup_size ) ) )
    {
        return "MANGLE_NAME( ERROR_INVALID )";
    }

    /* See scorep_error_codes.h for the definition of this array */
    return error_reverse_lookup[ index ].errorName;
}


const char*
MANGLE_NAME( Error_GetDescription ) ( const MANGLE_NAME( Error_Code ) errorCode )
{
    if ( MANGLE_NAME( SUCCESS ) == errorCode )
    {
        return "Success";
    }

    if ( MANGLE_NAME( WARNING ) == errorCode )
    {
        return "Warning";
    }

    int64_t index = errorCode - MANGLE_NAME( ERROR_CODE_OFFSET );

    if ( ( errorCode >= 0 ) || ( errorCode < MANGLE_NAME( ERROR_CODE_OFFSET ) ) ||
         ( index < 0 ) || ( index >= MANGLE_NAME( error_reverse_lookup_size ) ) )
    {
        return "Unknown error code";
    }

    /* See scorep_error_codes.h for the definition of this array */
    return error_reverse_lookup[ index ].errorDescription;
}


MANGLE_NAME( Error_Code )
MANGLE_NAME( Error_PosixToScorep ) ( const int posixErrno )
{
    uint64_t i;

    if ( posixErrno == 0 )
    {
        return MANGLE_NAME( SUCCESS );
    }

    for ( i = 0; i < posix_errno_to_scorep_error_translation_table_size; i++ )
    {
        if ( posix_errno_to_scorep_error_translation_table[ i ].posixErrno ==
             posixErrno )
        {
            return posix_errno_to_scorep_error_translation_table[ i ].scorepErrorCode;
        }
    }

    return MANGLE_NAME( ERROR_INVALID );
}
