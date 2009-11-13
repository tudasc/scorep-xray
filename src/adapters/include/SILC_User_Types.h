/** @file SILC_User_Types.h
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de

    This file contains type definitions for manual user instrumentation.
 */

#ifndef SILC_USER_TYPES_H
#define SILC_USER_TYPES_H

#include "SILC_Types.h"

#include <stdint.h>

/* **************************************************************************************
 * Typedefs
 * *************************************************************************************/

/** @ingroup SILC_User_External
    @{
 */
/** Type for the region type */
typedef uint32_t SILC_User_RegionType;

/** Type for the user metric type */
typedef uint32_t SILC_User_MetricType;

/** @} */

/* **************************************************************************************
 * Defines for the Region types
 * *************************************************************************************/

/** @ingroup SILC_User_Interface
    @{
 */

/** @def SILC_USER_REGION_TYPE_COMMON
    Region without any specific type.
 */
#define SILC_USER_REGION_TYPE_COMMON 0

/** @def SILC_USER_REGION_TYPE_FUNCTION
    Marks the region as being the codeblock of a function.
 */
#define SILC_USER_REGION_TYPE_FUNCTION 1

/** @def SILC_USER_REGION_TYPE_LOOP
    Marks the region as being the codeblock of a look with the same number of iterations
    on all processes.
 */
#define SILC_USER_REGION_TYPE_LOOP 2

/** @def SILC_USER_REGION_TYPE_DYNAMIC
    Marks the regions as dynamic.
 */
#define SILC_USER_REGION_TYPE_DYNAMIC  4

/** @def SILC_USER_REGION_TYPE_PHASE
    @Marks the region as being a root node of a phase.
 */
#define SILC_USER_REGION_TYPE_PHASE    8

/* **************************************************************************************
 * Defines for the data type of a user counter
 * *************************************************************************************/

/** @def SILC_USER_METRIC_TYPE_INT64
    Indicates that a user counter is of data type signed 64 bit integer.
 */
#define SILC_USER_METRIC_TYPE_INT64 SILC_COUNTER_INT64

/** @def SILC_USER_METRIC_TYPE_UINT64
    Indicates that a user counter is of data type unsigned 64 bit integer.
 */
#define SILC_USER_METRIC_TYPE_UINT64 SILC_COUNTER_INT64

/** @def SILC_USER_METRIC_TYPE_DOUBLE
    Indicates that a user counter is of data type double.
 */
#define SILC_USER_METRIC_TYPE_DOUBLE SILC_COUNTER_DOUBLE

/* **************************************************************************************
 * Defines for the context of user counters
 * *************************************************************************************/

/** @def SILC_USER_METRIC_CONTEXT_GLOBAL
    Indicates that a user counter is is measured for the global context.
 */
#define SILC_USER_METRIC_CONTEXT_GLOBAL 0

/** @def SILC_USER_METRIC_CONTEXT_CALLPATH
    Indicates that a user counter is is measured for every callpath.
 */
#define SILC_USER_METRIC_CONTEXT_CALLPATH 1

/* **************************************************************************************
 * Default and uninitialized handles
 * *************************************************************************************/

/** The metric group handle for the default group. This group is created when the
    user adapter is initialized.
 */
#define SILC_USER_METRIC_GROUP_DEFAULT SILC_User_DefaultMetricGroup

/** @} */

#endif // SILC_USER_TYPES_H
