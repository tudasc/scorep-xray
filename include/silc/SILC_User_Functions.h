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

#ifndef SILC_USER_FUNCTIONS_H
#define SILC_USER_FUNCTIONS_H

/**
    @file SILC_User_Functions.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA
    @ingroup    SILC_User_External

    This File contains the function definitions which are called from the user manual
    instrumentation. The user should not insert calls to theis functions directly, but
    use the macros provided in SILC_User.h.
 */

#include "SILC_User_Types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
    @defgroup SILC_User_External External visisble functions of the SILC user adapter
    @ingroup SILC_User

    This module contains all external visible items of the user adapter except the
    macros for the manual user instrumentation.

    @{
 */

/* **************************************************************************************
 * Region functions
 * *************************************************************************************/

/**
    Generates an enter event for the specified region. If the region was not registered
    before, it registers the region. On the first enter, the region is registered to
    the measurement system.
    @param handle     The handle for this region. It must be defined before.
    @param lastFileName Pointer to the last source file name. In most cases, it is
                      expected that in most cases no regions are in included
                      files. If the compiler inserts always the same string adress
                      for file names, one static variable in a source file
                      (@ref silc_user_last_file_name) can remember
                      the last used filename from a source file and string comparisons
                      can be avoided.
                      Here the adress of @ref SILC_User_LastFileName should be provided.
    @param lastFile   The handle for the last used source file. It is remembered in every
                      source file in a static variable.
                      Thus, in most cases string comparisons can be avoided.
                      Here, the adress of @ref SILC_User_LastFileHandle should be
                      provided.
    @param name       The name of the region.
    @param regionType The type of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void
SILC_User_RegionBegin
(
    SILC_RegionHandle*         handle,
    const char**               lastFileName,
    SILC_SourceFileHandle*     lastFile,
    const char*                name,
    const SILC_User_RegionType regionType,
    const char*                fileName,
    const uint32_t             lineNo
);

/**
    Generates an exit event for the specified region.
    @param handle     The handle for this region. It must be defined before.
 */
void
SILC_User_RegionEnd
(
    const SILC_RegionHandle handle
);

/**
    Registers a region. The region handle must be defined before. If the handle
    was already initialized, no operation is performed.
    @param handle     The handle for this region. It must be defined before.
    @param lastFileName Pointer to the last source file name. In most cases, it is
                      expected that in most cases no regions are in included
                      files. If the compiler inserts always the same string adress
                      for file names, one static variable in a source file
                      (@ref silc_user_last_file_name) can remember
                      the last used filename from a source file and string comparisons
                      can be avoided.
                      Here the adress of @ref SILC_User_LastFileName should be provided.
    @param lastFile   The handle for the last used source file. It is remembered in every
                      source file in a static variable.
                      Thus, in most cases string comparisons can be avoided.
                      Here, the adress of @ref SILC_User_LastFileHandle should be
                      provided.
    @param name       The name of the region.
    @param regionType The type of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void
SILC_User_RegionInit
(
    SILC_RegionHandle*         handle,
    const char**               lastFileName,
    SILC_SourceFileHandle*     lastFile,
    const char*                name,
    const SILC_User_RegionType regionType,
    const char*                fileName,
    const uint32_t             lineNo
);

/**
    Generates an enter event for the specified region. The region must be declared and
    initialized before.
    @param handle     The handle for this region. It must be defined before.
 */
void
SILC_User_RegionEnter
(
    const SILC_RegionHandle handle
);

/* **************************************************************************************
 * Parameter functions
 * *************************************************************************************/

/**
    Generates a parameter event for a parameter of 64 bit integer data type.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void
SILC_User_ParameterInt64
(
    const char* name,
    int64_t     value
);

/**
    Generates a parameter event for a parameter of string type.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void
SILC_User_ParameterString
(
    const char* name,
    char*       value
);

/* **************************************************************************************
 * User metric functions
 * *************************************************************************************/

/**
    Initilizes a user metric group. Every group must be initialized before it is used
    for the first time.
    @param groupHandle A variable where the handle for the new group i stored. It must
                       be declared with SILC_USER_METRIC_GROUP_DEF.
 */
void
SILC_User_InitMetricGroup
(
    SILC_CounterGroupHandle* groupHandle,
    const char*              name
);

/**
    Initializes a user metric. Every user metric must be registered before it is used
    the first time.
    @param metricHandle A handle which identify the user metric.
    @param unit        A string for the unit of the user metric.
    @param metricType Specifies the data type of the user metric. Possible are
                       SILC_USER_METRIC_TYPE_INT64 for 64 bit signed integer,
                       SILC_USER_METRIC_TYPE_UINT64 for 64 bit unsigned integer, and
                       SILC_USER_METRIC_TYPE_DOUBLE for doubles.
    @param context     Specifies for which context the metrices are obtained.
                       They can be obtained either for the global context or
                       for each callpath. For global context enter
                       SILC_USER_METRIC_CONTEXT_GLOBAL, else for measuring
                       per callpath enter SILC_USER_METRIC_CONTEXT_CALLPATH.
    @param group       The name of the user couter group to which this metric belongs.
                       If the group does not exist already, it will be created.
 */
void
SILC_User_InitMetric
(
    SILC_CounterHandle*           metricHandle,
    const char*                   name,
    const char*                   unit,
    const SILC_User_MetricType    metricType,
    const int8_t                  context,
    const SILC_CounterGroupHandle group
);

/**
    Triggers an user metric of type integer. Every user metric must be registered
    and initialized before it is used for the first time.
    @param metricHandle A handle which identify the user metric.
    @param value        The value for the metric.
 */
void
SILC_User_TriggerMetricInt64
(
    SILC_CounterHandle metricHandle,
    int64_t            value
);

/**
    Triggers an user metric of type integer. Every user metric must be registered
    and initialized before it is used for the first time.
    @param metricHandle A handle which identify the user metric.
    @param value        The value for the metric.
 */
void
SILC_User_TriggerMetricDouble
(
    SILC_CounterHandle metricHandle,
    double             value
);

/**
    Declaration of the variable for storing the default metric group handle.
 */
extern SILC_CounterGroupHandle SILC_User_DefaultMetricGroup;

/* **************************************************************************************
 * Virtual Topologies
 * *************************************************************************************/

/**
    Defines a two-dimentional cartesian topology.
    @param name     A string containing the name of the topology.
    @param numX     Number of processes in the x-dimension.
    @param numY     Number of processes in the y-dimension.
    @param periodX  A truth value, if the x-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodY  A truth value, if the y-dimension is periodical. A value of zero means
                    false, other values mean true.
    @return A handle to the newly created topology.
 */
int32_t
SILC_User_DefineTopology2D
(
    const char*   name,
    const int32_t numX,
    const int32_t numY,
    const int32_t periodX,
    const int32_t periodY
);

/**
    Defines a three-dimentional cartesian topology.
    @param name     A string containing the name of the topology.
    @param numX     Number of processes in the x-dimension.
    @param numY     Number of processes in the y-dimension.
    @param numZ     Number of processes in the z-dimension.
    @param periodX  A truth value, if the x-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodY  A truth value, if the y-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodZ  A truth value, if the z-dimension is periodical. A value of zero means
                    false, other values mean true.
    @return A handle to the newly created topology.
 */
int32_t
SILC_User_DefineTopology3D
(
    const char*   name,
    const int32_t numX,
    const int32_t numY,
    const int32_t numZ,
    const int32_t periodX,
    const int32_t periodY,
    const int32_t periodZ
);

/**
    Defines the a coordinate in a two-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
 */
void
SILC_User_DefineCoordinate2D
(
    const int32_t topId,
    const int32_t coordX,
    const int32_t coordY
);

/**
    Defines the a coordinate in a three-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
    @param coordZ   Z-coordinate
 */
void
SILC_User_DefineCoordinate3D
(
    const int32_t topId,
    const int32_t coordX,
    const int32_t coordY,
    const int32_t coordZ
);

/* **************************************************************************************
 * Control functions
 * *************************************************************************************/

/**
    Enables recording of events. If already enabled, this command has no effect.
    The control is not restricted to events from the user adapter, but enables the
    recording of all events.
 */
void
SILC_User_EnableRecording();

/**
    Disables recording of events. If already disabled, this command has no effect.
    The control is not restricted to events from the user adapter, but disables the
    recording of all events.
 */
void
SILC_User_DisableRecording();

/**
    Checks if the recording is enabled.
    @returns false if the recording of events is disabled, else it returns true.
 */
bool
SILC_User_RecordingEnabled();

#ifdef __cplusplus
} /* extern "C" */

/* **************************************************************************************
 * Class SILC_User_Region
 * *************************************************************************************/

/**
    @class SILC_User_Region
    This class implements the SILC_USER_REGION statement. Its constructor and destructor
    generates the enter and respectively the exit event for the instrumented function.
 */
class SILC_User_Region
{
public:
/**
    Generates an enter event for the specified region. It should not be inserted by the
    user directly. The user should use the SILC_USER_REGION(name) statement instead.
    @param regionName The name of the region.
    @param regionType The type of the region.
    @param file       The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
    SILC_User_Region( const char*                regionName,
                      const SILC_User_RegionType regionType,
                      const char*                file,
                      const uint32_t             lineNo )
    {
        SILC_User_RegionBegin( &region_handle, &SILC_User_LastFileName,
                               &SILC_User_LastFileHandle, regionName, regionType,
                               file, lineNo );
    }

    /**
        Generates an exit event for the instrumented region
     */
    ~
    SILC_User_Region()
    {
        SILC_User_RegionEnd( region_handle );
    }

private:
    /**
        Stores the region handle
     */
    SILC_RegionHandle region_handle;
};
#endif /* __cplusplus */

/** @} */

#endif /* SILC_USER_FUNCTIONS_H */
