#ifndef SILC_USER_FUNCTIONS_H
#define SILC_USER_FUNCTIONS_H

#include "SILC_API_Types.h"

/* **************************************************************************************
 * Region functions
 * *************************************************************************************/

/** Generates an enter event for the specified region. If the region was not registered
    before, it registers the region. On the first enter, the region is registered to
    the measurement system.
    @param handle     The handle for this region. It must be defined before.
    @param name       The name of the region.
    @param regionType The type of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void SILC_User_RegionBegin( SILC_API_RegionHandle&     handle,
                            const char*                name,
                            const SILC_User_RegionType regionType,
                            const char*                fileName,
                            const uint32_t             lineNo );

/** Generates an exit event for the specified region.
    @param name       The name of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void SILC_User_RegionEnd( const char*    name,
                          const char*    fileName,
                          const uint32_t lineNo );

/* **************************************************************************************
 * Parameter functions
 * *************************************************************************************/

/** Generates a parameter event for a parameter of 64 bit integer data type.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void SILC_User_ParameterInt64( const char* name,
                               int64_t     value );

/** Generates a parameter event for a parameter of string type.
    @param name  The unique name for the paramater.
    @param value The value for the parameter.
 */
void SILC_User_ParameterString( const char* name,
                                char*       value );

/* **************************************************************************************
 * User metric functions
 * *************************************************************************************/

/** Initilizes a user metric group. Every group must be initialized before it is used
    for the first time.
    @param groupHandle A variable where the handle for the new group i stored. It must
                       be declared with SILC_USER_METRIC_GROUP_DEF.
 */
void SILC_User_InitMetricGroup( SILC_API_CounterGroupHandle& groupHandle,
                                const char*                  name );

/** Initializes a user metric. Every user metric must be registered before it is used
    the first time.
    @param name        A unique name to identify the user metric.
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
void SILC_User_InitMetric( SILC_API_CounterHandle&           metricHandle,
                           const char*                       name,
                           const char*                       unit,
                           const SILC_User_MetricType        metricType,
                           const int8_t                      context,
                           const SILC_API_CounterGroupHandle group );

/** Triggers a user metric of type 64 bit signed integer. Before a metric can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user metric.
    @param value The value for the metric.
 */
void SILC_User_TriggerMetricInt64( const char* name,
                                   int64_t     value );

/** Triggers a user metric of type 64 bit unsigned integer. Before a metric can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user metric.
    @param value The value for the metric.
 */
void SILC_User_TriggerMetricUint64( const char* name,
                                    uint64_t    value );

/** Triggers a user metric of type double. Before a metric can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user metric.
    @param value The value for the metric.
 */
void SILC_User_TriggerMetricDouble( const char* name,
                                    double      value );

/* **************************************************************************************
 * Virtual Topologies
 * *************************************************************************************/

/** Defines a two-dimentional cartesian topology.
    @param name     A string containing the name of the topology.
    @param numX     Number of processes in the x-dimension.
    @param numY     Number of processes in the y-dimension.
    @param periodX  A truth value, if the x-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodY  A truth value, if the y-dimension is periodical. A value of zero means
                    false, other values mean true.
    @return A handle to the newly created topology.
 */
int32_t SILC_User_DefineTopology2D( const char*   name,
                                    const int32_t numX,
                                    const int32_t numY,
                                    const int32_t periodX,
                                    const int32_t periodY );

/** Defines a three-dimentional cartesian topology.
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
int32_t SILC_User_DefineTopology2D( const char*     name,
                                    const int32_t   numX,
                                    const int32_t   numY,
                                    const int32_t   numZ,
                                    const int32_t   periodX,
                                    const int32_t   periodY,
                                    const - int32_t periodZ );

/** Defines the a coordinate in a two-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
 */
void SILC_User_DefineCoordinate2D( const int32_t topId,
                                   const int32_t coordX,
                                   const int32_t coordY );

/** Defines the a coordinate in a three-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
    @param coordZ   Z-coordinate
 */
void SILC_User_DefineCoordinate3D( const int32_t topId,
                                   const int32_t coordX,
                                   const int32_t coordY,
                                   const int32_t coordZ );


/* **************************************************************************************
 * Class SILC_User_Region
 * *************************************************************************************/
#ifdef __cplusplus

/** @class SILC_User_Region
    This class implements the SILC_USER_REGION statement. Its constructor and destructor
    generates the enter and respectively the exit event for the instrumented function.
 */
class SILC_User_Region {
public:
/** Generates an enter event for the specified region. It should not be inserted by the
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
                      const uint32_t lineNo )
        : file_name( file ), line_no( lineNo )
    {
        SILC_User_RegionBegin( region_handle, regionName, regionType, file,
                               lineNo );
    }

    /** Generates an exit event for the instrumented region
     */
    ~SILC_User_Region()
    {
        SILC_User_RegionEnd( region_handle, file_name, line_no );
    }

private:
    /** Stores the file name of the file which contains the instrumented code block */
    const char* file_name;

    /** Stores the line number where the instrumentationwas inserted */
    const int line_no;

    /** Stores the region handle */
    SILC_API_RegionHandle region_handle;
};
#endif

#endif // SILC_USER_FUNCTIONS_H
