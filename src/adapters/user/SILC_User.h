#ifndef SILC_USER_H
#define SILC_USER_H

#ifdef SILC_USER_ENABLE
#include "SILC_User_Types.h"
#include "SILC_User_Functions.h"


/* **************************************************************************************
 * Region enclosing macros
 * *************************************************************************************/

/** @def SILC_USER_REGION_BEGIN(name)
    This macro marks the start of a user defined region.
    @param name  A string which contains the name of the region.
                 The name should be unique.
                 It is used to identify the region. Regions
                 with the same name may not be distinguished correctly.
 */
#define SILC_USER_REGION_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_REGION, __FILE__, __LINE__ );

/** @def SILC_USER_REGION_END(name)
    This macro marks the end of a user defined region.
    @param name  A string which contains the name of the region.
                 It must be the same name which is used as the start of the region.
 */
#define SILC_USER_REGION_END( name ) SILC_User_RegionEnd( name, __FILE__, \
                                                          __LINE__ );

/** @def SILC_USER_FUNC_BEGIN
    This macro marks the start of a function. It should be inserted at the beginning
    of the instrumented function. It will generate a region, with the function
    name.
 */
#define SILC_USER_FUNC_BEGIN SILC_User_RegionBegin( \
        __func__, SILC_USER_REGION_TYPE_FUNCTION, __FILE__, __LINE__ );

/** @def SILC_USER_FUNC_END
    This macro marks the end of a function. It should be inserted at the end
    of the instrumented function.
 */
#define SILC_USER_FUNC_END SILC_User_RegionEnd( __func__, __FILE__, __LINE__ );

/** @def SILC_USER_LOOP_BEGIN(name)
    This macro marks the start of the body of a loop with a fixed number of iterations.
    It creates a user region. The knowledge wether it is a loop with the same fixed
    number of iterations in each process allow more effective implementation.
    @param name  A string which contains the name of the region.
                 The name should be unique.
                 It is used to identify the region. Regions
                 with the same name may not be distinguished correctly.
 */
#define SILC_USER_LOOP_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_LOOP, __FILE__, __LINE__ );

/** @def SILC_USER_LOOP_END(name)
    This macro marks the end of a user defined loop region.
    @param name  A string which contains the name of the loop region.
                 It must be the same name which is used as the start of the region.
 */
#define SILC_USER_LOOP_END( name ) SILC_User_RegionEnd( name, __FILE__, \
                                                        __LINE__ );

/** @def SILC_USER_PHASE_BEGIN(name)
    This macro marks the start of a phase. If a phase has multiple start and end points
    every start point of the phase must use the same name.
    It is implemented via regions, thus, it creates a user region with a
    special type phase. From the different regions the phase can be reconstructed.
    At any time in the program there is only one active phase, which is the one which
    started last. If no phase is defined exlicitly, the default phase is active, which is
    implicitly started at program start.
    @param name  A string which contains the name of the phase.
                 If the phase consists of multiple blocks, every block of the phase
                 must use the same name.
 */
#define SILC_USER_PHASE_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_PHASE, __FILE__, __LINE__ );

/** @def SILC_USER_PHASE_END(name)
    This macro marks the end of a phase. The new active phase is the phase that was
    active, before this phase started.
    @param name  A string which contains the he name of the loop region.
                 It must be the same name which is used as the start of the region.
 */
#define SILC_USER_PHASE_END( name ) SILC_User_RegionEnd( name, __FILE__, \
                                                         __LINE__ );

/** @def SILC_USER_DYNAMIC_REGION_BEGIN(name)
    This macro marks the start of a dynamic user defined region.
    See also the description of SILC_REGION_BEGIN(name). The end of the dynamic region
    is marked by a SILC_REGION_END(name) statement.
    @param name  A string which contains the he name of the region.
                The name should be unique.
                It is used to identify the region. Regions
                with the same name may not be distinguished correctly.
 */
#define SILC_USER_DYNAMIC_REGION_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_REGION | SILC_USER_REGION_TYPE_DYNAMIC, \
        __FILE__, \
        __LINE__ );

/** @def SILC_USER_DYNAMIC_FUNCTION_BEGIN
    This macro marks the start of a dynamic region with the name of the current function.
    See also the description of SILC_FUNC_BEGIN. The end of the dynamic region
    is marked by a SILC_FUNC_END statement.
 */
#define SILC_USER_DYNAMIY_FUNCTION_BEGIN SILC_User_RegionBegin( \
        __func__, SILC_USER_REGION_TYPE_FUNCTION | \
        SILC_USER_REGION_TYPE_DYNAMIC, \
        __FILE__, __LINE__ );

/** @def SILC_USER_DYNAMIC_LOOP_BEGIN(name)
    This macro marks the start of a dynamic user defined region inside a loop with the
    same fixed number of iterations on every process.
    See also the description of SILC_LOOP_BEGIN(name). The end of the dynamic region
    is marked by a SILC_LOOP_END(name) statement.
    @param name  A string which contains the name of the region.
                The name should be unique.
                It is used to identify the region. Regions
                with the same name may not be distinguished correctly.
 */
#define SILC_USER_DYNAMIC_LOOP_BEGIN( name ) SILC_User_RegionBegin( name, 13, \
                                                                    __FILE__, \
                                                                    __LINE__ );

/** @def SILC_USER_DYNAMIC_LOOP_BEGIN(name)
    This macro marks the start of a dynamic phase.
    See also the description of @SILC_PHASE_BEGIN(name). The end of the phase
    is marked by a @SILC_PHASE_END(name) statement.
    @param name  A string which contains the name of the phase.
 */
#define SILC_USER_DYNAMIC_PHASE_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_PHASE | SILC_USER_REGION_TYPE_DYNAMIC, \
        __FILE__, \
        __LINE__ );

/** @def SILC_USER_LOOP_PHASE_BEGIN(name)
    This macro marks the start of a phase which contains only of a block inside a loop
    with the same fixed number of iterations on every process.
    See also the description of SILC_LOOP_BEGIN(name) and @SILC_PHASE_BEGIN(name).
    The end of the dynamic region
    is marked by a SILC_PHASE_END(name) statement.
    @param name  A string which contains the name of the region.
                The name should be unique.
                It is used to identify the region. Regions
                with the same name may not be distinguished correctly.
 */
#define SILC_USER_LOOP_PHASE_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_PHASE | SILC_USER_REGION_TYPE_LOOP, \
        __FILE__, \
        __LINE__ );

/** @def SILC_USER_DYNAMIC_LOOP_PHASE_BEGIN(name)
    This macro marks the start of a dynamic phase which contains only of a block
    inside a loop
    with the same fixed number of iterations on every process.
    See also the description of SILC_LOOP_BEGIN(name) and @SILC_PHASE_BEGIN(name).
    The end of the dynamic region
    is marked by a SILC_PHASE_END(name) statement.
    @param name  A string which contains the name of the region.
                The name should be unique.
                It is used to identify the region. Regions
                with the same name may not be distinguished correctly.
 */
#define SILC_USER_DYNAMIC_LOOP_PHASE_BEGIN( name ) SILC_User_RegionBegin( \
        name, SILC_USER_REGION_TYPE_PHASE | SILC_USER_REGION_TYPE_LOOP | \
        SILC_USER_REGION_TYPE_DYNAMIC, __FILE__, __LINE__ );


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
 */
#define SILC_USER_PARAMETER_INT64( name, value )  SILC_User_ParameterInt64( \
        name, value );

/** @def SILC_USER_PARAMETER_String(name,value)
    This statement adds a string type parameter for parameter-based
    profiling to the current region.
    The calltree for the region is split according to the different values of the
    parameters with the same name.
    It is possible to add an arbitrary number of parameters to a region. Each parameter
    must have a unique name. However, it is not recommended to use more than 1 parameter
    per region.
    During one visit it is not allowed to use the same name twice
    for two different parameters.
 */
#define SILC_USER_PARAMETER_STRING( name, value ) SILC_User_ParameterString( \
        name, value );


/* **************************************************************************************
 * User Counter macros
 * *************************************************************************************/

/** @def SILC_USER_COUNTER_DEF(name, unit, type, context)
    Defines a new user counter. Each counter must be defined before it is triggered the
    first time.
    @param name A string containing a unique name for the counter.
    @param unit A string containing a the unit of the data.
    @param type Specifies the data type of the counter. It must be one of the following:
                SILC_USER_COUNTER_TYPE_INT64, SILC_USER_COUNTER_TYPE_UINT64,
                SILC_USER_COUNTER_TYPE_DOUBLE, SILC_USER_COUNTER_TYPE_FLOAT.
    @param context Specifies the context for which the counter is measured. IT must be
                one of the following: SILC_USER_COUNTER_CONTEXT_GLOBAL, or
                SILC_USER_COUNTER_CONTEXT_CALLPATH.
    @param group A string containing the name of the group to which this counter belongs.
                If this group does not exist, it will be created.
 */
#define SILC_USER_COUNTER_DEF( name, unit, type, context, \
                               group ) Silc_UserCounterDef( name, unit, type, \
                                                            context, \
                                                            group );

/** @def SILC_USER_COUNTER_INT64(name,value)
    Triggers a new event for a user counter of a 64 bit integer data type. Each user
    counter must be defined with SILC_USER_COUNTER_DEF before it is triggered for the
    first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a 64 bit integer.
 */
#define SILC_USER_COUNTER_INT64( name, value )  SILC_User_TriggerCounterInt64( \
        name, value );

/** @def SILC_USER_COUNTER_UINT64(name,value)
    Triggers a new event for a user counter of a 64 bit unsigned  integer data type.
    Each user counter must be defined with SILC_USER_COUNTER_DEF before it is triggered
    for the first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a 64 bit unsigned integer.
 */
#define SILC_USER_COUNTER_UINT64( name, value )  SILC_User_TriggerCounterUint64( \
        name, value );

/** @def SILC_USER_COUNTER_DOUBLE(name,value)
    Triggers a new event for a user counter of a double precision floating point data type.
    Each user counter must be defined with SILC_USER_COUNTER_DEF before it is triggered
    for the first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a double.
 */
#define SILC_USER_COUNTER_DOUBLE( name, value ) SILC_User_TriggerCounterDouble( \
        name, value );

/** @def SILC_USER_COUNTER_FLOAT(name,value)
    Triggers a new event for a user counter of a sibgle precision floating point data type.
    Each user counter must be defined with SILC_USER_COUNTER_DEF before it is triggered
    for the first time.
    @param name A string containing the name of the counter. It must be the same name
                that is used during definition of the counter.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a float.
 */
#define SILC_USER_COUNTER_FLOAT( name, value )  SILC_User_TriggerCounterFloat( \
        name, value );

/* **************************************************************************************
 * C++ specific macros
 * *************************************************************************************/
#ifdef __cplusplus

/** @def SILC_USER_TRACER(name)
    Instruments a codeblock as a region with the given name. It inserts a local variable
    of the type class SILC_User_Tracer. Its constructor generates the enter event and
    its destructor generates the exit event. Thus, only one statement is necessary to
    instrument the code block. This statement is only in C++ available.
 */
#define SILC_USER_TRACER( name ) SILC_User_Tracer SILC_User_TracerInst( \
        name, SILC_USER_REGION_TYPE_FUNCTION, __FILE__, __LINE__ );

#endif // __cplusplus

/* **************************************************************************************
 * Control macros
 * *************************************************************************************/
extern uint8_t SILC_User_IsOn;

/** @def SILC_USER_ON
    Enables the measurement. If already enabled, this command has no effect.
 */
#define SILC_USER_ON SILC_User_IsOn = 0xf;

/** @def SILC_USER_OFF
    Disables the measurement. If already disabled, this command has no effect.
 */
#define SILC_USER_OFF SILC_User_IsOn = 0;

/** @def SILC_USER_IS_ON
    Returns zero if the measurement is disabled, else it returns a nonzero value.
 */
#define SILC_USER_IS_ON SILC_User_IsOn

#else // SILC_USER_ENABLE

/* **************************************************************************************
 * Marker
 * *************************************************************************************/

/** @def SILC_USER_MARKER_DEF
    Defines a marker.
    @param name A string containing the name of the marker.
    @param type Defines the type of the marker. The following types are possible:
                SILC_USER_MARKER_TYPE_ERROR, SILC_USER_MARKER_TYPE_WARNING,
                SILC_USER_MARKER_TYPE_HINT.
 */
# define SILC_USER_MARKER_DEF( name, type ) SILC_User_MarkerDef( name, type );

/** @def SILC_USER_MARKER
    Triggers a marker. The marker must be defined before it is triggered for the first
    time.
    @param name A string containing the name of the marker.
    @param text A string containing a text.
 */
# define SILC_USER_MARKER( name, text )     SILC_User_Marker( name, text );


/* **************************************************************************************
 * Empty macros, if user instrumentation is disabled
 * *************************************************************************************/
#define SILC_USER_REGION_BEGIN( name )
#define SILC_USER_REGION_END( name )
#define SILC_USER_FUNC_BEGIN
#define SILC_USER_FUNC_END
#define SILC_USER_LOOP_BEGIN( name )
#define SILC_USER_LOOP_END( name )
#define SILC_USER_PHASE_BEGIN( name )
#define SILC_USER_PHASE_END( name )
#define SILC_USER_DYNAMIC_REGION_BEGIN( name )
#define SILC_USER_DYNAMIY_FUNCTION_BEGIN
#define SILC_USER_DYNAMIC_LOOP_BEGIN( name )
#define SILC_USER_DYNAMIC_PHASE_BEGIN( name )
#define SILC_USER_LOOP_PHASE_BEGIN( name )
#define SILC_USER_DYNAMIC_LOOP_PHASE_BEGIN( name )
#define SILC_USER_PARAMETER_INT64( name, value )
#define SILC_USER_PARAMETER_STRING( name, value )
#define SILC_USER_COUNTER_DEF( name, unit, type, context, group )
#define SILC_USER_COUNTER_INT64( name, value )
#define SILC_USER_COUNTER_UINT64( name, value )
#define SILC_USER_COUNTER_DOUBLE( name, value )
#define SILC_USER_COUNTER_FLOAT( name, value )
#define SILC_USER_TRACER( name )
#define SILC_USER_ON
#define SILC_USER_OFF
#define SILC_USER_IS_ON
#define SILC_USER_MARKER_DEF( name, type )
#define SILC_USER_MARKER( name, text )

#endif // SILC_USER_ENABLE

#endif // SILC_USER_H
