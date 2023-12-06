/*
 * This file is part of the Score-P software ecosystem (http://www.score-p.org)
 *
 * Copyright (c) 2016, 2023,
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
 * While C99 and C++11 specify the predefined identifier <tt>__func__</tt> to
 * retrieve the current function name, it is not part of the C++98 standard.
 * However, many compilers support it also in C++98 mode or provide
 * compiler-specific alternatives.  This preprocessor macro addresses this
 * issue by providing a mapping to the corresponding predefined identifier.
 */
#if defined( __cplusplus ) && ( __cplusplus < 201103L ) && defined( __FUJITSU )
    #define UTILS_FUNCTION_NAME __FUNCTION__
#else
    #define UTILS_FUNCTION_NAME __func__
#endif

/**
 * @def   UTILS_PREDECL_ATTR_NORETURN
 * @def   UTILS_POSTDECL_ATTR_NORETURN
 * @brief Portable macros to mark a function that it will not return to the caller.
 *
 */
#if defined( __cplusplus ) && ( __cplusplus >= 201103L )
/* *INDENT-OFF*   Prevent uncrustify from adding spaces between brackets */
    #define UTILS_PREDECL_ATTR_NORETURN  [[noreturn]]
    #define UTILS_POSTDECL_ATTR_NORETURN /*  */
/* *INDENT-ON* */
#elif defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 201112L )
    #define UTILS_PREDECL_ATTR_NORETURN  _Noreturn
    #define UTILS_POSTDECL_ATTR_NORETURN /*  */
#else
    #if defined( _WIN32 )
        #define UTILS_PREDECL_ATTR_NORETURN  __declspec( noreturn )
        #define UTILS_POSTDECL_ATTR_NORETURN /*  */
    #else
        #define UTILS_PREDECL_ATTR_NORETURN  /*  */
        #define UTILS_POSTDECL_ATTR_NORETURN __attribute__( ( noreturn ) )
    #endif
#endif

#endif /* UTILS_PORTABILITY_H */
