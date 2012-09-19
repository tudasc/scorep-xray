/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
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
 * @file        SCOREP_MPIHooks.c
 * @maintainer  Yury Olenyik <oleynik@in.tum.de>
 *
 * @brief   mpi hooks functions to be used by MPI profiling
 *
 * @status alpha
 *
 */

#include <config.h>

#include "SCOREP_Mpi.h"
#include "SCOREP_MPIHooks.h"
#include <UTILS_Debug.h>
#include "scorep_mpiprofile.h"

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
SCOREP_Hooks_Pre_MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp
)
{
}

/**
 * Post-communication hook for MPI_Send
 */
void
SCOREP_Hooks_Post_MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_value
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

void
SCOREP_Hooks_Post_MPI_Bsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

void
SCOREP_Hooks_Post_MPI_Ssend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

void
SCOREP_Hooks_Post_MPI_Rsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
                 &tp_request );

    /* store the request in the pool, it is later used to check whether the asscoiated buffer could be reused*/
    scorep_mpiprofile_store_timepack_request_in_pool( tp_request, position );
}

/**
 * Pre-communication hook for MPI_Recv
 */
void
SCOREP_Hooks_Pre_MPI_Recv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Status*  status,
    uint64_t     start_time_stamp
)
{
}

/**
 * Post-communication hook for MPI_Recv
 */
void
SCOREP_Hooks_Post_MPI_Recv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Status*  status,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                    world_comm_dup.comm,
                    &s );

    scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                           localTimePack );
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Isend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, position %d, dest %d, tag %d, time %llu\n", __func__, position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Issend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, position %d, dest %d, tag %d, time %llu\n", __func__, position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Ibsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, position %d, dest %d, tag %d, time %llu\n", __func__, position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Irsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                 world_comm_dup.comm,
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, position %d, dest %d, tag %d, time %llu\n", __func__, position, global_dest, tag, start_time_stamp );
}

void
SCOREP_Hooks_Post_MPI_Irecv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    int global_source = 0;

    if ( source != MPI_ANY_SOURCE )
    {
        /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
        if ( scorep_mpiprofiling_rank_to_pe( source, comm, &global_source ) != 0 )
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* orig_req = scorep_mpi_request_get( *request );
    orig_req->online_analysis_pod = ( void* )online_analysis_pod;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, source(*-%d,%d), tag(*-%d,%d)\n", __func__, online_analysis_pod->tp_comm_partner_wc, global_source, online_analysis_pod->tp_tag_wc, tag );
}

void
SCOREP_Hooks_Post_MPI_Send_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Ssend_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Rsend_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Bsend_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );
    scorep_request->online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Recv_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );

    int global_source = 0;

    if ( source != MPI_ANY_SOURCE )
    {
        /* translate rank within the comm to the rank in the mpiprofiler world communicator, if not translated don't profile */
        if ( scorep_mpiprofiling_rank_to_pe( source, comm, &global_source ) != 0 )
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

    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* orig_req = scorep_mpi_request_get( *request );
    orig_req->online_analysis_pod = ( void* )online_analysis_pod;
}

void
SCOREP_Hooks_Post_MPI_Start
(
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
    /* get scorep internal request tracking datastructure */
    scorep_mpi_request* scorep_request = scorep_mpi_request_get( *request );

    if ( !scorep_request )
    {
        return;
    }
    if ( !scorep_request->online_analysis_pod ||
         !( scorep_request->flags & SCOREP_MPI_REQUEST_IS_PERSISTENT ) ||
         !( scorep_request->flags & SCOREP_MPI_REQUEST_IS_ACTIVE ) )
    {
        return;
    }

    /* get online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_request->online_analysis_pod;

    if ( ( scorep_request->flags & SCOREP_MPI_REQUEST_SEND ) && ( scorep_request->dest != MPI_PROC_NULL ) )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
                     world_comm_dup.comm,
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
    else if ( scorep_request->flags & SCOREP_MPI_REQUEST_RECV )
    {
        // what the hell will I do here??... nothing!
    }
}

void
SCOREP_Hooks_Pre_MPI_Request_free
(
    scorep_mpi_request* scorep_req
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    if ( !scorep_req )
    {
        return;
    }
    if ( !scorep_req->online_analysis_pod )
    {
        return;
    }

//	scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_req->online_analysis_pod;
//		MPI_Status s;
//	PMPI_Wait(&(online_analysis_pod->tp_request),&s);
//	int canceled_flag=0;
//    int err=PMPI_Test_cancelled(&s,&canceled_flag);
//    if(err!=MPI_SUCCESS)
//      printf("mpi failed\n");
//    if(canceled_flag)
//      printf("canceled successfully\n");

    /* free online analysis request pod */
    free( scorep_req->online_analysis_pod );
}

void
SCOREP_Hooks_Post_MPI_Cancel
(
    scorep_mpi_request* scorep_req
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    if ( !scorep_req )
    {
        return;
    }
    if ( !scorep_req->online_analysis_pod )
    {
        return;
    }
    /* cancel piggybacked send operation. In case of recv nothing has to be done, since the piggybacked recv
     * is triggered only at the completetion time of the original recv*/
    if ( scorep_req->flags & SCOREP_MPI_REQUEST_SEND )
    {
        scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )scorep_req->online_analysis_pod;
        PMPI_Cancel( &( online_analysis_pod->tp_request ) );
    }
}

void
SCOREP_Hooks_Post_MPI_Asynch_Complete
(
    scorep_mpi_request* orig_req,
    MPI_Status*         status,
    int64_t             start_time_stamp
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );

    int global_source = MPI_PROC_NULL;
    int orig_tag;

    /* get request tracking flags */
    if ( orig_req == NULL )
    {
        return;
    }
    unsigned flags = orig_req->flags;

    /* get online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )orig_req->online_analysis_pod;

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
    if ( ( flags & SCOREP_MPI_REQUEST_RECV ) && !canceled_flag )
    {
        /* if the associated recv operation had a wild-card as a source, get the source from status
         * and translate it to the global rank. Else get it from the pod*/
        if ( online_analysis_pod->tp_comm_partner_wc == 1 )
        {
            MPI_Comm orig_comm = orig_req->comm;
            if ( scorep_mpiprofiling_rank_to_pe( status->MPI_SOURCE, orig_comm, &global_source ) != 0 )
            {
                global_source = MPI_PROC_NULL;
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: WARNING: The source global rank of the non-blocking receive operation using wild-card as a source could not be recovered. Online MPI Wait States analysis might produce wrong results or hang the application.\n" );
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
                            world_comm_dup.comm,
                            &s );

            /* Release timepacks */
            scorep_mpiprofile_release_remote_time_pack( remoteTimePack );


            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, source %d, tag %d, my_time %llu\n", __func__, global_source, orig_tag, start_time_stamp );
        }
    }

    /* Free the memory of the pod unless the request is persistent*/
    if ( !( flags & SCOREP_MPI_REQUEST_IS_PERSISTENT ) )
    {
        free( orig_req->online_analysis_pod );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, online_pod freed\n", __func__ );
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING,  "EXIT: HOOK : myrank = %d,%s\n", myrank, __func__ );
}

void
SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking
(
    scorep_mpi_request* orig_req,
    MPI_Status*         status,
    int64_t             start_time_stamp
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );

    int global_source = MPI_PROC_NULL;
    int orig_tag;

    /* get request tracking flags */
    if ( orig_req == NULL )
    {
        return;
    }
    unsigned flags = orig_req->flags;

    /* get online wait states analysis request tracking pod */
    scorep_wait_state_request_tracking* online_analysis_pod = ( scorep_wait_state_request_tracking* )orig_req->online_analysis_pod;

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
    if ( ( flags & SCOREP_MPI_REQUEST_RECV ) && !canceled_flag )
    {
        /* if the associated recv operation had a wild-card as a source, get the source from status
         * and translate it to the global rank. Else get it from the pod*/
        if ( online_analysis_pod->tp_comm_partner_wc == 1 )
        {
            MPI_Comm orig_comm = orig_req->comm;
            if ( scorep_mpiprofiling_rank_to_pe( status->MPI_SOURCE, orig_comm, &global_source ) != 0 )
            {
                global_source = MPI_PROC_NULL;
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: WARNING: The source global rank of the non-blocking receive operation using wild-card as a source could not be recovered. Online MPI Wait States analysis might produce wrong results or hang the application.\n" );
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
                            world_comm_dup.comm,
                            &s );
            /* Compare local and remote timepacks */
            scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                                   localTimePack );
            /* Release timepacks */
            scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
            scorep_mpiprofile_release_local_time_pack( localTimePack );

            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, source %d, tag %d, my_time %llu\n", __func__, global_source, orig_tag, start_time_stamp );
        }
    }

    /* Free the memory of the pod unless the request is persistent*/
    if ( !( flags & SCOREP_MPI_REQUEST_IS_PERSISTENT ) )
    {
        free( orig_req->online_analysis_pod );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, online_pod freed\n", __func__ );
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING,  "EXIT: HOOK : myrank = %d,%s\n", myrank, __func__ );
}

void
SCOREP_Hooks_Post_MPI_Exscan
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SCOREP_Hooks_Post_MPI_Alltoallw
(
    void*        sendbuf,
    int          sendcounts[],
    int          sdispls[],
    MPI_Datatype sendtypes[],
    void*        recvbuf,
    int          recvcounts[],
    int          rdispls[],
    MPI_Datatype recvtypes[],
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SCOREP_Hooks_Post_MPI_Reduce_scatter
(
    void*        sendbuf,
    void*        recvbuf,
    int*         recvcounts,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SCOREP_Hooks_Post_MPI_Reduce_scatter_block
(
    void*        sendbuf,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SCOREP_Hooks_Post_MPI_Scan
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

/*----------------------------------------------
 * NxN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Alltoall
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
SCOREP_Hooks_Post_MPI_Alltoallv
(
    void*        sendbuf,
    int*         sendcounts,
    int*         sdispls,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int*         recvcounts,
    int*         rdispls,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
SCOREP_Hooks_Post_MPI_Barrier
(
    MPI_Comm comm,
    uint64_t start_time_stamp,
    int      return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
SCOREP_Hooks_Post_MPI_Gather
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    if ( myrank == root )
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

    if ( myrank == root )
    {
        scorep_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                               commSize );
        scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    }
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Gatherv
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int*         recvcounts,
    int*         displs,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    if ( myrank == root )
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

    if ( myrank == root )
    {
        scorep_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                               commSize );
        scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    }
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Reduce
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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

    if ( myrank == root )
    {
        scorep_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                               commSize );
    }
    scorep_mpiprofile_release_remote_time_packs( remoteTimePacks );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Allreduce
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
SCOREP_Hooks_Post_MPI_Allgather
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
SCOREP_Hooks_Post_MPI_Allgatherv
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int*         recvcounts,
    int*         displs,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
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
SCOREP_Hooks_Post_MPI_Bcast
(
    void*        buffer,
    int          count,
    MPI_Datatype datatype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );

    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( myrank != root )
    {
        scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                               localTimePack );
    }
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}

void
SCOREP_Hooks_Post_MPI_Scatter
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( myrank != root )
    {
        scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                               localTimePack );
    }
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}
void
SCOREP_Hooks_Post_MPI_Scatterv
(
    void*        sendbuf,
    int*         sendcounts,
    int*         displs,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val

)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s", myrank, __func__ );
    void* localTimePack = scorep_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = scorep_mpiprofile_get_remote_time_pack();
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( myrank != root )
    {
        scorep_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                               localTimePack );
    }
    scorep_mpiprofile_release_remote_time_pack( remoteTimePack );
    scorep_mpiprofile_release_local_time_pack( localTimePack );
}
