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

#ifndef SCOREP_DEBUG_H
#define SCOREP_DEBUG_H

/**
 * @file            MANGLE_NAME( Debug.h )
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status          REVIEW
 * @ingroup         MANGLE_NAME( Exception_module )
 *
 * @brief           Module for debug output handling in SCOREP
 *
 * @author          Dominic Eschweiler <d.eschweiler@fz-juelich.de>
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifndef HAVE
#define HAVE( H ) ( defined( HAVE_##H ) && HAVE_##H )
#endif



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MANGLE_NAME( Exception_module ) SCOREP Debug and Error Handling
 *
 * This module provides an interface for error handling and debugging output.
 *
 * For invoke error handling, two macros are provided: @ref SCOREP_ERROR for generic
 * SCOREP errors, and MANGLE_NAME( ERROR_POSIX ) for handling errors occured in posix calls.
 * To handle an error, own error handlers can be registered using
 * @ref MANGLE_NAME( Error_RegisterCallback ). The default error handler prints the error
 * message to the standard error output.
 *
 * For debug output the macro @ref MANGLE_NAME( DEBUG_PRINTF ) is provided. For each debug
 * message, a debug level must be provided. The print out of the messages can be filtered
 * depending on the debug level.
 * The second debug macro is @ref MANGLE_NAME( ASSERT ) which ensures that a condition holds.
 *
 * The debug macros resolve to nothing if HAVE_MANGLE_NAME( DEBUG ) is not defined or zero.
 * Then no overhead and no debug output is created.
 *
 * @{
 */

/**
 * List of debug levels. Every debug message printed with @ref
 * MANGLE_NAME( DEBUG_PRINTF ) must provide a debug level. Based on the debug
 * level, the debug output can be filtered for the debugged parts. The debug level
 * is a bitstring, where every level is represented by one bit. It is
 * possible to combine several levels.
 */
typedef enum
{
    MANGLE_NAME( DEBUG_FUNCTION_ENTRY )      = 1 <<  0,
    MANGLE_NAME( DEBUG_FUNCTION_EXIT )       = 1 <<  1,
    MANGLE_NAME( DEBUG_USER_CALLBACK_ENTRY ) = 1 <<  2,
    MANGLE_NAME( DEBUG_USER_CALLBACK_EXIT )  = 1 <<  3,
    MANGLE_NAME( DEBUG_INTERNAL_STEP )       = 1 <<  4,

    MANGLE_NAME( DEBUG_WARNING )             = 1 <<  5,

    MANGLE_NAME( DEBUG_MPI )                 = 1 <<  6,
    MANGLE_NAME( DEBUG_USER )                = 1 <<  7,
    MANGLE_NAME( DEBUG_COMPILER )            = 1 <<  8,
    MANGLE_NAME( DEBUG_OPENMP )              = 1 <<  9,
    MANGLE_NAME( DEBUG_EVENTS )              = 1 << 10,
    MANGLE_NAME( DEBUG_DEFINITIONS )         = 1 << 11,
    MANGLE_NAME( DEBUG_CONFIG )              = 1 << 12,
    MANGLE_NAME( DEBUG_PROFILE )             = 1 << 13,
    MANGLE_NAME( DEBUG_OA )                  = 1 << 14,
    MANGLE_NAME( DEBUG_MPIPROFILING )        = 1 << 15,
    MANGLE_NAME( DEBUG_FILTERING )           = 1 << 16,
    MANGLE_NAME( DEBUG_METRIC )              = 1 << 17,
    MANGLE_NAME( DEBUG_TRACING )             = 1 << 18,
    MANGLE_NAME( DEBUG_MEMORY )              = 1 << 19,
    MANGLE_NAME( DEBUG_CUDA )                = 1 << 20,
    MANGLE_NAME( DEBUG_MAQAO )               = 1 << 22,

    /* keep this last */
    MANGLE_NAME( DEBUG_DEPRECATED )          = 1 << 30
} MANGLE_NAME ( Debug_Levels );


/**
 * @def MANGLE_NAME( DEBUG_PRINTF )
 * Following prep is the SCOREP debug messaging function. It could be used like
 * printf, but must get passed a debug level as first parameter. In difference
 * to other debug message systems, the debug level is a bit mask where every
 * level could be switched of and on individually. To set the correct debug level
 * please use the enum MANGLE_NAME( DebugLevels ).
 *
 * To set the current debug level, the source must not be recompiled. The current
 * debug level bitmask could be set through adding the bit-values of the debug
 * level which should be printed and assigning the result to the shell variable
 * MANGLE_NAME( DEBUG ). For example MANGLE_NAME( DEBUG )=7 for debug level 1 and 2 and 4.
 * @param debugLevel The debugLevel which must be enabled to print out the message.
 * @param ...        A format string followed by the parameters defined in the format
 *                   string. The format string and the parameters have the same syntax
 *                   like in the POSIX printf function.
 */

#if defined( PACKAGE_SCOREP )
    #if HAVE( SCOREP_DEBUG )
    #define SCOREP_DEBUG_PRINTF( debugLevel, ... ) SCOREP_Debug_Printf( \
        debugLevel,                                                 \
        PACKAGE_NAME,                                               \
        PACKAGE_SRCDIR,                                             \
        PACKAGE_BUILDDIR,                                           \
        __FILE__,                                                   \
        __LINE__,                                                   \
        __func__,                                                   \
        __VA_ARGS__ )
    #else
    #define SCOREP_DEBUG_PRINTF( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#elif defined( PACKAGE_OTF2 )
    #if HAVE( SCOREP_DEBUG )
    #define OTF2_DEBUG_PRINTF( debugLevel, ... ) OTF2_Debug_Printf( \
        debugLevel,                                             \
        PACKAGE_NAME,                                           \
        PACKAGE_SRCDIR,                                         \
        PACKAGE_BUILDDIR,                                       \
        __FILE__,                                               \
        __LINE__,                                               \
        __func__,                                               \
        __VA_ARGS__ )
    #else
    #define OTF2_DEBUG_PRINTF( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#else
#error Unsupported package.
#endif


/**
 * Use this if you don't want the prefix and newline of MANGLE_NAME( DEBUG_PRINTF )()
 */
#if defined( PACKAGE_SCOREP )
    #if HAVE( SCOREP_DEBUG )
    #define SCOREP_DEBUG_RAW_PRINTF( debugLevel, ... )  \
    SCOREP_Debug_RawPrintf( debugLevel, __VA_ARGS__ )
    #else
    #define SCOREP_DEBUG_RAW_PRINTF( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#elif defined( PACKAGE_OTF2 )
    #if HAVE( SCOREP_DEBUG )
    #define OTF2_DEBUG_RAW_PRINTF( debugLevel, ... )  \
    OTF2_Debug_RawPrintf( debugLevel, __VA_ARGS__ )
    #else
    #define OTF2_DEBUG_RAW_PRINTF( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#else
#error Unsupported package.
#endif


/**
 * Use this if you just want the prefix and no new line.
 */
#if defined( PACKAGE_SCOREP )
    #if HAVE( SCOREP_DEBUG )
    #define SCOREP_DEBUG_PREFIX( debugLevel ) SCOREP_Debug_Prefix(  \
        debugLevel,                                             \
        PACKAGE_NAME,                                           \
        PACKAGE_SRCDIR,                                         \
        PACKAGE_BUILDDIR,                                       \
        __FILE__,                                               \
        __LINE__,                                               \
        __func__ )
    #else
    #define SCOREP_DEBUG_PREFIX( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#elif defined( PACKAGE_OTF2 )
    #if HAVE( SCOREP_DEBUG )
    #define OTF2_DEBUG_PREFIX( debugLevel ) OTF2_Debug_Prefix(  \
        debugLevel,                                         \
        PACKAGE_NAME,                                       \
        PACKAGE_SRCDIR,                                     \
        PACKAGE_BUILDDIR,                                   \
        __FILE__,                                           \
        __LINE__,                                           \
        __func__ )
    #else
    #define OTF2_DEBUG_PREFIX( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#else
#error Unsupported package.
#endif


/**
 * Use this to hide code, especialy variables, which are only accessed in
 * debug mode.
 *
 * @note You should not use a ';' after this statement. But ... needs to be a
 *       valid C statement including ';'.
 */
#if defined( PACKAGE_SCOREP )
    #if HAVE( SCOREP_DEBUG )
    #define SCOREP_DEBUG_ONLY( ... ) __VA_ARGS__
    #else
    #define SCOREP_DEBUG_ONLY( ... )
    #endif /* SCOREP_DEBUG */
#elif defined( PACKAGE_OTF2 )
    #if HAVE( SCOREP_DEBUG )
    #define OTF2_DEBUG_ONLY( ... ) __VA_ARGS__
    #else
    #define OTF2_DEBUG_ONLY( ... )
    #endif /* SCOREP_DEBUG */
#else
#error Unsupported package.
#endif


/**
 * Use this to print a deprecation message.
 */
#if defined( PACKAGE_SCOREP )
    #if HAVE( SCOREP_DEBUG )
    #define SCOREP_DEPRECATED( ... ) SCOREP_Debug_Printf( \
        SCOREP_DEBUG_DEPRECATED,                      \
        PACKAGE_NAME,                                 \
        PACKAGE_SRCDIR,                               \
        PACKAGE_BUILDDIR,                             \
        __FILE__,                                     \
        __LINE__,                                     \
        __func__,                                     \
        __VA_ARGS__ )
    #else
    #define SCOREP_DEPRECATED( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#elif defined( PACKAGE_OTF2 )
    #if HAVE( SCOREP_DEBUG )
    #define OTF2_DEPRECATED( ... ) OTF2_Debug_Printf(     \
        OTF2_DEBUG_DEPRECATED,                        \
        PACKAGE_NAME,                                 \
        PACKAGE_SRCDIR,                               \
        PACKAGE_BUILDDIR,                             \
        __FILE__,                                     \
        __LINE__,                                     \
        __func__,                                     \
        __VA_ARGS__ )
    #else
    #define OTF2_DEPRECATED( ... ) do { } while ( 0 )
    #endif /* SCOREP_DEBUG */
#else
#error Unsupported package.
#endif


/**
 * Function implementation called by @ref MANGLE_NAME( DEBUG_PRINTF ). It prints a debug message
 * in the given debug level. Furthermore, it provides the function name, file name and
 * line number.
 * @param bitMask    The debug level which must be enabled to print out the message.
 * @param function   A string containing the name of the function where the debug messages
 *                   was called.
 * @param file       The file name of the file which contains the source code where the
 *                   message was created.
 * @param line       The line number of the source code line where the debug message
 *                   was created.
 * @param msgFormatString A format string followed by the parameters defined in
 *                        the format string. The format string and the
 *                        parameters have the same syntax like in the POSIX
 *                        printf function.
 */
void
MANGLE_NAME( Debug_Printf )
(
    uint64_t bitMask,
    const char* package,
    const char* scrdir,
    const char* builddir,
    const char* file,
    uint64_t line,
    const char* function,
    const char* msgFormatString,
    ...
);

/**
 * The same as @a MANGLE_NAME( Debug_Printf ). Except it does not print the prefix and no
 * newline at the end.
 *
 * @param bitMask    The debug level which must be enabled to print out the
 *                   message.
 * @param msgFormatString A format string followed by the parameters defined in
 *                        the format string. The format string and the
 *                        parameters have thesame syntax like in the POSIX
 *                        printf function.
 */
void
MANGLE_NAME( Debug_RawPrintf )
(
    const uint64_t bitMask,
    const char*    msgFormatString,
    ...
);

/**
 * The same as @a MANGLE_NAME( Debug_Printf ). Except it prints only the prefix.
 *
 * @param bitMask    The debug level which must be enabled to print out the
 *                   message.
 */
void
MANGLE_NAME( Debug_Prefix )
(
    const uint64_t bitMask,
    const char*    package,
    const char*    scrdir,
    const char*    builddir,
    const char*    file,
    const uint64_t line,
    const char*    function
);

/**
 * This function implements the MANGLE_NAME( ASSERT ) macro. If the assertion fails an error
 * message is output and the program is aborted. Do not insert calls to this function
 * directly, but use the MANGLE_NAME( ASSERT ) macro instead.
 *  @param truthValue Contains the result of an evaluated logical expression. If it is
 *                    zero the assertion failed.
 *  @param file       The file name of the file which contains the source code where the
 *                    message was created.
 *  @param line       The line number of the source code line where the debug message
 *                    was created.
 *  @param function   A string containing the name of the function where the debug
 *                    message was called.
 */
void
MANGLE_NAME( Abort )
(
    const bool truthValue,
    const char*    package,
    const char*    scrdir,
    const char*    builddir,
    const char*    message,
    const char*    file,
    const uint64_t line,
    const char*    func
);


/**
 * @def MANGLE_NAME( ASSERT )
 * Definition of the scorep assertion macro. It evaluates an @a expression. If it is false,
 * an error message is output and the program is aborted. To use the assertion,
 * MANGLE_NAME( NO_DEBUG ) must not be defined.
 * @param expression A logical expression which should be verified. If it is zero the
 *                    assertion fails.
 */
#if defined( PACKAGE_SCOREP )

    #if !defined SCOREP_NO_DEBUG

    #define __SCOREP_ABORT_STRX( x ) #x
    #define __SCOREP_ABORT_STR( x ) __SCOREP_ABORT_STRX( x )

    #define SCOREP_ASSERT( expression ) SCOREP_Abort( !!( expression ), \
                                                      "Assertion '" __SCOREP_ABORT_STR( expression ) "' failed", \
                                                      PACKAGE_NAME,     \
                                                      PACKAGE_SRCDIR,   \
                                                      PACKAGE_BUILDDIR, \
                                                      __FILE__,         \
                                                      __LINE__,         \
                                                      __func__ )

    #define SCOREP_BUG_ON( expression, message ) SCOREP_Abort( !( expression ), \
                                                               "Bug: " message, \
                                                               PACKAGE_NAME, \
                                                               PACKAGE_SRCDIR, \
                                                               PACKAGE_BUILDDIR, \
                                                               __FILE__, \
                                                               __LINE__, \
                                                               __func__ )

    #else

    #define SCOREP_ASSERT( expression ) do { ( void )( expression ); } while ( 0 )
    #define SCOREP_BUG_ON( expression, message ) do { ( void )( expression ); } while ( 0 )

    #endif

    #define SCOREP_BUG( message )               \
    do {                                    \
        SCOREP_Abort( 0,                    \
                      "Bug: " message,      \
                      PACKAGE_NAME,         \
                      PACKAGE_SRCDIR,       \
                      PACKAGE_BUILDDIR,     \
                      __FILE__,             \
                      __LINE__,             \
                      __func__ );           \
        abort();                            \
    } while ( 0 )

/**
 * Emit a warning, but only on first occurrence
 */
    #define SCOREP_WARN_ONCE( message )         \
    do {                                                            \
        static int scorep_warn_once_##__LINE__; if ( !scorep_warn_once_##__LINE__ ) { scorep_warn_once_##__LINE__ = 1; SCOREP_Error_Handler(  PACKAGE_NAME, PACKAGE_SRCDIR, PACKAGE_BUILDDIR, __FILE__, __LINE__, __func__, SCOREP_WARNING, "%s.", message ); } \
    } while ( 0 )

/**
 * Inform the user about not yet implemented functions by printing the function name and the source file.
 */
    #define SCOREP_DEBUG_NOT_YET_IMPLEMENTED() SCOREP_WARN_ONCE( "Not yet implemented" )

#elif defined( PACKAGE_OTF2 )

    #if !defined OTF2_NO_DEBUG

    #define __OTF2_ABORT_STRX( x ) #x
    #define __OTF2_ABORT_STR( x ) __OTF2_ABORT_STRX( x )

    #define OTF2_ASSERT( expression ) OTF2_Abort( !!( expression ),     \
                                                  "Assertion '" __OTF2_ABORT_STR( expression ) "' failed", \
                                                  PACKAGE_NAME,         \
                                                  PACKAGE_SRCDIR,       \
                                                  PACKAGE_BUILDDIR,     \
                                                  __FILE__,             \
                                                  __LINE__,             \
                                                  __func__ )

    #define OTF2_BUG_ON( expression, message ) OTF2_Abort( !( expression ),  \
                                                           "Bug: " message,  \
                                                           PACKAGE_NAME,     \
                                                           PACKAGE_SRCDIR,   \
                                                           PACKAGE_BUILDDIR, \
                                                           __FILE__,    \
                                                           __LINE__,    \
                                                           __func__ )

    #else

    #define OTF2_ASSERT( expression ) do { ( void )( expression ); } while ( 0 )
    #define OTF2_BUG_ON( expression, message ) do { ( void )( expression ); } while ( 0 )

    #endif

    #define OTF2_BUG( message )                 \
    do {                                    \
        OTF2_Abort( 0,                      \
                    "Bug: " message,        \
                    PACKAGE_NAME,           \
                    PACKAGE_SRCDIR,         \
                    PACKAGE_BUILDDIR,       \
                    __FILE__,               \
                    __LINE__,               \
                    __func__ );             \
        abort();                            \
    } while ( 0 )

/**
 * Emit a warning, but only on first occurrence
 */
    #define OTF2_WARN_ONCE( message )                                   \
    do {                                                            \
        static int scorep_warn_once_##__LINE__; if ( !scorep_warn_once_##__LINE__ ) { scorep_warn_once_##__LINE__ = 1; OTF2_Error_Handler(  PACKAGE_NAME, PACKAGE_SRCDIR, PACKAGE_BUILDDIR, __FILE__, __LINE__, __func__, OTF2_WARNING, "%s.", message ); } \
    } while ( 0 )

/**
 * Inform the user about not yet implemented functions by printing the function name and the source file.
 */
    #define OTF2_DEBUG_NOT_YET_IMPLEMENTED() OTF2_WARN_ONCE( "Not yet implemented" )

#else
#error Unsupported package.
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/** @} */

#endif /* SCOREP_DEBUG_H */
