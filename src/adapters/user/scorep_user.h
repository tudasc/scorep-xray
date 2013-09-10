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
