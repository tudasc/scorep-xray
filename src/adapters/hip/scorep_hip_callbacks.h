/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
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
 *  Propagation of the HIP callbacks implementation's initialize and finalize
 *  functions.
 */

#ifndef SCOREP_HIP_CALLBACKS_H
#define SCOREP_HIP_CALLBACKS_H

/**
 * Initialize the ScoreP HIP callbacks implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_hip_callbacks_init( void );

/**
 * Finalize the ScoreP HIP callbacks implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_hip_callbacks_finalize( void );

/**
 * Enable HIP callbacks.
 */
void
scorep_hip_callbacks_enable( void );

/**
 * Disable HIP callbacks.
 */
void
scorep_hip_callbacks_disable( void );

#endif /* SCOREP_HIP_CALLBACKS_H */
