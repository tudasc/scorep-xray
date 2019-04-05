/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    POSIX_IO_Wrapper
 *
 * @brief Registration of POSIX I/O functions
 * For all POSIX I/O functions a region is registered at initialization
 * time of the IO recording adapter.
 */

#ifndef SCOREP_POSIX_IO_REGIONS_H
#define SCOREP_POSIX_IO_REGIONS_H

#include "scorep_posix_io.h"

#include <SCOREP_Definitions.h>


#ifdef SCOREP_LIBWRAP_STATIC

/*
 * In static mode:
 * - Declaration of Score-P region handles for wrapped POSIX I/O functions
 * - Function declaration of wrapped POSIX I/O functions
 */
#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    extern SCOREP_RegionHandle scorep_posix_io_region_ ## func; \
    return_type func func_args;

#elif SCOREP_LIBWRAP_SHARED

/*
 * In shared mode:
 * - Declaration of Score-P region handles for wrapped POSIX I/O functions
 */
#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    extern SCOREP_RegionHandle scorep_posix_io_region_ ## func;

#else

#error Unsupported POSIX I/O wrapping mode

#endif /* link modes */


#include "scorep_posix_io_function_list.inc"

/**
 * Register POSIX I/O functions and initialize data structures
 */
void
scorep_posix_io_register_regions( void );

#endif /* SCOREP_POSIX_IO_REGIONS_H */
