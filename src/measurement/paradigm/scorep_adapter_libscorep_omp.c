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
 * @file       scorep_adapter_libscorep_omp.c
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

/**
 * List of adapters.
 */
const SCOREP_Adapter* scorep_adapters[] = {
    &SCOREP_Compiler_Adapter,
    &SCOREP_User_Adapter,
    &SCOREP_Pomp_Adapter
};


const size_t scorep_number_of_adapters = sizeof( scorep_adapters ) /
                                         sizeof( scorep_adapters[ 0 ] );
