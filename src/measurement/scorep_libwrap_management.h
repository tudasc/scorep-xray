/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Function definitions of library wrapping management
 */

#ifndef SCOREP_LIBWRAP_MANAGEMENT_H
#define SCOREP_LIBWRAP_MANAGEMENT_H

#include <scorep/SCOREP_Libwrap.h>

/**
 * This function initializes library wrapping infrastructure.
 */
void
SCOREP_Libwrap_Initialize( void );

/**
 * This function finalizes library wrapping infrastructure.
 */
void
SCOREP_Libwrap_Finalize( void );

#endif /* SCOREP_LIBWRAP_MANAGEMENT_H */
