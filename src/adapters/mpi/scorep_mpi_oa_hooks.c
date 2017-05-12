/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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
 * @brief   mpi hooks functions to be used by MPI profiling
 *
 *
 */

#include <config.h>

#include "SCOREP_Mpi.h"
#include "scorep_mpi_oa_hooks.h"
#define SCOREP_DEBUG_MODULE_NAME MPIPROFILING
#include <UTILS_Debug.h>
#include "scorep_mpi_oa_profile.h"
#include "scorep_mpi_oa_profile_mgmt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*----------------------------------------------
 * 1x1 pre- and post- communication hooks
   -----------------------------------------------*/


/**
 * Pre-communication hook for MPI_Send
 */
void
SCOREP_Hooks_Pre_MPI_Send( SCOREP_MPI_CONST_DECL void* buf,
                           int                         count,
                           MPI_Datatype                datatype,
                           int                         dest,
                           int                         tag,
                           MPI_Comm                    comm,
                           uint64_t                    start_time_stamp )
{
}

/**
 * Post-communication hook for MPI_Send
 */
void
SCOREP_Hooks_Post_MPI_Send( SCOREP_MPI_CONST_DECL void* buf,
                            int                         count,
                            MPI_Datatype                datatype,
                            int                         dest,
                            int                         tag,
                            MPI_Comm                    comm,
                            uint64_t                    start_time_stamp,
                            int                         return_value )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

void
SCOREP_Hooks_Post_MPI_Bsend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp,
                             int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

void
SCOREP_Hooks_Post_MPI_Ssend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp,
                             int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

void
SCOREP_Hooks_Post_MPI_Rsend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp,
                             int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

/**
 * Pre-communication hook for MPI_Recv
 */
void
SCOREP_Hooks_Pre_MPI_Recv( void*        buf,
                           int          count,
                           MPI_Datatype datatype,
                           int          source,
                           int          tag,
                           MPI_Comm     comm,
                           MPI_Status*  status,
                           uint64_t     start_time_stamp )
{
}

/**
 * Post-communication hook for MPI_Recv
 */
void
SCOREP_Hooks_Post_MPI_Recv( void*        buf,
                            int          count,
                            MPI_Datatype datatype,
                            int          source,
                            int          tag,
                            MPI_Comm     comm,
                            MPI_Status*  status,
                            uint64_t     start_time_stamp,
                            int          return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    source = status->MPI_SOURCE;
    tag    = status->MPI_TAG;
    void*      remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    MPI_Status s;
    PMPI_Recv(      remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    source,
                    tag,
                    scorep_mpiprofiling_world_comm_dup.comm,
                    &s );

    scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                           localTimePack );
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Isend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             MPI_Request*                request,
                             int64_t                     start_time_stamp,
                             int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store tiempack request position in the timpacks pool for later use in send cancel operation*/
    online_analysis_pod->send_tp_position_in_pool = position;

    /* Store timepack request for later use in case of send cancel.
     * In general we don't need to complete the timepack send operation, since it will
     * be automatically checked by the timepack pool. Only in case of original send cancel
     * operation timpack request is needed to cancel the timepack send aswell*/
    online_analysis_pod->tp_request = tp_request;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_EXIT( "position %d, dest %d, tag %d, time %llu", position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Issend( SCOREP_MPI_CONST_DECL void* buf,
                              int                         count,
                              MPI_Datatype                datatype,
                              int                         dest,
                              int                         tag,
                              MPI_Comm                    comm,
                              MPI_Request*                request,
                              int64_t                     start_time_stamp,
                              int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store tiempack request position in the timpacks pool for later use in send cancel operation*/
    online_analysis_pod->send_tp_position_in_pool = position;

    /* Store timepack request for later use in case of send cancel.
     * In general we don't need to complete the timepack send operation, since it will
     * be automatically checked by the timepack pool. Only in case of original send cancel
     * operation timpack request is needed to cancel the timepack send aswell*/
    online_analysis_pod->tp_request = tp_request;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_EXIT( "position %d, dest %d, tag %d, time %llu", position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Ibsend( SCOREP_MPI_CONST_DECL void* buf,
                              int                         count,
                              MPI_Datatype                datatype,
                              int                         dest,
                              int                         tag,
                              MPI_Comm                    comm,
                              MPI_Request*                request,
                              int64_t                     start_time_stamp,
                              int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store tiempack request position in the timpacks pool for later use in send cancel operation*/
    online_analysis_pod->send_tp_position_in_pool = position;

    /* Store timepack request for later use in case of send cancel.
     * In general we don't need to complete the timepack send operation, since it will
     * be automatically checked by the timepack pool. Only in case of original send cancel
     * operation timpack request is needed to cancel the timepack send aswell*/
    online_analysis_pod->tp_request = tp_request;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_EXIT( "position %d, dest %d, tag %d, time %llu", position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Irsend( SCOREP_MPI_CONST_DECL void* buf,
                              int                         count,
                              MPI_Datatype                datatype,
                              int                         dest,
                              int                         tag,
                              MPI_Comm                    comm,
                              MPI_Request*                request,
                              int64_t                     start_time_stamp,
                              int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* get a free timepack buffer from the pool*/
    scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

    /* intialize local timepack with the time stamp and the global rank of this proc*/
    scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

    /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
    PMPI_Isend(  localTimePack,
                 MPIPROFILER_TIMEPACK_BUFSIZE,
                 MPI_PACKED,
                 global_dest,
                 tag,
                 scorep_mpiprofiling_world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store tiempack request position in the timpacks pool for later use in send cancel operation*/
    online_analysis_pod->send_tp_position_in_pool = position;

    /* Store timepack request for later use in case of send cancel.
     * In general we don't need to complete the timepack send operation, since it will
     * be automatically checked by the timepack pool. Only in case of original send cancel
     * operation timpack request is needed to cancel the timepack send aswell*/
    online_analysis_pod->tp_request = tp_request;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_EXIT( "position %d, dest %d, tag %d, time %llu", position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Irecv( void*        buf,
                             int          count,
                             MPI_Datatype datatype,
                             int          source,
                             int          tag,
                             MPI_Comm     comm,
                             MPI_Request* request,
                             int64_t      start_time_stamp,
                             int          return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    MPI_Group group         = MPI_GROUP_NULL;
    int       global_source = 0;

    if ( source != MPI_ANY_SOURCE )
    {
        /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
        if ( scorep_mpiprofiling_rank_to_pe( source, comm, &global_source ) != 0 )
        {
            return;
        }
    }
    else
    {
        /*
         * get the internal MPI_Group of the communicator for later rank
         * translation, can't store the MPI_Comm, because it may have gone
         * until completion of this request
         */
        if ( scorep_mpiprofiling_get_group( comm, &group ) != 0 )
        {
            return;
        }
    }

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* If source is MPI_ANY_SOURCE, set the wild-card source flag for this request */
    if ( source == MPI_ANY_SOURCE )
    {
        online_analysis_pod->tp_comm_partner_wc = 1;
    }
    else
    {
        online_analysis_pod->tp_comm_partner_wc = 0;
    }

    /* If tag is MPI_TAG_ANY, set the wild-card tag flag for this request */
    if ( tag == MPI_ANY_TAG )
    {
        online_analysis_pod->tp_tag_wc = 1;
    }
    else
    {
        online_analysis_pod->tp_tag_wc = 0;
    }

    /* Store global rank of the source and the communication tag associated with the recv operation */
    online_analysis_pod->tp_comm_partner = global_source;
    online_analysis_pod->tp_tag          = tag;

    /* store the associated MPI_Group for later rank translation of the source */
    online_analysis_pod->group = group;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* orig_req = scorep_mpi_request_get( *request );
    orig_req->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_EXIT( "source(*-%d,%d), tag(*-%d,%d)", online_analysis_pod->tp_comm_partner_wc, global_source, online_analysis_pod->tp_tag_wc, tag );
}

void
SCOREP_Hooks_Post_MPI_Send_init( SCOREP_MPI_CONST_DECL void* buf,
                                 int                         count,
                                 MPI_Datatype                datatype,
                                 int                         dest,
                                 int                         tag,
                                 MPI_Comm                    comm,
                                 MPI_Request*                request,
                                 int64_t                     start_time_stamp,
                                 int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store communoication partner rank and tag for later piggyback send operation */
    online_analysis_pod->tp_comm_partner = global_dest;
    online_analysis_pod->tp_tag          = tag;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Ssend_init( SCOREP_MPI_CONST_DECL void* buf,
                                  int                         count,
                                  MPI_Datatype                datatype,
                                  int                         dest,
                                  int                         tag,
                                  MPI_Comm                    comm,
                                  MPI_Request*                request,
                                  int64_t                     start_time_stamp,
                                  int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store communoication partner rank and tag for later piggyback send operation */
    online_analysis_pod->tp_comm_partner = global_dest;
    online_analysis_pod->tp_tag          = tag;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Rsend_init( SCOREP_MPI_CONST_DECL void* buf,
                                  int                         count,
                                  MPI_Datatype                datatype,
                                  int                         dest,
                                  int                         tag,
                                  MPI_Comm                    comm,
                                  MPI_Request*                request,
                                  int64_t                     start_time_stamp,
                                  int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store communoication partner rank and tag for later piggyback send operation */
    online_analysis_pod->tp_comm_partner = global_dest;
    online_analysis_pod->tp_tag          = tag;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Bsend_init( SCOREP_MPI_CONST_DECL void* buf,
                                  int                         count,
                                  MPI_Datatype                datatype,
                                  int                         dest,
                                  int                         tag,
                                  MPI_Comm                    comm,
                                  MPI_Request*                request,
                                  int64_t                     start_time_stamp,
                                  int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void*       localTimePack;
    int         position;
    MPI_Request tp_request;
    int         global_dest;

    /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
    if ( scorep_mpiprofiling_rank_to_pe( dest, comm, &global_dest ) != 0 )
    {
        return;
    }

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* store communoication partner rank and tag for later piggyback send operation */
    online_analysis_pod->tp_comm_partner = global_dest;
    online_analysis_pod->tp_tag          = tag;

    /* no rank translation done at completion */
    online_analysis_pod->group = MPI_GROUP_NULL;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Recv_init( void*        buf,
                                 int          count,
                                 MPI_Datatype datatype,
                                 int          source,
                                 int          tag,
                                 MPI_Comm     comm,
                                 MPI_Request* request,
                                 int64_t      start_time_stamp,
                                 int          return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );

    MPI_Group group         = MPI_GROUP_NULL;
    int       global_source = 0;

    if ( source != MPI_ANY_SOURCE )
    {
        /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
        if ( scorep_mpiprofiling_rank_to_pe( source, comm, &global_source ) != 0 )
        {
            return;
        }
    }
    else
    {
        /*
         * get the internal MPI_Group of the communicator for later rank
         * translation, can't store the MPI_Comm, because it may have gone
         * until completion of this request
         */
        if ( scorep_mpiprofiling_get_group( comm, &group ) != 0 )
        {
            return;
        }
    }

    /* allocate memory for the online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = malloc( sizeof( scorep_wait_state_request_tracking ) );

    /* If source is MPI_ANY_SOURCE, set the wild-card source flag for this request */
    if ( source == MPI_ANY_SOURCE )
    {
        online_analysis_pod->tp_comm_partner_wc = 1;
    }
    else
    {
        online_analysis_pod->tp_comm_partner_wc = 0;
    }

    /* If tag is MPI_TAG_ANY, set the wild-card tag flag for this request */
    if ( tag == MPI_ANY_TAG )
    {
        online_analysis_pod->tp_tag_wc = 1;
    }
    else
    {
        online_analysis_pod->tp_tag_wc = 0;
    }

    /* Store global rank of the source and the communication tag associated with the recv operation */
    online_analysis_pod->tp_comm_partner = global_source;
    online_analysis_pod->tp_tag          = tag;

    /* store the associated MPI_Group for later rank translation of the source */
    online_analysis_pod->group = group;

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* orig_req = scorep_mpi_request_get( *request );
    orig_req->payload.p2p.online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Start( MPI_Request* request,
                             int64_t      start_time_stamp,
                             int          return_val )
{
    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );

    if ( !scorep_request )
    {
        return;
    }
    if ( !scorep_request->payload.p2p.online_analysis_pod ||
         !( scorep_request->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) ||
         !( scorep_request->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) )
    {
        return;
    }

    /* get online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_request->payload.p2p.online_analysis_pod;

    if ( ( scorep_request->request_type == SCOREP_MPI_REQUEST_TYPE_SEND ) && ( scorep_request->payload.p2p.dest != MPI_PROC_NULL ) )
    {
        UTILS_DEBUG( "myrank = %d", scorep_mpiprofiling_myrank );
        void*       localTimePack;
        int         position;
        MPI_Request tp_request;

        /* get a free timepack buffer from the pool*/
        scorep_mpiprofile_get_timepack_from_pool( &localTimePack, &position );

        /* intialize local timepack with the time stamp and the global rank of this proc*/
        scorep_mpiprofile_init_timepack( localTimePack, start_time_stamp );

        /* initiate a send of the local timepack to the destination process using a mpiprofile world communicator and the translated global rank*/
        PMPI_Isend(  localTimePack,
                     MPIPROFILER_TIMEPACK_BUFSIZE,
                     MPI_PACKED,
                     online_analysis_pod->tp_comm_partner,
                     online_analysis_pod->tp_tag,
                     scorep_mpiprofiling_world_comm_dup.comm,
                     &tp_request );

        /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
        scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );

        /* store tiempack request position in the timpacks pool for later use in send cancel operation*/
        online_analysis_pod->send_tp_position_in_pool = position;

        /* Store timepack request for later use in case of send cancel.
         * In general we don't need to complete the timepack send operation, since it will
         * be automatically checked by the timepack pool. Only in case of original send cancel
         * operation timpack request is needed to cancel the timepack send aswell*/
        online_analysis_pod->tp_request = tp_request;
    }
    else if ( scorep_request->request_type == SCOREP_MPI_REQUEST_TYPE_RECV )
    {
        // what the hell will I do here??... nothing!
    }
}

void
SCOREP_Hooks_Pre_MPI_Request_free( scorep_mpi_request* scorep_req )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    if ( !scorep_req )
    {
        return;
    }
    if ( !scorep_req->payload.p2p.online_analysis_pod )
    {
        return;
    }

//	scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_req->payload.p2p.online_analysis_pod;
//		MPI_Status s;
//	PMPI_Wait(&(online_analysis_pod->tp_request),&s);
//	int canceled_flag=0;
//    int err=PMPI_Test_cancelled(&s,&canceled_flag);
//    if(err!=MPI_SUCCESS)
//      printf("mpi failed\n");
//    if(canceled_flag)
//      printf("canceled successfully\n");

    /* free online analysis request pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_req->payload.p2p.online_analysis_pod;
    if ( online_analysis_pod->group != MPI_GROUP_NULL )
    {
        PMPI_Group_free( &online_analysis_pod->group );
    }
    free( scorep_req->payload.p2p.online_analysis_pod );
    scorep_req->payload.p2p.online_analysis_pod = NULL;
}

void
SCOREP_Hooks_Post_MPI_Cancel( scorep_mpi_request* scorep_req )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    if ( !scorep_req )
    {
        return;
    }
    if ( !scorep_req->payload.p2p.online_analysis_pod )
    {
        return;
    }
    /* cancel piggybacked send operation. In case of recv nothing has to be done, since the piggybacked recv
     * is triggered only at the completetion time of the original recv*/
    if ( scorep_req->request_type == SCOREP_MPI_REQUEST_TYPE_SEND )
    {
        scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_req->payload.p2p.online_analysis_pod;
        PMPI_Cancel( &( online_analysis_pod->tp_request ) );
    }
}

void
SCOREP_Hooks_Post_MPI_Asynch_Complete( scorep_mpi_request* orig_req,
                                       MPI_Status*         status,
                                       int64_t             start_time_stamp )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );

    int global_source = MPI_PROC_NULL;
    int orig_tag;

    /* get request tracking flags */
    if ( orig_req == NULL )
    {
        return;
    }

    /* get online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )orig_req->payload.p2p.online_analysis_pod;

    /* if the online wait states analysis request tracking pod is NULL then no piggyback message is sent.
     * This happens when the global rank of the source is out of the MPI_COMM_WORLD of this proc*/
    if ( online_analysis_pod == NULL )
    {
        return;
    }

    /* test if the communication associated with the request was canceled */
    int canceled_flag = 0;
    PMPI_Test_cancelled( status, &canceled_flag );

    /* Do online mpi wait states analysis only if it is a recv operation.
     * If waiting for a send operation, nothing to be done here. The associated piggyback send operation
     * will be completed by the timepack pool mamnagement*/
    if ( ( orig_req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV ) && !canceled_flag )
    {
        /* if the associated recv operation had a wild-card as a source, get the source from status
         * and translate it to the global rank. Else get it from the pod*/
        if ( online_analysis_pod->tp_comm_partner_wc == 1 )
        {
            if ( scorep_mpiprofiling_rank_to_pe_by_group( status->MPI_SOURCE, online_analysis_pod->group, &global_source ) != 0 )
            {
                global_source = MPI_PROC_NULL;
                UTILS_DEBUG( "WARNING: The source global rank of the non-blocking receive operation using wild-card as a source could not be recovered. Online MPI Wait States analysis might produce wrong results or hang the application." );
            }
        }
        else
        {
            global_source = online_analysis_pod->tp_comm_partner;
        }


        /* if the associated recv operation had a wild-card as a tag get the tag from the status.
         * Else get it from the pod*/
        if ( online_analysis_pod->tp_tag_wc == 1 )
        {
            orig_tag = status->MPI_TAG;
        }
        else
        {
            orig_tag = online_analysis_pod->tp_tag;
        }

        if ( global_source != MPI_PROC_NULL )
        {
            /* Get buffer for receiving remote timepack */
            void* remoteTimePack = scorep_mpiprofile_get_remote_time_pack();

            MPI_Status s;
            /* Receive remote timepack */
            PMPI_Recv(      remoteTimePack,
                            MPIPROFILER_TIMEPACK_BUFSIZE,
                            MPI_PACKED,
                            global_source,
                            orig_tag,
                            scorep_mpiprofiling_world_comm_dup.comm,
                            &s );

            /* Release timepacks */
            scorep_mpiprofile_release_remote_time_pack( remoteTimePack );


            UTILS_DEBUG( "source %d, tag %d, my_time %llu", global_source, orig_tag, start_time_stamp );
        }
    }

    /* Free the memory of the pod unless the request is persistent*/
    if ( !( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) )
    {
        SCOREP_Hooks_Pre_MPI_Request_free( orig_req );
        UTILS_DEBUG( "online_pod freed" );
    }
    UTILS_DEBUG_EXIT( "myrank = %d", scorep_mpiprofiling_myrank );
}

void
SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( scorep_mpi_request* orig_req,
                                                MPI_Status*         status,
                                                int64_t             start_time_stamp )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );

    int global_source = MPI_PROC_NULL;
    int orig_tag;

    /* get request tracking flags */
    if ( orig_req == NULL )
    {
        return;
    }

    /* get online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )orig_req->payload.p2p.online_analysis_pod;

    /* if the online wait states analysis request tracking pod is NULL then no piggyback message is sent.
     * This happens when the global rank of the source is out of the MPI_COMM_WORLD of this proc*/
    if ( online_analysis_pod == NULL )
    {
        return;
    }

    /* test if the communication associated with the request was canceled */
    int canceled_flag = 0;
    PMPI_Test_cancelled( status, &canceled_flag );

    /* Do online mpi wait states analysis only if it is a recv operation.
     * If waiting for a send operation, nothing to be done here. The associated piggyback send operation
     * will be completed by the timepack pool mamnagement*/
    if ( ( orig_req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV ) && !canceled_flag )
    {
        /* if the associated recv operation had a wild-card as a source, get the source from status
         * and translate it to the global rank. Else get it from the pod*/
        if ( online_analysis_pod->tp_comm_partner_wc == 1 )
        {
            if ( scorep_mpiprofiling_rank_to_pe_by_group( status->MPI_SOURCE, online_analysis_pod->group, &global_source ) != 0 )
            {
                global_source = MPI_PROC_NULL;
                UTILS_DEBUG( "WARNING: The source global rank of the non-blocking receive operation using wild-card as a source could not be recovered. Online MPI Wait States analysis might produce wrong results or hang the application." );
            }
        }
        else
        {
            global_source = online_analysis_pod->tp_comm_partner;
        }


        /* if the associated recv operation had a wild-card as a tag get the tag from the status.
         * Else get it from the pod*/
        if ( online_analysis_pod->tp_tag_wc == 1 )
        {
            orig_tag = status->MPI_TAG;
        }
        else
        {
            orig_tag = online_analysis_pod->tp_tag;
        }

        if ( global_source != MPI_PROC_NULL )
        {
            /* Get buffer initialized with local timepack */
            void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
            /* Get buffer for receiving remote timepack */
            void* remoteTimePack = scorep_mpiprofile_get_remote_time_pack();

            MPI_Status s;
            /* Receive remote timepack */
            PMPI_Recv(      remoteTimePack,
                            MPIPROFILER_TIMEPACK_BUFSIZE,
                            MPI_PACKED,
                            global_source,
                            orig_tag,
                            scorep_mpiprofiling_world_comm_dup.comm,
                            &s );
            /* Compare local and remote timepacks */
            scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                                   localTimePack );
            /* Release timepacks */
            scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
            scorep_mpiprofile_release_local_time_pack( localTimePack );

            UTILS_DEBUG( "source %d, tag %d, my_time %llu", global_source, orig_tag, start_time_stamp );
        }
    }

    /* Free the memory of the pod unless the request is persistent*/
    if ( !( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) )
    {
        SCOREP_Hooks_Pre_MPI_Request_free( orig_req );
        UTILS_DEBUG( "online_pod freed" );
    }
    UTILS_DEBUG_EXIT( "myrank = %d", scorep_mpiprofiling_myrank );
}

void
SCOREP_Hooks_Post_MPI_Exscan( SCOREP_MPI_CONST_DECL void* sendbuf,
                              void*                       recvbuf,
                              int                         count,
                              MPI_Datatype                datatype,
                              MPI_Op                      op,
                              MPI_Comm                    comm,
                              uint64_t                    start_time_stamp,
                              int                         return_val )
{
}

void
SCOREP_Hooks_Post_MPI_Alltoallw( SCOREP_MPI_CONST_DECL void*        sendbuf,
                                 SCOREP_MPI_CONST_DECL int          sendcounts[],
                                 SCOREP_MPI_CONST_DECL int          sdispls[],
                                 SCOREP_MPI_CONST_DECL MPI_Datatype sendtypes[],
                                 void*                              recvbuf,
                                 SCOREP_MPI_CONST_DECL int          recvcounts[],
                                 SCOREP_MPI_CONST_DECL int          rdispls[],
                                 SCOREP_MPI_CONST_DECL MPI_Datatype recvtypes[],
                                 MPI_Comm                           comm,
                                 uint64_t                           start_time_stamp,
                                 int                                return_val )
{
}

void
SCOREP_Hooks_Post_MPI_Reduce_scatter( SCOREP_MPI_CONST_DECL void* sendbuf,
                                      void*                       recvbuf,
                                      SCOREP_MPI_CONST_DECL int*  recvcounts,
                                      MPI_Datatype                datatype,
                                      MPI_Op                      op,
                                      MPI_Comm                    comm,
                                      uint64_t                    start_time_stamp,
                                      int                         return_val )
{
}

void
SCOREP_Hooks_Post_MPI_Reduce_scatter_block( SCOREP_MPI_CONST_DECL void* sendbuf,
                                            void*                       recvbuf,
                                            int                         recvcount,
                                            MPI_Datatype                datatype,
                                            MPI_Op                      op,
                                            MPI_Comm                    comm,
                                            uint64_t                    start_time_stamp,
                                            int                         return_val )
{
}

void
SCOREP_Hooks_Post_MPI_Scan( SCOREP_MPI_CONST_DECL void* sendbuf,
                            void*                       recvbuf,
                            int                         count,
                            MPI_Datatype                datatype,
                            MPI_Op                      op,
                            MPI_Comm                    comm,
                            uint64_t                    start_time_stamp,
                            int                         return_val )
{
}

/*----------------------------------------------
 * NxN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Alltoall( SCOREP_MPI_CONST_DECL void* sendbuf,
                                int                         sendcount,
                                MPI_Datatype                sendtype,
                                void*                       recvbuf,
                                int                         recvcount,
                                MPI_Datatype                recvtype,
                                MPI_Comm                    comm,
                                uint64_t                    start_time_stamp,
                                int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    MPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );
    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE, \
                    MPI_PACKED,
                    comm );
    scorep_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                              localTimePack,
                                              commSize );

    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Alltoallv( SCOREP_MPI_CONST_DECL void* sendbuf,
                                 SCOREP_MPI_CONST_DECL int*  sendcounts,
                                 SCOREP_MPI_CONST_DECL int*  sdispls,
                                 MPI_Datatype                sendtype,
                                 void*                       recvbuf,
                                 SCOREP_MPI_CONST_DECL int*  recvcounts,
                                 SCOREP_MPI_CONST_DECL int*  rdispls,
                                 MPI_Datatype                recvtype,
                                 MPI_Comm                    comm,
                                 uint64_t                    start_time_stamp,
                                 int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    MPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );
    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE, \
                    MPI_PACKED,
                    comm );
    scorep_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                              localTimePack,
                                              commSize );

    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Barrier( MPI_Comm comm,
                               uint64_t start_time_stamp,
                               int      return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    MPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );
    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE, \
                    MPI_PACKED,
                    comm );
    scorep_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                              localTimePack,
                                              commSize );
    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

/*----------------------------------------------
 * Nx1 pre- and post- communication hooks
   -----------------------------------------------*/

/**
 * Post-communication hook for MPI_Recv
 */
void
SCOREP_Hooks_Post_MPI_Gather( SCOREP_MPI_CONST_DECL void* sendbuf,
                              int                         sendcount,
                              MPI_Datatype                sendtype,
                              void*                       recvbuf,
                              int                         recvcount,
                              MPI_Datatype                recvtype,
                              int                         root,
                              MPI_Comm                    comm,
                              uint64_t                    start_time_stamp,
                              int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    if ( scorep_mpiprofiling_myrank == root )
    {
        PMPI_Comm_size( comm, &commSize );
        remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );
    }
    PMPI_Gather(    localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );

    if ( scorep_mpiprofiling_myrank == root )
    {
        scorep_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                               commSize );
        scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    }
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Gatherv( SCOREP_MPI_CONST_DECL void* sendbuf,
                               int                         sendcount,
                               MPI_Datatype                sendtype,
                               void*                       recvbuf,
                               SCOREP_MPI_CONST_DECL int*  recvcounts,
                               SCOREP_MPI_CONST_DECL int*  displs,
                               MPI_Datatype                recvtype,
                               int                         root,
                               MPI_Comm                    comm,
                               uint64_t                    start_time_stamp,
                               int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    if ( scorep_mpiprofiling_myrank == root )
    {
        PMPI_Comm_size( comm, &commSize );
        remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );
    }
    PMPI_Gather(    localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );

    if ( scorep_mpiprofiling_myrank == root )
    {
        scorep_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                               commSize );
        scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    }
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Reduce( SCOREP_MPI_CONST_DECL void* sendbuf,
                              void*                       recvbuf,
                              int                         count,
                              MPI_Datatype                datatype,
                              MPI_Op                      op,
                              int                         root,
                              MPI_Comm                    comm,
                              uint64_t                    start_time_stamp,
                              int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );

    PMPI_Gather(    localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );

    if ( scorep_mpiprofiling_myrank == root )
    {
        scorep_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                               commSize );
    }
    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Allreduce( SCOREP_MPI_CONST_DECL void* sendbuf,
                                 void*                       recvbuf,
                                 int                         count,
                                 MPI_Datatype                datatype,
                                 MPI_Op                      op,
                                 MPI_Comm                    comm,
                                 uint64_t                    start_time_stamp,
                                 int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );

    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    comm );


    scorep_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                              localTimePack,
                                              commSize );
    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );

    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Allgather( SCOREP_MPI_CONST_DECL void* sendbuf,
                                 int                         sendcount,
                                 MPI_Datatype                sendtype,
                                 void*                       recvbuf,
                                 int                         recvcount,
                                 MPI_Datatype                recvtype,
                                 MPI_Comm                    comm,
                                 uint64_t                    start_time_stamp,
                                 int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );

    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    comm );


    scorep_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                              localTimePack,
                                              commSize );
    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );

    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Allgatherv( SCOREP_MPI_CONST_DECL void* sendbuf,
                                  int                         sendcount,
                                  MPI_Datatype                sendtype,
                                  void*                       recvbuf,
                                  SCOREP_MPI_CONST_DECL int*  recvcounts,
                                  SCOREP_MPI_CONST_DECL int*  displs,
                                  MPI_Datatype                recvtype,
                                  MPI_Comm                    comm,
                                  uint64_t                    start_time_stamp,
                                  int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = scorep_mpiprofile_get_remote_time_packs( commSize );

    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    comm );


    scorep_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                              localTimePack,
                                              commSize );
    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

/*----------------------------------------------
 * 1xN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Bcast( void*        buffer,
                             int          count,
                             MPI_Datatype datatype,
                             int          root,
                             MPI_Comm     comm,
                             uint64_t     start_time_stamp,
                             int          return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );

    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( scorep_mpiprofiling_myrank != root )
    {
        scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                               localTimePack );
    }
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Scatter( SCOREP_MPI_CONST_DECL void* sendbuf,
                               int                         sendcount,
                               MPI_Datatype                sendtype,
                               void*                       recvbuf,
                               int                         recvcount,
                               MPI_Datatype                recvtype,
                               int                         root,
                               MPI_Comm                    comm,
                               uint64_t                    start_time_stamp,
                               int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( scorep_mpiprofiling_myrank != root )
    {
        scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                               localTimePack );
    }
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}
void
SCOREP_Hooks_Post_MPI_Scatterv( SCOREP_MPI_CONST_DECL void* sendbuf,
                                SCOREP_MPI_CONST_DECL int*  sendcounts,
                                SCOREP_MPI_CONST_DECL int*  displs,
                                MPI_Datatype                sendtype,
                                void*                       recvbuf,
                                int                         recvcount,
                                MPI_Datatype                recvtype,
                                int                         root,
                                MPI_Comm                    comm,
                                uint64_t                    start_time_stamp,
                                int                         return_val )
{
    UTILS_DEBUG_ENTRY( "myrank = %d", scorep_mpiprofiling_myrank );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( scorep_mpiprofiling_myrank != root )
    {
        scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                               localTimePack );
    }
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}
