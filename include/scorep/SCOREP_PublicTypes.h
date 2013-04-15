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

#ifndef SCOREP_PUBLICTYPES_H
#define SCOREP_PUBLICTYPES_H

#include <stdint.h>


/** ingroup SCOREP_Types
    @{
 */

/**
 * Opaque handle to memory that can be easily moved between
 * processes. Used for definitions as the have to be moved during
 * unification.
 */
typedef uint32_t SCOREP_Allocator_MovableMemory;

/**
 * Symbolic constant representing an invalid or NULL handle of type
 * SCOREP_Allocator_MovableMemory.
 *
 */
#define SCOREP_MOVABLE_NULL 0

/**
 * Type of a opaque handle to any definition.
 */
typedef SCOREP_Allocator_MovableMemory SCOREP_AnyHandle;

/**
 * Type of a opaque handle to a source file definition.
 * @see SCOREP_DefineSourceFile()
 */
typedef SCOREP_AnyHandle SCOREP_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SCOREP_INVALID_SOURCE_FILE SCOREP_MOVABLE_NULL

/**
 * Type of a opaque handle to a metric definition.
 * @see SCOREP_DefineMetric()
 */
typedef SCOREP_AnyHandle SCOREP_MetricHandle;

/**
 * Symbolic constant representing an invalid or unknown metric
 * definition.
 */
#define SCOREP_INVALID_METRIC SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a sampling set definition.
 * @see SCOREP_DefineSamplingSet()
 */
typedef SCOREP_AnyHandle SCOREP_SamplingSetHandle;

/**
 * Symbolic constant representing an invalid or unknown metric class definition.
 */
#define SCOREP_INVALID_SAMPLING_SET SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a region definition.
 * @see SCOREP_DefineRegion()
 */
typedef SCOREP_AnyHandle SCOREP_RegionHandle;

/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SCOREP_INVALID_REGION SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a RMA window definition.
 * @see SCOREP_DefineRMAWindow()
 */
typedef SCOREP_AnyHandle SCOREP_RMAWindowHandle;


/**@}*/

#endif // SCOREP_PUBLICTYPES_H
