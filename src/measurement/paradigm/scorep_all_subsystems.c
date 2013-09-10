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
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 *
 * @brief       Holds the list of adapters linked into the measurement system.
 */


#include <config.h>

#include <scorep_subsystem.h>

extern const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_PompUserAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_PompOmpAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_MetricService;
extern const SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter;

/**
 * List of subsystems.
 */
const SCOREP_Subsystem* scorep_subsystems[] = {
    &SCOREP_Subsystem_MetricService,
    &SCOREP_Subsystem_CompilerAdapter,
    &SCOREP_Subsystem_UserAdapter,
    &SCOREP_Subsystem_PompUserAdapter,
    &SCOREP_Subsystem_PompOmpAdapter,
    &SCOREP_Subsystem_MpiAdapter,
    &SCOREP_Subsystem_CudaAdapter
};

const size_t scorep_number_of_subsystems = sizeof( scorep_subsystems ) /
                                           sizeof( scorep_subsystems[ 0 ] );
