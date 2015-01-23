/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @brief Definitions of library wrapping functions
 *        which expands from macros
 *
 * @file
 */

#ifndef SCOREP_LIBWRAP_H
#define SCOREP_LIBWRAP_H

#include <stdint.h>
#include <scorep/SCOREP_PublicTypes.h>


/** @def SCOREP_LIBWRAP_FILTERED_REGION
 *  Symbolic constant representing a filtered region
 */
#define SCOREP_LIBWRAP_FILTERED_REGION ( ( SCOREP_RegionHandle ) - 1 )

/** @brief Keeps all necessary information of the library wrapping object. */
typedef struct SCOREP_LibwrapHandle SCOREP_LibwrapHandle;

/** Determine which mode of library type is used */
typedef enum SCOREP_LibwrapMode
{
    SCOREP_LIBWRAP_MODE_SHARED,  /**< wrapping dynamically linked library */
    SCOREP_LIBWRAP_MODE_STATIC,  /**< wrapping statically linked library */
    SCOREP_LIBWRAP_MODE_WEAK,    /**< wrapping a library that provides weak symbols */
} SCOREP_LibwrapMode;

/** Data structure for library wrapper attributes */
typedef struct SCOREP_LibwrapAttributes SCOREP_LibwrapAttributes;
struct SCOREP_LibwrapAttributes
{
    SCOREP_LibwrapMode mode;
    int                number_of_shared_libs;
    const char**       shared_libs;
};


/* function definitions */

/**
 * This function calls the internal Score-P function to create a region
 * handle and will return a valid region handle for the function.
 *
 * @param handle            Score-P library wrapper object
 * @param region            Score-P region handle
 * @param func              Region name
 * @param file              Source file name
 * @param line              Line number in source file
 */
extern void
SCOREP_Libwrap_DefineRegion( SCOREP_LibwrapHandle* handle,
                             SCOREP_RegionHandle*  region,
                             const char*           func,
                             const char*           file,
                             int                   line );

/**
 * This function opens the shared libraries and fill the handle array of
 * the wrapper object with the associated library handles. In all modes
 * it will build the wrapper object and fill the attributes structure.
 *
 * @param handle            Score-P library wrapper object
 * @param attributes        Attributes of the Score-P library wrapper object
 */
extern void
SCOREP_Libwrap_Create( SCOREP_LibwrapHandle**    handle,
                       SCOREP_LibwrapAttributes* attributes );

/**
 * This function only call if you want to wrap a shared library. It
 * extracts the function pointers out the library handles and returns
 * a function pointer to the actual symbol.
 *
 * @param handle            Score-P library wrapper object
 * @param func              Name of wrapped function
 * @param funcPtr           Pointer to the actual symbol
 */
extern void
SCOREP_Libwrap_SharedPtrInit( SCOREP_LibwrapHandle* handle,
                              const char*           func,
                              void**                funcPtr );

/**
 * Wrapper to write an event for entering wrapped function.
 *
 * @param region            Handle of entered region
 */
extern void
SCOREP_Libwrap_EnterRegion( SCOREP_RegionHandle region );

/**
 * Wrapper to write an event for leaving wrapped function.
 *
 * @param region            Handle of entered region
 */
extern void
SCOREP_Libwrap_ExitRegion( SCOREP_RegionHandle region );

#endif /* SCOREP_LIBWRAP_H */
