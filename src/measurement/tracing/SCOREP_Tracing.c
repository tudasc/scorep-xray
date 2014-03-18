/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>


#include <otf2/otf2.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME TRACING
#include <UTILS_Debug.h>

#include <SCOREP_Profile.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Config.h>
#include <scorep_status.h>
#include <scorep_environment.h>
#include <scorep_runtime_management.h>
#include <scorep_clock_synchronization.h>
#include <scorep_location.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Timing.h>
#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>


#include "scorep_tracing_internal.h"
#include "SCOREP_Tracing.h"
#include "SCOREP_Tracing_ThreadInteraction.h"
#include "scorep_tracing_definitions.h"


static OTF2_Archive* scorep_otf2_archive;
static SCOREP_Mutex  scorep_otf2_archive_lock;


/** @todo croessel in OTF2_Archive_Open we need to specify an event
    chunk-size and a definition chunk size. the chnunk size need to be
    larger than the largest item that is written. events are relatively
    small whereas some definition record grow with the number of
    processes. We nee two environment variable here. does the event chunk
    size equal the memory page size (scorep_env_page_size)? */
#define SCOREP_TRACING_CHUNK_SIZE ( 1024 * 1024 )


#include "scorep_tracing_confvars.inc.c"


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


static OTF2_Compression
scorep_tracing_get_compression( void )
{
#if HAVE( OTF2_COMPRESSION_ZLIB )
    if ( scorep_tracing_compress )
    {
        return OTF2_COMPRESSION_ZLIB;
    }
#endif

    return OTF2_COMPRESSION_NONE;
}


static bool
scorep_trace_find_location_for_evt_writer_cb( SCOREP_Location* locationData,
                                              void*            userData )
{
    void**            find_location_args = userData;
    OTF2_EvtWriter*   evt_writer         = find_location_args[ 0 ];
    SCOREP_Location** found_location     = find_location_args[ 1 ];

    SCOREP_TracingData* tracing_data =
        SCOREP_Location_GetTracingData( locationData );

    if ( evt_writer == tracing_data->otf_writer )
    {
        *found_location = locationData;
        return true;
    }
    return false;
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
        SCOREP_Location* location                = NULL;
        void*            find_location_args[ 2 ] = { callerData, &location };
        SCOREP_Location_ForAll( scorep_trace_find_location_for_evt_writer_cb,
                                &find_location_args );
        UTILS_ASSERT( location );
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
    uint64_t timestamp = SCOREP_GetClockTicks();

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
        *perBufferData = SCOREP_Memory_CreateTracingPageManager();
    }

    void* chunk = SCOREP_Allocator_Alloc( *perBufferData, chunkSize );

    /* ignore allocation failures, OTF2 will flush and free chunks */

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
        /* drop also the page manager */
        SCOREP_Allocator_DeletePageManager( *perBufferData );
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
SCOREP_Tracing_Initialize( void )
{
    UTILS_ASSERT( !scorep_otf2_archive );

#if !HAVE( SCOREP_DEBUG )
    OTF2_Error_RegisterCallback( scorep_tracing_otf2_error_callback, NULL );
#endif

    /* Check for valid scorep_tracing_max_procs_per_sion_file */
    if ( 0 == scorep_tracing_max_procs_per_sion_file )
    {
        UTILS_FATAL(
            "Invalid value for SCOREP_TRACING_MAX_PROCS_PER_SION_FILE: %" PRIu64,
            scorep_tracing_max_procs_per_sion_file );
    }

    /* @todo croessel step1: remove the "4 *" intoduced on Michael's request
     * when overflow checking for definitions is implemented.
     * step2: provide environment variables to adjust the chunck sizes.
     */
    scorep_otf2_archive = OTF2_Archive_Open( SCOREP_GetExperimentDirName(),
                                             "traces",
                                             OTF2_FILEMODE_WRITE,
                                             SCOREP_TRACING_CHUNK_SIZE,
                                             4 * SCOREP_TRACING_CHUNK_SIZE,
                                             scorep_tracing_get_file_substrate(),
                                             scorep_tracing_get_compression() );
    UTILS_BUG_ON( !scorep_otf2_archive, "Couldn't create OTF2 archive." );

    scorep_tracing_register_flush_callbacks( scorep_otf2_archive );
    scorep_tracing_register_memory_callbacks( scorep_otf2_archive );

    OTF2_Archive_SetCreator( scorep_otf2_archive, PACKAGE_STRING );

    SCOREP_MutexCreate( &scorep_otf2_archive_lock );
}


void
SCOREP_Tracing_Finalize( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    /// @todo? set archive to "unified"/"not unified"
    OTF2_Archive_Close( scorep_otf2_archive );
    scorep_otf2_archive = 0;

    SCOREP_MutexDestroy( &scorep_otf2_archive_lock );
}


void
SCOREP_Tracing_OnMppInit( void )
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    SCOREP_ErrorCode err =
        scorep_tracing_set_collective_callbacks( scorep_otf2_archive );
    UTILS_ASSERT( err == SCOREP_SUCCESS );

    OTF2_ErrorCode otf2_err = OTF2_Archive_OpenEvtFiles( scorep_otf2_archive );
    UTILS_ASSERT( otf2_err == OTF2_SUCCESS );
}


void
SCOREP_Tracing_LockArchive( void )
{
    SCOREP_MutexLock( scorep_otf2_archive_lock );
}


void
SCOREP_Tracing_UnlockArchive( void )
{
    SCOREP_MutexUnlock( scorep_otf2_archive_lock );
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

    SCOREP_TracingData* tracing_data =
        SCOREP_Location_GetTracingData( locationData );

    UTILS_ASSERT( tracing_data->otf_writer );

    uint64_t number_of_events;
    OTF2_EvtWriter_GetNumberOfEvents( tracing_data->otf_writer,
                                      &number_of_events );

    /* update number of events */
    location_definition->number_of_events = number_of_events;

    /* close the event writer */
    OTF2_Archive_CloseEvtWriter( scorep_otf2_archive,
                                 tracing_data->otf_writer );
    tracing_data->otf_writer = NULL;
    return false;
}


void
SCOREP_Tracing_FinalizeEventWriters( void )
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    UTILS_ASSERT( scorep_otf2_archive );

    SCOREP_Location_ForAll( scorep_trace_finalize_event_writer_cb,
                            NULL );

    OTF2_ErrorCode err = OTF2_Archive_CloseEvtFiles( scorep_otf2_archive );
    UTILS_ASSERT( OTF2_SUCCESS == err );
}


void
SCOREP_Tracing_WriteDefinitions( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    /* Write for all local locations the same local definition file */
    OTF2_Archive_OpenDefFiles( scorep_otf2_archive );
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

        OTF2_Archive_CloseDefWriter( scorep_otf2_archive,
                                     local_definition_writer );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    OTF2_Archive_CloseDefFiles( scorep_otf2_archive );


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
            SCOREP_GetClockResolution(),
            epoch_begin,
            epoch_end - epoch_begin );
        scorep_tracing_write_global_definitions( global_definition_writer );

        OTF2_Archive_CloseGlobalDefWriter( scorep_otf2_archive,
                                           global_definition_writer );
    }
}


void
SCOREP_Tracing_WriteProperties( void )
{
    UTILS_ASSERT( scorep_otf2_archive );

    scorep_tracing_set_properties( scorep_otf2_archive );
}
