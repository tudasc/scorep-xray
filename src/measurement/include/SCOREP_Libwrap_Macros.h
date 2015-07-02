/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @brief These macros represent the interface for library wrapping
 *        to the Score-P measurement core.
 *
 * @file
 */

#ifndef SCOREP_LIBWRAP_MACROS_H
#define SCOREP_LIBWRAP_MACROS_H

#include <stdlib.h>
#include <SCOREP_Libwrap.h>


/** @def SCOREP_LIBWRAP_NULL
 *  NULL handle within Score-P library wrapper
 */
#define SCOREP_LIBWRAP_NULL NULL

/**
 * @def SCOREP_LIBWRAP_FUNC_ENTER
 * Write event for entering wrapped function
 *
 */
#define SCOREP_LIBWRAP_FUNC_ENTER                                   \
    if ( scorep_libwrap_region != SCOREP_LIBWRAP_FILTERED_REGION )  \
    {                                                               \
        SCOREP_Libwrap_EnterRegion( scorep_libwrap_region );        \
    }

/**
 * @def SCOREP_LIBWRAP_FUNC_EXIT
 * Write event for leaving wrapped function
 *
 */
#define SCOREP_LIBWRAP_FUNC_EXIT                                    \
    if ( scorep_libwrap_region != SCOREP_LIBWRAP_FILTERED_REGION )  \
    {                                                               \
        SCOREP_Libwrap_ExitRegion( scorep_libwrap_region );         \
    }

/* ********************************************************************
* STATIC MODE
* ********************************************************************/
#ifdef SCOREP_LIBWRAP_STATIC

/**
 * @def SCOREP_LIBWRAP_MODE
 * Define static library wrapper mode
 */
#define SCOREP_LIBWRAP_MODE SCOREP_LIBWRAP_MODE_STATIC

/**
 * @def SCOREP_LIBWRAP_FUNC_NAME
 * Macro for parameter expansion
 *
 * @param func              Function name
 */
#define SCOREP_LIBWRAP_FUNC_NAME( func ) __wrap_##func

/**
 * @def SCOREP_LIBWRAP_INTERNAL_FUNC_CALL
 * Call real function symbol from Score-P internal adapters
 *
 * @param funcptr          Function pointer
 * @param func             Function name
 * @param args             Function arguments
 */
#define SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( funcptr, func, args ) \
    SCOREP_LIBWRAP_FUNC_CALL( NULL, func, args )

/**
 * @def SCOREP_LIBWRAP_FUNC_CALL
 * Call real function symbol
 *
 * @param handle           Library wrapper handle
 * @param func             Function name
 * @param args             Function arguments
 */
#define SCOREP_LIBWRAP_FUNC_CALL( handle, func, args ) \
    __real_##func args

/**
 * @def SCOREP_LIBWRAP_FUNC_INIT
 * Prepare wrapping of requested function
 *
 * @param handle           Library wrapper handle
 * @param attributes       Library wrapper handle attributes
 * @param rettype          Return type
 * @param func             Function name
 * @param argtypes         Argument types
 * @param file             Source code location (file as `const char*`)
 * @param line             Source code location (line as `int`)
 */
#define SCOREP_LIBWRAP_FUNC_INIT( handle, attributes, rettype, func, argtypes, file, line ) \
    SCOREP_LIBWRAP_FUNC_INIT_STATIC( handle, attributes, func, file, line )

/**
 * @def SCOREP_LIBWRAP_FUNC_INIT_STATIC
 * Initialize library wrapping of requested function and
 * create a static region handle called scorep_libwrap_region.
 *
 * @param handle           Library wrapper handle
 * @param attributes       Library wrapper handle attributes
 * @param func             Function name
 * @param file             Source code location (file as `const char*`)
 * @param line             Source code location (line as `int`)
 */
#define SCOREP_LIBWRAP_FUNC_INIT_STATIC( handle, attributes, func, file, line )             \
    static SCOREP_RegionHandle scorep_libwrap_region = SCOREP_INVALID_REGION;               \
    if ( handle == SCOREP_LIBWRAP_NULL )                                                    \
    {                                                                                       \
        SCOREP_Libwrap_Create( &handle, &attributes );                                      \
    }                                                                                       \
    if ( scorep_libwrap_region == SCOREP_INVALID_REGION )                                   \
    {                                                                                       \
        SCOREP_Libwrap_DefineRegion( handle, &scorep_libwrap_region, #func, file, line );   \
    }

/* *********************************************************************
 * end of static mode
 * ********************************************************************/


/* *********************************************************************
 *  SHARED MODE
 * ********************************************************************/
#elif SCOREP_LIBWRAP_SHARED

/** @def SCOREP_LIBWRAP_MODE
 *  Define shared library wrapper mode
 */
#define SCOREP_LIBWRAP_MODE SCOREP_LIBWRAP_MODE_SHARED

/**
 * @def SCOREP_LIBWRAP_FUNC_NAME
 * Macro for parameter expansion
 *
 * @param _func             Function name
 */
#define SCOREP_LIBWRAP_FUNC_NAME( func ) func

/**
 * @def SCOREP_LIBWRAP_FUNC_PTR
 * Define function pointer
 */
#define SCOREP_LIBWRAP_FUNC_PTR funcptr

/**
 * @def SCOREP_LIBWRAP_INTERNAL_FUNC_CALL
 * Call real function symbol from Score-P internal adapters
 *
 * @param funcptr           Function pointer
 * @param func              Function pointer
 * @param args              Function arguments
 */
#define SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( funcptr, func, args ) \
    ( *funcptr )args

/**
 * @def SCOREP_LIBWRAP_FUNC_CALL
 * Call real function symbol
 *
 * @param handle            Library wrapper handle
 * @param func              Function name
 * @param args              Function arguments
 */
#define SCOREP_LIBWRAP_FUNC_CALL( handle, func, args ) \
    ( *SCOREP_LIBWRAP_FUNC_PTR )args

/**
 * @def SCOREP_LIBWRAP_FUNC_INIT
 * Prepare wrapping of requested function
 *
 * @param handle            Library wrapper handle
 * @param attributes        Library wrapper handle attributes
 * @param rettype           Return type
 * @param func              Function name
 * @param argtypes          Argument types
 * @param file              Source code location (file as `const char*`)
 * @param line              Source code location (line as `int`)
 */
#define SCOREP_LIBWRAP_FUNC_INIT( handle, attributes, rettype, func, argtypes, file, line ) \
    SCOREP_LIBWRAP_FUNC_INIT_SHARED( handle, attributes, rettype, func, argtypes, file, line )

/**
 * @def SCOREP_LIBWRAP_FUNC_INIT_SHARED
 * Initialize library wrapping of requested function and
 * create a static region handle called scorep_libwrap_region.
 *
 * @param handle            Library wrapper handle
 * @param attributes        Library wrapper handle attributes
 * @param rettype           Return type
 * @param func              Function name
 * @param argtypes          Argument types
 * @param file              Source code location (file as `const char*`)
 * @param line              Source code location (line as `int`)
 */
#define SCOREP_LIBWRAP_FUNC_INIT_SHARED( handle, attributes, rettype, func, argtypes, file, line )  \
    static rettype( *SCOREP_LIBWRAP_FUNC_PTR ) argtypes = SCOREP_LIBWRAP_NULL;                      \
    static SCOREP_RegionHandle scorep_libwrap_region = SCOREP_INVALID_REGION;                       \
    if ( handle == SCOREP_LIBWRAP_NULL )                                                            \
    {                                                                                               \
        SCOREP_Libwrap_Create( &handle, &attributes );                                              \
    }                                                                                               \
    if ( SCOREP_LIBWRAP_FUNC_PTR == SCOREP_LIBWRAP_NULL )                                           \
    {                                                                                               \
        SCOREP_Libwrap_SharedPtrInit( handle, #func, ( void** )( &SCOREP_LIBWRAP_FUNC_PTR ) );      \
    }                                                                                               \
    if ( scorep_libwrap_region == SCOREP_INVALID_REGION )                                           \
    {                                                                                               \
        SCOREP_Libwrap_DefineRegion( handle, &scorep_libwrap_region, #func, file, line );           \
    }

/* *********************************************************************
 * END OF SHARED MODE
 * ********************************************************************/


/* *********************************************************************
 * WEAK SYMBOL MODE
 * ********************************************************************/
#elif SCOREP_LIBWRAP_WEAK

/** @def SCOREP_LIBWRAP_MODE
 *  Define weak symbol library wrapper mode
 */
#define SCOREP_LIBWRAP_MODE SCOREP_LIBWRAP_MODE_WEAK

/**
 * @def SCOREP_LIBWRAP_FUNC_NAME
 * Macro for parameter expansion
 *
 * @param func              Function name
 */
#define SCOREP_LIBWRAP_FUNC_NAME( func ) func

#ifndef SCOREP_LIBWRAP_STRONG_PREFIX
/**
 * @def SCOREP_LIBWRAP_STRONG_PREFIX
 * Set default prefix of strong symbols if not already defined
 */
#define SCOREP_LIBWRAP_STRONG_PREFIX p
#endif

/**
 * @def _SCOREP_CONCATENATE_PREFIX
 * Concatenate strong symbol prefix to function
 *
 * @param prefix            Strong symbol prefix
 * @param func              Function name
 * @param args              Function arguments
 */
#define _SCOREP_CONCATENATE_PREFIX( prefix, func, args ) prefix##func args

/**
 * @def _SCOREP_PREPEND_PREFIX
 * Prepare concatenation of strong symbol prefix
 *
 * @param prefix            Strong symbol prefix
 * @param func              Function name
 * @param args              Function arguments
 */
#define _SCOREP_PREPEND_PREFIX( prefix, func, args ) \
    _SCOREP_CONCATENATE_PREFIX( prefix, func, args )

/**
 * @def SCOREP_LIBWRAP_INTERNAL_FUNC_CALL
 * Call real function symbol from Score-P internal adapters
 *
 * @param funcptr           Function pointer
 * @param func              Function name
 * @param args              Function arguments
 */
#define SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( funcptr, func, args ) \
    SCOREP_LIBWRAP_FUNC_CALL( NULL, func, args )

/**
 * @def SCOREP_LIBWRAP_FUNC_CALL
 * Call real function symbol
 *
 * @param handle            Library wrapper handle
 * @param func              Function name
 * @param args              Function arguments
 */
#define SCOREP_LIBWRAP_FUNC_CALL( handle, func, args ) \
    _SCOREP_PREPEND_PREFIX( SCOREP_LIBWRAP_STRONG_PREFIX, func, args )

/**
 * @def SCOREP_LIBWRAP_FUNC_INIT
 * Prepare wrapping of requested function
 *
 * @param handle            Library wrapper handle
 * @param attributes        Library wrapper handle attributes
 * @param rettype           Return type
 * @param func              Function name
 * @param argtypes          Argument types
 * @param file              Source code location (file as `const char*`)
 * @param line              Source code location (line as `int`)
 */
#define SCOREP_LIBWRAP_FUNC_INIT( handle, attributes, rettype, func, argtypes, file, line ) \
    SCOREP_LIBWRAP_FUNC_INIT_WEAK( handle, attributes, func, file, line )

/**
 * @def SCOREP_LIBWRAP_FUNC_INIT_WEAK
 * Initialize library wrapping of requested function and
 * create a static region handle called scorep_libwrap_region.
 *
 * @param handle            Library wrapper handle
 * @param attributes        Library wrapper handle attributes
 * @param func              Function name
 * @param file              Source code location (file as `const char*`)
 * @param line              Source code location (line as `int`)
 */
#define SCOREP_LIBWRAP_FUNC_INIT_WEAK( handle, attributes, func, file, line )               \
    static SCOREP_RegionHandle scorep_libwrap_region = SCOREP_INVALID_REGION;               \
    if ( handle == SCOREP_LIBWRAP_NULL )                                                    \
    {                                                                                       \
        SCOREP_Libwrap_Create( &handle, &attributes );                                      \
    }                                                                                       \
    if ( scorep_libwrap_region == SCOREP_INVALID_REGION )                                   \
    {                                                                                       \
        SCOREP_Libwrap_DefineRegion( handle, &scorep_libwrap_region, #func, file, line );   \
    }

/* *********************************************************************
 * END OF WEAK MODE
 * ********************************************************************/

#else

#error no library mode specified

#endif /* link modes */

#endif /* SCOREP_LIBWRAP_MACROS_H */
