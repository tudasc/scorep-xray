/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 *  @status     beta
 *  @file       scorep_cupti_callbacks.h
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  Propagation of the CUPTI callbacks implementation's initialize and finalize
 *  functions.
 */

#ifndef SCOREP_CUPTI_CALLBACKS_H
#define SCOREP_CUPTI_CALLBACKS_H

/*
 * Initialize the ScoreP CUPTI callbacks implementation.
 */
extern void
scorep_cupti_callbacks_init( void );

/*
 * Finalize the ScoreP CUPTI callbacks implementation.
 */
extern void
scorep_cupti_callbacks_finalize( void );

#endif  /* SCOREP_CUPTI_CALLBACKS_H */
