/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_IO_EVENTS_H
#define SCOREP_PROFILE_IO_EVENTS_H

/**
 * @file
 *
 */

#include <SCOREP_Types.h>
#include <SCOREP_Location.h>
#include <SCOREP_DefinitionHandles.h>

/**
 * Initilaizes the metrics and handles for I/O recording.
 */
void
scorep_profile_io_init( void );

void
scorep_profile_io_operation_complete( SCOREP_Location*       location,
                                      uint64_t               timestamp,
                                      SCOREP_IoHandleHandle  handle,
                                      SCOREP_IoOperationMode mode,
                                      uint64_t               bytesResult,
                                      uint64_t               matchingId );

#endif /* SCOREP_PROFILE_IO_EVENTS_H */
