/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file scorep_monolithic_subsystems.c
 */


#include <config.h>

#include <SCOREP_Subsystem.h>

extern const SCOREP_Subsystem SCOREP_Subsystem_Substrates;
extern const SCOREP_Subsystem SCOREP_Subsystem_TaskStack;
extern const SCOREP_Subsystem SCOREP_Subsystem_MetricService;
#if HAVE( UNWINDING_SUPPORT )
extern const SCOREP_Subsystem SCOREP_Subsystem_UnwindingService;
#endif
#if HAVE( SAMPLING_SUPPORT )
extern const SCOREP_Subsystem SCOREP_Subsystem_SamplingService;
#endif
#if HAVE( COMPILER_INSTRUMENTATION )
extern const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter;
#endif
extern const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_ShmemAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_ThreadForkJoin;
extern const SCOREP_Subsystem SCOREP_Subsystem_Opari2OpenmpAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter;
extern const SCOREP_Subsystem SCOREP_Subsystem_IoManagement;

/** @brief list of linked in subsystems. */
const SCOREP_Subsystem* scorep_subsystems[] = {
    &SCOREP_Subsystem_Substrates,
    &SCOREP_Subsystem_TaskStack,
    &SCOREP_Subsystem_MetricService,
#if HAVE( UNWINDING_SUPPORT )
    &SCOREP_Subsystem_UnwindingService,
#endif
#if HAVE( SAMPLING_SUPPORT )
    &SCOREP_Subsystem_SamplingService,
#endif
#if HAVE( COMPILER_INSTRUMENTATION )
    &SCOREP_Subsystem_CompilerAdapter,
#endif
    &SCOREP_Subsystem_UserAdapter,
#ifdef SCOREP_SUBSYSTEMS_OMP
    &SCOREP_Subsystem_Opari2OpenmpAdapter,
#endif
#ifdef SCOREP_SUBSYSTEMS_MPI
    &SCOREP_Subsystem_MpiAdapter,
    &SCOREP_Subsystem_IoManagement,
#endif
#ifdef SCOREP_SUBSYSTEMS_SHMEM
    &SCOREP_Subsystem_ShmemAdapter,
#endif
#if HAVE_CUDA_SUPPORT
    &SCOREP_Subsystem_CudaAdapter,
#endif
    &SCOREP_Subsystem_ThreadForkJoin
};

const size_t scorep_number_of_subsystems = sizeof( scorep_subsystems ) /
                                           sizeof( scorep_subsystems[ 0 ] );
