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
 * @file
 *
 *
 * Contains helper data structures which are used by the Sun/Oracle Studio
 * compiler adapter.
 */


#ifndef SCOREP_COMPILER_SUN_DATA_
#define SCOREP_COMPILER_SUN_DATA_

#include <SCOREP_Definitions.h>


/**
 * Handle for the main region, which is not instrumented by the compiler.
 */
extern SCOREP_RegionHandle scorep_compiler_main_handle;

/**
 * Register new region with Score-P system
 * 'ragion_name' is passed in from SUN compiler
 */
extern SCOREP_RegionHandle
scorep_compiler_register_region( char* region_name );


#endif /* SCOREP_COMPILER_SUN_DATA_ */
