/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @brief The accelerator management interface
 */

#ifndef SCOREP_ACCELERATOR_MANAGEMENT_H
#define SCOREP_ACCELERATOR_MANAGEMENT_H

#include <SCOREP_DefinitionHandles.h>

/**
 * @brief Create a new accellerator context.
 *
 * @param deviceHandle  The system tree node handle for the device to create
 *                      the context.
 * @param name          The name for the context.
 */
SCOREP_LocationGroupHandle
SCOREP_AcceleratorMgmt_CreateContext( SCOREP_SystemTreeNodeHandle deviceHandle,
                                      const char*                 name );

/**
 * @brief Parameter for the callsite hash to connect kerneal and kernal launch.
 */
SCOREP_ParameterHandle
SCOREP_AcceleratorMgmt_GetCallsiteParameter( void );

#endif /* SCOREP_ACCELERATOR_MANAGEMENT_H */
