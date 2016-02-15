/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


#ifndef SCOREP_COMPILER_INIT_H
#define SCOREP_COMPILER_INIT_H


/**
 * @ file      src/adapters/compiler/SCOREP_Compiler_Init.h
 *
 * @brief Compiler adapter initialization and finalization function definition
 *        object.
 */

#include <SCOREP_Subsystem.h>
#include <SCOREP_Mutex.h>

#include <stdbool.h>

/**
   The adapter initialization and finalization function structure for the
   compiler adapter.
 */
extern const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter;

/** @internal
     Mutex to avoid parallel assignment of region handles to the same region.
 */
extern SCOREP_Mutex scorep_compiler_region_mutex;


#endif /* SCOREP_COMPILER_INIT_H */
