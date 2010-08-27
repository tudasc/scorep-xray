/*
 * This file is part of the SILC project (http://www.silc.de)
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

#ifndef SILC_PUBLICTYPES_H
#define SILC_PUBLICTYPES_H

#include <stdint.h>


#include <silc_utility/SILC_Allocator.h>


/** ingroup SILC_Types
    @{
 */

/**
 * Type of a opaque handle to a source file definition.
 * @see SILC_DefineSourceFile()
 */
typedef SILC_Allocator_MovableMemory SILC_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SILC_INVALID_SOURCE_FILE SILC_MOVABLE_NULL

/**
 * Type of a opaque handle to a counter group definition.
 * @see SILC_DefineCounterGroup()
 */
typedef SILC_Allocator_MovableMemory SILC_CounterGroupHandle;

/**
 * Symbolic constant representing an invalid or unknown counter group
 * definition.
 */
#define SILC_INVALID_COUNTER_GROUP SILC_MOVABLE_NULL


/**
 * Type of a opaque handle to a counter definition.
 * @see SILC_DefineCounter()
 */
typedef SILC_Allocator_MovableMemory SILC_CounterHandle;

/**
 * Symbolic constant representing an invalid or unknown counter definition.
 */
#define SILC_INVALID_COUNTER SILC_MOVABLE_NULL


/**
 * Type of a opaque handle to a region definition.
 * @see SILC_DefineRegion()
 */
typedef SILC_Allocator_MovableMemory SILC_RegionHandle;

/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SILC_INVALID_REGION SILC_MOVABLE_NULL


/**@}*/

#endif // SILC_PUBLICTYPES_H
