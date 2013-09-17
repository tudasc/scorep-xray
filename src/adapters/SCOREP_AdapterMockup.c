/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains an empty init structure that can be compiled into
 * a mockup adapter for every adapter by defining the adapter name.
 */

#include <config.h>
#include <SCOREP_Subsystem.h>

const SCOREP_Subsystem
UTILS_JOIN_3SYMS( SCOREP_Subsystem_, ADAPTER, Adapter ) =
{
    .subsystem_name              = UTILS_STRINGIFY( ADAPTER ),
    .subsystem_register          = NULL,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
