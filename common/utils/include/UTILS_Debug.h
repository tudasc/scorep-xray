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
 * Copyright (c) 2009-2012, 2016, 2021-2022,
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

#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

/**
 * @file
 * @ingroup         UTILS_Exception_module
 *
 * @brief           Module for debug output handling in UTILS
 *
 */

#include <stdint.h>
#include <stdio.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <UTILS_Portability.h>

UTILS_BEGIN_C_DECLS

/**
 * @defgroup UTILS_Exception_module SCOREP Debug and Error Handling
 *
 * This module provides an interface for error handling and debugging output.
 *
 * For invoke error handling, two macros are provided: @ref UTILS_ERROR for generic
 * UTILS errors, and UTILS_ERROR_POSIX for handling errors occurred in posix calls.
 * To handle an error, own error handlers can be registered using
 * @ref UTILS_Error_RegisterCallback. The default error handler prints the error
 * message to the standard error output.
 *
 * For debug output the macro @ref UTILS_DEBUG_PRINTF is provided. For each debug
 * message, a debug level must be provided. The print out of the messages can be filtered
 * depending on the debug level.  By default, debug messages are printed to @c stdout;
 * an alternative output stream can be set using @ref UTILS_Debug_SetLogStream.
 * The second debug macro is @ref UTILS_ASSERT which ensures that a condition holds.
 *
 * The debug macros resolve to nothing if HAVE_UTILS_DEBUG is not defined or zero.
 * Then no overhead and no debug output is created.
 *
 * @{
 */

/**
 * List of debug levels. Every debug message printed with @ref
 * UTILS_DEBUG_PRINTF must provide a debug level. Based on the debug
 * level, the debug output can be filtered for the debugged parts. The debug level
 * is a bitstring, where every level is represented by one bit. It is
 * possible to combine several levels.
 */
enum
{
    /*
     * add new debug modules to the UTILS_DEBUG_MODULES list in
     * src/config-custom.h
     */
    #define UTILS_DEFINE_DEBUG_MODULE( name, bit ) \
    PACKAGE_MANGLE_NAME( DEBUG_ ## name ) = ( UINT64_C( 1 ) << ( bit ) )
    UTILS_DEBUG_MODULES
    #undef UTILS_DEFINE_DEBUG_MODULE
};

/* internal flags */
#define UTILS_DEBUG_FUNCTION_ENTRY ( UINT64_C( 1 ) << 62 )
#define UTILS_DEBUG_FUNCTION_EXIT  ( UINT64_C( 1 ) << 63 )

#define HAVE_UTILS_DEBUG UTILS_JOIN_SYMS( HAVE_, PACKAGE_MANGLE_NAME( DEBUG ) )

/**
 * @def UTILS_DEBUG_PRINTF
 * Following prep is the UTILS debug messaging function. It could be used like
 * printf, but must get passed a debug level as first parameter. In difference
 * to other debug message systems, the debug level is a bit mask where every
 * level could be switched of and on individually. To set the correct debug level
 * please use the enum UTILS_Debug_Levels.
 *
 * To set the current debug level, the source must not be recompiled. The current
 * debug level bitmask could be set through adding the bit-values of the debug
 * level which should be printed and assigning the result to the shell variable
 * ${PACKAGE}_DEBUG. For example ${PACKAGE}_DEBUG=7 for debug level 1 and 2 and 4.
 * @param debugLevel The debugLevel which must be enabled to print out the message.
 * @param ...        A format string followed by the parameters defined in the format
 *                   string. The format string and the parameters have the same syntax
 *                   like in the POSIX printf function.
 */

#if HAVE( UTILS_DEBUG )

/* *INDENT-OFF* */

#define UTILS_DEBUG_PRINTF( debugLevel, ... ) \
    do \
    { \
        if ( UTILS_Debug_IsEnabled( debugLevel ) ) \
        { \
            UTILS_Debug_Printf( \
                debugLevel, \
                AFS_PACKAGE_SRCDIR, \
                __FILE__, \
                __LINE__, \
                UTILS_FUNCTION_NAME, \
                __VA_ARGS__ ); \
        } \
    } \
    while ( 0 )

#define HAVE_DEBUG_MODULE_NAME_( sym ) defined( sym ## _DEBUG_MODULE_NAME )
#define HAVE_DEBUG_MODULE_NAME( sym )  HAVE_DEBUG_MODULE_NAME_( sym )

#if HAVE_DEBUG_MODULE_NAME( AFS_PACKAGE_NAME )

#define UTILS_DEBUG( ... ) \
    do \
    { \
        if ( UTILS_Debug_IsEnabled( UTILS_JOIN_SYMS( AFS_PACKAGE_NAME, \
                                        UTILS_JOIN_SYMS( _DEBUG_, \
                                            PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) ) ) ) ) \
        { \
            UTILS_Debug_Printf( \
                0, \
                AFS_PACKAGE_SRCDIR, \
                __FILE__, \
                __LINE__, \
                UTILS_FUNCTION_NAME, \
                "" __VA_ARGS__ ); \
        } \
    } \
    while ( 0 )

#define UTILS_DEBUG_ENTRY( ... ) \
    do \
    { \
        if ( UTILS_Debug_IsEnabled( UTILS_JOIN_SYMS( AFS_PACKAGE_NAME, \
                                        UTILS_JOIN_SYMS( _DEBUG_, \
                                            PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) ) ) ) ) \
        { \
            UTILS_Debug_Printf( \
                UTILS_DEBUG_FUNCTION_ENTRY, \
                AFS_PACKAGE_SRCDIR, \
                __FILE__, \
                __LINE__, \
                UTILS_FUNCTION_NAME, \
                "" __VA_ARGS__ ); \
        } \
    } \
    while ( 0 )

#define UTILS_DEBUG_EXIT( ... ) \
    do \
    { \
        if ( UTILS_Debug_IsEnabled( UTILS_JOIN_SYMS( AFS_PACKAGE_NAME, \
                                        UTILS_JOIN_SYMS( _DEBUG_, \
                                            PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) ) ) ) ) \
        { \
            UTILS_Debug_Printf( \
                UTILS_DEBUG_FUNCTION_EXIT, \
                AFS_PACKAGE_SRCDIR, \
                __FILE__, \
                __LINE__, \
                UTILS_FUNCTION_NAME, \
                "" __VA_ARGS__ ); \
        } \
    } \
    while ( 0 )

#else

#define UTILS_DEBUG( ... ) \
    PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) = "You need to define a debug module name before including <UTILS_Debug.h>."

#define UTILS_DEBUG_ENTRY( ... ) \
    PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) = "You need to define a debug module name before including <UTILS_Debug.h>."

#define UTILS_DEBUG_EXIT( ... ) \
    PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) = "You need to define a debug module name before including <UTILS_Debug.h>."

#endif

/* *INDENT-ON* */

#else

#define UTILS_DEBUG_PRINTF( ... ) do { } while ( 0 )
#define UTILS_DEBUG( ... )        do { } while ( 0 )
#define UTILS_DEBUG_ENTRY( ... )  do { } while ( 0 )
#define UTILS_DEBUG_EXIT( ... )   do { } while ( 0 )

#endif /* HAVE_UTILS_DEBUG */


/**
 * Use this to hide code, especially variables, which are only accessed in
 * debug mode.
 *
 * @note You should not use a ';' after this statement. But ... needs to be a
 *       valid C statement including ';'.
 */
#if HAVE( UTILS_DEBUG )

#define UTILS_DEBUG_ONLY( ... ) __VA_ARGS__

#else

#define UTILS_DEBUG_ONLY( ... )

#endif /* HAVE_UTILS_DEBUG */


/**
 * Set the output stream for debug messages to @p stream.  Client code is
 * responsible for properly opening/closing @p stream.  Passing @c NULL
 * resets the stream to @c stdout (the default).
 *
 * @param stream Valid output stream
 */
#define UTILS_Debug_SetLogStream PACKAGE_MANGLE_NAME( UTILS_Debug_SetLogStream )
void
UTILS_Debug_SetLogStream( FILE* stream );

/**
 * Checks whether the given debug level is enabled.
 *
 * @param bitMask The debug level to be tested.
 * @return Returns @c true if enabled, @c false otherwise.
 */
#define UTILS_Debug_IsEnabled PACKAGE_MANGLE_NAME( UTILS_Debug_IsEnabled )
static inline bool
UTILS_Debug_IsEnabled( uint64_t bitMask )
{
    /* Internal variables set by `utils_debug_init` -- read only! */
    #define utils_debug_initialized PACKAGE_MANGLE_name( utils_debug_initialized )
    #define utils_debug_level       PACKAGE_MANGLE_name( utils_debug_level )
    extern bool     utils_debug_initialized;
    extern uint64_t utils_debug_level;

    if ( !utils_debug_initialized )
    {
        #define utils_debug_init PACKAGE_MANGLE_name( utils_debug_init )
        extern void utils_debug_init( void );

        utils_debug_init();
    }

    bitMask &= ~( UTILS_DEBUG_FUNCTION_ENTRY | UTILS_DEBUG_FUNCTION_EXIT );

    return ( utils_debug_level & bitMask ) == bitMask;
}

/**
 * Function implementation called by @ref UTILS_DEBUG_PRINTF. It prints a debug message
 * in the given debug level. Furthermore, it provides the function name, file name and
 * line number.
 * @param kind       The "kind" part of the debug level (zero, UTILS_DEBUG_FUNCTION_ENTRY,
 *                   or UTILS_DEBUG_FUNCTION_EXIT; all other bits are ignored).
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
#define UTILS_Debug_Printf PACKAGE_MANGLE_NAME( UTILS_Debug_Printf )
void
UTILS_Debug_Printf( uint64_t    kind,
                    const char* srcdir,
                    const char* file,
                    uint64_t    line,
                    const char* function,
                    const char* msgFormatString,
                    ... );

UTILS_END_C_DECLS

/** @} */

#if defined( __cplusplus )
    #include <UTILS_DebugStream.hpp>
#endif

#endif /* UTILS_DEBUG_H */
