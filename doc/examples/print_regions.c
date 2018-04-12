/**
 * This plugin prints entered and exited region per location
 * Additionally it prints when recording of a process had been enabled/disabled
 * The call order is:
 *
 * For all processes:
 * (1) SCOREP_SUBSTRATE_PLUGIN_ENTRY( PrintRegions )
 * (2) init(id)
 * (3) get_event_functions( SCOREP_SUBSTRATES_RECORDING_ENABLED )
 * (4) get_event_functions( SCOREP_SUBSTRATES_RECORDING_DISABLED )
 * (5) set_callbacks()
 * (optional) if user instrumentation used SCOREP_SetDefaultRecodingMode(false)
 *        disable()
 *
 * Until program finished
 * {
 *   (A) For every process when user instrumentation enables/disables recording
 *         enable()/disable()
 *   (B) For all locations, if recording enabled:
 *         enter()/exit()
 * }
 *
 * For all processes
 * (optional) if user instrumentation used SCOREP_SetDefaultRecodingMode(false)
 *        For every process:
 *        enable()
 * (N) finalize()
 */


#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <scorep/SCOREP_SubstratePlugins.h>

static const SCOREP_SubstratePluginCallbacks* callbacks;

static size_t plugin_id;

static void
print( const char*             event,
       struct SCOREP_Location* location,
       uint64_t                timestamp,
       SCOREP_RegionHandle     regionHandle,
       uint64_t                stackdepth )
{
    uint64_t    i;
    const char* slocation = callbacks->SCOREP_Location_GetName( location );
    const char* sregion   = callbacks->SCOREP_RegionHandle_GetName( regionHandle );

    printf( "%" PRIu64 ": %s >%" PRIu64 " %s %s\n", timestamp, slocation, stackdepth, event, sregion );
}

/* An enter event has been received from Score-P */
static void
enter_region(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues )
{
    uint64_t* stackdepth = callbacks->SCOREP_Location_GetData( location, plugin_id );
    print( "Enter", location, timestamp, regionHandle, *stackdepth );
    ( *stackdepth )++;
}

/* An exit event has been received from Score-P */
static void
exit_region(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues )
{
    uint64_t* stackdepth = callbacks->SCOREP_Location_GetData( location, plugin_id );
    ( *stackdepth )--;
    print( "Exit", location, timestamp, regionHandle, *stackdepth );
}

/* is only called when process is currently not parallel */
static void
disable(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues )
{
    const char* slocation = callbacks->SCOREP_Location_GetName( location );
    const char* sregion   = callbacks->SCOREP_RegionHandle_GetName( regionHandle );
    printf( "--------Disabled recording (%s - %s)------------\n",
            slocation,
            sregion );
}

/* is only called when process is currently not parallel */
static void
enable(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues )
{
    const char* slocation = callbacks->SCOREP_Location_GetName( location );
    const char* sregion   = callbacks->SCOREP_RegionHandle_GetName( regionHandle );
    printf( "--------Enabled recording (%s - %s)------------\n",
            slocation,
            sregion );
}

/* Register event functions */
static uint32_t
get_event_functions(
    SCOREP_Substrates_Mode       mode,
    SCOREP_Substrates_Callback** returned )
{
    SCOREP_Substrates_Callback* functions = calloc( SCOREP_SUBSTRATES_NUM_EVENTS,
                                                    sizeof( SCOREP_Substrates_Callback ) );

    /* Only print region events when recording is enabled */
    if ( mode == SCOREP_SUBSTRATES_RECORDING_ENABLED )
    {
        functions[ SCOREP_EVENT_ENTER_REGION ] = ( SCOREP_Substrates_Callback )enter_region;
        functions[ SCOREP_EVENT_EXIT_REGION ]  = ( SCOREP_Substrates_Callback )exit_region;
        /* function prototypes for other events can be found in scorep/SCOREP_SubstrateEvents.h */

        /* enable and disable are guaranteed to be called for SCOREP_SUBSTRATES_RECORDING_ENABLED */
        functions[ SCOREP_EVENT_ENABLE_RECORDING ]  = ( SCOREP_Substrates_Callback )enable;
        functions[ SCOREP_EVENT_DISABLE_RECORDING ] = ( SCOREP_Substrates_Callback )disable;
    }

    *returned = functions;
    return SCOREP_SUBSTRATES_NUM_EVENTS;
}

/* Receive callbacks from Score-P */
static void
set_callbacks( const SCOREP_SubstratePluginCallbacks* incoming_callbacks,
               size_t                                 size )
{
    assert( sizeof( SCOREP_SubstratePluginCallbacks ) <= size );
    callbacks = incoming_callbacks;
}

/* assign id */
static void
assign_id( size_t id )
{
    plugin_id = id;
}


static void
create_location( const struct SCOREP_Location* location,
                 const struct SCOREP_Location* parentLocation )
{
    /* we store the stackdepth per location and initialize it with 0 */
    uint64_t* stackdepth = calloc( sizeof( uint64_t ), 1 );
    callbacks->SCOREP_Location_SetData( location, plugin_id, ( void* )stackdepth );
}

/* Define plugins and some plugin functions */
SCOREP_SUBSTRATE_PLUGIN_ENTRY( PrintRegions )
{
    SCOREP_SubstratePluginInfo info;
    memset( &info, 0, sizeof( SCOREP_SubstratePluginInfo ) );
    info.plugin_version      = SCOREP_SUBSTRATE_PLUGIN_VERSION;
    info.set_callbacks       = set_callbacks;
    info.create_location     = create_location;
    info.assign_id           = assign_id;
    info.get_event_functions = get_event_functions;
    return info;
}
