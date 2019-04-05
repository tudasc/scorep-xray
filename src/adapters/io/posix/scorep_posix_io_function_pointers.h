/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2016,
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
 * A function pointer of each wrapped POSIX I/O function is registered at
 * initialization time of the POSIX I/O adapter.
 */

#ifndef SCOREP_POSIX_IO_FUNCTION_POINTERS_H
#define SCOREP_POSIX_IO_FUNCTION_POINTERS_H


#include "scorep_posix_io.h"

#include <SCOREP_Definitions.h>

/**
 * Register POSIX I/O functions and initialize data structures
 */
void
scorep_posix_io_register_function_pointers( void );

/**
 * Register POSIX I/O functions and initialize data structures
 */
void
scorep_posix_io_early_init_function_pointers( void );

#endif /* SCOREP_POSIX_IO_FUNCTION_POINTERS_H */
