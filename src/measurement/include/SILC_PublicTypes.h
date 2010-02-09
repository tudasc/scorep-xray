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

/** ingroup SILC_Types
    @{
 */

/**
 * Type of a opaque handle to a source file definition.
 * @see SILC_DefineSourceFile()
 */
typedef uint32_t SILC_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SILC_INVALID_SOURCE_FILE UINT32_MAX

/**
 * Type of a opaque handle to a counter group definition.
 * @see SILC_DefineCounterGroup()
 */
typedef uint32_t SILC_CounterGroupHandle;


/**
 * Symbolic constant representing an invalid or unknown counter group
 * definition.
 */
#define SILC_INVALID_COUNTER_GROUP UINT32_MAX


/**
 * Type of a opaque handle to a counter definition.
 * @see SILC_DefineCounter()
 */
typedef uint32_t SILC_CounterHandle;


/**
 * Symbolic constant representing an invalid or unknown counter definition.
 */
#define SILC_INVALID_COUNTER UINT32_MAX


/**
 * Type of a opaque handle to a region definition.
 * @see SILC_DefineRegion()
 */
typedef uint32_t SILC_RegionHandle;


/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SILC_INVALID_REGION UINT32_MAX


/**@}*/

#endif // SILC_PUBLICTYPES_H
