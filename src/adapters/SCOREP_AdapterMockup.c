/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
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
 * @file       SCOREP_Adapter_Mockup.c
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
