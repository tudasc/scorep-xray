/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */

#ifndef SCOREP_SHMEM_COMMUNICATOR_MGMT_H
#define SCOREP_SHMEM_COMMUNICATOR_MGMT_H

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Types.h>

#include <shmem.h>
#include <stdbool.h>


struct scorep_definitions_manager_entry;
extern struct scorep_definitions_manager_entry scorep_shmem_pe_groups;


/**
 * Payload data of SHMEM communicator
 */
typedef struct scorep_shmem_comm_definition_payload
{
    int                    pe_start;
    int                    log_pe_stride;
    int                    pe_size;
    SCOREP_RmaWindowHandle rma_win;
} scorep_shmem_comm_definition_payload;

/**
 * Get window handle for requested group of active processing elements.
 */
SCOREP_RmaWindowHandle
scorep_shmem_get_pe_group( int peStart,
                           int logPEStride,
                           int PE_size );

/**
 * Trigger writing of RmaWinDestroy record for each window in the hash table.
 */
void
scorep_shmem_close_pe_group( void );


#endif /* SCOREP_SHMEM_COMMUNICATOR_MGMT_H */
