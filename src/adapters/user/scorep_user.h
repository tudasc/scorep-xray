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

/**
 *  @file
 *
 */

#ifndef SCOREP_INTERNAL_USER_H
#define SCOREP_INTERNAL_USER_H

void
scorep_user_init_regions( void );

void
scorep_user_finalize_regions( void );

void
scorep_user_init_metrics( void );

void
scorep_user_finalize_metrics( void );

void
scorep_selective_init( void );

SCOREP_ErrorCode
scorep_selective_register( void );

void
scorep_selective_finalize( void );

#endif /* SCOREP_INTERNAL_USER_H */
