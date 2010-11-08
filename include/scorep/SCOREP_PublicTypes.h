/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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

#ifndef SCOREP_PUBLICTYPES_H
#define SCOREP_PUBLICTYPES_H

#include <stdint.h>


#include <scorep_utility/SCOREP_Allocator.h>


/** ingroup SCOREP_Types
    @{
 */

/**
 * Type of a opaque handle to a source file definition.
 * @see SCOREP_DefineSourceFile()
 */
typedef SCOREP_Allocator_MovableMemory SCOREP_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SCOREP_INVALID_SOURCE_FILE SCOREP_MOVABLE_NULL

/**
 * Type of a opaque handle to a counter group definition.
 * @see SCOREP_DefineCounterGroup()
 */
typedef SCOREP_Allocator_MovableMemory SCOREP_CounterGroupHandle;

/**
 * Symbolic constant representing an invalid or unknown counter group
 * definition.
 */
#define SCOREP_INVALID_COUNTER_GROUP SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a counter definition.
 * @see SCOREP_DefineCounter()
 */
typedef SCOREP_Allocator_MovableMemory SCOREP_CounterHandle;

/**
 * Symbolic constant representing an invalid or unknown counter definition.
 */
#define SCOREP_INVALID_COUNTER SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a region definition.
 * @see SCOREP_DefineRegion()
 */
typedef SCOREP_Allocator_MovableMemory SCOREP_RegionHandle;

/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SCOREP_INVALID_REGION SCOREP_MOVABLE_NULL


/**@}*/

#endif // SCOREP_PUBLICTYPES_H
