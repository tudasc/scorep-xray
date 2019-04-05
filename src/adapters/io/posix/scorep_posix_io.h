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
 *  @file
 *
 *  @brief Definitions for the Score-P POSIX I/O wrapper.
 */

#ifndef SCOREP_POSIX_IO_H
#define SCOREP_POSIX_IO_H


#include <SCOREP_Hashtab.h>
#include <SCOREP_Mutex.h>

// @todo add POSIX I/O includes
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>

#if HAVE( POSIX_AIO_SUPPORT )
#include <aio.h>
#endif

/*
 * We need a signed integer of 8 bytes for the 64bit I/O wrapper, but we can not
 * use #define _FILE_OFFSET_BITS 64, as this would remove 'off_t' from the scope.
 * As we know the expected size of the type, we just define it for ourselves.
 */
typedef int64_t scorep_off64_t;

#ifdef SCOREP_LIBWRAP_SHARED

#define SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) \
    scorep_posix_io_funcptr_ ## func

#endif

#include <scorep/SCOREP_Libwrap_Macros.h>

#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    SCOREP_LIBWRAP_DECLARE_REAL_FUNC( ( return_type ), func, func_args );

#include "scorep_posix_io_function_list.inc"

/**
 * @internal
 *
 * Initialize internal POSIX management.
 */
void
scorep_posix_io_init( void );

void
scorep_posix_io_fini( void );

/**
 * @internal
 *
 * Initialize internal ISO C management.
 */
void
scorep_posix_io_isoc_init( void );

void
scorep_posix_io_isoc_fini( void );

#endif  /* SCOREP_POSIX_IO_H */
