/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2018-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2017,
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
 */

/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Contains the implementation of MPI Reqests management
 */

#include <config.h>

#include "scorep_mpi_request_mgmt.h"
#include "SCOREP_Mpi.h"
#include "scorep_mpi_communicator.h"
#include <SCOREP_Events.h>
#include <SCOREP_IoManagement.h>

#include <UTILS_Error.h>

#include <stdlib.h>
#include <string.h>

/**
 * @internal
 * size of hash table
 */
#define SCOREP_MPI_REQUEST_TABLE_SIZE 256

/**
 * @internal
 * Hash access structure
 */
struct scorep_mpi_request_hash
{
    struct scorep_mpi_request_block* head_block;
    struct scorep_mpi_request_block* last_block;
    scorep_mpi_request*              lastreq;
    int                              lastidx;
};

/**
 * @internal
 * size of element list behind a hash entry
 */
#define SCOREP_MPI_REQUEST_BLOCK_SIZE 16

/**
 * @internal
 * Block of linked list of request blocks
 */
struct scorep_mpi_request_block
{
    scorep_mpi_request               req[ SCOREP_MPI_REQUEST_BLOCK_SIZE ];
    struct scorep_mpi_request_block* next;
    struct scorep_mpi_request_block* prev;
};


/**
 * @internal
 * Data structure to store request info for effective request tracking
 */
static struct scorep_mpi_request_hash
    scorep_mpi_request_table[ SCOREP_MPI_REQUEST_TABLE_SIZE ] = {
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */ /* 128 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE } /*  32 */ /* 256 */
};


/**
 * @internal
 * Request id counter
 */
static SCOREP_MpiRequestId scorep_mpi_last_request_id = 0;

/**
 * @internal
 * @brief  Return entry in the hash table that holds the request tracking data
 * @param  req MPI request handle
 * @return Pointer to request hash entry
 */
static struct scorep_mpi_request_hash*
scorep_mpi_get_request_hash_entry( MPI_Request req )
{
    unsigned char* cptr = ( unsigned char* )&req;
    unsigned char  h    = cptr[ 0 ];
    /* int i; */

    /*
     * The hash function.
     * At least on BlueGene and Jump, MPI_Request is a 32-bit integer, which
     * more-or-less seems to represent a request count. Hence we can use a
     * simple and fast hash like msb^lsb here.
     *
     * On Linux/LAM, MPI_Request is still 4 bytes, but the representation is
     * different. The simple hash function would not use all hash bits; here,
     * the loop over all bytes sketched below would be better:
     *
     *   for (i = 1; i < sizeof(MPI_Request); ++i)
     *     h ^= cptr[i];
     */

    h ^= cptr[ sizeof( MPI_Request ) - 1 ];
    return &scorep_mpi_request_table[ h ];
}


SCOREP_MpiRequestId
scorep_mpi_get_request_id( void )
{
    return ++scorep_mpi_last_request_id;
}

static scorep_mpi_request*
scorep_mpi_request_create_entry( MPI_Request             request,
                                 SCOREP_MpiRequestId     id,
                                 scorep_mpi_request_type request_type,
                                 scorep_mpi_request_flag flags )
{
    struct scorep_mpi_request_block* new_block;
    struct scorep_mpi_request_hash*  hash_entry = scorep_mpi_get_request_hash_entry( request );

    hash_entry->lastidx++;
    if ( hash_entry->lastidx >= SCOREP_MPI_REQUEST_BLOCK_SIZE )
    {
        if ( hash_entry->head_block == 0 )
        {
            /* first time: allocate and initialize first block */
            new_block              = malloc( sizeof( struct scorep_mpi_request_block ) );
            new_block->next        = 0;
            new_block->prev        = 0;
            hash_entry->head_block = hash_entry->last_block = new_block;
        }
        else if ( hash_entry->last_block == 0 )
        {
            /* request list empty: re-initialize */
            hash_entry->last_block = hash_entry->head_block;
        }
        else
        {
            if ( hash_entry->last_block->next == 0 )
            {
                /* request list full: expand */
                new_block                    = malloc( sizeof( struct scorep_mpi_request_block ) );
                new_block->next              = 0;
                new_block->prev              = hash_entry->last_block;
                hash_entry->last_block->next = new_block;
            }
            /* use next available block */
            hash_entry->last_block = hash_entry->last_block->next;
        }
        hash_entry->lastreq = &( hash_entry->last_block->req[ 0 ] );
        hash_entry->lastidx = 0;
    }
    else
    {
        hash_entry->lastreq++;
    }

    scorep_mpi_request* req = hash_entry->lastreq;
    req->request      = request;
    req->id           = id;
    req->request_type = request_type;
    req->flags        = flags;
    return req;
}

void
scorep_mpi_request_p2p_create( MPI_Request             request,
                               scorep_mpi_request_type type,
                               scorep_mpi_request_flag flags,
                               int                     tag,
                               int                     dest,
                               uint64_t                bytes,
                               MPI_Datatype            datatype,
                               MPI_Comm                comm,
                               SCOREP_MpiRequestId     id )
{
    scorep_mpi_request* req = scorep_mpi_request_create_entry( request, id, type, flags );

    /* store request information */
    req->payload.p2p.tag   = tag;
    req->payload.p2p.dest  = dest;
    req->payload.p2p.bytes = bytes;
#if HAVE( DECL_PMPI_TYPE_DUP )
    PMPI_Type_dup( datatype, &req->payload.p2p.datatype );
#else
    req->payload.p2p.datatype = datatype;
#endif
    req->payload.p2p.comm_handle         = SCOREP_MPI_COMM_HANDLE( comm );
    req->payload.p2p.online_analysis_pod = NULL;
}

void
scorep_mpi_request_io_create( MPI_Request             request,
                              scorep_mpi_request_type type,
                              uint64_t                bytes,
                              MPI_Datatype            datatype,
                              MPI_File                fh,
                              SCOREP_MpiRequestId     id )
{
    scorep_mpi_request* req = scorep_mpi_request_create_entry( request, id, type,
                                                               SCOREP_MPI_REQUEST_FLAG_NONE );

    /* store request information */
    req->payload.io.bytes = bytes;
#if HAVE( DECL_PMPI_TYPE_DUP )
    PMPI_Type_dup( datatype, &req->payload.io.datatype );
#else
    req->payload.io.datatype = datatype;
#endif
    req->payload.io.fh = fh;
}

void
scorep_mpi_request_comm_idup_create( MPI_Request request,
                                     MPI_Comm    parentComm,
                                     MPI_Comm*   newComm )
{
    scorep_mpi_request* req = scorep_mpi_request_create_entry(
        request,
        UINT64_MAX, /* no events will be triggered, thus no ID needed */
        SCOREP_MPI_REQUEST_TYPE_COMM_IDUP,
        SCOREP_MPI_REQUEST_FLAG_NONE );

    /* store request information */
    req->payload.comm_idup.new_comm           = newComm;
    req->payload.comm_idup.parent_comm_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;
    if ( parentComm != MPI_COMM_NULL )
    {
        req->payload.comm_idup.parent_comm_handle = SCOREP_MPI_COMM_HANDLE( parentComm );
    }
}

void
scorep_mpi_request_win_create( MPI_Request             mpiRequest,
                               scorep_mpi_rma_request* rmaRequest )
{
    scorep_mpi_request* req = scorep_mpi_request_create_entry( mpiRequest,
                                                               rmaRequest->matching_id,
                                                               SCOREP_MPI_REQUEST_TYPE_RMA,
                                                               SCOREP_MPI_REQUEST_FLAG_NONE );

    /* store request information */
    req->payload.rma.request_ptr = rmaRequest;
}

scorep_mpi_request*
scorep_mpi_request_get( MPI_Request request )
{
    struct scorep_mpi_request_hash*  hash_entry = scorep_mpi_get_request_hash_entry( request );
    int                              i;
    struct scorep_mpi_request_block* block;
    scorep_mpi_request*              curr;

    /* list empty */
    if ( !hash_entry->lastreq )
    {
        return 0;
    }

    /* search all requests in all blocks */
    block = hash_entry->head_block;
    while ( block )
    {
        curr = &( block->req[ 0 ] );
        for ( i = 0; i < SCOREP_MPI_REQUEST_BLOCK_SIZE; ++i )
        {
            /* found? */
            if ( curr->request == request )
            {
                return curr;
            }

            /* end of list? */
            if ( curr == hash_entry->lastreq )
            {
                return 0;
            }

            curr++;
        }
        block = block->next;
    }
    return 0;
}

void
scorep_mpi_request_free( scorep_mpi_request* req )
{
    struct scorep_mpi_request_hash* hash_entry = scorep_mpi_get_request_hash_entry( req->request );

    /*
     * Drop type duplicate, but only if we could have make a duplicate in the
     * first place
     */
#if HAVE( DECL_PMPI_TYPE_DUP )
    if ( req->request_type == SCOREP_MPI_REQUEST_TYPE_SEND
         || req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV )
    {
        PMPI_Type_free( &req->payload.p2p.datatype );
    }
#endif

    /* delete request by copying last request in place of req */
    if ( !hash_entry->lastreq )
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_LAST_REQUEST,
                     "Please tell me what you were trying to do!" );
    }
    *req                              = *( hash_entry->lastreq );
    hash_entry->lastreq->request_type = SCOREP_MPI_REQUEST_TYPE_NONE;
    hash_entry->lastreq->flags        = SCOREP_MPI_REQUEST_FLAG_NONE;
    hash_entry->lastreq->request      = 0;

    /* adjust pointer to last request */
    hash_entry->lastidx--;
    if ( hash_entry->lastidx < 0 )
    {
        /* reached low end of block */
        if ( hash_entry->last_block->prev )
        {
            /* goto previous block if existing */
            hash_entry->lastidx = SCOREP_MPI_REQUEST_BLOCK_SIZE - 1;
            hash_entry->lastreq = &( hash_entry->last_block->prev->req[ hash_entry->lastidx ] );
        }
        else
        {
            /* no previous block: re-initialize */
            hash_entry->lastidx = SCOREP_MPI_REQUEST_BLOCK_SIZE;
            hash_entry->lastreq = 0;
        }
        hash_entry->last_block = hash_entry->last_block->prev;
    }
    else
    {
        hash_entry->lastreq--;
    }
}

void
scorep_mpi_test_request( scorep_mpi_request* req )
{
    if ( req->request_type == SCOREP_MPI_REQUEST_TYPE_IO_READ || req->request_type == SCOREP_MPI_REQUEST_TYPE_IO_WRITE )
    {
        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_MPI,
                                                                     &( req->payload.io.fh ) );
        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationTest( io_handle, req->id );
        }
    }
    else
    {
        SCOREP_MpiRequestTested( req->id );
    }
}

void
scorep_mpi_check_request( scorep_mpi_request* req,
                          MPI_Status*         status )
{
    const int p2p_events_active = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P );
    const int io_events_active  = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO );
    const int xnb_active        = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK );

    if ( !req ||
         ( ( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) &&
           !( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) ) )
    {
        return;
    }

    int cancelled = 0;
    if ( req->flags & SCOREP_MPI_REQUEST_FLAG_CAN_CANCEL )
    {
        PMPI_Test_cancelled( status, &cancelled );
    }
    if ( cancelled )
    {
        if ( xnb_active && req->id != UINT64_MAX )
        {
            SCOREP_MpiRequestCancelled( req->id );
        }
    }
    else
    {
        int count, sz;

        switch ( req->request_type )
        {
            case SCOREP_MPI_REQUEST_TYPE_RECV:
                if ( p2p_events_active && status->MPI_SOURCE != MPI_PROC_NULL )
                {
                    /* if receive request, write receive trace record */

                    PMPI_Type_size( req->payload.p2p.datatype, &sz );
                    PMPI_Get_count( status, req->payload.p2p.datatype, &count );

                    if ( xnb_active )
                    {
                        SCOREP_MpiIrecv( status->MPI_SOURCE, req->payload.p2p.comm_handle,
                                         status->MPI_TAG, ( uint64_t )count * sz, req->id );
                    }
                    else
                    {
                        SCOREP_MpiRecv( status->MPI_SOURCE, req->payload.p2p.comm_handle,
                                        status->MPI_TAG, ( uint64_t )count * sz );
                    }
                }
                break;

            case SCOREP_MPI_REQUEST_TYPE_SEND:
                if ( p2p_events_active && xnb_active )
                {
                    SCOREP_MpiIsendComplete( req->id );
                }
                break;

            case SCOREP_MPI_REQUEST_TYPE_COMM_IDUP:
                scorep_mpi_comm_create_finalize( *req->payload.comm_idup.new_comm,
                                                 req->payload.comm_idup.parent_comm_handle );
                break;

            case SCOREP_MPI_REQUEST_TYPE_RMA:
                // sanity check for rma_request handle
                UTILS_BUG_ON( req->payload.rma.request_ptr != NULL,
                              "No request information associated with MPI request." );
                UTILS_BUG_ON( req->payload.rma.request_ptr->mpi_handle == req->request,
                              "Request information inconsistent with associated MPI request" );

                // if request has not yet been completed locally
                // write an RmaOpCompleteNonBlocking event
                if ( !req->payload.rma.request_ptr->completed_locally )
                {
                    SCOREP_RmaOpCompleteNonBlocking( req->payload.rma.request_ptr->window, req->payload.rma.request_ptr->matching_id );
                    req->payload.rma.request_ptr->completed_locally = true;

                    if ( !req->payload.rma.request_ptr->schedule_removal
                         && req->payload.rma.request_ptr->completion_type == SCOREP_MPI_RMA_REQUEST_COMBINED_COMPLETION )
                    {
                        SCOREP_RmaOpCompleteRemote( req->payload.rma.request_ptr->window, req->payload.rma.request_ptr->matching_id );
                        req->payload.rma.request_ptr->schedule_removal = true;
                    }
                }
                if ( req->payload.rma.request_ptr->schedule_removal )
                {
                    scorep_mpi_rma_request_remove_by_ptr( req->payload.rma.request_ptr );
                }
                break;

            case SCOREP_MPI_REQUEST_TYPE_IO_READ:
            case SCOREP_MPI_REQUEST_TYPE_IO_WRITE:
                if ( io_events_active && xnb_active )
                {
                    PMPI_Type_size( req->payload.io.datatype, &sz );
                    PMPI_Get_count( status, req->payload.io.datatype, &count );

                    SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_MPI,
                                                                                 &( req->payload.io.fh ) );
                    if ( io_handle != SCOREP_INVALID_IO_HANDLE )
                    {
                        SCOREP_IoOperationComplete( io_handle,
                                                    req->request_type == SCOREP_MPI_REQUEST_TYPE_IO_READ
                                                    ? SCOREP_IO_OPERATION_MODE_READ
                                                    : SCOREP_IO_OPERATION_MODE_WRITE,
                                                    ( uint64_t )sz * count,
                                                    req->id /* matching id */ );
                    }
                }
                break;

            default:
                break;
        }
    }

    if ( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT )
    {
        /* if persistent request, set to inactive,
           and, if requested delete request */
        req->flags &= ~SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE;
        if ( req->flags & SCOREP_MPI_REQUEST_FLAG_DEALLOCATE )
        {
            scorep_mpi_request_free( req );
        }
    }
    else
    {
        /* if non-persistent request, delete always request */
        scorep_mpi_request_free( req );
    }
}

static MPI_Request* orig_req_arr      = 0;
static int          orig_req_arr_size = 0;

void
scorep_mpi_save_request_array( MPI_Request* arr_req, int arr_req_size )
{
    if ( orig_req_arr_size == 0 )
    {
        /* -- never used: initialize -- */
        orig_req_arr      = malloc( arr_req_size * sizeof( MPI_Request ) );
        orig_req_arr_size = arr_req_size;
    }
    else if ( arr_req_size > orig_req_arr_size )
    {
        /* -- not enough room: expand -- */
        orig_req_arr      = realloc( orig_req_arr, arr_req_size * sizeof( MPI_Request ) );
        orig_req_arr_size = arr_req_size;
    }

    /* -- copy array -- */
    /* for (i=0; i<arr_req_size; ++i) orig_req_arr[i] = arr_req[i]; */
    memcpy( orig_req_arr, arr_req, arr_req_size * sizeof( MPI_Request ) );
}

scorep_mpi_request*
scorep_mpi_saved_request_get( int i )
{
    return scorep_mpi_request_get( orig_req_arr[ i ] );
}

void
scorep_mpi_request_finalize( void )
{
    struct scorep_mpi_request_block* block;
    int                              i;

    /* free request blocks */

    for ( i = 0; i < SCOREP_MPI_REQUEST_TABLE_SIZE; ++i )
    {
        while ( scorep_mpi_request_table[ i ].head_block )
        {
            block                                    = scorep_mpi_request_table[ i ].head_block;
            scorep_mpi_request_table[ i ].head_block = scorep_mpi_request_table[ i ].head_block->next;
            free( block );
        }
    }
}
