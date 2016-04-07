/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Propagation of the CUPTI callbacks implementation's initialize and finalize
 *  functions.
 */

#ifndef SCOREP_CUPTI_CALLBACKS_H
#define SCOREP_CUPTI_CALLBACKS_H

#include "scorep_cupti.h"

/* Defines CUPTI callbacks states to monitor enable/disable state */
#define SCOREP_CUPTI_CALLBACKS_STATE_NONE                   ( 0 )
#define SCOREP_CUPTI_CALLBACKS_STATE_RUNTIME                ( 1 << 0 )
#define SCOREP_CUPTI_CALLBACKS_STATE_DRIVER                 ( 1 << 1 )
extern uint8_t scorep_cupti_callbacks_state;

/* flag: tracing of CUDA runtime API enabled? */
extern bool scorep_record_runtime_api;

/* flag: tracing of CUDA driver API enabled? */
extern bool scorep_record_driver_api;

#define SCOREP_SET_CALLBACKS_STATE( _state ) \
    { \
        scorep_cuda_location_data* loc_data = \
            SCOREP_Location_GetSubsystemData( SCOREP_Location_GetCurrentCPULocation(), \
                                              scorep_cuda_subsystem_id ); \
        UTILS_ASSERT( loc_data ); \
        loc_data->callbacksState |= _state; \
    }

#define SCOREP_UNSET_CALLBACKS_STATE( _state ) \
    { \
        scorep_cuda_location_data* loc_data = \
            SCOREP_Location_GetSubsystemData( SCOREP_Location_GetCurrentCPULocation(), \
                                              scorep_cuda_subsystem_id ); \
        UTILS_ASSERT( loc_data ); \
        loc_data->callbacksState &= ~_state; \
    }

#define SCOREP_IS_CALLBACK_STATE_SET( _state, _location ) \
    ( ( ( scorep_cuda_location_data* )SCOREP_Location_GetSubsystemData( _location, \
                                                                        scorep_cuda_subsystem_id ) )->callbacksState & _state )

/* Resume CUDA driver API callbacks in Score-P, if recording is enabled */
#define SCOREP_RESUME_CUDRV_CALLBACKS() \
    if ( scorep_cupti_callbacks_state & SCOREP_CUPTI_CALLBACKS_STATE_DRIVER ) \
    { \
        SCOREP_SET_CALLBACKS_STATE( SCOREP_CUPTI_CALLBACKS_STATE_DRIVER ) \
    }

/* Suspend CUDA driver API callbacks in Score-P, if recording is enabled */
#define SCOREP_SUSPEND_CUDRV_CALLBACKS() \
    if ( scorep_cupti_callbacks_state & SCOREP_CUPTI_CALLBACKS_STATE_DRIVER ) \
    { \
        SCOREP_UNSET_CALLBACKS_STATE( SCOREP_CUPTI_CALLBACKS_STATE_DRIVER ) \
    }

/**
 * Initialize the ScoreP CUPTI callbacks implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_callbacks_init( void );

/**
 * Finalize the ScoreP CUPTI callbacks implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_callbacks_finalize( void );

/**
 * Enable CUPTI callback domains depending on the requested GPU features.
 *
 * @param enable 'true' to enable CUPTI callbacks, 'false' to disable callbacks
 */
void
scorep_cupti_callbacks_enable( bool enable );

#endif /* SCOREP_CUPTI_CALLBACKS_H */
