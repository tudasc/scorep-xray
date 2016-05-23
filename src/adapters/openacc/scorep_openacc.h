/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2016,
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
 *  Commonly used definitions and functionality in the OpenACC adapter.
 */

#ifndef SCOREP_OPENACC_H
#define SCOREP_OPENACC_H

#include <SCOREP_Definitions.h>
#include <SCOREP_AllocMetric.h>

#include <openacc.h>
#include <acc_prof.h>

extern bool scorep_openacc_features_initialized;

/* Attributes for additional information on OpenACC regions */
extern SCOREP_AttributeHandle scorep_openacc_attribute_implicit;
extern SCOREP_AttributeHandle scorep_openacc_attribute_transfer_bytes;
extern SCOREP_AttributeHandle scorep_openacc_attribute_variable_name;
extern SCOREP_AttributeHandle scorep_openacc_attribute_kernel_name;
extern SCOREP_AttributeHandle scorep_openacc_attribute_kernel_gangs;
extern SCOREP_AttributeHandle scorep_openacc_attribute_kernel_workers;
extern SCOREP_AttributeHandle scorep_openacc_attribute_kernel_veclen;

/**
 * Get a Score-P region handle. The function parameters uniquely identify an
 * OpenACC region. This function implicitly creates a new Score-P region handle
 * if it cannot be found.
 *
 * @param lineNo     line number
 * @param regionType type of OpenACC region/event
 * @param srcFile    source file
 *
 * @return Score-P region handle
 */
SCOREP_RegionHandle
scorep_openacc_get_region_handle( int         lineNo,
                                  acc_event_t regionType,
                                  const char* srcFile );

/**
 * Get an OpenACC device memory metric handle for the given OpenACC device.
 * This function implicitly creates a new Score-P allocation metric handle, if
 * it has not yet been created for the given device.
 *
 * @param deviceType OpenCC device type
 * @param deviceNumber device number
 *
 * @return Score-P allocation metric handle for OpenACC device memory
 */
SCOREP_AllocMetric*
scorep_openacc_get_alloc_metric_handle( acc_device_t deviceType,
                                        int          deviceNumber );

#endif  /* SCOREP_OPENACC_H */
