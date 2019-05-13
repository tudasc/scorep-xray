/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    POSIX_IO_Wrapper
 *
 * @brief MGMT for the ISO C I/O adapter
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include "scorep_posix_io.h"

#include <unistd.h>

#include <SCOREP_IoManagement.h>

#define SCOREP_DEBUG_MODULE_NAME IO
#include <UTILS_Debug.h>

#include "scorep_posix_io_regions.h"
#include "scorep_posix_io_function_pointers.h"

SCOREP_IoHandleHandle scorep_posix_io_flush_all_handle = SCOREP_INVALID_IO_HANDLE;

/* *******************************************************************
 * Internal management routine
 * ******************************************************************/
/**
 * Create definition handles for default stdin/stdout/stderr streams.
 */
void
scorep_posix_io_isoc_init( void )
{
    SCOREP_IoMgmt_RegisterParadigm( SCOREP_IO_PARADIGM_ISOC,
                                    SCOREP_IO_PARADIGM_CLASS_SERIAL,
                                    "ISO C I/O",
                                    SCOREP_IO_PARADIGM_FLAG_NONE,
                                    sizeof( FILE* ),
                                    SCOREP_INVALID_IO_PARADIGM_PROPERTY );

    SCOREP_IoMgmt_CreatePreCreatedHandle( SCOREP_IO_PARADIGM_ISOC,
                                          SCOREP_INVALID_IO_FILE,
                                          SCOREP_IO_HANDLE_FLAG_PRE_CREATED,
                                          SCOREP_IO_ACCESS_MODE_READ_ONLY,
                                          SCOREP_IO_STATUS_FLAG_NONE,
                                          SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                          1 /* unify all I/O handles into one */,
                                          "stdin",
                                          &stdin );

    SCOREP_IoMgmt_CreatePreCreatedHandle( SCOREP_IO_PARADIGM_ISOC,
                                          SCOREP_INVALID_IO_FILE,
                                          SCOREP_IO_HANDLE_FLAG_PRE_CREATED,
                                          SCOREP_IO_ACCESS_MODE_WRITE_ONLY,
                                          SCOREP_IO_STATUS_FLAG_NONE,
                                          SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                          1 /* unify all I/O handles into one */,
                                          "stdout",
                                          &stdout );

    SCOREP_IoMgmt_CreatePreCreatedHandle( SCOREP_IO_PARADIGM_ISOC,
                                          SCOREP_INVALID_IO_FILE,
                                          SCOREP_IO_HANDLE_FLAG_PRE_CREATED,
                                          SCOREP_IO_ACCESS_MODE_WRITE_ONLY,
                                          SCOREP_IO_STATUS_FLAG_NONE,
                                          SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                          1 /* unify all I/O handles into one */,
                                          "stderr",
                                          &stderr );

    /* Define proxy handles that aren't associated to a physical file and therefore don't add them to the hash tables */
    scorep_posix_io_flush_all_handle =
        SCOREP_Definitions_NewIoHandle( "all open output streams",
                                        SCOREP_INVALID_IO_FILE,
                                        SCOREP_IO_PARADIGM_ISOC,
                                        SCOREP_IO_HANDLE_FLAG_PRE_CREATED | SCOREP_IO_HANDLE_FLAG_ALL_PROXY,
                                        SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                        SCOREP_INVALID_IO_HANDLE,
                                        1 /* unify all I/O handles into one */,
                                        true,
                                        0,
                                        NULL,
                                        SCOREP_IO_ACCESS_MODE_READ_WRITE,
                                        SCOREP_IO_STATUS_FLAG_NONE );
}

void
scorep_posix_io_isoc_fini( void )
{
    SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IO_PARADIGM_ISOC );
}
