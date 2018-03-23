/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 *
 */

#include <config.h>

#include <scorep_ipc.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME SHMEM
#include <UTILS_Debug.h>

#include <UTILS_Debug.h>
#include <scorep_shmem_internal.h>
#include <stdlib.h>
#include <string.h>


#ifndef CALL_SHMEM
#error Macro 'CALL_SHMEM' is not defined
#endif


#define CEIL( a, b )       ( ( ( a ) / ( b ) ) + ( ( ( a ) % ( b ) ) > 0 ? 1 : 0 ) )
#define ROUNDUPTO( a, b )  ( CEIL( a, b ) * b )
#define MIN( a, b )        ( a > b ? a : b )

#define INVALID_TRANSFER_STATE     -1

#define TRANSFER_START              1
#define TRANSFER_SENDER_COMPLETE    2
#define TRANSFER_RECEIVER_COMPLETE  3

#define BUFFER_SIZE                 ( 8 * 1024 )

struct SCOREP_Ipc_Group
{
    int pe_start;
    int log_pe_stride;
    int pe_size;
    int is_group_set;
};

SCOREP_Ipc_Group scorep_ipc_group_world;
static int       sizeof_ipc_datatypes[ SCOREP_IPC_NUMBER_OF_DATATYPES ];


/*
 * Helper functions
 */

static inline int
resolve_group_start( SCOREP_Ipc_Group* group )
{
    return group ? group->pe_start : scorep_ipc_group_world.pe_start;
}

static inline int
resolve_group_stride( SCOREP_Ipc_Group* group )
{
    return group ? group->log_pe_stride : scorep_ipc_group_world.log_pe_stride;
}

static inline int
resolve_group_size( SCOREP_Ipc_Group* group )
{
    return group ? group->pe_size : scorep_ipc_group_world.pe_size;
}


static void*   symmetric_buffer_a;
static void*   symmetric_buffer_b;
static int*    transfer_counter;
static int*    transfer_status;
static int*    current_ready_pe;
static long*   barrier_psync;
static long*   bcast_psync;
static long*   collect_psync;
static long*   reduce_psync;
static double* pwork;
static size_t  current_pwork_size;
static void*
get_pwork( size_t size,
           int    count );

/*
 * IPC API functions
 */

void
SCOREP_Ipc_Init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Duplicate 'comm world' */
    scorep_ipc_group_world.pe_start      = 0;
    scorep_ipc_group_world.log_pe_stride = 0;
    scorep_ipc_group_world.pe_size       = XCALL_SHMEM( SCOREP_SHMEM_N_PES )();
    scorep_ipc_group_world.is_group_set  = 1;

    /* Assign data types */
#define SCOREP_SHMEM_BYTE          sizeof( unsigned char )
#define SCOREP_SHMEM_CHAR          sizeof( char )
#define SCOREP_SHMEM_UNSIGNED_CHAR sizeof( unsigned char )
#define SCOREP_SHMEM_INT           sizeof( int )
#define SCOREP_SHMEM_UNSIGNED      sizeof( unsigned )
#define SCOREP_SHMEM_UNSIGNED      sizeof( unsigned )
#define SCOREP_SHMEM_INT32_T       sizeof( int32_t )
#define SCOREP_SHMEM_UINT32_T      sizeof( uint32_t )
#define SCOREP_SHMEM_INT64_T       sizeof( int64_t )
#define SCOREP_SHMEM_UINT64_T      sizeof( uint64_t )
#define SCOREP_SHMEM_DOUBLE        sizeof( double )

#define SCOREP_IPC_DATATYPE( datatype ) \
    sizeof_ipc_datatypes[ SCOREP_IPC_ ## datatype ] = SCOREP_SHMEM_ ## datatype;
    SCOREP_IPC_DATATYPES
#undef SCOREP_IPC_DATATYPE

#undef SCOREP_SHMEM_BYTE
#undef SCOREP_SHMEM_CHAR
#undef SCOREP_SHMEM_UNSIGNED_CHAR
#undef SCOREP_SHMEM_INT
#undef SCOREP_SHMEM_UNSIGNED
#undef SCOREP_SHMEM_INT32
#undef SCOREP_SHMEM_UINT32
#undef SCOREP_SHMEM_INT64
#undef SCOREP_SHMEM_UINT64
#undef SCOREP_SHMEM_DOUBLE

    /* Allocate memory in symmetric heap */
    symmetric_buffer_a = CALL_SHMEM( shmalloc )( BUFFER_SIZE );
    UTILS_ASSERT( symmetric_buffer_a );

    symmetric_buffer_b = CALL_SHMEM( shmalloc )( BUFFER_SIZE );
    UTILS_ASSERT( symmetric_buffer_b );

    transfer_status = ( int* )CALL_SHMEM( shmalloc )( sizeof( int ) );
    UTILS_ASSERT( transfer_status );
    *transfer_status = INVALID_TRANSFER_STATE;

    current_ready_pe = ( int* )CALL_SHMEM( shmalloc )( sizeof( int ) );
    UTILS_ASSERT( current_ready_pe );
    *current_ready_pe = -1;

    transfer_counter = ( int* )CALL_SHMEM( shmalloc )( scorep_ipc_group_world.pe_size * sizeof( int ) );
    UTILS_ASSERT( transfer_counter );
    memset( transfer_counter, 0, scorep_ipc_group_world.pe_size * sizeof( int ) );

    barrier_psync = CALL_SHMEM( shmalloc )( _SHMEM_BARRIER_SYNC_SIZE * sizeof( long ) );
    UTILS_ASSERT( barrier_psync );
    for ( uint32_t i = 0; i < _SHMEM_BARRIER_SYNC_SIZE; i++ )
    {
        barrier_psync[ i ] = _SHMEM_SYNC_VALUE;
    }

    bcast_psync = CALL_SHMEM( shmalloc )( _SHMEM_BCAST_SYNC_SIZE * sizeof( long ) );
    UTILS_ASSERT( bcast_psync );
    for ( uint32_t i = 0; i < _SHMEM_BCAST_SYNC_SIZE; i++ )
    {
        bcast_psync[ i ] = _SHMEM_SYNC_VALUE;
    }

    collect_psync = CALL_SHMEM( shmalloc )( _SHMEM_COLLECT_SYNC_SIZE * sizeof( long ) );
    UTILS_ASSERT( collect_psync );
    for ( uint32_t i = 0; i < _SHMEM_COLLECT_SYNC_SIZE; i++ )
    {
        collect_psync[ i ] = _SHMEM_SYNC_VALUE;
    }

    reduce_psync = CALL_SHMEM( shmalloc )( _SHMEM_REDUCE_SYNC_SIZE * sizeof( long ) );
    UTILS_ASSERT( reduce_psync );
    for ( uint32_t i = 0; i < _SHMEM_REDUCE_SYNC_SIZE; i++ )
    {
        reduce_psync[ i ] = _SHMEM_SYNC_VALUE;
    }

    current_pwork_size = _SHMEM_REDUCE_MIN_WRKDATA_SIZE * sizeof( double );
    pwork              = CALL_SHMEM( shmalloc )( current_pwork_size );
    UTILS_ASSERT( pwork );

    CALL_SHMEM( shmem_barrier_all )();
}


void
SCOREP_Ipc_Finalize( void )
{
    UTILS_ASSERT( symmetric_buffer_a );
    CALL_SHMEM( shfree )( symmetric_buffer_a );
    symmetric_buffer_a = NULL;

    UTILS_ASSERT( symmetric_buffer_b );
    CALL_SHMEM( shfree )( symmetric_buffer_b );
    symmetric_buffer_b = NULL;

    UTILS_ASSERT( transfer_status );
    CALL_SHMEM( shfree )( transfer_status );
    transfer_status = NULL;

    UTILS_ASSERT( current_ready_pe );
    CALL_SHMEM( shfree )( current_ready_pe );
    current_ready_pe = NULL;

    UTILS_ASSERT( transfer_counter );
    CALL_SHMEM( shfree )( transfer_counter );
    transfer_counter = NULL;

    UTILS_ASSERT( barrier_psync );
    CALL_SHMEM( shfree )( barrier_psync );
    barrier_psync = NULL;

    UTILS_ASSERT( bcast_psync );
    CALL_SHMEM( shfree )( bcast_psync );
    bcast_psync = NULL;

    UTILS_ASSERT( collect_psync );
    CALL_SHMEM( shfree )( collect_psync );
    collect_psync = NULL;

    UTILS_ASSERT( reduce_psync );
    CALL_SHMEM( shfree )( reduce_psync );
    reduce_psync = NULL;

    UTILS_ASSERT( pwork );
    CALL_SHMEM( shfree )( pwork );
    pwork = NULL;

    CALL_SHMEM( shmem_barrier_all )();
}


SCOREP_Ipc_Group*
SCOREP_Ipc_GetFileGroup( int nProcsPerFile )
{
    return NULL;
}


int
SCOREP_IpcGroup_GetSize( SCOREP_Ipc_Group* group )
{
    return resolve_group_size( group );
}


int
SCOREP_IpcGroup_GetRank( SCOREP_Ipc_Group* group )
{
    return XCALL_SHMEM( SCOREP_SHMEM_MY_PE )();
}


int
SCOREP_IpcGroup_Send( SCOREP_Ipc_Group*   group,
                      const void*         buf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype,
                      int                 dest )
{
    UTILS_BUG_ON( count * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  count * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    UTILS_DEBUG_ENTRY( "buf %p, count %i, data type %i, dest %i",
                       buf, count, datatype, dest );

    /* Increase counter which marks current transfer */
    transfer_counter[ dest ]++;

    /* Wait until previous data transfers are completed */
    int remote_transfer_counter;
    int rank = SCOREP_Ipc_GetRank();
    do
    {
        remote_transfer_counter = CALL_SHMEM( shmem_int_g )( &( transfer_counter[ rank ] ), dest );
    }
    while ( remote_transfer_counter != ( ( volatile int* )transfer_counter )[ dest ] );

    /* Wait until receiver is ready */
    CALL_SHMEM( shmem_int_wait_until )( transfer_status, SHMEM_CMP_EQ, TRANSFER_START );

    /* Use shmem_quiet operation to ensure ordering of put operations */
    CALL_SHMEM( shmem_quiet )();

    /* Send operation */
    CALL_SHMEM( shmem_putmem )( symmetric_buffer_a,
                                buf,
                                count * sizeof_ipc_datatypes[ datatype ],
                                dest );

    /* Use shmem_quiet operation to ensure ordering of put operations */
    CALL_SHMEM( shmem_quiet )();

    /* Set transfer status of sender to completed */
    CALL_SHMEM( shmem_int_p )( transfer_status, TRANSFER_SENDER_COMPLETE, dest );

    /* Use shmem_quiet operation to ensure ordering of put operations */
    CALL_SHMEM( shmem_quiet )();

    /* Wait until receiver has finished */
    CALL_SHMEM( shmem_int_wait_until )( transfer_status, SHMEM_CMP_EQ, TRANSFER_RECEIVER_COMPLETE );

    /* Reset status flags */
    *transfer_status = INVALID_TRANSFER_STATE;

    return 0;
}


int
SCOREP_IpcGroup_Recv( SCOREP_Ipc_Group*   group,
                      void*               buf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype,
                      int                 source )
{
    UTILS_DEBUG_ENTRY( "buf %p, count %i, data type %i, source %i",
                       buf, count, datatype, source );

    /* Increase counter which marks current transfer */
    transfer_counter[ source ]++;

    /* Wait until previous data transfers are completed */
    int remote_transfer_counter;
    int rank = SCOREP_Ipc_GetRank();
    do
    {
        remote_transfer_counter = CALL_SHMEM( shmem_int_g )( &transfer_counter[ rank ], source );
    }
    while ( remote_transfer_counter != ( ( volatile int* )transfer_counter )[ source ] );

    /* Use shmem_quiet operation to ensure ordering of put operations */
    CALL_SHMEM( shmem_quiet )();

    /* Signal start of data transfer */
    CALL_SHMEM( shmem_int_p )( transfer_status, TRANSFER_START, source );

    /* Use shmem_quiet operation to ensure ordering of put operations */
    CALL_SHMEM( shmem_quiet )();

    /* Wait until sender has finished data transfer */
    CALL_SHMEM( shmem_int_wait_until )( transfer_status, SHMEM_CMP_EQ, TRANSFER_SENDER_COMPLETE );

    /* Copy symmetric memory block to buffer */
    memcpy( buf, symmetric_buffer_a, count * sizeof_ipc_datatypes[ datatype ] );

    /* Signal end of data transfer */
    CALL_SHMEM( shmem_int_p )( transfer_status, TRANSFER_RECEIVER_COMPLETE, source );

    /* Use shmem_quiet operation to ensure ordering of put operations */
    CALL_SHMEM( shmem_quiet )();

    /* Reset status flags */
    *transfer_status = INVALID_TRANSFER_STATE;

    return 0;
}


int
SCOREP_IpcGroup_Barrier( SCOREP_Ipc_Group* group )
{
    int start  = resolve_group_start( group );
    int stride = resolve_group_stride( group );
    int size   = resolve_group_size( group );

    UTILS_DEBUG_ENTRY( "(%d, %d, %d)", start, stride, size );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Bcast( SCOREP_Ipc_Group*   group,
                       void*               buf,
                       int                 count,
                       SCOREP_Ipc_Datatype datatype,
                       int                 root )
{
    int rank         = SCOREP_Ipc_GetRank();
    int start        = resolve_group_start( group );
    int stride       = resolve_group_stride( group );
    int size         = resolve_group_size( group );
    int num_elements = count;

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d)",
                       start, stride, size, count, datatype, root );

    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        num_elements = ROUNDUPTO( count, 4 );
    }
    UTILS_BUG_ON( num_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  num_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    /* Copy buffer to symmetric memory block */
    if ( root == rank )
    {
        memcpy( symmetric_buffer_a, buf, count * sizeof_ipc_datatypes[ datatype ] );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Broadcast operation */
    int num_elements_sent;
    switch ( datatype )
    {
        case SCOREP_IPC_BYTE:
        case SCOREP_IPC_CHAR:
        case SCOREP_IPC_UNSIGNED_CHAR:
            num_elements_sent = CEIL( count, 4 );
            CALL_SHMEM( shmem_broadcast32 )( symmetric_buffer_a,
                                             symmetric_buffer_a,
                                             num_elements_sent,
                                             root, start, stride, size,
                                             bcast_psync );
            break;

        case SCOREP_IPC_INT:
        case SCOREP_IPC_UNSIGNED:
        case SCOREP_IPC_INT32_T:
        case SCOREP_IPC_UINT32_T:
            CALL_SHMEM( shmem_broadcast32 )( symmetric_buffer_a,
                                             symmetric_buffer_a,
                                             count,
                                             root, start, stride, size,
                                             bcast_psync );
            break;

        case SCOREP_IPC_INT64_T:
        case SCOREP_IPC_UINT64_T:
        case SCOREP_IPC_DOUBLE:
            CALL_SHMEM( shmem_broadcast64 )( symmetric_buffer_a,
                                             symmetric_buffer_a,
                                             count,
                                             root, start, stride, size,
                                             bcast_psync );
            break;

        default:
            UTILS_BUG( "Bcast: Invalid IPC datatype: %d", datatype );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    if ( root != rank )
    {
        /* Copy symmetric memory block to buffer */
        memcpy( buf, symmetric_buffer_a, count * sizeof_ipc_datatypes[ datatype ] );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Gather( SCOREP_Ipc_Group*   group,
                        const void*         sendbuf,
                        void*               recvbuf,
                        int                 count,
                        SCOREP_Ipc_Datatype datatype,
                        int                 root )
{
    int rank         = SCOREP_Ipc_GetRank();
    int start        = resolve_group_start( group );
    int stride       = resolve_group_stride( group );
    int size         = resolve_group_size( group );
    int num_elements = count;

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d)",
                       start, stride, size, count, datatype, root );

    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        num_elements = ROUNDUPTO( count, 4 );
    }
    UTILS_BUG_ON( size * num_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  size * num_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    /* Copy buffer to symmetric memory block */
    memcpy( symmetric_buffer_a, sendbuf, count * sizeof_ipc_datatypes[ datatype ] );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Broadcast operation */
    int num_elements_sent;
    switch ( datatype )
    {
        case SCOREP_IPC_BYTE:
        case SCOREP_IPC_CHAR:
        case SCOREP_IPC_UNSIGNED_CHAR:
            num_elements_sent = CEIL( count, 4 );
            CALL_SHMEM( shmem_fcollect32 )( symmetric_buffer_b,
                                            symmetric_buffer_a,
                                            num_elements_sent,
                                            start, stride, size,
                                            collect_psync );
            break;

        case SCOREP_IPC_INT:
        case SCOREP_IPC_UNSIGNED:
        case SCOREP_IPC_INT32_T:
        case SCOREP_IPC_UINT32_T:
            CALL_SHMEM( shmem_fcollect32 )( symmetric_buffer_b,
                                            symmetric_buffer_a,
                                            count,
                                            start, stride, size,
                                            collect_psync );
            break;

        case SCOREP_IPC_INT64_T:
        case SCOREP_IPC_UINT64_T:
        case SCOREP_IPC_DOUBLE:
            CALL_SHMEM( shmem_fcollect64 )( symmetric_buffer_b,
                                            symmetric_buffer_a,
                                            count,
                                            start, stride, size,
                                            collect_psync );
            break;

        default:
            UTILS_BUG( "Gather: Invalid IPC datatype: %d", datatype );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Copy symmetric memory block to buffer */
    if ( rank == root )
    {
        if ( datatype == SCOREP_IPC_BYTE ||
             datatype == SCOREP_IPC_CHAR ||
             datatype == SCOREP_IPC_UNSIGNED_CHAR )
        {
            int recvbuf_index = 0;
            for ( int i = 0; i < size; i++ )
            {
                for ( int j = 0; j < count; j++ )
                {
                    ( ( char* )recvbuf )[ recvbuf_index++ ] = ( ( char* )symmetric_buffer_b )[ j + ( i * count ) ];
                }
            }
        }
        else
        {
            memcpy( recvbuf, symmetric_buffer_b, size * count * sizeof_ipc_datatypes[ datatype ] );
        }
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Gatherv( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         int                 sendcount,
                         void*               recvbuf,
                         const int*          recvcnts,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root )
{
    int rank   = SCOREP_Ipc_GetRank();
    int start  = resolve_group_start( group );
    int stride = resolve_group_stride( group );
    int size   = resolve_group_size( group );

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d)",
                       start, stride, size, sendcount, datatype, root );

    int sendcount_extra = 0;
#if HAVE( BROKEN_SHMEM_COLLECT )
    sendcount_extra = 1;
#endif

    /*
     * Sum up 'recvcount's from all processing elements
     * and allocate symmetric memory blocks
     */
    int total_number_of_recv_elements = 0;
    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        int num_send_elements = ROUNDUPTO( sendcount + sendcount_extra, 4 );
        UTILS_BUG_ON( num_send_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                      "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                      num_send_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

        if ( rank == root )
        {
            for ( int i = 0; i < SCOREP_Ipc_GetSize(); i++ )
            {
                int num_recv_elements = ROUNDUPTO( recvcnts[ i ] + sendcount_extra, 4 );
                total_number_of_recv_elements += num_recv_elements;
            }
        }
    }
    else
    {
        UTILS_BUG_ON( ( sendcount + sendcount_extra ) * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                      "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                      ( sendcount + sendcount_extra ) * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

        if ( rank == root )
        {
            for ( int i = 0; i < SCOREP_Ipc_GetSize(); i++ )
            {
                total_number_of_recv_elements += recvcnts[ i ] + sendcount_extra;
            }
        }
    }
    if ( rank == root )
    {
        UTILS_BUG_ON( total_number_of_recv_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                      "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                      total_number_of_recv_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );
    }

    /* Copy buffer to symmetric memory block */
    memcpy( symmetric_buffer_a, sendbuf, sendcount * sizeof_ipc_datatypes[ datatype ] );
    memset( ( char* )symmetric_buffer_a + sendcount * sizeof_ipc_datatypes[ datatype ],
            0, sendcount_extra * sizeof_ipc_datatypes[ datatype ] );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Broadcast operation */
    int num_elements_sent;
    switch ( datatype )
    {
        case SCOREP_IPC_BYTE:
        case SCOREP_IPC_CHAR:
        case SCOREP_IPC_UNSIGNED_CHAR:
            num_elements_sent = CEIL( sendcount + sendcount_extra, 4 );
            CALL_SHMEM( shmem_collect32 )( symmetric_buffer_b,
                                           symmetric_buffer_a,
                                           num_elements_sent,
                                           start, stride, size,
                                           collect_psync );
            break;

        case SCOREP_IPC_INT:
        case SCOREP_IPC_UNSIGNED:
        case SCOREP_IPC_INT32_T:
        case SCOREP_IPC_UINT32_T:
            CALL_SHMEM( shmem_collect32 )( symmetric_buffer_b,
                                           symmetric_buffer_a,
                                           sendcount + sendcount_extra,
                                           start, stride, size,
                                           collect_psync );
            break;

        case SCOREP_IPC_INT64_T:
        case SCOREP_IPC_UINT64_T:
        case SCOREP_IPC_DOUBLE:
            CALL_SHMEM( shmem_collect64 )( symmetric_buffer_b,
                                           symmetric_buffer_a,
                                           sendcount + sendcount_extra,
                                           start, stride, size,
                                           collect_psync );
            break;

        default:
            UTILS_BUG( "Gatherv: Invalid IPC datatype: %d", datatype );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Copy symmetric memory block to buffer */
    if ( rank == root )
    {
        if ( datatype == SCOREP_IPC_BYTE ||
             datatype == SCOREP_IPC_CHAR ||
             datatype == SCOREP_IPC_UNSIGNED_CHAR )
        {
            int recvbuf_index  = 0;
            int current_offset = 0;
            for ( int i = 0; i < size; i++ )
            {
                for ( int j = 0; j < recvcnts[ i ]; j++ )
                {
                    ( ( char* )recvbuf )[ recvbuf_index++ ] = ( ( char* )symmetric_buffer_b )[ j + current_offset ];
                }
                current_offset += ROUNDUPTO( recvcnts[ i ] + sendcount_extra, 4 );
            }
        }
        else
        {
            size_t recvbuf_offset   = 0;
            size_t symmetric_offset = 0;
            for ( int i = 0; i < size; i++ )
            {
                memcpy( ( char* )recvbuf + recvbuf_offset,
                        ( char* )symmetric_buffer_b + symmetric_offset,
                        recvcnts[ i ] * sizeof_ipc_datatypes[ datatype ] );
                recvbuf_offset   += recvcnts[ i ] * sizeof_ipc_datatypes[ datatype ];
                symmetric_offset += ( recvcnts[ i ] + sendcount_extra ) * sizeof_ipc_datatypes[ datatype ];
            }
        }
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Allgather( SCOREP_Ipc_Group*   group,
                           const void*         sendbuf,
                           void*               recvbuf,
                           int                 count,
                           SCOREP_Ipc_Datatype datatype )
{
    if ( count <= 0 )
    {
        return 0;
    }

    int start        = resolve_group_start( group );
    int stride       = resolve_group_stride( group );
    int size         = resolve_group_size( group );
    int num_elements = count;

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d)",
                       start, stride, size, count, datatype );

    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        num_elements = ROUNDUPTO( count, 4 );
    }
    UTILS_BUG_ON( size * num_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  size * num_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    /* Copy buffer to symmetric memory block */
    memcpy( symmetric_buffer_a, sendbuf, count * sizeof_ipc_datatypes[ datatype ] );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Broadcast operation */
    int num_elements_sent;
    switch ( datatype )
    {
        case SCOREP_IPC_BYTE:
        case SCOREP_IPC_CHAR:
        case SCOREP_IPC_UNSIGNED_CHAR:
            num_elements_sent = CEIL( count, 4 );
            CALL_SHMEM( shmem_fcollect32 )( symmetric_buffer_b,
                                            symmetric_buffer_a,
                                            num_elements_sent,
                                            start, stride, size,
                                            collect_psync );
            break;

        case SCOREP_IPC_INT:
        case SCOREP_IPC_UNSIGNED:
        case SCOREP_IPC_INT32_T:
        case SCOREP_IPC_UINT32_T:
            CALL_SHMEM( shmem_fcollect32 )( symmetric_buffer_b,
                                            symmetric_buffer_a,
                                            count,
                                            start, stride, size,
                                            collect_psync );
            break;

        case SCOREP_IPC_INT64_T:
        case SCOREP_IPC_UINT64_T:
        case SCOREP_IPC_DOUBLE:
            CALL_SHMEM( shmem_fcollect64 )( symmetric_buffer_b,
                                            symmetric_buffer_a,
                                            count,
                                            start, stride, size,
                                            collect_psync );
            break;

        default:
            UTILS_BUG( "Allgather: Invalid IPC datatype: %d", datatype );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Copy symmetric memory block to buffer */
    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        int recvbuf_index = 0;
        for ( int i = 0; i < size; i++ )
        {
            for ( int j = 0; j < count; j++ )
            {
                ( ( char* )recvbuf )[ recvbuf_index++ ] = ( ( char* )symmetric_buffer_b )[ j + ( i * count ) ];
            }
        }
    }
    else
    {
        memcpy( recvbuf, symmetric_buffer_b, size * count * sizeof_ipc_datatypes[ datatype ] );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Reduce( SCOREP_Ipc_Group*    group,
                        const void*          sendbuf,
                        void*                recvbuf,
                        int                  count,
                        SCOREP_Ipc_Datatype  datatype,
                        SCOREP_Ipc_Operation operation,
                        int                  root )
{
    if ( count <= 0 )
    {
        return 0;
    }

    int rank         = SCOREP_Ipc_GetRank();
    int start        = resolve_group_start( group );
    int stride       = resolve_group_stride( group );
    int size         = resolve_group_size( group );
    int num_elements = count;

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d, %d)",
                       start, stride, size, count, datatype, operation, root );

    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        num_elements = ROUNDUPTO( count, 2 );
    }
    UTILS_BUG_ON( num_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  num_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    /* Copy buffer to symmetric memory block */
    memcpy( symmetric_buffer_a, sendbuf, count * sizeof_ipc_datatypes[ datatype ] );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Reduction operation */
    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        int nreduce = CEIL( count, 2 );
        switch ( operation )
        {
            case SCOREP_IPC_BAND:
                CALL_SHMEM( shmem_short_and_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            case SCOREP_IPC_BOR:
                CALL_SHMEM( shmem_short_or_to_all )( symmetric_buffer_b,
                                                     symmetric_buffer_a,
                                                     nreduce,
                                                     start, stride, size,
                                                     get_pwork( sizeof( short ), nreduce ),
                                                     reduce_psync );
                break;
            case SCOREP_IPC_MIN:
                CALL_SHMEM( shmem_short_min_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            case SCOREP_IPC_MAX:
                CALL_SHMEM( shmem_short_max_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            case SCOREP_IPC_SUM:
                CALL_SHMEM( shmem_short_sum_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            default:
                UTILS_BUG( "Reduce: Invalid IPC operation: %d", operation );
        }
    }
    else if ( datatype == SCOREP_IPC_INT ||
              datatype == SCOREP_IPC_UNSIGNED ||
              datatype == SCOREP_IPC_INT32_T ||
              datatype == SCOREP_IPC_UINT32_T )
    {
        switch ( operation )
        {
            case SCOREP_IPC_BAND:
                CALL_SHMEM( shmem_int_and_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            case SCOREP_IPC_BOR:
                CALL_SHMEM( shmem_int_or_to_all )( symmetric_buffer_b,
                                                   symmetric_buffer_a,
                                                   count,
                                                   start, stride, size,
                                                   get_pwork( sizeof( int ), count ),
                                                   reduce_psync );
                break;
            case SCOREP_IPC_MIN:
                CALL_SHMEM( shmem_int_min_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            case SCOREP_IPC_MAX:
                CALL_SHMEM( shmem_int_max_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            case SCOREP_IPC_SUM:
                CALL_SHMEM( shmem_int_sum_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            default:
                UTILS_BUG( "Reduce: Invalid IPC operation: %d", operation );
        }
    }
    else if ( datatype == SCOREP_IPC_INT64_T ||
              datatype == SCOREP_IPC_UINT64_T ||
              datatype == SCOREP_IPC_DOUBLE )
    {
        switch ( operation )
        {
            case SCOREP_IPC_BAND:
                CALL_SHMEM( shmem_longlong_and_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            case SCOREP_IPC_BOR:
                CALL_SHMEM( shmem_longlong_or_to_all )( symmetric_buffer_b,
                                                        symmetric_buffer_a,
                                                        count,
                                                        start, stride, size,
                                                        ( long long* )pwork,
                                                        reduce_psync );
                break;
            case SCOREP_IPC_MIN:
                CALL_SHMEM( shmem_longlong_min_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            case SCOREP_IPC_MAX:
                CALL_SHMEM( shmem_longlong_max_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            case SCOREP_IPC_SUM:
                CALL_SHMEM( shmem_longlong_sum_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            default:
                UTILS_BUG( "Reduce: Invalid IPC operation: %d", operation );
        }
    }
    else
    {
        UTILS_BUG( "Reduce: Invalid IPC datatype: %d", datatype );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Copy symmetric memory block to buffer */
    if ( rank == root )
    {
        memcpy( recvbuf, symmetric_buffer_b, count * sizeof_ipc_datatypes[ datatype ] );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Allreduce( SCOREP_Ipc_Group*    group,
                           const void*          sendbuf,
                           void*                recvbuf,
                           int                  count,
                           SCOREP_Ipc_Datatype  datatype,
                           SCOREP_Ipc_Operation operation )
{
    int start        = resolve_group_start( group );
    int stride       = resolve_group_stride( group );
    int size         = resolve_group_size( group );
    int num_elements = count;

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d)",
                       start, stride, size, count, datatype, operation );

    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        num_elements = ROUNDUPTO( count, 2 );
    }
    UTILS_BUG_ON( num_elements * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  num_elements * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    /* Copy buffer to symmetric memory block */
    memcpy( symmetric_buffer_a, sendbuf, count * sizeof_ipc_datatypes[ datatype ] );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Reduction operation */
    if ( datatype == SCOREP_IPC_BYTE ||
         datatype == SCOREP_IPC_CHAR ||
         datatype == SCOREP_IPC_UNSIGNED_CHAR )
    {
        int nreduce = CEIL( count, 2 );
        switch ( operation )
        {
            case SCOREP_IPC_BAND:
                CALL_SHMEM( shmem_short_and_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            case SCOREP_IPC_BOR:
                CALL_SHMEM( shmem_short_or_to_all )( symmetric_buffer_b,
                                                     symmetric_buffer_a,
                                                     nreduce,
                                                     start, stride, size,
                                                     get_pwork( sizeof( short ), nreduce ),
                                                     reduce_psync );
                break;
            case SCOREP_IPC_MIN:
                CALL_SHMEM( shmem_short_min_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            case SCOREP_IPC_MAX:
                CALL_SHMEM( shmem_short_max_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            case SCOREP_IPC_SUM:
                CALL_SHMEM( shmem_short_sum_to_all )( symmetric_buffer_b,
                                                      symmetric_buffer_a,
                                                      nreduce,
                                                      start, stride, size,
                                                      get_pwork( sizeof( short ), nreduce ),
                                                      reduce_psync );
                break;
            default:
                UTILS_BUG( "Allreduce: Invalid IPC operation: %d", operation );
        }
    }
    else if ( datatype == SCOREP_IPC_INT ||
              datatype == SCOREP_IPC_UNSIGNED ||
              datatype == SCOREP_IPC_INT32_T ||
              datatype == SCOREP_IPC_UINT32_T )
    {
        switch ( operation )
        {
            case SCOREP_IPC_BAND:
                CALL_SHMEM( shmem_int_and_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            case SCOREP_IPC_BOR:
                CALL_SHMEM( shmem_int_or_to_all )( symmetric_buffer_b,
                                                   symmetric_buffer_a,
                                                   count,
                                                   start, stride, size,
                                                   get_pwork( sizeof( int ), count ),
                                                   reduce_psync );
                break;
            case SCOREP_IPC_MIN:
                CALL_SHMEM( shmem_int_min_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            case SCOREP_IPC_MAX:
                CALL_SHMEM( shmem_int_max_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            case SCOREP_IPC_SUM:
                CALL_SHMEM( shmem_int_sum_to_all )( symmetric_buffer_b,
                                                    symmetric_buffer_a,
                                                    count,
                                                    start, stride, size,
                                                    get_pwork( sizeof( int ), count ),
                                                    reduce_psync );
                break;
            default:
                UTILS_BUG( "Allreduce: Invalid IPC operation: %d", operation );
        }
    }
    else if ( datatype == SCOREP_IPC_INT64_T ||
              datatype == SCOREP_IPC_UINT64_T ||
              datatype == SCOREP_IPC_DOUBLE )
    {
        switch ( operation )
        {
            case SCOREP_IPC_BAND:
                CALL_SHMEM( shmem_longlong_and_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            case SCOREP_IPC_BOR:
                CALL_SHMEM( shmem_longlong_or_to_all )( symmetric_buffer_b,
                                                        symmetric_buffer_a,
                                                        count,
                                                        start, stride, size,
                                                        ( long long* )pwork,
                                                        reduce_psync );
                break;
            case SCOREP_IPC_MIN:
                CALL_SHMEM( shmem_longlong_min_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            case SCOREP_IPC_MAX:
                CALL_SHMEM( shmem_longlong_max_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            case SCOREP_IPC_SUM:
                CALL_SHMEM( shmem_longlong_sum_to_all )( symmetric_buffer_b,
                                                         symmetric_buffer_a,
                                                         count,
                                                         start, stride, size,
                                                         ( long long* )pwork,
                                                         reduce_psync );
                break;
            default:
                UTILS_BUG( "Allreduce: Invalid IPC operation: %d", operation );
        }
    }
    else
    {
        UTILS_BUG( "Allreduce: Invalid IPC datatype: %d", datatype );
    }

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    /* Copy symmetric memory block to buffer */
    memcpy( recvbuf, symmetric_buffer_b, count * sizeof_ipc_datatypes[ datatype ] );

    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Scatter( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         void*               recvbuf,
                         int                 count,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root )
{
    UTILS_BUG_ON( count * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  count * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    int rank   = SCOREP_Ipc_GetRank();
    int start  = resolve_group_start( group );
    int stride = resolve_group_stride( group );
    int size   = resolve_group_size( group );

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d)",
                       start, stride, size, count, datatype, root );

    if ( rank == root )
    {
        /* root is the only sender */

        CALL_SHMEM( shmem_quiet )();
        for ( int receiver = start; receiver < start + size; receiver++ )
        {
            if ( receiver == root )
            {
                /* Root should not send data to itself
                 * Copy data directly to buffer */
                memcpy( recvbuf,
                        &( ( ( char* )sendbuf )[ receiver * count * sizeof_ipc_datatypes[ datatype ] ] ),
                        count * sizeof_ipc_datatypes[ datatype ] );
                continue;
            }

            CALL_SHMEM( shmem_putmem )( symmetric_buffer_b,
                                        &( ( ( char* )sendbuf )[ receiver * count * sizeof_ipc_datatypes[ datatype ] ] ),
                                        count * sizeof_ipc_datatypes[ datatype ],
                                        receiver );
        }
        CALL_SHMEM( shmem_quiet )();
        CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );
    }
    else
    {
        /* All processing elements except root are receivers */

        /* Wait until data has arrived */
        CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

        /* Copy symmetric memory block to buffer */
        memcpy( recvbuf, symmetric_buffer_b, count * sizeof_ipc_datatypes[ datatype ] );
    }

    /* Wait until data has arrived */
    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}


int
SCOREP_IpcGroup_Scatterv( SCOREP_Ipc_Group*   group,
                          const void*         sendbuf,
                          const int*          sendcounts,
                          void*               recvbuf,
                          int                 recvcount,
                          SCOREP_Ipc_Datatype datatype,
                          int                 root )
{
    UTILS_BUG_ON( recvcount * sizeof_ipc_datatypes[ datatype ] > BUFFER_SIZE,
                  "SHMEM symmetric buffer of insufficient size. %d bytes requested, %d bytes available.",
                  recvcount * sizeof_ipc_datatypes[ datatype ], BUFFER_SIZE );

    int rank   = SCOREP_Ipc_GetRank();
    int start  = resolve_group_start( group );
    int stride = resolve_group_stride( group );
    int size   = resolve_group_size( group );

    UTILS_DEBUG_ENTRY( "(%d, %d, %d, %d/%d, %d)",
                       start, stride, size, recvcount, datatype, root );

    if ( rank == root )
    {
        /* root is the only sender */

        int size = SCOREP_IpcGroup_GetSize( group );

        /* Please note: at the moment SHMEM IPC groups consist of consecutive processing elements */
        CALL_SHMEM( shmem_quiet )();
        int total    = 0;
        int receiver = start;
        for ( int i = 0; i < size; i++ )
        {
            if ( receiver == root )
            {
                /* Root should not send data to itself
                 * Copy data directly to buffer */
                memcpy( recvbuf,
                        &( ( ( char* )sendbuf )[ total * sizeof_ipc_datatypes[ datatype ] ] ),
                        sendcounts[ i ] * sizeof_ipc_datatypes[ datatype ] );
            }
            else
            {
                CALL_SHMEM( shmem_putmem )( symmetric_buffer_b,
                                            &( ( ( char* )sendbuf )[ total * sizeof_ipc_datatypes[ datatype ] ] ),
                                            sendcounts[ i ] * sizeof_ipc_datatypes[ datatype ],
                                            receiver );
            }

            total += sendcounts[ i ];
            receiver++;
        }
        CALL_SHMEM( shmem_quiet )();
        CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );
    }
    else
    {
        /* All processing elements except root are receivers */

        /* Wait until data has arrived */
        CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

        /* Copy symmetric memory block to buffer */
        memcpy( recvbuf, symmetric_buffer_b, recvcount * sizeof_ipc_datatypes[ datatype ] );
    }

    /* Wait until data has arrived */
    CALL_SHMEM( shmem_barrier )( start, stride, size, barrier_psync );

    return 0;
}

void*
get_pwork( size_t size,
           int    count )
{
    size_t nreduce_size = ( ( count / 2 ) + 1 ) * size;
    if ( nreduce_size < current_pwork_size )
    {
        pwork = CALL_SHMEM( shrealloc )( pwork, nreduce_size );
        UTILS_BUG_ON( !pwork, "Cannot allocate symmetric work array of size %zu",
                      nreduce_size );
        current_pwork_size = nreduce_size;
    }

    return pwork;
}

SCOREP_Ipc_Group*
SCOREP_IpcGroup_Split( SCOREP_Ipc_Group* parent,
                       int               color,
                       int               key )
{
    UTILS_BUG( "SCOREP_IpcGroup_Split for shmem is not supported." );

    /* not reached */
    return NULL;
}

void
SCOREP_IpcGroup_Free( SCOREP_Ipc_Group* group )
{
    UTILS_BUG( "SCOREP_IpcGroup_Free for shmem is not supported." );
}
