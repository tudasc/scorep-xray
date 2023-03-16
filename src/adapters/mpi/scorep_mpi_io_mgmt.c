/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016, 2019, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017, 2019,
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
 */

/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Contains the I/O management functions
 */

#include <config.h>

#include "scorep_mpi_io_mgmt.h"

#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_IoManagement.h>
#include <SCOREP_FastHashtab.h>
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>

#include <stdlib.h>
#include <stdbool.h>

void
scorep_mpi_io_init( void )
{
    SCOREP_IoMgmt_RegisterParadigm( SCOREP_IO_PARADIGM_MPI,
                                    SCOREP_IO_PARADIGM_CLASS_PARALLEL,
                                    "MPI I/O",
                                    SCOREP_IO_PARADIGM_FLAG_NONE,
                                    sizeof( MPI_File ),
                                    SCOREP_INVALID_IO_PARADIGM_PROPERTY );
}

void
scorep_mpi_io_finalize( void )
{
    SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IO_PARADIGM_MPI );
}

typedef SCOREP_IoHandleHandle io_split_table_key_t;
typedef struct mpi_io_split_op
{
    SCOREP_MpiRequestId matching_id;
    MPI_Datatype        datatype;
    bool                is_active;
} mpi_io_split_op;
typedef mpi_io_split_op* io_split_table_value_t;

#define IO_SPLIT_TABLE_HASH_EXPONENT 7

static inline uint32_t
io_split_table_bucket_idx( io_split_table_key_t key )
{
    return SCOREP_LOCAL_HANDLE_DEREF( key, IoHandle )->hash_value & hashmask( IO_SPLIT_TABLE_HASH_EXPONENT );
}

static inline bool
io_split_table_equals( io_split_table_key_t key1,
                       io_split_table_key_t key2 )
{
    return key1 == key2;
}

static inline void*
io_split_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
io_split_table_free_chunk( void* chunk )
{
}

static inline io_split_table_value_t
io_split_table_value_ctor( io_split_table_key_t* key,
                           void*                 ctorData )
{
    io_split_table_value_t io_split_ctor_data = ctorData;

    io_split_table_value_t value = SCOREP_Memory_AllocForMisc( sizeof( *value ) );
    *value = *io_split_ctor_data;
#if HAVE( MPI_2_0_SYMBOL_PMPI_TYPE_DUP )
    PMPI_Type_dup( value->datatype, &value->datatype );
#endif

    return value;
}

/* nPairsPerChunk: 4+8 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( io_split_table,
                             10,
                             hashsize( IO_SPLIT_TABLE_HASH_EXPONENT ) );

void
scorep_mpi_io_split_begin( SCOREP_IoHandleHandle ioHandle,
                           uint64_t              matchingId,
                           MPI_Datatype          datatype )
{
    mpi_io_split_op io_split_ctor_data =
    {
        .matching_id = matchingId,
        .datatype    = datatype, /* will get duplicated in the ctor */
        .is_active   = false     /* newly ctored objects are not active */
    };

    io_split_table_value_t split_op = NULL;
    io_split_table_get_and_insert( ioHandle, &io_split_ctor_data, &split_op );

    UTILS_BUG_ON( split_op->is_active, "Already active split I/O operation on handle %u", ioHandle );

    split_op->is_active = true;
}

void
scorep_mpi_io_split_end( SCOREP_IoHandleHandle ioHandle,
                         uint64_t*             matchingId,
                         MPI_Datatype*         datatype )
{
    io_split_table_value_t split_op;
    if ( !io_split_table_get( ioHandle, &split_op ) || !split_op->is_active )
    {
        UTILS_FATAL( "Started split I/O operation not found for handle %u", ioHandle );
    }

    *matchingId = split_op->matching_id;
    *datatype   = split_op->datatype;

    split_op->is_active = false;
}
