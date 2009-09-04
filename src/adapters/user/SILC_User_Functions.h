#ifndef SILC_USER_FUNCTIONS_H
#define SILC_USER_FUNCTIONS_H

/* **************************************************************************************
 * Region functions
 * *************************************************************************************/

/** Generates an enter event for the specified region. If the region was not registered
    before, it registers the region.
    @param name       The name of the region.
    @param regionType The type of the region.
    @param fileName   The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
void SILC_User_RegionBegin( const char*                name,
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
 * User counter functions
 * *************************************************************************************/

/** Defines a user counter. Every user counter must be registered before it is used the first time.
    @param name        A unique name to identify the user counter.
    @param unit        A string for the unit of the user counter.
    @param counterType Specifies the data type of the user counter. Possible are
                       SILC_USER_COUNTER_TYPE_INT64 for 64 bit signed integer,
                       SILC_USER_COUNTER_TYPE_UINT64 for 64 bit unsigned integer,
                       SILC_USER_COUNTER_TYPE_DOUBLE for doubles, and
                       SILC_USER_COUNTER_TYPE_FLOAT for floats.
    @param context     Specifies for which context the metrices are obtained.
                       They can be obtained either for the global context or
                       for each callpath. For global context enter
                       SILC_USER_COUNTER_CONTEXT_GLOBAL, else for measuring
                       per callpath enter SILC_USER_COUNTER_CONTEXT_CALLPATH.
    @param group       The name of the user couter group to which this counter belongs.
                       If the group does not exist already, it will be created.
 */
void SILC_User_DefineCounter( const char*                 name,
                              const char*                 unit,
                              const SILC_User_CounterType counterType,
                              const int8_t                context,
                              const char*                 group );

/** Triggers a user counter of type 64 bit signed integer. Before a counter can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user counter.
    @param value The value for the counter.
 */
void SILC_User_TriggerCounterInt64( const char* name,
                                    int64_t     value );

/** Triggers a user counter of type 64 bit unsigned integer. Before a counter can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user counter.
    @param value The value for the counter.
 */
void SILC_User_TriggerCounterUint64( const char* name,
                                     uint64_t    value );

/** Triggers a user counter of type double. Before a counter can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user counter.
    @param value The value for the counter.
 */
void SILC_User_TriggerCounterDouble( const char* name,
                                     double      value );

/** Triggers a user counter of type float. Before a counter can
    be triggered for the first time, it must be registered.
    @param name The unique name of the user counter.
    @param value The value for the counter.
 */
void SILC_User_TriggerCounterFloat( const char* name,
                                    float       value );


/* **************************************************************************************
 * Class SILC_User_Tracer
 * *************************************************************************************/
#ifdef __cplusplus

/** @class SILC_User_Tracer
    This class implements the SILC_USER_TRACER statement. Its constructor and destructor
    generates the enter and respectively the exit event for the instrumented function.
 */
class SILC_User_Tracer {
public:
/** Generates an enter event for the specified region. It should not be inserted by the
    user directly. The user should use the SILC_USER_TRACER(name) statement instead.
    @param regionName The name of the region.
    @param regionType The type of the region.
    @param file       The filename of the source file which contains the instrumented
                      region.
    @param lineNo     The line number of the first source code line of the instrumented
                      region.
 */
    SILC_User_Tracer( const char*                regionName,
                      const SILC_User_RegionType regionType,
                      const char*                file,
                      const uint32_t lineNo )
        : region_name( regionName ), file_name( file ), line_no( lineNo )
    {
        SILC_User_RegionStart( regionName, regionType, file, lineNo );
    }

/** Generates an exit event for the instrumented region
 */
    ~SILC_User_Tracer()
    {
        SILC_User_RegionEnd( region_name, file_name, line_no );
    }

private:
    const char* region_name;
    const char* file_name;
    const int   line_no;
};
#endif

#endif // SILC_USER_FUNCTIONS_H
