/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2019,
 * RWTH Aachen University, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_MPI_RMA_REQUEST_H
#define SCOREP_MPI_RMA_REQUEST_H

/**
 * @file
 * @ingroup MPI_Wrapper
 *
 * This file implements request tracking for implicit and explicit requests in
 * RMA communication. Any RMA operation can be forced to complete locally or
 * remote referencing only the window or window in combination with a target
 * rank. Therefore these special query support is needed.
 *
 * The data structure used here is a simple skip list that should have O(log N)
 * behavior with high probability.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Types.h>

#include <mpi.h>

/**
 * @brief Completion type for RMA requests
 */
typedef enum
{
    SCOREP_MPI_RMA_REQUEST_SEPARATE_COMPLETION = 0,
    SCOREP_MPI_RMA_REQUEST_COMBINED_COMPLETION = 1,
} SCOREP_Mpi_RmaCompletionType;

/**
 * @brief RMA-request tracking structure
 */
typedef struct
{
    SCOREP_RmaWindowHandle       window;            ///< RMA Window handle, used as key
    int                          target;            ///< Target rank, used as key
    SCOREP_MpiRequestId          matching_id;       ///< Matching ID for operation
    MPI_Request                  mpi_handle;        ///< MPI_Request handle
    SCOREP_Mpi_RmaCompletionType completion_type;   ///< Completion type of RMA operation
    bool                         completed_locally; ///< RmaCompleteNonblocking record written?
    bool                         schedule_removal;  ///< Remove entry?
} scorep_mpi_rma_request;


/**
 * @brief Callback for foreach routine
 */
typedef void (scorep_mpi_rma_request_cb)( scorep_mpi_rma_request* request );

/**
 * @brief Initialize the RMA request tracking
 */
void
scorep_mpi_rma_request_init( void );

/**
 * @brief Finalize the RMA request tracking
 */
void
scorep_mpi_rma_request_finalize( void );

/**
 * @brief Create an RMA request entry
 * @param window RMA window handle
 * @param target Target rank
 * @param mpiHandle MPI request handle
 * @param completionType Flag indicating completion type of request
 * @param matchingId Matching ID
 */
scorep_mpi_rma_request*
scorep_mpi_rma_request_create( SCOREP_RmaWindowHandle       window,
                               int                          target,
                               MPI_Request                  mpiHandle,
                               SCOREP_Mpi_RmaCompletionType completionType,
                               SCOREP_MpiRequestId          matchingId );

/**
 * @brief Find smallest element for a given window handle and target rank
 * @param window RMA window handle
 * @param target Target rank
 * @param mpiHandle MPI request handle
 * @param completionType Flag indicating completion type of request
 * @return Pointer to request element
 */
scorep_mpi_rma_request*
scorep_mpi_rma_request_find( SCOREP_RmaWindowHandle       window,
                             int                          target,
                             MPI_Request                  mpiHandle,
                             SCOREP_Mpi_RmaCompletionType completionType );

/**
 * @brief Remove element identified by given attributes
 * @param window RMA window handle
 * @param target Target rank
 * @param completion_type Flag indicating completion type of request
 */
void
scorep_mpi_rma_request_remove( SCOREP_RmaWindowHandle       window,
                               int                          target,
                               MPI_Request                  mpiHandle,
                               SCOREP_Mpi_RmaCompletionType completionType );

/**
 * @brief Remove a request identified by a request pointer
 * @param request Pointer to request to be removed
 */
void
scorep_mpi_rma_request_remove_by_ptr( scorep_mpi_rma_request* request );

/**
 * @brief perform a specific action for each request of a given window
 * @param window RMA window handle
 * @param callback Function callback to call for each request
 */
void
scorep_mpi_rma_request_foreach_on_window( SCOREP_RmaWindowHandle     window,
                                          scorep_mpi_rma_request_cb* callback );

/**
 * @brief perform a specific action for each request to a given target for a window
 * @param window RMA window handle
 * @param target Target rank
 * @param callback Function callback to call for each request
 */
void
scorep_mpi_rma_request_foreach_to_target( SCOREP_RmaWindowHandle     window,
                                          int                        target,
                                          scorep_mpi_rma_request_cb* callback );

/**
 * @brief Write standard completion events and mark request for removal
 * @param request request element
 */
void
scorep_mpi_rma_request_write_standard_completion_and_remove( scorep_mpi_rma_request* request );

/**
 * @brief Write standard completion events without marking request for removal
 * @param request request element
 */
void
scorep_mpi_rma_request_write_standard_completion( scorep_mpi_rma_request* request );

/**
 * @brief Write full (local and remote) completion events unconditionally, and
 *      mark request for removal
 * @param request request element
 */
void
scorep_mpi_rma_request_write_full_completion( scorep_mpi_rma_request* request );

#endif /* SCOREP_MPI_RMA_REQUEST_H */
