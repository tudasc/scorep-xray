/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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


/**
 * @file       SCOREP_Tracing.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include <scorep_utility/SCOREP_Debug.h>

#include <scorep_status.h>
#include <scorep_environment.h>
#include <scorep_runtime_management.h>
#include <scorep_thread.h>
#include <SCOREP_Memory.h>
#include <scorep_definitions.h>
#include <scorep_definition_structs.h>
#include <scorep_mpi.h>

#include "SCOREP_Tracing.h"

/* *INDENT-OFF* */
/* *INDENT-ON*  */


/* For the SCOREP_Tracing_GetFileSubstrate() see paradigm/scorep_sion_*.c */


OTF2_Compression
SCOREP_Tracing_GetCompression()
{
    if ( SCOREP_Env_CompressTraces() )
    {
        return OTF2_COMPRESSION_ZLIB;
    }
    else
    {
        return OTF2_COMPRESSION_NONE;
    }
}


uint64_t
SCOREP_Tracing_GetChunkSize()
{
    /** @todo croessel in OTF2_Archive_New we need to specify an event
        chunk-size and a definition chunk size. the chnunk size need to be
        larger than the largest item that is written. events are relatively
        small whereas some definition record grow with the number of
        processes. We nee two environment variable here. does the event chunk
        size equal the memory page size (scorep_env_page_size)? */
    return 1024 * 1024; // 1MB
}


static void
scorep_trace_finalize_event_writer_cb( SCOREP_Thread_LocationData* locationData,
                                       void*                       userData )
{
    SCOREP_LocationHandle       location_handle =
        SCOREP_Thread_GetLocationHandle( locationData );
    SCOREP_Location_Definition* location_definition =
        SCOREP_LOCAL_HANDLE_DEREF( location_handle, Location );

    SCOREP_Trace_LocationData* trace_data =
        SCOREP_Thread_GetTraceLocationData( locationData );

    assert( trace_data->otf_writer );

    uint64_t number_of_events;
    OTF2_EvtWriter_GetNumberOfEvents( trace_data->otf_writer,
                                      &number_of_events );

    /* update number of events */
    location_definition->number_of_events = number_of_events;

    /* close the event writer */
    OTF2_Archive_CloseEvtWriter( scorep_otf2_archive,
                                 trace_data->otf_writer );
    trace_data->otf_writer = NULL;
}


void
SCOREP_Tracing_FinalizeEventWriters( void )
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    assert( scorep_otf2_archive );

    SCOREP_Thread_ForAllLocations( scorep_trace_finalize_event_writer_cb,
                                   NULL );
}


static void*
scorep_tracing_chunk_allocate( void*         userData,
                               OTF2_FileType fileType,
                               uint64_t      locationId,
                               void**        perBufferData,
                               uint64_t      chunkSize )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY,
                         "%" PRIu64, chunkSize );

    if ( !*perBufferData )
    {
        /* This manager has a pre-allocated page, which is much smaller
           than the chunksize, which is wasted now */
        *perBufferData = SCOREP_Memory_CreatePageManager();
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY,
                         "%s", final ? "final" : "intermediate" );

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


void
SCOREP_Tracing_RegisterMemoryCallbacks( OTF2_Archive* archive )
{
    assert( archive );

    SCOREP_Error_Code status =
        OTF2_Archive_SetMemoryCallbacks( archive,
                                         &scorep_tracing_chunk_callbacks,
                                         NULL );
    assert( status == SCOREP_SUCCESS );
}
