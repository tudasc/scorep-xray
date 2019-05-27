/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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
 * @brief Holds the list of all existing SCOREP_Subsystem objects. Needed for
 * scorep-info and scorep-backend-info only. The list of subsystems active in
 * an instrumented application is generated during instrumentation (via
 * scorep-config ----adapter-init).
 */


#include <config.h>

#include <scorep_subsystem_management.h>

extern const SCOREP_Subsystem SCOREP_Subsystem_Substrates;
extern const SCOREP_Subsystem SCOREP_Subsystem_TaskStack;
extern const SCOREP_Subsystem SCOREP_Subsystem_MetricService;
extern const SCOREP_Subsystem SCOREP_Subsystem_UnwindingService;
extern const SCOREP_Subsystem SCOREP_Subsystem_SamplingService;
extern const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_ShmemAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_Opari2UserAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_Opari2OpenmpAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_OpenclAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_OpenaccAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_ThreadForkJoin;
extern const SCOREP_Subsystem SCOREP_Subsystem_PthreadAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_ThreadCreateWait;
extern const SCOREP_Subsystem SCOREP_Subsystem_MemoryAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_Topologies;
extern const SCOREP_Subsystem SCOREP_Subsystem_PlatformTopology;

/**
 * List of subsystems.
 */
const SCOREP_Subsystem* scorep_subsystems[] = {
    &SCOREP_Subsystem_Substrates,
    &SCOREP_Subsystem_TaskStack,
    &SCOREP_Subsystem_MetricService,
    &SCOREP_Subsystem_UnwindingService,
    &SCOREP_Subsystem_SamplingService,
    &SCOREP_Subsystem_Topologies,
    &SCOREP_Subsystem_PlatformTopology,
    &SCOREP_Subsystem_CompilerAdapter,
    &SCOREP_Subsystem_UserAdapter,
    &SCOREP_Subsystem_Opari2UserAdapter,
    &SCOREP_Subsystem_MpiAdapter,
    &SCOREP_Subsystem_ShmemAdapter,
    &SCOREP_Subsystem_ThreadForkJoin,
    &SCOREP_Subsystem_Opari2OpenmpAdapter,
    &SCOREP_Subsystem_ThreadCreateWait,
    &SCOREP_Subsystem_PthreadAdapter,
    &SCOREP_Subsystem_CudaAdapter,
    &SCOREP_Subsystem_OpenclAdapter,
    &SCOREP_Subsystem_OpenaccAdapter,
    &SCOREP_Subsystem_MemoryAdapter
};

const size_t scorep_number_of_subsystems = sizeof( scorep_subsystems ) /
                                           sizeof( scorep_subsystems[ 0 ] );
