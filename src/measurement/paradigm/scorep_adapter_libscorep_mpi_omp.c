/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_adapter_libscorep_mpi_omp.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <scorep_subsystem.h>

#include <SCOREP_Compiler_Init.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Pomp_Init.h>
#include <SCOREP_Mpi_Init.h>
#include <SCOREP_Metric.h>

/**
 * List of adapters.
 */
const SCOREP_Subsystem* scorep_subsystems[] = {
    &SCOREP_Compiler_Adapter,
    &SCOREP_User_Adapter,
    &SCOREP_Pomp_Adapter,
    &SCOREP_Mpi_Adapter,
    &SCOREP_Metric_Service
};


const size_t scorep_number_of_subsystems = sizeof( scorep_subsystems ) /
                                           sizeof( scorep_subsystems[ 0 ] );
