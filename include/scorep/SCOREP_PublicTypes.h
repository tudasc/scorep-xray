/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
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
 * @see SCOREP_Definitions_NewSourceFile()
 */
typedef SCOREP_AnyHandle SCOREP_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SCOREP_INVALID_SOURCE_FILE SCOREP_MOVABLE_NULL

/**
 * Type of a opaque handle to a metric definition.
 * @see SCOREP_Definitions_NewMetric()
 */
typedef SCOREP_AnyHandle SCOREP_MetricHandle;

/**
 * Symbolic constant representing an invalid or unknown metric
 * definition.
 */
#define SCOREP_INVALID_METRIC SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a sampling set definition.
 * @see SCOREP_Definitions_NewSamplingSet()
 */
typedef SCOREP_AnyHandle SCOREP_SamplingSetHandle;

/**
 * Symbolic constant representing an invalid or unknown metric class definition.
 */
#define SCOREP_INVALID_SAMPLING_SET SCOREP_MOVABLE_NULL


/**
 * Type of a opaque handle to a region definition.
 * @see SCOREP_Definitions_NewRegion()
 */
typedef SCOREP_AnyHandle SCOREP_RegionHandle;

/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SCOREP_INVALID_REGION SCOREP_MOVABLE_NULL


/**@}*/

#endif /* SCOREP_PUBLICTYPES_H */
