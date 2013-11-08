/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
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

#include <scorep_ipc.h>

static inline SCOREP_Ipc_Datatype
get_ipc_type( OTF2_Type type )
{
    switch ( type )
    {
        case OTF2_TYPE_INT8:
            return SCOREP_IPC_CHAR;
        case OTF2_TYPE_UINT8:
            return SCOREP_IPC_UNSIGNED_CHAR;
        case OTF2_TYPE_INT32:
            return SCOREP_IPC_INT32_T;
        case OTF2_TYPE_UINT32:
            return SCOREP_IPC_UINT32_T;
        case OTF2_TYPE_INT64:
            return SCOREP_IPC_INT64_T;
        case OTF2_TYPE_UINT64:
            return SCOREP_IPC_UINT64_T;
        case OTF2_TYPE_DOUBLE:
            return SCOREP_IPC_DOUBLE;

        default:
            UTILS_BUG( "Unhandled OTF2 type: %u", type );
            return SCOREP_IPC_CHAR;
    }
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_get_size( void*                   userData,
                                          OTF2_CollectiveContext* commContext,
                                          uint32_t*               size )
{
    *size = SCOREP_Ipc_GetSize();

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_get_rank( void*                   userData,
                                          OTF2_CollectiveContext* commContext,
                                          uint32_t*               rank )
{
    *rank = SCOREP_Ipc_GetRank();

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_barrier( void*                   userData,
                                         OTF2_CollectiveContext* commContext )
{
    SCOREP_Ipc_Barrier();

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_bcast( void*                   userData,
                                       OTF2_CollectiveContext* commContext,
                                       void*                   data,
                                       OTF2_Type               type,
                                       uint32_t                numberElements,
                                       uint32_t                root )
{
    SCOREP_Ipc_Bcast( data,
                      numberElements,
                      get_ipc_type( type ),
                      root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_gather( void*                   userData,
                                        OTF2_CollectiveContext* commContext,
                                        void*                   inData,
                                        void*                   outData,
                                        OTF2_Type               type,
                                        uint32_t                numberElements,
                                        uint32_t                root )
{
    SCOREP_Ipc_Gather( inData,
                       outData,
                       numberElements,
                       get_ipc_type( type ),
                       root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_gatherv( void*                   userData,
                                         OTF2_CollectiveContext* commContext,
                                         void*                   inData,
                                         void*                   outData,
                                         OTF2_Type               type,
                                         uint32_t                numberElements,
                                         uint32_t                root )
{
    int* recvcnts = NULL;
    int* displs   = NULL;
    int  size     = SCOREP_Ipc_GetSize();
    int  rank     = SCOREP_Ipc_GetRank();
    if ( rank == root )
    {
        recvcnts = calloc( size, sizeof( *recvcnts ) );
        displs   = calloc( size, sizeof( *displs ) );
    }

    SCOREP_Ipc_Gather( &numberElements,
                       recvcnts,
                       1,
                       SCOREP_IPC_UINT32_T,
                       root );
    if ( rank == root )
    {
        int displ = 0;
        for ( int i = 0; i < size; i++ )
        {
            displs[ i ] = displ;
            displ      += recvcnts[ i ];
        }
    }

    SCOREP_Ipc_Gatherv( inData,
                        numberElements,
                        outData,
                        recvcnts,
                        displs,
                        get_ipc_type( type ),
                        root );

    if ( rank == root )
    {
        free( recvcnts );
        free( displs );
    }

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_scatter( void*                   userData,
                                         OTF2_CollectiveContext* commContext,
                                         void*                   inData,
                                         void*                   outData,
                                         OTF2_Type               type,
                                         uint32_t                numberElements,
                                         uint32_t                root )
{
    SCOREP_Ipc_Scatter( inData,
                        outData,
                        numberElements,
                        get_ipc_type( type ),
                        root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
scorep_tracing_otf2_collectives_scatterv( void*                   userData,
                                          OTF2_CollectiveContext* commContext,
                                          void*                   inData,
                                          void*                   outData,
                                          OTF2_Type               type,
                                          uint32_t                numberElements,
                                          uint32_t                root )
{
    int* sendcnts = NULL;
    int* displs   = NULL;
    int  size     = SCOREP_Ipc_GetSize();
    int  rank     = SCOREP_Ipc_GetRank();
    if ( rank == root )
    {
        sendcnts = calloc( size, sizeof( *sendcnts ) );
        displs   = calloc( size, sizeof( *displs ) );
    }

    SCOREP_Ipc_Gather( &numberElements,
                       sendcnts,
                       1,
                       SCOREP_IPC_UINT32_T,
                       root );
    if ( rank == root )
    {
        int displ = 0;
        for ( int i = 0; i < size; i++ )
        {
            displs[ i ] = displ;
            displ      += sendcnts[ i ];
        }
    }

    SCOREP_Ipc_Scatterv( outData,
                         sendcnts,
                         displs,
                         inData,
                         numberElements,
                         get_ipc_type( type ),
                         root );

    if ( rank == root )
    {
        free( sendcnts );
        free( displs );
    }

    return OTF2_CALLBACK_SUCCESS;
}


static const OTF2_CollectiveCallbacks scorep_tracing_otf2_collectives =
{
    .otf2_release           = NULL,
    .otf2_get_size          = scorep_tracing_otf2_collectives_get_size,
    .otf2_get_rank          = scorep_tracing_otf2_collectives_get_rank,
    .otf2_create_local_comm = NULL,
    .otf2_free_local_comm   = NULL,
    .otf2_barrier           = scorep_tracing_otf2_collectives_barrier,
    .otf2_bcast             = scorep_tracing_otf2_collectives_bcast,
    .otf2_gather            = scorep_tracing_otf2_collectives_gather,
    .otf2_gatherv           = scorep_tracing_otf2_collectives_gatherv,
    .otf2_scatter           = scorep_tracing_otf2_collectives_scatter,
    .otf2_scatterv          = scorep_tracing_otf2_collectives_scatterv
};


SCOREP_ErrorCode
scorep_tracing_set_collective_callbacks( OTF2_Archive* archive )
{
    OTF2_ErrorCode err =
        OTF2_Archive_SetCollectiveCallbacks( archive,
                                             &scorep_tracing_otf2_collectives,
                                             NULL,
                                             NULL,
                                             NULL );
    return OTF2_SUCCESS == err
           ? SCOREP_SUCCESS
           : SCOREP_ERROR_PROCESSED_WITH_FAULTS;
}
