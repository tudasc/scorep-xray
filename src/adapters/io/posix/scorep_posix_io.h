/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2019,
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

#include <SCOREP_Definitions.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Mutex.h>

#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/select.h>

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

/** Artificial POSIX I/O handle representing all currently active I/O handles */
extern SCOREP_IoHandleHandle scorep_posix_io_sync_all_handle;

/** Artificial ISO C I/O handle representing all currently active I/O handles */
extern SCOREP_IoHandleHandle scorep_posix_io_flush_all_handle;

#if HAVE( POSIX_AIO_SUPPORT )

extern SCOREP_Hashtab* scorep_posix_io_aio_request_table;
extern SCOREP_Mutex    scorep_posix_io_aio_request_table_mutex;

#endif

/**
 * Translate the POSIX I/O mode of an open operation to its Score-P equivalent.
 *
 * @param mode      POSIX I/O mode of the open operation (e.g., O_RDONLY, O_WRONLY, O_RDWR).
 *
 * @return Score-P equivalent of the POSIX I/O mode.
 */
SCOREP_IoAccessMode
scorep_posix_io_get_scorep_io_access_mode( int mode );

/**
 * Translate the POSIX I/O flags of an I/O operation to their Score-P equivalents.
 *
 * @param[in]  flags          POSIX I/O flags of the I/O operation (e.g., O_APPEND).
 * @param[out] creationFlags  Score-P equivalents of the POSIX I/O flags set at file creation.
 * @param[out] statusFlags    Score-P equivalents of the POSIX I/O flags which might be changed during lifetime of the I/O handle.
 */
void
scorep_posix_io_get_scorep_io_flags( int                    flags,
                                     SCOREP_IoCreationFlag* creationFlags,
                                     SCOREP_IoStatusFlag*   statusFlags );

/**
 * Translate the POSIX I/O flags from an file descriptor to their Score-P equivalents.
 *
 * @param[in]  fd             File descriptor from which the flags should be taken.
 * @param[out] accessMode     Score-P equivalents of the POSIX I/O access mode.
 * @param[out] creationFlags  Score-P equivalents of the POSIX I/O flags set at file creation.
 * @param[out] statusFlags    Score-P equivalents of the POSIX I/O flags which might be changed during lifetime of the I/O handle.
 *
 * @return True if translation was success full (also indicates that @a fd is valid)
 */
bool
scorep_posix_io_get_scorep_io_flags_from_fd( int                    fd,
                                             SCOREP_IoAccessMode*   accessMode,
                                             SCOREP_IoCreationFlag* creationFlags,
                                             SCOREP_IoStatusFlag*   statusFlags );

/**
 * Translate the POSIX I/O seek option to its Score-P equivalent.
 *
 * @param whence        Option of a POSIX I/O seek operation.
 *
 * @return Score-P equivalent of the POSIX I/O mode.
 */
SCOREP_IoSeekOption
scorep_posix_io_get_scorep_io_seek_option( int whence );

/**
 * @internal
 *
 * Initialize internal POSIX I/O management.
 */
void
scorep_posix_io_init( void );

void
scorep_posix_io_fini( void );

/**
 * @internal
 *
 * Initialize internal ISO C I/O management.
 */
void
scorep_posix_io_isoc_init( void );

void
scorep_posix_io_isoc_fini( void );

#endif  /* SCOREP_POSIX_IO_H */
