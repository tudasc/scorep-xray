/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * Contains helper data structures which are used by the Sun/Oracle Studio
 * compiler adapter.
 */


#ifndef SCOREP_COMPILER_SUN_DATA_H
#define SCOREP_COMPILER_SUN_DATA_H

#include <SCOREP_Types.h>


/**
 * Register new region with Score-P system
 * 'ragion_name' is passed in from SUN compiler
 */
extern SCOREP_RegionHandle
scorep_compiler_register_region( char* region_name );


#endif /* SCOREP_COMPILER_SUN_DATA_H */
