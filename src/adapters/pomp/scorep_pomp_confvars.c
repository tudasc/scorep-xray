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
 * @file       src/adapters/pomp/scorep_pomp_confvars.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Debug.h>


#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>
#include <SCOREP_Location.h>


#include "scorep_pomp_confvars.inc.c"

/** Struct which contains the adapter iniitialization and finalization functions for the
    POMP2 adapter.
 */
const SCOREP_Subsystem SCOREP_Pomp_Adapter =
{
    .subsystem_name              = "POMP2 Adapter / Version 1.0 (config variables only)",
    .subsystem_register          = NULL,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL
};
