/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
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

#include <scorep_adapter.h>

#include <SCOREP_Compiler_Init.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Pomp_Init.h>
#include <SCOREP_Mpi_Init.h>

/**
 * List of adapters.
 */
const SCOREP_Adapter* scorep_adapters[] = {
    &SCOREP_Compiler_Adapter,
    &SCOREP_User_Adapter,
    &SCOREP_Pomp_Adapter,
    &SCOREP_Mpi_Adapter
};


const size_t scorep_number_of_adapters = sizeof( scorep_adapters ) /
                                         sizeof( scorep_adapters[ 0 ] );
