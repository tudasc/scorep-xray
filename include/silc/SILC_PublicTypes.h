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
typedef struct SILC_SourceFile_Definition  SILC_SourceFile_Definition;
typedef struct SILC_SourceFile_Definition* SILC_SourceFileHandle;

/**
 * Symbolic constant representing an invalid or unknown source file definition.
 *
 */
#define SILC_INVALID_SOURCE_FILE NULL

/**
 * Type of a opaque handle to a counter group definition.
 * @see SILC_DefineCounterGroup()
 */
typedef struct SILC_CounterGroup_Definition  SILC_CounterGroup_Definition;
typedef struct SILC_CounterGroup_Definition* SILC_CounterGroupHandle;


/**
 * Symbolic constant representing an invalid or unknown counter group
 * definition.
 */
#define SILC_INVALID_COUNTER_GROUP NULL


/**
 * Type of a opaque handle to a counter definition.
 * @see SILC_DefineCounter()
 */
typedef struct SILC_Counter_Definition  SILC_Counter_Definition;
typedef struct SILC_Counter_Definition* SILC_CounterHandle;


/**
 * Symbolic constant representing an invalid or unknown counter definition.
 */
#define SILC_INVALID_COUNTER NULL


/**
 * Type of a opaque handle to a region definition.
 * @see SILC_DefineRegion()
 */
typedef struct SILC_Region_Definition  SILC_Region_Definition;
typedef struct SILC_Region_Definition* SILC_RegionHandle;


/**
 * Symbolic constant representing an invalid or unknown region definition.
 */
#define SILC_INVALID_REGION NULL


/**@}*/

#endif // SILC_PUBLICTYPES_H
