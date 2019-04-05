/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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
 * @brief Registration of POSIX I/O function pointers
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include "scorep_posix_io_function_pointers.h"
#include <scorep/SCOREP_Libwrap.h>

#include <stdio.h>


#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    SCOREP_LIBWRAP_DEFINE_REAL_FUNC( ( return_type ), func, func_args );

#include "scorep_posix_io_function_list.inc"

/**
 * Register POSIX I/O functions and initialize data structures
 */
static void
posix_io_init_function_pointers( SCOREP_LibwrapHandle* handle )
{
#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    SCOREP_Libwrap_SharedPtrInit( handle, #func, \
                                  ( void** )( &SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) ) );

#include "scorep_posix_io_function_list.inc"
}


static const char* wrapped_lib_name[] =
{
    "libc.so",
#if HAVE( POSIX_AIO_SUPPORT )
    "librt.so"
#endif
};
static SCOREP_LibwrapHandle*    posix_io_libwrap_handle     = NULL;
static SCOREP_LibwrapAttributes posix_io_libwrap_attributes =
{
    SCOREP_LIBWRAP_VERSION,
    "posix",                                                      /* name of the library wrapper */
    "POSIX I/O",                                                  /* display name of the library wrapper */
    SCOREP_LIBWRAP_MODE_SHARED,                                   /* libwrap mode */
    posix_io_init_function_pointers,                              /* init function */
    sizeof( wrapped_lib_name ) / sizeof( wrapped_lib_name[ 0 ] ), /* number of wrapped libraries */
    wrapped_lib_name                                              /* name of wrapped library */
};


/**
 * Register POSIX I/O functions and initialize data structures
 */
void
scorep_posix_io_register_function_pointers( void )
{
    SCOREP_Libwrap_Create( &posix_io_libwrap_handle,
                           &posix_io_libwrap_attributes );
}

void
scorep_posix_io_early_init_function_pointers( void )
{
#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    SCOREP_Libwrap_EarlySharedPtrInit( #func, \
                                       ( void** )( &SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) ) );

#include "scorep_posix_io_function_list.inc"
}
