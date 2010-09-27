/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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

#ifndef SCOREP_USER_TYPES_H
#define SCOREP_USER_TYPES_H

/**
   @file       SCOREP_User_Types.h
   @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
   @status     ALPHA
   @ingroup    SCOREP_User_Interface

   @brief This file contains type definitions for manual user instrumentation.
 */

#include "SCOREP_PublicTypes.h"

/* **************************************************************************************
 * Typedefs
 * *************************************************************************************/

/** @ingroup SCOREP_User_External
    @{
 */
/** Type for the region type */
typedef uint32_t SCOREP_User_RegionType;

/** Type for the user metric type */
typedef uint32_t SCOREP_User_MetricType;

/** @} */

/* **************************************************************************************
 * Defines for the Region types
 * *************************************************************************************/

/** @ingroup SCOREP_User_Interface
    @{
 */

/** @def SCOREP_USER_REGION_TYPE_COMMON
    Region without any specific type.
 */
#define SCOREP_USER_REGION_TYPE_COMMON 0

/** @def SCOREP_USER_REGION_TYPE_FUNCTION
    Marks the region as being the codeblock of a function.
 */
#define SCOREP_USER_REGION_TYPE_FUNCTION 1

/** @def SCOREP_USER_REGION_TYPE_LOOP
    Marks the region as being the codeblock of a look with the same number of iterations
    on all processes.
 */
#define SCOREP_USER_REGION_TYPE_LOOP 2

/** @def SCOREP_USER_REGION_TYPE_DYNAMIC
    Marks the regions as dynamic.
 */
#define SCOREP_USER_REGION_TYPE_DYNAMIC  4

/** @def SCOREP_USER_REGION_TYPE_PHASE
    @Marks the region as being a root node of a phase.
 */
#define SCOREP_USER_REGION_TYPE_PHASE    8

/* **************************************************************************************
 * Defines for the data type of a user counter
 * *************************************************************************************/

/** @def SCOREP_USER_METRIC_TYPE_INT64
    Indicates that a user counter is of data type signed 64 bit integer.
 */
#define SCOREP_USER_METRIC_TYPE_INT64 0

/** @def SCOREP_USER_METRIC_TYPE_UINT64
    Indicates that a user counter is of data type unsigned 64 bit integer.
 */
#define SCOREP_USER_METRIC_TYPE_UINT64 0

/** @def SCOREP_USER_METRIC_TYPE_DOUBLE
    Indicates that a user counter is of data type double.
 */
#define SCOREP_USER_METRIC_TYPE_DOUBLE 1

/* **************************************************************************************
 * Defines for the context of user counters
 * *************************************************************************************/

/** @def SCOREP_USER_METRIC_CONTEXT_GLOBAL
    Indicates that a user counter is is measured for the global context.
 */
#define SCOREP_USER_METRIC_CONTEXT_GLOBAL 0

/** @def SCOREP_USER_METRIC_CONTEXT_CALLPATH
    Indicates that a user counter is is measured for every callpath.
 */
#define SCOREP_USER_METRIC_CONTEXT_CALLPATH 1

/* **************************************************************************************
 * Default and uninitialized handles
 * *************************************************************************************/

/** The metric group handle for the default group. This group is created when the
    user adapter is initialized.
 */
#define SCOREP_USER_METRIC_GROUP_DEFAULT SCOREP_User_DefaultMetricGroup

/** @} */

#endif // SCOREP_USER_TYPES_H
