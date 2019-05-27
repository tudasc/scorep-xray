/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>

#include "SCOREP_Tracing.h"

#include <otf2/otf2.h>
#include <otf2/OTF2_EventSizeEstimator.h>

#include <SCOREP_RuntimeManagement.h>
#include <scorep_runtime_management.h>
#include <scorep_status.h>
#include <scorep_location_management.h>
#include "scorep_rewind_stack_management.h"
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>
#include "scorep_tracing_internal.h"
#include <SCOREP_Definitions.h>
#include "scorep_tracing_definitions.h"
#include <scorep_clock_synchronization.h>
#include "scorep_tracing_internal.h"

#include <SCOREP_Substrates_Management.h>
#include <scorep_substrates_definition.h>

#define SCOREP_DEBUG_MODULE_NAME TRACING
#include <UTILS_Debug.h>

#include <inttypes.h>

static OTF2_Archive* scorep_otf2_archive;


/* We can't rely on SCOREP_Status_IsMppInitialized(), as it is already
 * true before our SCOREP_Tracing_OnMppInit() was called, thus
 * track the state by ourself too. */
static bool event_files_opened;


size_t scorep_tracing_substrate_id;


/** @todo croessel in OTF2_Archive_Open we need to specify an event
    chunk size and a definition chunk size. */
#define SCOREP_TRACING_CHUNK_SIZE ( 1024 * 1024 )


#include "scorep_tracing_confvars.inc.c"

SCOREP_StringHandle scorep_tracing_cct_file      = SCOREP_INVALID_STRING;
SCOREP_StringHandle scorep_tracing_cct_ip_offset = SCOREP_INVALID_STRING;
SCOREP_StringHandle scorep_tracing_cct_ip        = SCOREP_INVALID_STRING;


static OTF2_FileSubstrate
scorep_tracing_get_file_substrate( void )
{
#if HAVE( OTF2_SUBSTRATE_SION )
    if ( scorep_tracing_use_sion )
    {
        return OTF2_SUBSTRATE_SION;
    }
#endif

    return OTF2_SUBSTRATE_POSIX;
}


static OTF2_FlushType
scorep_on_trace_pre_flush( void*         userData,
                           OTF2_FileType fileType,
                           uint64_t      locationId,
                           void*         callerData,
                           bool          final )
{
    if ( fileType == OTF2_FILETYPE_EVENTS )
    {
        if ( !event_files_opened )
        {
            UTILS_FATAL( "Trace buffer flush before MPP was initialized." );
        }
        SCOREP_OnTracingBufferFlushBegin( final );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_TRACING,
                        "[%d]: %s flush on %s#%" PRIu64 "\n",
                        SCOREP_Status_GetRank(),
                        final ? "final" : "intermediate",
                        fileType == OTF2_FILETYPE_ANCHOR ? "Anchor" :
                        fileType == OTF2_FILETYPE_GLOBAL_DEFS ? "GlobalDef" :
                        fileType == OTF2_FILETYPE_LOCAL_DEFS ? "Def" : "Evt",
                        fileType == OTF2_FILETYPE_GLOBAL_DEFS ? 0 : locationId );

    if ( fileType == OTF2_FILETYPE_EVENTS && !final )
    {
        /* A buffer flush happen in an event buffer before the end of the measurement */

        /*
         * disable recording, but forever
         * - not via scorep_recording_enabled,
         *   scorep_recording_enabled can be enabled by the user again.
         * - not via SCOREP_IsTracingEnabled()
         *   because we still need this to correctly finalize
         */

        fprintf( stderr,
                 "[Score-P] Trace buffer flush on rank %d.\n",
                 SCOREP_Status_GetRank() );
        fprintf( stderr,
                 "[Score-P] Increase SCOREP_TOTAL_MEMORY and try again.\n" );
    }

    OTF2_FlushType do_flush = OTF2_FLUSH;
    if ( final )
    {
        /* Always flush if this is the final one. */
        do_flush = OTF2_FLUSH;
    }

    if ( fileType == OTF2_FILETYPE_EVENTS )
    {
        void*          user_data = NULL;
        OTF2_ErrorCode err       = OTF2_EvtWriter_GetUserData( callerData, &user_data );
        UTILS_ASSERT( err == OTF2_SUCCESS && user_data );

        SCOREP_Location* location = ( SCOREP_Location* )user_data;
        SCOREP_Location_EnsureGlobalId( location );
        scorep_rewind_stack_delete( location );
    }

    return do_flush;
}


static uint64_t
scorep_on_trace_post_flush( void*         userData,
                            OTF2_FileType fileType,
                            uint64_t      locationId )
{
    uint64_t timestamp = SCOREP_Timer_GetClockTicks();

    if ( fileType == OTF2_FILETYPE_EVENTS )
    {
        SCOREP_OnTracingBufferFlushEnd( timestamp );
    }

    return timestamp;
}


static OTF2_FlushCallbacks flush_callbacks =
{
    .otf2_pre_flush  = scorep_on_trace_pre_flush,
    .otf2_post_flush = scorep_on_trace_post_flush
};


static void
scorep_tracing_register_flush_callbacks( OTF2_Archive* archive )
{
    OTF2_ErrorCode status =
        OTF2_Archive_SetFlushCallbacks( archive,
                                        &flush_callbacks,
                                        NULL );
    UTILS_ASSERT( status == OTF2_SUCCESS );
}


static void*
scorep_tracing_chunk_allocate( void*         userData,
                               OTF2_FileType fileType,
                               uint64_t      locationId,
                               void**        perBufferData,
                               uint64_t      chunkSize )
{
    UTILS_DEBUG_ENTRY( "chunk size: %" PRIu64, chunkSize );

    if ( !*perBufferData )
    {
        /* This manager has a pre-allocated page, which is much smaller
           than the chunksize, which is wasted now */
        if ( OTF2_FILETYPE_EVENTS == fileType )
        {
            // take the pre-allocated one from the current location
            *perBufferData = SCOREP_Location_GetOrCreateMemoryPageManager(
                SCOREP_Location_GetCurrentCPULocation(),
                SCOREP_MEMORY_TYPE_TRACING_EVENTS );
        }
        else
        {
            *perBufferData = SCOREP_Memory_CreateTracingPageManager();
        }
    }

    void* chunk = SCOREP_Allocator_Alloc( *perBufferData, chunkSize );

    /* ignore allocation failures, OTF2 will flush and free chunks */
#if HAVE( UTILS_DEBUG )
    if ( !chunk )
    {
        UTILS_WARNING( "Cannot allocate %" PRIu64 " bytes for tracing; but OTF2 will flush and free chunks.", chunkSize );
    }
#endif

    return chunk;
}


static void
scorep_tracing_chunk_free_all( void*         userData,
                               OTF2_FileType fileType,
                               uint64_t      locationId,
                               void**        perBufferData,
                               bool          final )
{
    UTILS_DEBUG_ENTRY( "%s", final ? "final" : "intermediate" );

    /* maybe we were called without one allocate */
    if ( !*perBufferData )
    {
        return;
    }

    /* drop all used pages */
    SCOREP_Allocator_Free( *perBufferData );

    if ( final )
    {
        if ( OTF2_FILETYPE_EVENTS != fileType )
        {
            /* drop also the page manager */
            SCOREP_Allocator_DeletePageManager( *perBufferData );
        }
    }
}


static OTF2_MemoryCallbacks scorep_tracing_chunk_callbacks =
{
    .otf2_allocate = scorep_tracing_chunk_allocate,
    .otf2_free_all = scorep_tracing_chunk_free_all
};


static void
scorep_tracing_register_memory_callbacks( OTF2_Archive* archive )
{
    OTF2_ErrorCode status =
        OTF2_Archive_SetMemoryCallbacks( archive,
                                         &scorep_tracing_chunk_callbacks,
                                         NULL );
    UTILS_ASSERT( status == OTF2_SUCCESS );
}


#if !HAVE( SCOREP_DEBUG )
static OTF2_ErrorCode
scorep_tracing_otf2_error_callback( void*          userData,
                                    const char*    file,
                                    uint64_t       line,
                                    const char*    function,
                                    OTF2_ErrorCode errorCode,
                                    const char*    msgFormatString,
                                    va_list        va )
{
    return errorCode;
}
#endif


void
SCOREP_Tracing_Initialize( size_t substrateId )
{
    UTILS_ASSERT( !scorep_otf2_archive );

    scorep_tracing_substrate_id = substrateId;

#if !HAVE( SCOREP_DEBUG )
    OTF2_Error_RegisterCallback( scorep_tracing_otf2_error_callback, NULL );
#endif

#if !HAVE( OTF2_SUBSTRATE_SION )
    if ( scorep_tracing_use_sion )
    {
        UTILS_WARNING( "Ignoring SIONlib trace substrate request via SCOREP_TRACING_USE_SION, "
                       "as OTF2 does not have support for it." );
    }
#endif

    /* Check for valid scorep_tracing_max_procs_per_sion_file */
    if ( 0 == scorep_tracing_max_procs_per_sion_file )
    {
        UTILS_FATAL(
            "Invalid value for SCOREP_TRACING_MAX_PROCS_PER_SION_FILE: %" PRIu64,
            scorep_tracing_max_procs_per_sion_file );
    }

    scorep_otf2_archive = OTF2_Archive_Open( SCOREP_GetExperimentDirName(),
                                             "traces",
                                             OTF2_FILEMODE_WRITE,
                                             SCOREP_TRACING_CHUNK_SIZE,
                                             OTF2_UNDEFINED_UINT64,
                                             scorep_tracing_get_file_substrate(),
                                             OTF2_COMPRESSION_NONE );
    UTILS_BUG_ON( !scorep_otf2_archive, "Couldn't create OTF2 archive." );

    scorep_tracing_register_flush_callbacks( scorep_otf2_archive );
    scorep_tracing_register_memory_callbacks( scorep_otf2_archive );

    SCOREP_ErrorCode err =
        scorep_tracing_set_locking_callbacks( scorep_otf2_archive );
    UTILS_ASSERT( err == SCOREP_SUCCESS );

    OTF2_Archive_SetCreator( scorep_otf2_archive, PACKAGE_STRING );

    if ( SCOREP_IsUnwindingEnabled() )
    {
        scorep_tracing_cct_file      = SCOREP_Definitions_NewString( "EXECUTABLE/SHARED OBJECT" );
        scorep_tracing_cct_ip_offset = SCOREP_Definitions_NewString( "INSTRUCTION OFFSET" );
        scorep_tracing_cct_ip        = SCOREP_Definitions_NewString( "INSTRUCTION ADDRESS" );
    }
}


size_t
SCOREP_Tracing_Finalize( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    /// @todo? set archive to "unified"/"not unified"
    OTF2_ErrorCode ret = OTF2_Archive_Close( scorep_otf2_archive );
    if ( OTF2_SUCCESS != ret )
    {
        UTILS_FATAL( "Could not finalize OTF2 archive: %s",
                     OTF2_Error_GetDescription( ret ) );
    }
    scorep_otf2_archive = 0;
    return scorep_tracing_substrate_id;
}


void
SCOREP_Tracing_OnMppInit( void )
{
    SCOREP_ErrorCode err =
        scorep_tracing_set_collective_callbacks( scorep_otf2_archive );
    UTILS_ASSERT( err == SCOREP_SUCCESS );

    OTF2_ErrorCode otf2_err = OTF2_Archive_OpenEvtFiles( scorep_otf2_archive );
    if ( OTF2_SUCCESS != otf2_err )
    {
        UTILS_FATAL( "Could not open OTF2 event files: %s",
                     OTF2_Error_GetDescription( otf2_err ) );
    }

    /* We can flush only *after* OTF2 was properly MPP initialized,
     * SCOREP_Status_IsMppInitialized() was already true, before this function
     * was called */
    event_files_opened = true;
}


OTF2_EvtWriter*
SCOREP_Tracing_GetEventWriter( void )
{
    OTF2_EvtWriter* evt_writer = OTF2_Archive_GetEvtWriter(
        scorep_otf2_archive,
        OTF2_UNDEFINED_LOCATION );
    if ( !evt_writer )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }

    return evt_writer;
}


static bool
scorep_trace_finalize_event_writer_cb( SCOREP_Location* locationData,
                                       void*            userData )
{
    SCOREP_LocationHandle location_handle =
        SCOREP_Location_GetLocationHandle( locationData );
    SCOREP_LocationDef* location_definition =
        SCOREP_LOCAL_HANDLE_DEREF( location_handle, Location );

    SCOREP_TracingData* tracing_data = scorep_tracing_get_trace_data( locationData );


    UTILS_ASSERT( tracing_data->otf_writer );

    uint64_t number_of_events;
    OTF2_EvtWriter_GetNumberOfEvents( tracing_data->otf_writer,
                                      &number_of_events );

    /* update number of events */
    location_definition->number_of_events = number_of_events;

    /* close the event writer */
    OTF2_ErrorCode ret = OTF2_Archive_CloseEvtWriter( scorep_otf2_archive,
                                                      tracing_data->otf_writer );
    if ( OTF2_SUCCESS != ret )
    {
        UTILS_FATAL( "Could not finalize OTF2 event writer %" PRIu64 ": %s",
                     location_definition->global_location_id,
                     OTF2_Error_GetDescription( ret ) );
    }

    tracing_data->otf_writer = NULL;
    return false;
}


void
SCOREP_Tracing_FinalizeEventWriters( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    SCOREP_Location_ForAll( scorep_trace_finalize_event_writer_cb,
                            NULL );

    OTF2_ErrorCode err = OTF2_Archive_CloseEvtFiles( scorep_otf2_archive );
    if ( OTF2_SUCCESS != err )
    {
        UTILS_FATAL( "Could not close OTF2 event files: %s",
                     OTF2_Error_GetDescription( err ) );
    }
}


static void
write_definitions( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    OTF2_ErrorCode ret;

    uint64_t definition_chunk_size = OTF2_UNDEFINED_UINT64;
    if ( SCOREP_Status_GetRank() == 0 )
    {
        OTF2_EventSizeEstimator* estimator = OTF2_EventSizeEstimator_New();
        /* We do not write metric or region groups, thus we set only the number of locations */
        OTF2_EventSizeEstimator_SetNumberOfLocationDefinitions(
            estimator,
            scorep_unified_definition_manager->location.counter );
        definition_chunk_size = OTF2_EventSizeEstimator_GetDefChunkSize( estimator );
        OTF2_EventSizeEstimator_Delete( estimator );
    }

    /* OTF2 does the broadcast for us */
    ret = OTF2_Archive_SetDefChunkSize( scorep_otf2_archive,
                                        definition_chunk_size );
    if ( OTF2_SUCCESS != ret && SCOREP_Status_GetRank() == 0 )
    {
        UTILS_FATAL( "Could not set OTF2 definition chunks size to %" PRIu64 ": %s",
                     definition_chunk_size,
                     OTF2_Error_GetDescription( ret ) );
        /* lets assume, that the other ranks also abort, if the root aborts */
    }

    /* Write for all local locations the same local definition file */
    ret = OTF2_Archive_OpenDefFiles( scorep_otf2_archive );
    if ( OTF2_SUCCESS != ret )
    {
        UTILS_FATAL( "Could not open OTF2 definition files: %s",
                     OTF2_Error_GetDescription( ret ) );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Location, location )
    {
        OTF2_DefWriter* local_definition_writer = OTF2_Archive_GetDefWriter(
            scorep_otf2_archive,
            definition->global_location_id );
        if ( !local_definition_writer )
        {
            /* aborts */
            SCOREP_Memory_HandleOutOfMemory();
        }

        scorep_tracing_write_mappings( local_definition_writer );
        scorep_tracing_write_clock_offsets( local_definition_writer );
        scorep_tracing_write_local_definitions( local_definition_writer );

        ret = OTF2_Archive_CloseDefWriter( scorep_otf2_archive,
                                           local_definition_writer );
        if ( OTF2_SUCCESS != ret )
        {
            UTILS_FATAL( "Could not finalize OTF2 definition writer: %s",
                         OTF2_Error_GetDescription( ret ) );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    ret = OTF2_Archive_CloseDefFiles( scorep_otf2_archive );
    if ( OTF2_SUCCESS != ret )
    {
        UTILS_FATAL( "Could not close OTF2 definition files: %s",
                     OTF2_Error_GetDescription( ret ) );
    }


    uint64_t epoch_begin;
    uint64_t epoch_end;
    SCOREP_GetGlobalEpoch( &epoch_begin, &epoch_end );
    if ( SCOREP_Status_GetRank() == 0 )
    {
        OTF2_GlobalDefWriter* global_definition_writer =
            OTF2_Archive_GetGlobalDefWriter( scorep_otf2_archive );
        if ( !global_definition_writer )
        {
            /* aborts */
            SCOREP_Memory_HandleOutOfMemory();
        }

        OTF2_GlobalDefWriter_WriteClockProperties(
            global_definition_writer,
            SCOREP_Timer_GetClockResolution(),
            epoch_begin,
            epoch_end - epoch_begin );
        scorep_tracing_write_global_definitions( global_definition_writer );

        ret = OTF2_Archive_CloseGlobalDefWriter( scorep_otf2_archive,
                                                 global_definition_writer );
        if ( OTF2_SUCCESS != ret )
        {
            UTILS_FATAL( "Could not finalize global OTF2 definition writer: %s",
                         OTF2_Error_GetDescription( ret ) );
        }
    }
}


static void
write_properties( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    scorep_tracing_set_properties( scorep_otf2_archive );
}


void
SCOREP_Tracing_Write( void )
{
    write_properties();
    write_definitions();
}
