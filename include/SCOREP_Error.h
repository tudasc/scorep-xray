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

#ifndef SCOREP_ERROR_H
#define SCOREP_ERROR_H

/**
 * @file            MANGLE_NAME( Error.h )
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status          REVIEW
 * @ingroup         MANGLE_NAME( Exception_module )
 *
 * @brief           Module for error handling in SCOREP.
 *
 * @author          Dominic Eschweiler <d.eschweiler@fz-juelich.de>
 */

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined( PACKAGE_SCOREP )
#include <SCOREP_Error_Codes.h>
#elif defined( PACKAGE_OTF2 )
#include <otf2/OTF2_Error_Codes.h>
#else
#error Unsupported package.
#endif

/**
 * @def SCOREP_ERROR
 * This is a prep function, which delegates error information to the error
 * callback.
 * @ingroup MANGLE_NAME( Exception_module )
 */
#if defined( PACKAGE_SCOREP )
#define SCOREP_ERROR( errCode, ... ) SCOREP_Error_Handler( \
        PACKAGE_NAME, \
        PACKAGE_SRCDIR, \
        PACKAGE_BUILDDIR, \
        __FILE__, \
        __LINE__, \
        __func__, \
        errCode, \
        __VA_ARGS__ )
#elif defined( PACKAGE_OTF2 )
#define OTF2_ERROR( errCode, ... ) OTF2_Error_Handler( \
        PACKAGE_NAME, \
        PACKAGE_SRCDIR, \
        PACKAGE_BUILDDIR, \
        __FILE__, \
        __LINE__, \
        __func__, \
        errCode, \
        __VA_ARGS__ )
#else
#error Unsupported package.
#endif

/**
 * Delegation error handler function, which is used by the prep SCOREP_ERROR to
 * to avert that external programmers use the function pointer directly.
 *
 * @param function        : Name of the function where the error appeared
 * @param file            : Name of the source-code file where the error appeared
 * @param line            : Line number in the source-code where the error appeared
 * @param errorCode       : Error Code
 * @param msgFormatString : Format string like it is used at the printf family.
 *
 * @returns Should return the ErrorCode
 * @ingroup MANGLE_NAME( Exception_module )
 */
MANGLE_NAME( Error_Code )
MANGLE_NAME( Error_Handler )
(
    const char*       package,
    const char*       srcdir,
    const char*       builddir,
    const char*       file,
    uint64_t line,
    const char*       function,
    MANGLE_NAME( Error_Code ) errorCode,
    const char*       msgFormatString,
    ...
);

/**
 * @def MANGLE_NAME( ERROR_POSIX )
 * This is a prep function, which is able to handle external POSIX
 * error codes with the SCOREP error handling system.
 *
 * @param ... The first argument needs to be a string constant.
 *
 * @ingroup MANGLE_NAME( Exception_module )
 */
#if defined( PACKAGE_SCOREP )
#define SCOREP_ERROR_POSIX( ... ) SCOREP_ERROR( \
        SCOREP_Error_PosixToScorep( errno ), \
        "POSIX: " __VA_ARGS__ )
#elif defined( PACKAGE_OTF2 )
#define OTF2_ERROR_POSIX( ... ) OTF2_ERROR( \
        OTF2_Error_PosixToScorep( errno ), \
        "POSIX: " __VA_ARGS__ )
#else
#error Unsupported package.
#endif

/**
 * Translates a POSIX error code into a SCOREP error code.
 *
 * @param posixErrorCode : Error Code
 *
 * @returns Returns a SCOREP error code (see MANGLE_NAME( Error_Codes.h ))
 * @ingroup MANGLE_NAME( Exception_module )
 */
MANGLE_NAME( Error_Code )
MANGLE_NAME( Error_PosixToScorep )
(
    const int posixErrorCode
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCOREP_ERROR_H */
