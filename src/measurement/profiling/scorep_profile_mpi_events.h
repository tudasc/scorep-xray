/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_MPI_EVENTS_H
#define SCOREP_PROFILE_MPI_EVENTS_H

/**
 * @file
 *
 */

#include <SCOREP_DefinitionHandles.h>

/**
 * Initilaizes the metrics and handles for one-sided communication.
 * They are not initialized with SCOREP_Profile_initializeMpp because
 * the CUDA adapter uses also one-sided events.
 */
void
scorep_profile_init_rma( void );

/**
 * Returns the handle for the 'bytes recv' metric.
 */
SCOREP_MetricHandle
scorep_profile_get_bytes_recv_metric_handle( void );

/**
 * Returns the handle for the 'bytes sent' metric.
 */
SCOREP_MetricHandle
scorep_profile_get_bytes_send_metric_handle( void );

#endif /* SCOREP_PROFILE_MPI_EVENTS_H */
