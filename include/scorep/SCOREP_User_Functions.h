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

#ifndef SCOREP_USER_FUNCTIONS_H
#define SCOREP_USER_FUNCTIONS_H

/**
    @file       SCOREP_User_Functions.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA
    @ingroup    SCOREP_User_External

    This File contains the function definitions which are called from the user manual
    instrumentation.

    @note The user should not insert calls to theis functions directly, but
    use the macros provided in SCOREP_User.h.
 */

#include <scorep/SCOREP_User_Types.h>
#include <scorep/SCOREP_User_Variables.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
    @defgroup SCOREP_User_External External visisble functions of the SCOREP user adapter
    @ingroup SCOREP_User

    This module contains all external visible items of the user adapter except the
    macros for the manual user instrumentation.

    @note We strongly recommend not to insert
    calls to this functions for instrumentation, but use the provided macros instead.

    @{
 */

/* **************************************************************************************
 * Region functions
 * *************************************************************************************/

/**
    Generates an enter event for the specified region. If the region was not registered
    before, it registers the region. On the first enter, the region is registered to
    the measurement system.
    @note We strongly recommend not to insert calls to this
    function for instrumentation, but use the SCOREP_USER_REGION_BEGIN macro instead.
    @param handle     The handle for this region. It must be defined before.
    @param lastFileName Pointer to the last source file name. In most cases, it is
                      expected that in most cases no regions are in included
                      files. If the compiler inserts always the same string adress
                      for file names, one static variable in a source file
                      (@ref scorep_user_last_file_name) can remember
                      the last used filename from a source file and string comparisons
                      can be avoided.
                      Here the adress of @ref SCOREP_User_LastFileName should be provided.
    @param lastFile   The handle for the last used source file. It is remembered in every
                      source file in a static variable.
                      Thus, in most cases string comparisons can be avoided.
                      Here, the adress of @ref SCOREP_User_LastFileHandle should be
                      provided.
    @param name       The name of the region.
    @param regionType The type of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void
SCOREP_User_RegionBegin
(
    SCOREP_User_RegionHandle*    handle,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFile,
    const char*                  name,
    const SCOREP_User_RegionType regionType,
    const char*                  fileName,
    const uint32_t               lineNo
);

/**
    Generates an exit event for the specified region.
    @note We strongly recommend not to insert calls to this
    function for instrumentation, but use the SCOREP_USER_REGION_END macro instead.
    @param handle     The handle for this region. It must be defined before.
 */
void
SCOREP_User_RegionEnd
(
    const SCOREP_User_RegionHandle handle
);

/**
    Registers a region. The region handle must be defined before. If the handle
    was already initialized, no operation is performed.
    @note We strongly recommend not to insert calls to this
    function for instrumentation, but use the SCOREP_USER_REGION_INIT macro instead.
    @param handle     The handle for this region. It must be defined before.
    @param lastFileName Pointer to the last source file name. In most cases, it is
                      expected that in most cases no regions are in included
                      files. If the compiler inserts always the same string adress
                      for file names, one static variable in a source file
                      (@ref scorep_user_last_file_name) can remember
                      the last used filename from a source file and string comparisons
                      can be avoided.
                      Here the adress of @ref SCOREP_User_LastFileName should be provided.
    @param lastFile   The handle for the last used source file. It is remembered in every
                      source file in a static variable.
                      Thus, in most cases string comparisons can be avoided.
                      Here, the adress of @ref SCOREP_User_LastFileHandle should be
                      provided.
    @param name       The name of the region.
    @param regionType The type of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void
SCOREP_User_RegionInit
(
    SCOREP_User_RegionHandle*    handle,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFile,
    const char*                  name,
    const SCOREP_User_RegionType regionType,
    const char*                  fileName,
    const uint32_t               lineNo
);

/**
    Generates an enter event for the specified region. The region must be declared and
    initialized before.
    @note We strongly recommend not to insert calls to this
    function for instrumentation, but use the SCOREP_USER_REGION_ENTER macro instead.
    @param handle     The handle for this region. It must be defined before.
 */
void
SCOREP_User_RegionEnter
(
    const SCOREP_User_RegionHandle handle
);

/* **************************************************************************************
 * Parameter functions
 * *************************************************************************************/

/**
    Generates a parameter event for a parameter of 64 bit integer data type.
    @note We strongly recommend not to insert calls to this function for instrumentation,
    but use the SCOREP_USER_PARAMETER_INT64 macro instead.
    @param handle Pointer where the parameter handle is stored. If it is
                  SCOREP_USER_INVALID_PARAMETER, the parameter will be registered.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void
SCOREP_User_ParameterInt64
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    int64_t                      value
);

/**
    Generates a parameter event for a parameter of 64 bit unsigned integer data type.
    @note We strongly recommend not to insert calls to this function for instrumentation,
    but use the SCOREP_USER_PARAMETER_UINT64 macro instead.
    @param handle Pointer where the parameter handle is stored. If it is
                  SCOREP_USER_INVALID_PARAMETER, the parameter will be registered.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void
SCOREP_User_ParameterUint64
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    uint64_t                     value
);

/**
    Generates a parameter event for a parameter of string type.
    @note We strongly recommend not to insert calls to this function for instrumentation,
    but use the SCOREP_USER_PARAMETER_STRING macro instead.
    @param handle Pointer where the parameter handle is stored. If it is
                  SCOREP_USER_INVALID_PARAMETER, the parameter will be registered.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void
SCOREP_User_ParameterString
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    char*                        value
);

/* **************************************************************************************
 * User metric functions
 * *************************************************************************************/

/**
    Initilizes a user metric group. Every group must be initialized before it is used
    for the first time.
    @param groupHandle A variable where the handle for the new group i stored. It must
                       be declared with SCOREP_USER_METRIC_GROUP_DEF.
 */
void
SCOREP_User_InitMetricGroup
(
    SCOREP_CounterGroupHandle* groupHandle,
    const char*                name
);

/**
    Initializes a user metric. Every user metric must be registered before it is used
    the first time.
    @param metricHandle A handle which identify the user metric.
    @param unit        A string for the unit of the user metric.
    @param metricType Specifies the data type of the user metric. Possible are
                       SCOREP_USER_METRIC_TYPE_INT64 for 64 bit signed integer,
                       SCOREP_USER_METRIC_TYPE_UINT64 for 64 bit unsigned integer, and
                       SCOREP_USER_METRIC_TYPE_DOUBLE for doubles.
    @param context     Specifies for which context the metrices are obtained.
                       They can be obtained either for the global context or
                       for each callpath. For global context enter
                       SCOREP_USER_METRIC_CONTEXT_GLOBAL, else for measuring
                       per callpath enter SCOREP_USER_METRIC_CONTEXT_CALLPATH.
    @param group       The name of the user couter group to which this metric belongs.
                       If the group does not exist already, it will be created.
 */
void
SCOREP_User_InitMetric
(
    SCOREP_CounterHandle*           metricHandle,
    const char*                     name,
    const char*                     unit,
    const SCOREP_User_MetricType    metricType,
    const int8_t                    context,
    const SCOREP_CounterGroupHandle group
);

/**
    Triggers an user metric of type integer. Every user metric must be registered
    and initialized before it is used for the first time.
    @param metricHandle A handle which identify the user metric.
    @param value        The value for the metric.
 */
void
SCOREP_User_TriggerMetricInt64
(
    SCOREP_CounterHandle metricHandle,
    int64_t              value
);

/**
    Triggers an user metric of type integer. Every user metric must be registered
    and initialized before it is used for the first time.
    @param metricHandle A handle which identify the user metric.
    @param value        The value for the metric.
 */
void
SCOREP_User_TriggerMetricDouble
(
    SCOREP_CounterHandle metricHandle,
    double               value
);

/**
    Declaration of the variable for storing the default metric group handle.
 */
extern SCOREP_CounterGroupHandle SCOREP_User_DefaultMetricGroup;


/* *************************************************************************************
* Control functions
* *************************************************************************************/

/**
    Enables recording of events. If already enabled, this command has no effect.
    The control is not restricted to events from the user adapter, but enables the
    recording of all events.
    @note We strongly recommend not to insert calls to this function for instrumentation,
    but use the SCOREP_RECORDING_ON macro instead.
 */
void
SCOREP_User_EnableRecording();

/**
    Disables recording of events. If already disabled, this command has no effect.
    The control is not restricted to events from the user adapter, but disables the
    recording of all events.
    @note We strongly recommend not to insert calls to this function for instrumentation,
    but use the SCOREP_RECORDING_OFF macro instead.
 */
void
SCOREP_User_DisableRecording();

/**
    Checks if the recording is enabled.
    @note We strongly recommend not to insert calls to this function for instrumentation,
    but use the SCOREP_RECORDING_IS_ON macro instead.
    @returns false if the recording of events is disabled, else it returns true.
 */
bool
SCOREP_User_RecordingEnabled();

#ifdef __cplusplus
} /* extern "C" */

/* **************************************************************************************
 * Class SCOREP_User_Region
 * *************************************************************************************/

/**
    @class SCOREP_User_RegionClass
    This class implements the SCOREP_USER_REGION statement. Its constructor and destructor
    generates the enter and respectively the exit event for the instrumented function.
    @note We strongly recommend not to instatiate this class directly, or insert
    commands which contains this class for instrumentation directly,
    but use the SCOREP_USER_REGION macro instead.
 */
class SCOREP_User_RegionClass
{
public:
/**
    Generates an enter event for the specified region. It should not be inserted by a
    user directly. As user you should use the SCOREP_USER_REGION macro instead.
    @param regionHandle Pointer to a static region handle storage. This avoids
                        reinitialization, if the region is revisited.
    @param regionName   The name of the region.
    @param regionType   The type of the region.
    @param file         The filename of the source file which contains the instrumented
                        region.
    @param lineNo       The line number of the first source code line of the instrumented
                        region.
 */
    SCOREP_User_RegionClass( SCOREP_User_RegionHandle*    regionHandle,
                             const char*                  regionName,
                             const SCOREP_User_RegionType regionType,
                             const char*                  file,
                             const uint32_t               lineNo )
    {
        SCOREP_User_RegionBegin( regionHandle, &SCOREP_User_LastFileName,
                                 &SCOREP_User_LastFileHandle, regionName, regionType,
                                 file, lineNo );
        region_handle = *regionHandle;
    }

    /**
        Generates an exit event for the instrumented region
     */
    ~
    SCOREP_User_RegionClass()
    {
        SCOREP_User_RegionEnd( region_handle );
    }

private:
    /**
        Stores the region handle
     */
    SCOREP_User_RegionHandle region_handle;
};
#endif /* __cplusplus */

/** @} */

#endif /* SCOREP_USER_FUNCTIONS_H */
