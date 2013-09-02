/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @file       scorep_cupti_callbacks.h
 *
 *  Propagation of the CUPTI callbacks implementation's initialize and finalize
 *  functions.
 */

#ifndef SCOREP_CUPTI_CALLBACKS_H
#define SCOREP_CUPTI_CALLBACKS_H

/**
 * Initialize the ScoreP CUPTI callbacks implementation.
 */
extern void
scorep_cupti_callbacks_init( void );

/**
 * Finalize the ScoreP CUPTI callbacks implementation.
 */
extern void
scorep_cupti_callbacks_finalize( void );

/**
 * Enable CUPTI callback domains depending on the requested GPU features.
 *
 * @param enable 'true' to enable CUPTI callbacks, 'false' to disable callbacks
 */
extern void
scorep_cupti_callbacks_enable( bool enable );

#endif  /* SCOREP_CUPTI_CALLBACKS_H */
