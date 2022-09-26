/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011, 2021-2022,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2018-2019, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017-2019, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2022,
 * Deutsches Zentrum fuer Luft- und Raumfahrt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_MPI_REQUEST_MGMT_H
#define SCOREP_MPI_REQUEST_MGMT_H

/**
   @file
   @ingroup    MPI_Wrapper

   @brief Contains the declarations for the handling of MPI Requests.
 */

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <mpi.h>

#include "scorep_mpi_rma_request.h"

#include <stddef.h>

typedef enum scorep_mpi_request_type
{
    SCOREP_MPI_REQUEST_TYPE_NONE,
    SCOREP_MPI_REQUEST_TYPE_SEND,
    SCOREP_MPI_REQUEST_TYPE_RECV,
    SCOREP_MPI_REQUEST_TYPE_IO,
    SCOREP_MPI_REQUEST_TYPE_RMA,
    SCOREP_MPI_REQUEST_TYPE_COMM_IDUP,
    SCOREP_MPI_REQUEST_TYPE_ICOLL
} scorep_mpi_request_type;

enum scorep_mpi_requests_flags
{
    SCOREP_MPI_REQUEST_FLAG_NONE          = 0x000,
    SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT = 0x001,
    SCOREP_MPI_REQUEST_FLAG_DEALLOCATE    = 0x002,
    SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE     = 0x010,
    SCOREP_MPI_REQUEST_FLAG_ANY_TAG       = 0x020,
    SCOREP_MPI_REQUEST_FLAG_ANY_SRC       = 0x040,
    SCOREP_MPI_REQUEST_FLAG_CAN_CANCEL    = 0x080,
    SCOREP_MPI_REQUEST_FLAG_IS_COMPLETED  = 0x100
};

typedef uint64_t scorep_mpi_request_flag;

/**
 * @internal
 * This struct contains the management information for the location-specific buffers
 * which are used in the request management. We specifically need a continuous array
 * for the status array.
 */
typedef struct scorep_mpi_req_mgmt_location_data
{
    size_t       req_arr_size;
    size_t       f2c_arr_size;
    size_t       status_arr_size;
    MPI_Request* req_arr;
    MPI_Request* f2c_arr;
    MPI_Status*  status_arr;
} scorep_mpi_req_mgmt_location_data;

typedef struct
{
    int                              tag;
    int                              dest;
    uint64_t                         bytes;
    MPI_Datatype                     datatype;
    SCOREP_InterimCommunicatorHandle comm_handle;
} scorep_mpi_request_p2p_data;

typedef struct
{
    SCOREP_CollectiveType            coll_type;
    int                              root_loc;
    uint64_t                         bytes_sent;
    uint64_t                         bytes_recv;
    SCOREP_InterimCommunicatorHandle comm_handle;
} scorep_mpi_request_icoll_data;

typedef struct
{
    SCOREP_IoOperationMode mode;
    uint64_t               bytes;
    MPI_Datatype           datatype;
    MPI_File               fh;
} scorep_mpi_request_io_data;

typedef struct
{
    MPI_Comm*                        new_comm;
    SCOREP_InterimCommunicatorHandle parent_comm_handle;
} scorep_mpi_request_comm_idup_data;

typedef struct
{
    scorep_mpi_rma_request* request_ptr;
} scorep_mpi_request_rma_data;

typedef struct scorep_mpi_request scorep_mpi_request;
struct scorep_mpi_request
{
    MPI_Request             request;
    scorep_mpi_request_type request_type;
    scorep_mpi_request_flag flags;
    union
    {
        scorep_mpi_request_p2p_data       p2p;
        scorep_mpi_request_icoll_data     icoll;
        scorep_mpi_request_comm_idup_data comm_idup;
        scorep_mpi_request_io_data        io;
        scorep_mpi_request_rma_data       rma;
    } payload;
    SCOREP_MpiRequestId id;
    scorep_mpi_request* next;
    bool                marker;
};


/**
 * @brief Return a new request id
 */
SCOREP_MpiRequestId
scorep_mpi_get_request_id( void );

/**
 * @brief Unmark request after no more work needs to be done on the request
 */
void
scorep_mpi_unmark_request( scorep_mpi_request* req );

/**
 * @brief Create entry for a given MPI P2P request handle
 * @param request   MPI request handle
 * @param type      Type of request
 * @param flags     Bitmask containing flags set for this request
 * @param tag       MPI tag for this request
 * @param dest      Destination rank of request
 * @param bytes     Number of bytes transferred in request
 * @param datatype  MPI datatype handle
 * @param comm      MPI communicator handle
 * @param id        Request id
 */
void
scorep_mpi_request_p2p_create( MPI_Request             request,
                               scorep_mpi_request_type type,
                               scorep_mpi_request_flag flags,
                               int                     tag,
                               int                     dest,
                               uint64_t                bytes,
                               MPI_Datatype            datatype,
                               MPI_Comm                comm,
                               SCOREP_MpiRequestId     id );

/**
 * @brief Create entry for a given MPI non-blocking collective request handle
 * @param request        MPI request handle
 * @param flags          Bitmask containing flags set for this request
 * @param collectiveType Type of non-blocking collective operation
 * @param rootLoc        Root location of collective operation
 * @param bytesSent      Number of bytes sent in request
 * @param bytesRecv      Number of bytes received in request
 * @param comm           MPI communicator handle
 * @param id             Request id
 */
void
scorep_mpi_request_icoll_create( MPI_Request             request,
                                 scorep_mpi_request_flag flags,
                                 SCOREP_CollectiveType   collectiveType,
                                 int                     rootLoc,
                                 uint64_t                bytesSent,
                                 uint64_t                bytesRecv,
                                 MPI_Comm                comm,
                                 SCOREP_MpiRequestId     id );

/**
 * @brief Create entry for a given MPI Comm_Idup request handle
 * @param request    MPI request handle
 * @param parentComm Original Comm handle
 * @param newcomm    Handle of copied Comm
 * @param id         Request id
 */
void
scorep_mpi_request_comm_idup_create( MPI_Request         request,
                                     MPI_Comm            parentComm,
                                     MPI_Comm*           newcomm,
                                     SCOREP_MpiRequestId id  );

/**
 * @brief Create entry for an RMA request handle
 * @param mpiRequest MPI request handle
 * @param rmaRequest Score-P RMA request handle
 */
void
scorep_mpi_request_win_create( MPI_Request             mpiRequest,
                               scorep_mpi_rma_request* rmaRequest );

/**
 * @brief Create entry for a given MPI I/O request handle
 * @param request   MPI request handle
 * @param type      Type of request
 * @param bytes     Number of bytes transferred in request
 * @param datatype  MPI datatype handle
 * @param fh        MPI_File handle
 * @param id        Request id
 */
void
scorep_mpi_request_io_create( MPI_Request            request,
                              SCOREP_IoOperationMode mode,
                              uint64_t               bytes,
                              MPI_Datatype           datatype,
                              MPI_File               fh,
                              SCOREP_MpiRequestId    id );

/**
 * @brief  Retrieve internal request entry for an MPI request handle
 * @param  request MPI request handle
 * @return Pointer to corresponding internal request entry
 */
scorep_mpi_request*
scorep_mpi_request_get( MPI_Request request );

/**
 * @brief Free a request entry
 * @param req Pointer to request entry to be deleted
 */
void
scorep_mpi_request_free( scorep_mpi_request* req );

/**
 * @brief Use as the flag argument of @ref scorep_mpi_check_all_or_test_all and @ref scorep_mpi_check_all_or_none.
 */
static const int REQUESTS_COMPLETED = 1;

/**
 * @brief Either check all or test all of the saved requests.
 *
 * @param count Number of saved requests to check or test.
 * @param flag The 'flag' argument of MPI_Test|MPI_Testall.
 * @param array_of_statuses The 'array_of_statuses' argument of MPI_(Test|Wait)_all, or the 'status' argument of MPI_(Test|Wait).
 *
 * Assumes that the requests have been saved by a call to @ref scorep_mpi_save_request_array(..., count) before.
 * Assumes that:
 * - If flag is true, all operations for the active saved requests have been completed and the requests were deallocated|marked inactive by MPI_(Test|Wait)[all]. In this case, calls @ref scorep_mpi_check_request for all requests.
 * - If flag is false, not all operations have been completed and no request was modified by MPI_Test[all]. In this case calls @ref scorep_mpi_request_tested for all requests. The array_of_statuses is ignored.
 */
void
scorep_mpi_check_all_or_test_all( int         count,
                                  int         flag,
                                  MPI_Status* array_of_statuses );

/**
 * @brief Either check all or none of the saved requests.
 *
 * @param count Number of saved requests to check.
 * @param flag The 'flag' argument of MPI_Test|MPI_Testall.
 * @param array_of_statuses The 'array_of_statuses' argument of MPI_(Test|Wait)_all, or the 'status' argument of MPI_(Test|Wait).
 *
 * Assumes that the requests have been saved by a call to @ref scorep_mpi_save_request_array(..., count) before.
 * Assumes that:
 * - If flag is true, all operations for the active saved requests have been completed and the requests were deallocated|marked inactive by MPI_(Test|Wait)[all]. In this case, calls @ref scorep_mpi_check_request for all requests.
 * - If flag is false, not all operations have been completed and no request was modified by MPI_Test[all]. In this case nothing is done. The array_of_statuses is ignored.
 */
void
scorep_mpi_check_all_or_none( int         count,
                              int         flag,
                              MPI_Status* array_of_statuses );

/**
 * @brief Check some of the saved requests and test all others.
 *
 * @param incount The 'incount' argument as given to MPI_(Test|Wait)some. Number of saved requests.
 * @param outcount The 'outcount' argument as returned by MPI_(Test|Wait)some: Number of completed operations. Number of saved requests to check.
 * @param array_of_indices The 'array_of_indices' argument of MPI_(Test|Wait)some: Indices of completed requests in the input array. Indices of saved requests to check.
 * @param array_of_statuses The 'array_of_statuses' argument of MPI_(Test|Wait)some: Statuses of completed requests.
 *
 * Assumes that the input requests have been saved by a call to @ref scorep_mpi_save_request_array(..., incount) before.
 * Assumes that outcount != MPI_UNDEFINED. If outcount == 0, the contents of array_of_indices, array_of_statuses are ignored (array_of_indices is allowed to contain MPI_UNDEFINED).
 * Assumes that the first outcount entries in array_of_indices denote all completed operations for which the corresponding saved active requests have been deallocated/marked inactive by MPI_(Test|Wait)[any|some] (For MPI_(Test|Wait)any, the arrays are of lenght one). For those requests, @ref scorep_mpi_check_request is called. For all other saved requests, @ref scorep_mpi_request_tested is called.
 */
void
scorep_mpi_check_some_test_some( int         incount,
                                 int         outcount,
                                 int*        array_of_indices,
                                 MPI_Status* array_of_statuses );

/**
 * @brief Check some of the saved requests.
 *
 * @param incount The 'incount' argument as given to MPI_(Test|Wait)some. Number of saved requests.
 * @param outcount The 'outcount' argument as returned by MPI_(Test|Wait)some: Number of completed operations. Number of saved requests to check.
 * @param array_of_indices The 'array_of_indices' argument of MPI_(Test|Wait)some: Indices of completed requests in the input array. Indices of saved requests to check.
 * @param array_of_statuses The 'array_of_statuses' argument of MPI_(Test|Wait)some: Statuses of completed requests.
 *
 * Assumes that the input requests have been saved by a call to @ref scorep_mpi_save_request_array(..., incount) before.
 * Assumes that outcount != MPI_UNDEFINED. If outcount == 0, the contents of array_of_indices, array_of_statuses are ignored (array_of_indices is allowed to contain MPI_UNDEFINED).
 * Assumes that the first outcount entries in array_of_indices denote all completed operations for which the corresponding saved active requests have been deallocated/marked inactive by MPI_(Test|Wait)[any|some] (For MPI_(Test|Wait)any, the arrays are of lenght one). For those requests, @ref scorep_mpi_check_request is called. Nothing is done for the other requests.
 */
void
scorep_mpi_check_some( int         incount,
                       int         outcount,
                       int*        array_of_indices,
                       MPI_Status* array_of_statuses );

/**
 * @brief Test all saved requests
 *
 * @param count Number of saved requests to test
 *
 * Assumes that the input requests have been saved by a call to @ref scorep_mpi_save_request_array(..., incount) before.
 * Assumes that none of the operations belonging to the saved requests have been completed.
 *
 * Equivalent to (but more efficient than) @ref scorep_mpi_check_some_test_some( incount, 0, NULL, NULL).
 */
void
scorep_mpi_test_all( int count );

/**
 * @brief Trigger a "tested" event for this MPI request
 * @param req Pointer to request entry to be tested
 */
void
scorep_mpi_request_tested( scorep_mpi_request* req );

/**
 * @brief Check a completed request entry for nessecary measurement data
 * @param req    Pointer to request entry to be checked
 * @param status Corresponding status handle
 */
void
scorep_mpi_check_request( scorep_mpi_request* req,
                          MPI_Status*         status );

void
scorep_mpi_cleanup_request( scorep_mpi_request* req );

/**
 * @brief Provides thread-local request array for conversion of requests
 *        between Fortran and C (aka F2C)
 * @param size Size of array in number of requests
 */
void*
scorep_mpi_get_request_f2c_array( size_t size );

/**
 * @brief Copy request array into thread-local buffer for later use
 * @param arr_req      Pointer to request array to be copied
 * @param arr_req_size Size of array
 */
void
scorep_mpi_save_request_array( MPI_Request* arr_req,
                               size_t       arr_req_size );

/**
 * @brief  Retrieve internal request entry for the MPI request handle in thread-local buffer at a certain position
 * @param  i Index of MPI Request handle in thread-local buffer
 * @return Pointer to corresponding internal request entry
 */
scorep_mpi_request*
scorep_mpi_saved_request_get( size_t i );

/**
 * Get a pointer to a status array of at least 'size' statuses
 * @param  size minimal requested size
 * @return pointer to status array
 */
MPI_Status*
scorep_mpi_get_status_array( size_t size );

#endif /* SCOREP_MPI_REQUEST_MGMT_H */
