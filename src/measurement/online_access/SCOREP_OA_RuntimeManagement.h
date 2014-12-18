/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_OA_INIT_H
#define SCOREP_OA_INIT_H


/**
 * @file
 *
 * @brief   Declaration of OA initialization and finalization functions
 *
 */

#include <stdint.h>
#include <stdbool.h>

extern bool scorep_oa_is_requested;

#define SCOREP_OA_IS_REQUESTED            ( scorep_oa_is_requested )

int8_t
SCOREP_OA_Init( void );

int8_t
SCOREP_OA_Initialized( void );

void
SCOREP_OA_Finalize( void );

void
SCOREP_OA_Register( void );

#endif /* SCOREP_OA_INIT_H */
