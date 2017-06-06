/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef UTILS_PORTABILITY_H
#define UTILS_PORTABILITY_H

/**
 * @file
 * @ingroup  UTILS_Exception_module
 * @brief    Platform portability abstractions for the common utils
 *
 * This header file provides preprocessor macros to abstract from platform- or
 * compiler-dependent code.
 */

/**
 * @def   UTILS_FUNCTION_NAME
 * @brief Portable macro to retrieve the current function name
 *
 * While C99 specifies the predefined identifier <tt>__func__</tt> to retrieve
 * the current function name, it is not part of the C++98 standard.  However,
 * many compilers support it also in C++98 mode or provide compiler-specific
 * alternatives.  This preprocessor macro addresses this issue by providing a
 * mapping to the corresponding predefined identifier.
 */
#if defined( __cplusplus ) && defined( __FUJITSU )
    #define UTILS_FUNCTION_NAME __FUNCTION__
#else
    #define UTILS_FUNCTION_NAME __func__
#endif

/**
 * @def   UTILS_DECLSPEC_NORETURN
 * @def   UTILS_ATTRIBUTE_NORETURN
 * @brief Portable macros to mark a function that it will not return to the caller.
 *
 */
#if defined( _WIN32 )
    #define UTILS_DECLSPEC_NORETURN __declspec( noreturn )
    #define UTILS_ATTRIBUTE_NORETURN /*  */
#else
    #define UTILS_DECLSPEC_NORETURN  /*  */
    #define UTILS_ATTRIBUTE_NORETURN __attribute__( ( noreturn ) )
#endif

#endif /* UTILS_PORTABILITY_H */
