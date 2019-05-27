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
 * @file
 */


#include <config.h>

#include <SCOREP_Subsystem.h>

extern const SCOREP_Subsystem SCOREP_Subsystem_Substrates;
extern const SCOREP_Subsystem SCOREP_Subsystem_TaskStack;
extern const SCOREP_Subsystem SCOREP_Subsystem_MetricService;
extern const SCOREP_Subsystem SCOREP_Subsystem_Topologies;
extern const SCOREP_Subsystem SCOREP_Subsystem_PlatformTopology;
extern const SCOREP_Subsystem SCOREP_Subsystem_IoManagement;

/** @brief list of linked in subsystems. */
const SCOREP_Subsystem* scorep_subsystems[] = {
    &SCOREP_Subsystem_Substrates,
    &SCOREP_Subsystem_TaskStack,
    &SCOREP_Subsystem_MetricService,
    &SCOREP_Subsystem_Topologies,
    &SCOREP_Subsystem_PlatformTopology,
    &SCOREP_Subsystem_IoManagement
};

const size_t scorep_number_of_subsystems = sizeof( scorep_subsystems ) /
                                           sizeof( scorep_subsystems[ 0 ] );
