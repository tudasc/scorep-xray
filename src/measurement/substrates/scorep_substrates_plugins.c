/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains the implementation of the low-level substrate plugins
 *
 */
#include <config.h>

#include <SCOREP_Substrates_Management.h>
#include <SCOREP_Metric_Management.h>
#include <scorep_substrates_definition.h>

#define SCOREP_DEBUG_MODULE_NAME SUBSTRATE
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <stdio.h>

#include <scorep/SCOREP_SubstratePlugins.h>
#include "scorep_substrates_plugins.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_Config.h>

#include <scorep/SCOREP_SubstratePlugins.h>


#include "scorep_substrates_plugins_confvars.inc.c"

#include <scorep_location_management.h>
#include <scorep_runtime_management.h>
#include <SCOREP_Allocator.h>

#include <scorep_ipc.h>

#define BUFFER_SIZE 512

/* ID of this substrate */
static size_t substrate_id;

/**
 * callbacks from plugins
 */
static SCOREP_SubstratePluginInfo* registered_plugins;
static uint32_t                    nr_registered_plugins;

#define SET_CALLBACK( NAME ) \
    .NAME = NAME


static void*
get_plugin_data( const SCOREP_Location* location, size_t pluginId )
{
    void** plugin_data = SCOREP_Location_GetSubstrateData( ( SCOREP_Location* )location, substrate_id );
    return plugin_data[ pluginId ];
}

static void
set_plugin_data( const SCOREP_Location* location, size_t pluginId, void* data )
{
    void** plugin_data = SCOREP_Location_GetSubstrateData( ( SCOREP_Location* )location, substrate_id );
    plugin_data[ pluginId ] = data;
}

static const SCOREP_SubstratePluginCallbacks callbacks =
{
    /* casted to const */
    .SCOREP_Location_GetName     = ( const char* ( * )( const struct SCOREP_Location* ) )SCOREP_Location_GetName,
    .SCOREP_Location_GetGlobalId = ( uint64_t ( * )( const struct SCOREP_Location* ) )SCOREP_Location_GetGlobalId,
    .SCOREP_Location_GetId       = ( uint32_t ( * )( const struct SCOREP_Location* ) )SCOREP_Location_GetId,
    .SCOREP_Location_GetType     = ( SCOREP_LocationType ( * )( const struct SCOREP_Location* ) )SCOREP_Location_GetType,

    /* internal functions */
    .SCOREP_Location_SetData = set_plugin_data,
    .SCOREP_Location_GetData = get_plugin_data,

    /* functions in scorep */
    SET_CALLBACK( SCOREP_GetExperimentDirName ),
    SET_CALLBACK( SCOREP_Ipc_GetSize ),
    SET_CALLBACK( SCOREP_Ipc_GetRank ),
    SET_CALLBACK( SCOREP_Ipc_Send ),
    SET_CALLBACK( SCOREP_Ipc_Recv ),
    SET_CALLBACK( SCOREP_Ipc_Barrier ),
    SET_CALLBACK( SCOREP_Ipc_Bcast ),
    SET_CALLBACK( SCOREP_Ipc_Gather ),
    SET_CALLBACK( SCOREP_Ipc_Gatherv ),
    SET_CALLBACK( SCOREP_Ipc_Allgather ),
    SET_CALLBACK( SCOREP_Ipc_Reduce ),
    SET_CALLBACK( SCOREP_Ipc_Allreduce ),
    SET_CALLBACK( SCOREP_Ipc_Scatter ),
    SET_CALLBACK( SCOREP_Ipc_Scatterv ),
    SET_CALLBACK( SCOREP_CallingContextHandle_GetRegion ),
    SET_CALLBACK( SCOREP_CallingContextHandle_GetParent ),
    SET_CALLBACK( SCOREP_MetricHandle_GetValueType ),
    SET_CALLBACK( SCOREP_MetricHandle_GetName ),
    SET_CALLBACK( SCOREP_MetricHandle_GetProfilingType ),
    SET_CALLBACK( SCOREP_MetricHandle_GetMode ),
    SET_CALLBACK( SCOREP_MetricHandle_GetSourceType ),
    SET_CALLBACK( SCOREP_ParadigmHandle_GetClass ),
    SET_CALLBACK( SCOREP_ParadigmHandle_GetName ),
    SET_CALLBACK( SCOREP_ParadigmHandle_GetType ),
    SET_CALLBACK( SCOREP_ParameterHandle_GetName ),
    SET_CALLBACK( SCOREP_ParameterHandle_GetType ),
    SET_CALLBACK( SCOREP_RegionHandle_GetId ),
    SET_CALLBACK( SCOREP_RegionHandle_GetName ),
    SET_CALLBACK( SCOREP_RegionHandle_GetCanonicalName ),
    SET_CALLBACK( SCOREP_RegionHandle_GetFileName ),
    SET_CALLBACK( SCOREP_RegionHandle_GetBeginLine ),
    SET_CALLBACK( SCOREP_RegionHandle_GetEndLine ),
    SET_CALLBACK( SCOREP_RegionHandle_GetType ),
    SET_CALLBACK( SCOREP_RegionHandle_GetParadigmType ),
    SET_CALLBACK( SCOREP_SamplingSetHandle_GetNumberOfMetrics ),
    SET_CALLBACK( SCOREP_SamplingSetHandle_GetScope ),
    SET_CALLBACK( SCOREP_SamplingSetHandle_GetMetricHandles ),
    SET_CALLBACK( SCOREP_SamplingSetHandle_GetMetricOccurrence ),
    SET_CALLBACK( SCOREP_SamplingSetHandle_IsScoped ),
    SET_CALLBACK( SCOREP_SamplingSetHandle_GetSamplingSetClass ),
    SET_CALLBACK( SCOREP_SourceFileHandle_GetName ),
    SET_CALLBACK( SCOREP_StringHandle_Get ),
    SET_CALLBACK( SCOREP_Metric_WriteStrictlySynchronousMetrics ),
    SET_CALLBACK( SCOREP_Metric_WriteSynchronousMetrics ),
    SET_CALLBACK( SCOREP_Metric_WriteAsynchronousMetrics ),
    SET_CALLBACK( SCOREP_ConfigManifestSectionHeader ),
    SET_CALLBACK( SCOREP_ConfigManifestSectionEntry )
};

/* check whether snprintf worked as assumed */
#define SNPRINTF_CHECK( ret, n ) \
    do \
    { \
        UTILS_BUG_ON( ret < 0, "An encoding error occured when using snprintf." ); \
        UTILS_BUG_ON( ret > n, "An snprintf buffer was not large enough." ); \
    } while ( 0 )


void
SCOREP_Substrate_Plugins_EarlyInit( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_SUBSTRATE, " initialize substrate plugins." );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_SUBSTRATE, " list of substrate plugins = %s", scorep_substrate_plugins );

    /* Read content of environment variable */
    char* env_var_content = UTILS_CStr_dup( scorep_substrate_plugins );

    /* Not correctly initialized, thus env. variable not avail. */
    if ( env_var_content == NULL )
    {
        return;
    }

    /* Return if environment variable is empty */
    if ( strlen( env_var_content ) == 0 )
    {
        free( env_var_content );
        return;
    }

    /* Read plugin names from specification string */
    char* token = strtok( env_var_content, scorep_substrate_plugins_separator );



    /* Number of selected plugins */
    uint32_t num_selected_plugins = 0;

    /* List of plugin names, initialized in the following loop */
    char** plugins = NULL;

    /* parse env variable and filter duplicate entries */
    while ( token )
    {
        bool is_new_plugin = true;

        /* Avoid loading the same plugin multiple times */
        for ( uint32_t i = 0; i < num_selected_plugins; i++ )
        {
            if ( strcmp( plugins[ i ], token ) == 0 )
            {
                /* This plugin was already specified */
                is_new_plugin = false;
                break;
            }
        }

        if ( is_new_plugin )
        {
            num_selected_plugins++;
            /* Allocate the plugin name buffer */
            plugins = realloc( plugins, num_selected_plugins * sizeof( char* ) );
            UTILS_BUG_ON( plugins == NULL, "Out of memory." );
            /* Copy the content to the buffer */
            plugins[ num_selected_plugins - 1 ] = UTILS_CStr_dup( token );
        }

        /* Handle next plugin */
        token = strtok( NULL, scorep_substrate_plugins_separator );
    }
    free( env_var_content );

    /* for each SCOREP_SUBSTRATE_PLUGIN read definitions */

    /* Go through all plugins */
    for ( uint32_t i = 0; i < num_selected_plugins; i++ )
    {
        char* current_plugin_name = plugins[ i ];

        /* Buffer to extend plugin name with .so suffix */
        char buffer[ BUFFER_SIZE ];

        /* Load it from LD_LIBRARY_PATH*/
        int error = snprintf( buffer, BUFFER_SIZE, "libscorep_substrate_%s.so", current_plugin_name );
        SNPRINTF_CHECK( error, BUFFER_SIZE );

        /* Now use dlopen to load dynamic library
         * RTLD_NOW: all undefined symbols in the library are resolved
         *           before dlopen() returns, if this cannot be done,
         *           an error is returned. */
        void* dl_handle = dlopen( buffer, RTLD_NOW );

        /* If it is not valid */
        char* dl_lib_error = dlerror();
        if ( dl_lib_error != NULL )
        {
            UTILS_WARNING( "Could not open substrate plugin %s. Error message was: %s",
                           current_plugin_name,
                           dl_lib_error );
            continue;
        }
        /* Now get the address where the
         * info symbol is loaded into memory */

        error = snprintf( buffer, BUFFER_SIZE, "SCOREP_SubstratePlugin_%s_get_info", current_plugin_name );
        SNPRINTF_CHECK( error, BUFFER_SIZE );

        /* Union to avoid casting and compiler warnings */
        union
        {
            void*                      void_ptr;
            SCOREP_SubstratePluginInfo ( * function )( void );
        } get_info;

        get_info.void_ptr = dlsym( dl_handle, buffer );
        dl_lib_error      = dlerror();
        if ( dl_lib_error != NULL )
        {
            UTILS_WARNING( "Could not find symbol 'SCOREP_SubstratePlugin_%s_get_info' of substrate plugin %s. Error message was: %s",
                           current_plugin_name, current_plugin_name,
                           dl_lib_error );
            dlclose( dl_handle );
            continue;
        }
        /* get information from the plugin */
        SCOREP_SubstratePluginInfo info = get_info.function();

        if ( info.plugin_version > SCOREP_SUBSTRATE_PLUGIN_VERSION )
        {
            UTILS_WARNING( "Substrate plugin '%s' has been compiled with a more recent version than this instance of Score-P",
                           current_plugin_name );
        }

        /* do an early initialize? */
        if ( info.init != NULL )
        {
            /* early initialize */
            if ( ( error = info.init() ) != 0 )
            {
                UTILS_WARNING( "Error %d when initializing substrate plugin %s",
                               error, current_plugin_name );
                dlclose( dl_handle );
                continue;
            }
        }
        /* register the plugin. if early init failed, this will be skipped */
        registered_plugins = realloc( registered_plugins, ( nr_registered_plugins + 1 ) * sizeof( SCOREP_SubstratePluginInfo ) );
        UTILS_BUG_ON( registered_plugins == NULL, "Out of memory." );

        registered_plugins[ nr_registered_plugins ] = info;

        nr_registered_plugins++;
    }

    /* This loop is not included in the previous one to clearly distinguish between initialization phase and set_callbacks phase for the set of substrates */
    for ( uint32_t i = 0; i < nr_registered_plugins; i++ )
    {
        if ( registered_plugins[ i ].set_callbacks != NULL )
        {
            registered_plugins[ i ].set_callbacks( &callbacks, sizeof( SCOREP_SubstratePluginCallbacks ) );
        }
    }
}


void
SCOREP_Substrate_Plugins_InitLocationData( SCOREP_Location* location )
{
    void** allocated = SCOREP_Memory_AllocForMisc( sizeof( void** ) * nr_registered_plugins );
    memset( allocated, 0, sizeof( void** ) * nr_registered_plugins );
    SCOREP_Location_SetSubstrateData( location, allocated, substrate_id );
}


static void
initialize_plugins( size_t substrateId )
{
    substrate_id = substrateId;
    for ( size_t i = 0; i < nr_registered_plugins; i++ )
    {
        if ( registered_plugins[ i ].assign_id != NULL )
        {
            registered_plugins[ i ].assign_id( i );
        }
    }
}

static size_t
finalize_plugins( void )
{
    for ( size_t i = 0; i < nr_registered_plugins; i++ )
    {
        if ( registered_plugins[ i ].finalize != NULL )
        {
            registered_plugins[ i ].finalize();
        }
    }

    free( registered_plugins );
    return substrate_id;
}

#define ASSIGN_EVENT( array, EVENT, cb ) \
    array[ SCOREP_EVENT_##EVENT ] = ( SCOREP_Substrates_Callback )( cb );

uint32_t
SCOREP_Substrate_Plugins_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode, uint32_t currentPlugin, SCOREP_Substrates_Callback** returnedCallbacks, uint32_t* currentArrayLength )
{
    if ( currentPlugin >= nr_registered_plugins )
    {
        *returnedCallbacks = NULL;
        return 0;
    }
    if ( registered_plugins[ currentPlugin ].get_event_functions )
    {
        *currentArrayLength = registered_plugins[ currentPlugin ].get_event_functions( mode, returnedCallbacks );
        return currentPlugin + 1;
    }
    else
    {
        *currentArrayLength = 0;
        return currentPlugin + 1;
    }
}

uint32_t
SCOREP_Substrate_Plugins_GetNumberRegisteredPlugins( void )
{
    return nr_registered_plugins;
}

#define ASSIGN_MGMT( array, EVENT, cb ) \
    array[ SCOREP_MGMT_##EVENT ] = ( SCOREP_Substrates_Callback )( cb );


uint32_t
SCOREP_Substrate_Plugins_GetSubstrateMgmtCallbacks( uint32_t currentPlugin, SCOREP_Substrates_Callback** returnedCallbacks )
{
    if ( currentPlugin >= nr_registered_plugins )
    {
        *returnedCallbacks = NULL;
        return 0;
    }

    SCOREP_Substrates_Callback* callbacks = calloc( SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, sizeof( SCOREP_Substrates_Callback ) );

    /* These functions are handled by this file, not by the plugins themselves. Thus, only register them once. */
    if ( currentPlugin == 0 )
    {
        ASSIGN_MGMT( callbacks, INIT_SUBSTRATE, initialize_plugins );
        ASSIGN_MGMT( callbacks, FINALIZE_SUBSTRATE, finalize_plugins );
    }

    ASSIGN_MGMT( callbacks, DUMP_MANIFEST, registered_plugins[ currentPlugin ].dump_manifest );
    ASSIGN_MGMT( callbacks, INITIALIZE_MPP, registered_plugins[ currentPlugin ].init_mpp );
    ASSIGN_MGMT( callbacks, ON_LOCATION_CREATION, registered_plugins[ currentPlugin ].create_location );
    ASSIGN_MGMT( callbacks, ON_LOCATION_DELETION, registered_plugins[ currentPlugin ].delete_location );
    ASSIGN_MGMT( callbacks, ON_CPU_LOCATION_ACTIVATION, registered_plugins[ currentPlugin ].activate_cpu_location );
    ASSIGN_MGMT( callbacks, ON_CPU_LOCATION_DEACTIVATION, registered_plugins[ currentPlugin ].deactivate_cpu_location );
    ASSIGN_MGMT( callbacks, PRE_UNIFY_SUBSTRATE, registered_plugins[ currentPlugin ].pre_unify );
    ASSIGN_MGMT( callbacks, WRITE_DATA, registered_plugins[ currentPlugin ].write_data );
    ASSIGN_MGMT( callbacks, CORE_TASK_CREATE, registered_plugins[ currentPlugin ].core_task_create );
    ASSIGN_MGMT( callbacks, CORE_TASK_COMPLETE, registered_plugins[ currentPlugin ].core_task_complete );
    ASSIGN_MGMT( callbacks, NEW_DEFINITION_HANDLE, registered_plugins[ currentPlugin ].new_definition_handle );
    ASSIGN_MGMT( callbacks, GET_REQUIREMENT, registered_plugins[ currentPlugin ].get_requirement );

    *returnedCallbacks = callbacks;
    return currentPlugin + 1;
}

void
SCOREP_Substrate_Plugins_Register( void )
{
    SCOREP_ErrorCode status;
    status = SCOREP_ConfigRegister( "substrate", scorep_substrates_plugins_confvars );
    if ( status != SCOREP_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                     "Registration of substrate plugins configure variables failed." );
    }
}
