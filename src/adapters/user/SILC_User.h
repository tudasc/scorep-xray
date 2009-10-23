/** @file SILC_User.h
    This file conatins the macros for user manual instrumentation
 */

#ifndef SILC_USER_H
#define SILC_USER_H

#ifdef SILC_USER_ENABLE
#include "SILC_User_Types.h"
#include "SILC_API_Types.h"
#include "SILC_User_Functions.h"
#include "SILC_API_RuntimeManagement.h"

/* **************************************************************************************
 * Region enclosing macros
 * *************************************************************************************/

/** @def SILC_USER_REGION_DEFINE(handle)
    This macro defines a region. Every region has to be defined before it is first
    entered. The defines have to take place at the beginning of the enclosing block.
    @param handle  A variable name for a parameter must be provided. This variable is
                   declared in this macro. This handle is used in the
                   SILC_USER_REGION_BEGIN and SILC_USER_REGION_END statements to specify
                   which region is started, or ended.

    Example:
    @code
    void myfunc()
    {
      SILC_USER_REGION_DEFINE( my_region_handle )

      // do something

      SILC_USER_REGION_BEGIN( my_region_handle, "my_region",SILC_USER_REGION_TYPE_COMMON )

      // do something

      SILC_USER_REGION_END( my_region_handle )
    }
    @endcode
 */
#define SILC_USER_REGION_DEFINE( handle ) \
    static SILC_API_RegionHandle handle = SILC_USER_REGION_UNINITIALIZED;

/** @def SILC_USER_REGION_BEGIN(name)
    This macro marks the start of a user defined region. The SILC_USER_REGION_BEGIN and
    SILC_USER_REGION_END calls of all regions must be correctly nested.
    @param handle  The handle of the region to be started. This handle must be declared
                   using SILC_USER_REGION_DEFINE or SILC_GLOBAL_REGION_DEFINE before.
    @param name    A string containing the name of the new region. The name should be
                   unique.
    @param type    Specifies the type of the region. Possible values are
                   SILC_USER_REGION_TYPE_COMMON,
                   SILC_USER_REGION_TYPE_FUNCTION, SILC_USER_REGION_TYPE_LOOP,
                   SILC_USER_REGION_TYPE_DYNAMIC, SILC_USER_REGION_TYPE_PHASE, or a
                   combination of them.

    Example:
    @code
    void myfunc()
    {
      SILC_USER_REGION_DEFINE( my_region_handle )

      // do something

      SILC_USER_REGION_BEGIN( my_region_handle, "my_region",SILC_USER_REGION_TYPE_COMMON )

      // do something

      SILC_USER_REGION_END( my_region_handle )
    }
    @endcode
 */
#define SILC_USER_REGION_BEGIN( handle, name, type ) SILC_User_RegionBegin( \
        handle, name, type, __FILE__, __LINE__ );

/** @def SILC_USER_REGION_END(handle)
    This macro marks the end of a user defined region.  The SILC_USER_REGION_BEGIN and
    SILC_USER_REGION_END calls of all regions must be correctly nested.
    @param name  The handle of the region which ended here.
                 It must be the same handle which is used as the start of the region.

    Example:
    @code
    void myfunc()
    {
      SILC_USER_REGION_DEFINE( my_region_handle )

      // do something

      SILC_USER_REGION_BEGIN( my_region_handle, "my_region",SILC_USER_REGION_TYPE_COMMON )

      // do something

      SILC_USER_REGION_END( my_region_handle )
    }
    @endcode
 */
#define SILC_USER_REGION_END( handle ) SILC_User_RegionEnd( handle, __FILE__, \
                                                            __LINE__ );

/** @def SILC_USER_FUNC_BEGIN
    This macro marks the start of a function. It should be inserted at the beginning
    of the instrumented function. It will generate a region, with the function
    name.

    Example:
    @code
    void myfunc()
    {
      // declarations

      SILC_USER_FUNC_BEGIN

      // do something

      SILC_USER_FUNC_END
    }
    @endcode
 */
#define SILC_USER_FUNC_BEGIN static const SILC_API_RegionHandle \
    silc_user_func_handle = \
        SILC_USER_REGION_UNINITIALIZED; \
    SILC_User_RegionBegin( silc_user_func_handle, __func__, \
                           SILC_USER_REGION_TYPE_FUNCTION, __FILE__, __LINE__ );

/** @def SILC_USER_FUNC_END
    This macro marks the end of a function. It should be inserted at the end
    of the instrumented function.

    Example:
    @code
    void myfunc()
    {
      // declarations

      SILC_USER_FUNC_BEGIN

      // do something

      SILC_USER_FUNC_END
    }
    @endcode
 */
#define SILC_USER_FUNC_END SILC_User_RegionEnd( silc_user_func_handle, __FILE__, \
                                                __LINE__ );

/** @def SILC_GLOBAL_REGION_DEFINE( handle )
    This macro declares a region handle in a global scope for usage in more than one code
    block in more than one source file. Every global region must only declared once using
    this macro. All other files in which this region handle is accessed must declare the
    region handle with SILC_GLOBAL_REGION_EXTERNAL( handle ). A global region has no
    special associated source region with it. Enter and exit events for global regions
    are created with SILC_USER_REGION_BEGIN and SILC_USER_REGION_END, respectively.
    Its name and type is determined at the first enter event and is not changed on later
    events, even if other code blocks conatain a different name or type in their
    SILC_USER_REGION_BEGIN statement.
    @param handle  A name for a parameter must be provided. This parameter is declared in
                   the macro. This handle is used in the SILC_USER_REGION_BEGIN and
                   SILC_USER_REGION_END statements to specify which region is started,
                   or ended.

    Example:
    @code
    // In File1:
    SILC_GLOBAL_REGION_DEFINE( my_global_handle )

    void myfunc()
    {
      SILC_USER_REGION_BEGIN( my_global_handle, "my_global", SILC_USER_REGION_PHASE )

      // do something

      SILC_USER_REGION_END( my_global_handle )
    }
    @endcode
    @code
    // In File2:
    SILC_GLOBAL_EXTERNAL( my_global_handle )

    void foo()
    {
      SILC_USER_REGION_BEGIN( my_global_handle, "my_global", SILC_USER_REGION_PHASE )

      // do something

      SILC_USER_REGION_END( my_global_handle )
    }
    @endcode
 */
#define SILC_GLOBAL_REGION_DEFINE( handle ) \
    SILC_API_RegionHandle handle = SILC_USER_REGION_GLOBAL;

/** @def SILC_GLOBAL_REGION_EXTERNAL( handle )
    This macro declares an axternally defined global region.  Every global region must
    only declared once usingSILC_GLOBAL_REGION_DEFINE( handle ). All other files in
    which this region handle is accessed must declare the region handle with
    SILC_GLOBAL_REGION_EXTERNAL( handle ).  A global region has no
    special associated source region with it. Enter and exit events for global regions
    are created with SILC_USER_REGION_BEGIN and SILC_USER_REGION_END, respectively.
    Its name and type is determined at the first enter event and is not changed on later
    events, even if other code blocks conatain a different name or type in their
    SILC_USER_REGION_BEGIN statement.
    @param handle  A name for a variable must be provided. This variable name must be
                   the same like for the corresponding SILC_GLOBAL_REGION_DEFINE
                   statement. The handle is used in the SILC_USER_REGION_BEGIN and
                   SILC_USER_REGION_END statements to specify which region is started,
                   or ended.

    Example:
    @code
    // In File 1
    SILC_GLOBAL_REGION_DEFINE( my_global_handle )

    void myfunc()
    {
      SILC_USER_REGION_BEGIN( my_global_handle, "my_global", SILC_USER_REGION_PHASE )

      // do something

      SILC_USER_REGION_END( my_global_handle )
    }
    @endcode
    @code
    // In File 2
    SILC_GLOBAL_EXTERNAL( my_global_handle )

    void foo()
    {
      SILC_USER_REGION_BEGIN( my_global_handle, "my_global", SILC_USER_REGION_PHASE )

      // do something

      SILC_USER_REGION_END( my_global_handle )
    }
    @endcode
 */
#define SILC_GLOBAL_REGION_EXTERNAL( handle ) \
    extern SILC_API_RegionHandle handle;

/* **************************************************************************************
 * Parameter macros
 * *************************************************************************************/

/** @def SILC_USER_PARAMETER_INT64(name,value)
    This statement adds a 64 bit signed integer type parameter for parameter-based
    profiling to the current region.
    The calltree for the region is split according to the different values of the
    parameters with the same name.
    It is possible to add an arbitrary number of parameters to a region. Each parameter
    must have a unique name. However, it is not recommended to use more than 1 parameter
    per region.
    During one visit it is not allowed to use the same name twice
    for two different parameters.
    @param name   A string containing the name of the parameter.
    @param value  The value of the parameter. It must be possible for implicit casts to
                  cast it to a 64 bit integer.

    Example:
    @code
    void myfunc(int64 myint)
    {
      SILC_USER_REGION_DEFINE( my_region_handle )
      SILC_USER_REGION_BEGIN( my_region_handle, "my_region",SILC_USER_REGION_TYPE_COMMON )
      SILC_USER_PARAMETER_INT64("A nice int",myint)

      // do something

      SILC_USER_REGION_END( my_region_handle )
    }
    @endcode
 */
#define SILC_USER_PARAMETER_INT64( name, value )  SILC_User_ParameterInt64( \
        name, value );

/** @def SILC_USER_PARAMETER_STRING(name,value)
    This statement adds a string type parameter for parameter-based
    profiling to the current region.
    The calltree for the region is split according to the different values of the
    parameters with the same name.
    It is possible to add an arbitrary number of parameters to a region. Each parameter
    must have a unique name. However, it is not recommended to use more than 1 parameter
    per region.
    During one visit it is not allowed to use the same name twice
    for two different parameters.

    Example:
    @code
    void myfunc(char *mystring)
    {
      SILC_USER_REGION_DEFINE( my_region_handle )
      SILC_USER_REGION_BEGIN( my_region_handle, "my_region",SILC_USER_REGION_TYPE_COMMON )
      SILC_USER_PARAMETER_STRING("A nice string",mystring)

      // do something

      SILC_USER_REGION_END( my_region_handle )
    }
    @endcode
 */
#define SILC_USER_PARAMETER_STRING( name, value ) SILC_User_ParameterString( \
        name, value );


/* **************************************************************************************
 * User Counter macros
 * *************************************************************************************/

/** @def  SILC_USER_METRIC_GROUP_LOCAL(groupHandle)
    Declares a metric group.
    It defines a variable which must be in scope at all places where
    the metric group is used. If it is used in more than one place it need to be a global
    definition. Its scopy can never be beyond one source file. If a larger scope is
    needed use SILC_USER_METRIC_GROUP_GLOBAL.
    @param groupHandle The name of the variable which contains the groupHandle.

    Example:
    @code
    SILC_USER_METRIC_GROUP_LOCAL( my_local_group )
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_GROUP_INIT( my_local_group )
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, my_local_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_GROUP_LOCAL( groupHandle ) static \
    SILC_API_CounterGroupHandle groupHandle  \
        = SILC_API_INVALID_COUNTER_GROUP;

/** @def SILC_USER_METRIC_GROUP_GLOBAL( groupHandle)
    Declares a group handle for user metrics for usage in more than one file. Every global
    group handle must only be declared in one file using SILC_USER_METRIC_GROUP_GLOBAL.
    All other files in which the same group handle is  accessed, must declare the handle
    using SILC_USER_METRIC_GROUP_EXTERNAL.
    @param groupHandle The variable name for the variable declared in this statement.

    Example:
    @code
    // In File 1
    SILC_USER_METRIC_GROUP_GLOBAL( my_global_group )
    SILC_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SILC_USER_METRIC_GROUP_INIT( my_global_group )
      SILC_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SILC_USER_METRIC_GROUP_EXTERNAL( my_global_group )
    SILC_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      double my_double = get_some_double();
      uint64 my_int = get_some_int();
      SILC_USER_METRIC_LOCAL( my_local_metric )
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "m", \
                             SILC_USER_METRIC_TYPE_DOUBLE, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      SILC_USER_METRIC_DOUBLE( my_local_metric, my_double )
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_GROUP_GLOBAL( groupHandle )  SILC_API_CounterGroupHandle \
    groupHandle = SILC_API_INVALID_COUNTER_GROUP;

/** @def SILC_USER_METRIC_GROUP_EXTERNAL( groupHandle )
    Declares an external group handle. Every global
    group handle must only be declared in one file using SILC_USER_METRIC_GROUP_GLOBAL.
    All other files in which the same group handle is  accessed, must declare the handle
    using SILC_USER_METRIC_GROUP_EXTERNAL.
    @param groupHandle The variable name for the variable declared in this statement.

    Example:
    @code
    // In File 1
    SILC_USER_METRIC_GROUP_GLOBAL( my_global_group )
    SILC_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SILC_USER_METRIC_GROUP_INIT( my_global_group )
      SILC_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SILC_USER_METRIC_GROUP_EXTERNAL( my_global_group )
    SILC_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      double my_double = get_some_double();
      uint64 my_int = get_some_int();
      SILC_USER_METRIC_LOCAL( my_local_metric )
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "m", \
                             SILC_USER_METRIC_TYPE_DOUBLE, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      SILC_USER_METRIC_DOUBLE( my_local_metric, my_double )
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_GROUP_EXTERNAL( groupHandle ) \
    extern SILC_API_CounterGroupHandle groupHandle;

/** @def SILC_USER_METRIC_GROUP_INIT( groupHandle, name )
    Initializes a metric group. Each group need to be initialized before it is used for
    the first time.
    @param groupHandle The handle for the initilaized group. It must be declared using
                       SILC_USER_METRIC_GROUP_DEF.
    @param name        A string containing a unique name for that metric group.

    Example:
    @code
    SILC_USER_METRIC_GROUP_LOCAL( my_local_group )
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_GROUP_INIT( my_local_group )
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, my_local_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_GROUP_INIT( groupHandle, \
                                     name ) SILC_User_MetricGroupInit( \
        groupHandle, name );

/** @def SILC_USER_METRIC_LOCAL(metricHandle)
    Declares a handle for a user metric.
    It defines a variable which must be in scope at all places where
    the metric is used. If it is used in more than one place it need to be a global
    definition.
    @param metricHandle The name of the variable which will be declared for storing
                        the meric handle.

    Example:
    @code
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_LOCAL( metricHandle ) static SILC_User_MetricHandle \
    metricHandle                                                                \
        = SILC_API_INVALID_COUNTER;

/** @def SILC_USER_METRIC_GLOBAL( metricHandle )
    Declares a handle for a user metric as a global variable. It must be used
    if a metric handle is accessed in more than one file. Every
    global metric must be declared only in one file using SILC_USER_METRIC_GLOBAL. All
    other files in which this handle is accessed must declare it with
    SILC_USER_METRIC_EXTERNAL.
    @param metricHandle The variable name for the handle.

    Example:
    @code
    // In File 1
    SILC_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SILC_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SILC_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_GLOBAL( metricHandle ) SILC_User_MetricHandle metricHandle \
        = SILC_API_INVALID_COUNTER;

/** @def SILC_USER_METRIC_EXTERNAL( metricHandle )
    Declares an externally defined handle for a user metric. Every
    global metric must be declared only in one file using SILC_USER_METRIC_GLOBAL. All
    other files in which this handle is accessed must declare it with
    SILC_USER_METRIC_EXTERNAL.
    @param metricHandle The variable name of the handle. it must be the same name as used
                        in the corresponding SILC_USER_METRIC_GLOBAL statement.

    Example:
    @code
    // In File 1
    SILC_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SILC_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SILC_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_EXTERNAL( metricHandle ) \
    extern SILC_User_MetricHandle metricHandle;

/** @def SILC_USER_METRIC_INIT(metricHandle,name, unit, type, context,groupHandle)
    Initializes a new user counter. Each counter must be initialized before it is
    triggered the first time. The handle must be declared using SILC_USER_METRIC_LOCAL,
    SILC_USER_METRIC_GLOBAL, or SILC_USER_METRIC_EXTERNAL.
    @param metricHandle Provides a variable name of the variable to store the metric
                handle. The variable is declared by the macro.
    @param name A string containing a unique name for the counter.
    @param unit A string containing a the unit of the data.
    @param type Specifies the data type of the counter. It must be one of the following:
                SILC_USER_METRIC_TYPE_INT64, SILC_USER_METRIC_TYPE_UINT64,
                SILC_USER_METRIC_TYPE_DOUBLE, SILC_USER_METRIC_TYPE_FLOAT.
    @param context Specifies the context for which the counter is measured. IT must be
                one of the following: SILC_USER_METRIC_CONTEXT_GLOBAL, or
                SILC_USER_METRIC_CONTEXT_CALLPATH.
    @param groupHandle A handle of the group to which this counter belongs.
                If this group does not exist, it will be created. If the default group
                should be used, specify SILC_USER_METRIC_GROUP_DEFAULT as group handle.

    Example:
    @code
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_INIT( metricHandle, name, unit, type, context, \
                               groupHandle ) \
    Silc_UserMetricInit( metricHandle, name, unit, type, context, groupHandle );

/** @def SILC_USER_METRIC_INT64(name,value)
    Triggers a new event for a user counter of a 64 bit integer data type. Each user
    counter must be defined with SILC_USER_COUNTER_DEF before it is triggered for the
    first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a 64 bit integer.

    Example:
    @code
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_INT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      int64 my_int = get_some_int_value();
      SILC_USER_METRIC_INT64( my_local_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_INT64( metricHandle, \
                                value )  SILC_User_TriggerMetricInt64( \
        metricHandle, value );

/** @def SILC_USER_METRIC_UINT64(name,value)
    Triggers a new event for a user counter of a 64 bit unsigned  integer data type.
    Each user counter must be defined with SILC_USER_COUNTER_DEF before it is triggered
    for the first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a 64 bit unsigned integer.

    Example:
    @code
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_UINT64, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SILC_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode
 */
#define SILC_USER_METRIC_UINT64( metricHandle, \
                                 value )  SILC_User_TriggerMetricUint64( \
        metricHandle, value );

/** @def SILC_USER_METRIC_DOUBLE(name,value)
    Triggers a new event for a user counter of a double precision floating point data type.
    Each user counter must be defined with SILC_USER_COUNTER_DEF before it is triggered
    for the first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a double.

    Example:
    @code
    SILC_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SILC_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SILC_USER_METRIC_TYPE_DOUBLE, \
                             SILC_USER_METRIC_CONTEXT_GLOBAL, \
                             SILC_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      double my_double = get_some_double_value();
      SILC_USER_METRIC_DOUBLE( my_local_metric, my_double )
    }
    @endcode
 */
#define SILC_USER_METRIC_DOUBLE( metricHandle, \
                                 value ) SILC_User_TriggerMetricDouble( \
        metricHandle, value );


/* **************************************************************************************
 * C++ specific macros
 * *************************************************************************************/
#ifdef __cplusplus

/** @def SILC_USER_REGION(name,type)
    Instruments a codeblock as a region with the given name. It inserts a local variable
    of the type class SILC_User_Region. Its constructor generates the enter event and
    its destructor generates the exit event. Thus, only one statement is necessary to
    instrument the code block. This statement is only in C++ available.
    @param name    A string containing the name of the new region. The name should be
                   unique.
    @param type    Specifies the type of the region. Possible values are
                   SILC_USER_REGION_TYPE_COMMON,
                   SILC_USER_REGION_TYPE_FUNCTION, SILC_USER_REGION_TYPE_LOOP,
                   SILC_USER_REGION_TYPE_DYNAMIC, SILC_USER_REGION_TYPE_PHASE, or a
                   combination of them.

    Example:
    @code
    void myfunc()
    {
      SILC_USER_REGION_( "myfunc", SILC_USER_REGION_TYPE_FUNCTION )

      // do something
    }
    @endcode
 */
#define SILC_USER_REGION( name, \
                          type ) static SILC_User_Region silc_user_region_inst( \
        name, type, __FILE__, __LINE__ );

#endif // __cplusplus

/* **************************************************************************************
 * Control macros
 * *************************************************************************************/

/** @def SILC_RECORDING_ON
    Enables the measurement. If already enabled, this command has no effect.

    Example:
    @code
    void foo()
    {
      SILC_RECORDING_OFF

      // do something

     SILC_RECORDING_ON
    }
    @endcode
 */
#define SILC_RECORDING_ON SILC_API_EnableRecording();

/** @def SILC_RECORDING_OFF
    Disables the measurement. If already disabled, this command has no effect.

    Example:
    @code
    void foo()
    {
      SILC_RECORDING_OFF

      // do something

     SILC_RECORDING_ON
    }
    @endcode
 */
#define SILC_RECORDING_OFF SILC_API_DisableRecording();

/** @def SILC_RECORDING_IS_ON
    Returns zero if the measurement is disabled, else it returns a nonzero value.

    Example:
    @code
    void foo()
    {
      if ( SILC_RECORDING_IS_ON )
      {
        // do something
      }
    }
    @endcode
 */
#define SILC_RECORDING_IS_ON SILC_API_RecordingEnabled()

/* **************************************************************************************
 * Virtual Topologies
 * *************************************************************************************/
/** @def SILC_DEFINE_CARTESIAN_2D
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
#define SILC_DEFINE_CARTESIAN_2D( name, numX, numY, periodX, periodY ) \
    SILC_User_DefineTopology2D( name, numX, numY, periodX, periodY )

/** @def SILC_DEFINE_CARTESIAN_3D
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
#define SILC_DEFINE_CARTESIAN_3D( name, numX, numY, numZ, periodX, periodY, \
                                  periodZ ) \
    SILC_User_DefineTopology3D( name, numX, numY, numZ, periodX, periodY, \
                                periodZ )

/** @def SILC_DEFINE_COORDINATE_2D
    Defines the a coordinate in a two-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
 */
#define SILC_DEFINE_COORDINATE_2D( topId, coordX, coordY ) \
    SILC_User_DefineCoordinates2D( topId, coordX, coordY );

/** @def SILC_DEFINE_COORDINATE_3D
    Defines the a coordinate in a three-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
    @param coordZ   Z-coordinate
 */
#define SILC_DEFINE_COORDINATE_3D( topId, coordX, coordY, coordZ ) \
    SILC_User_DefineCoordinates3D( topId, coordX, coordY, coordZ );

/* **************************************************************************************
 * Empty macros, if user instrumentation is disabled
 * *************************************************************************************/
#else // SILC_USER_ENABLE

#define SILC_USER_REGION( name, type )
#define SILC_USER_REGION_DEFINE( handle, name, type )
#define SILC_USER_REGION_BEGIN( handle )
#define SILC_USER_REGION_END( handle )
#define SILC_USER_FUNC_BEGIN
#define SILC_USER_FUNC_END
#define SILC_GLOBAL_REGION_DEFINE( handle )
#define SILC_GLOBAL_REGION_EXTERNAL ( handle )
#define SILC_USER_PARAMETER_INT64( name, value )
#define SILC_USER_PARAMETER_STRING( name, value )
#define SILC_USER_METRIC_GROUP_LOCAL( groupHandle )
#define SILC_USER_METRIC_GROUP_GLOBAL( groupHandle )
#define SILC_USER_METRIC_GROUP_EXTERNAL( groupHandle )
#define SILC_USER_METRIC_LOCAL( metricHandle )
#define SILC_USER_METRIC_GLOBAL( metricHandle )
#define SILC_USER_METRIC_EXTERNAL( metricHandle )
#define SILC_USER_METRIC_INIT( metricHandle, name, unit, type, context, \
                               groupHandle )
#define SILC_USER_METRIC_INT64( metricHandle, value )
#define SILC_USER_METRIC_UINT64( metricHandle, value )
#define SILC_USER_METRIC_DOUBLE( metricHandle, value )
#define SILC_USER_REGION( name, type )
#define SILC_RECORDING_ON
#define SILC_RECORDING_OFF
#define SILC_RECORDING_IS_ON
#define SILC_DEFINE_CARTESIAN_2D( name, numX, numY, periodX, periodY )
#define SILC_DEFINE_CARTESIAN_3D( name, numX, numY, numZ, periodX, periodY, \
                                  periodZ )
#define SILC_DEFINE_COORDINATE_2D( topId, coordX, coordY )
#define SILC_DEFINE_COORDINATE_3D( topId, coordX, coordY, coordZ )

#endif // SILC_USER_ENABLE

#endif // SILC_USER_H
