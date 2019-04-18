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
 *  @brief Implementation of the initialization/finalization for the POSIX I/O adapter.
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include "SCOREP_PosixIo_Init.h"

#define SCOREP_DEBUG_MODULE_NAME IO
#include <UTILS_Debug.h>

#include "scorep_posix_io.h"
#include "scorep_posix_io_regions.h"
#ifdef SCOREP_LIBWRAP_SHARED
#include "scorep_posix_io_function_pointers.h"
#endif

static size_t subsystem_id = 0;

/**
 * Registers the required configuration variables of the POSIX I/O adapter
 * to the measurement system.
 *
 * @param subsystemId       Subsystem identifier
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
posix_io_subsystem_register( size_t subsystemId )
{
    subsystem_id = subsystemId;

    UTILS_DEBUG( "Register environment variables" );

//  return SCOREP_ConfigRegister( "posix", scorep_posix_configs );
    return SCOREP_SUCCESS;
}

/**
 * Initializes the POSIX I/O adapter.
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
posix_io_subsystem_init( void )
{
#ifdef SCOREP_LIBWRAP_SHARED
    scorep_posix_io_register_function_pointers();
#endif

    scorep_posix_io_init();
    scorep_posix_io_isoc_init();
    scorep_posix_io_register_regions();

    return SCOREP_SUCCESS;
}

/**
 * Finalizes the POSIX I/O adapter.
 */
static void
posix_io_subsystem_finalize( void )
{
    scorep_posix_io_isoc_fini();
    scorep_posix_io_fini();
}

/** POSIX I/O adapter with its callbacks */
SCOREP_Subsystem SCOREP_Subsystem_PosixIoAdapter =
{
    .subsystem_name              = "POSIX IO",
    .subsystem_register          = &posix_io_subsystem_register,
    .subsystem_init              = &posix_io_subsystem_init,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &posix_io_subsystem_finalize,
    .subsystem_deregister        = NULL
};
