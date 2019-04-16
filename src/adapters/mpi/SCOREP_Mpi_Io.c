/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for parallel I/O
 */

#include <config.h>
#include "SCOREP_Mpi.h"
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_IoManagement.h>

#include "scorep_mpi_communicator.h"

/**
 * MPI-I/O hashtable for managing I/O split operations.
 */
extern SCOREP_Hashtab* scorep_mpi_io_split_table;

typedef struct mpi_io_split_op
{
    SCOREP_MpiRequestId matching_id;
    MPI_Datatype        datatype;
    bool                is_active;
} mpi_io_split_op;

static inline SCOREP_IoAccessMode
mpi_io_get_access_mode( int amode )
{
    SCOREP_IoAccessMode scorep_mode = SCOREP_IO_ACCESS_MODE_NONE;

    if ( amode & MPI_MODE_RDONLY )
    {
        scorep_mode = SCOREP_IO_ACCESS_MODE_READ_ONLY;
    }
    else if ( amode & MPI_MODE_WRONLY )
    {
        scorep_mode = SCOREP_IO_ACCESS_MODE_WRITE_ONLY;
    }
    else if ( amode & MPI_MODE_RDWR )
    {
        scorep_mode = SCOREP_IO_ACCESS_MODE_READ_WRITE;
    }

    return scorep_mode;
}

static inline SCOREP_IoCreationFlag
mpi_io_get_creation_flags( int amode )
{
    SCOREP_IoCreationFlag creation_flags = SCOREP_IO_CREATION_FLAG_NONE;

    /* Handle creation flags */
    if ( amode & MPI_MODE_CREATE )
    {
        creation_flags |= SCOREP_IO_CREATION_FLAG_CREATE;
    }
    if ( amode & MPI_MODE_EXCL )
    {
        creation_flags |= SCOREP_IO_CREATION_FLAG_EXCLUSIVE;
    }

    return creation_flags;
}

static inline SCOREP_IoStatusFlag
mpi_io_get_status_flags( int amode )
{
    SCOREP_IoStatusFlag status_flags = SCOREP_IO_STATUS_FLAG_NONE;

    /* Handle status flags */
    if ( amode & MPI_MODE_APPEND )
    {
        status_flags |= SCOREP_IO_STATUS_FLAG_APPEND;
    }
    if ( amode & MPI_MODE_DELETE_ON_CLOSE )
    {
        status_flags |= SCOREP_IO_STATUS_FLAG_CLOSE_ON_EXEC;
    }

    return status_flags;
}

static inline SCOREP_IoSeekOption
mpi_io_get_seek_option( int whence )
{
    switch ( whence )
    {
        case MPI_SEEK_SET:
            return SCOREP_IO_SEEK_FROM_START;
        case MPI_SEEK_CUR:
            return SCOREP_IO_SEEK_FROM_CURRENT;
        case MPI_SEEK_END:
            return SCOREP_IO_SEEK_FROM_END;
    }

    return SCOREP_IO_SEEK_INVALID;
}

static inline mpi_io_split_op*
mpi_io_find_split_op( SCOREP_IoHandleHandle ioHandle )
{
    int32_t               tmp_io_handle = ( int32_t )ioHandle;
    SCOREP_Hashtab_Entry* entry         = SCOREP_Hashtab_Find( scorep_mpi_io_split_table,
                                                               ( void* )&tmp_io_handle,
                                                               NULL );

    return ( !entry ) ? NULL : entry->value.ptr;
}

static inline void
mpi_io_split_end( SCOREP_IoHandleHandle ioHandle,
                  uint64_t*             matching_id,
                  MPI_Datatype*         datatype )
{
    mpi_io_split_op* split_op = mpi_io_find_split_op( ioHandle );

    UTILS_BUG_ON( !split_op && !split_op->is_active, "Started split operation not found" );

    *matching_id = split_op->matching_id;
    *datatype    = split_op->datatype;

    split_op->is_active = false;
}


static inline void
mpi_io_split_begin( SCOREP_IoHandleHandle ioHandle,
                    uint64_t              matching_id,
                    MPI_Datatype          datatype )
{
    mpi_io_split_op* split_op = mpi_io_find_split_op( ioHandle );

    if ( !split_op )
    {
        void* allocated_memory = SCOREP_Memory_AllocForMisc( sizeof( mpi_io_split_op ) + sizeof( SCOREP_IoHandleHandle ) );

        split_op              = allocated_memory;
        split_op->matching_id = matching_id;
        split_op->datatype    = datatype;

        SCOREP_IoHandleHandle* tmp_io_handle = ( void* )( ( char* )allocated_memory + sizeof( *split_op ) );
        *tmp_io_handle = ioHandle;

        SCOREP_Hashtab_InsertPtr( scorep_mpi_io_split_table,
                                  ( void* )tmp_io_handle,
                                  ( void* )split_op,
                                  NULL );
    }
    else
    {
        split_op->matching_id = matching_id;
        split_op->datatype    = datatype;
        split_op->is_active   = true;
    }
}

static inline int
mpi_io_get_type_size( MPI_Datatype datatype )
{
    int type_size = 0;
    PMPI_Type_size( datatype, &type_size );
    return type_size;
}

/**
 * @name C wrappers for administrative functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_OPEN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_open )
/**
 * Measurement wrapper for MPI_File_open
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_open call with enter and exit events.
 */
int
MPI_File_open( MPI_Comm comm, SCOREP_MPI_CONST_DECL char* filename, int amode, MPI_Info info, MPI_File* fh )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_OPEN ] );

            SCOREP_InterimCommunicatorHandle    comm_handle  = SCOREP_MPI_COMM_HANDLE( comm );
            scorep_mpi_comm_definition_payload* comm_payload =
                SCOREP_InterimCommunicatorHandle_GetPayload( comm_handle );
            uint32_t unify_key = ++comm_payload->io_handle_counter /* avoid zero as key */;
            SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_MPI,
                                               SCOREP_IO_HANDLE_FLAG_NONE,
                                               comm_handle,
                                               unify_key, "" );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_OPEN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_open( comm, filename, amode, info, fh );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS )
            {
                SCOREP_IoFileHandle   file      = SCOREP_IoMgmt_GetIoFileHandle( filename );
                SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_CompleteHandleCreation(
                    SCOREP_IO_PARADIGM_MPI, file, fh );
                if ( io_handle != SCOREP_INVALID_IO_HANDLE )
                {
                    SCOREP_IoCreateHandle( io_handle,
                                           mpi_io_get_access_mode( amode ),
                                           mpi_io_get_creation_flags( amode ),
                                           mpi_io_get_status_flags( amode ) );
                }
            }
            else
            {
                SCOREP_IoMgmt_DropIncompleteHandle();
            }

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_OPEN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_OPEN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_CLOSE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_close )
/**
 * Measurement wrapper for MPI_File_close
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_close call with enter and exit events.
 */
int
MPI_File_close( MPI_File* fh )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;
    int                   amode;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CLOSE ] );
            io_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_MPI, fh );
            PMPI_File_get_amode( *fh, &amode );

            SCOREP_IoMgmt_PushHandle( io_handle );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CLOSE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_close( fh );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoMgmt_PopHandle( io_handle );
                if ( return_val == MPI_SUCCESS )
                {
                    if ( amode & MPI_MODE_DELETE_ON_CLOSE )
                    {
                        SCOREP_IoFileHandle file = SCOREP_IoHandleHandle_GetIoFile( io_handle );
                        SCOREP_IoDeleteFile( SCOREP_IO_PARADIGM_MPI, file );
                    }
                    SCOREP_IoDestroyHandle( io_handle );
                    SCOREP_IoMgmt_DestroyHandle( io_handle );
                }
                else
                {
                    SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_MPI, io_handle );
                }
            }
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CLOSE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CLOSE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_seek )
/**
 * Measurement wrapper for MPI_File_seek
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_seek call with enter and exit events.
 */
int
MPI_File_seek( MPI_File fh, MPI_Offset offset, int whence )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_seek( fh, offset, whence );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                MPI_Offset pos;
                PMPI_File_get_position( fh, &pos );
                SCOREP_IoSeekOption seek_option = mpi_io_get_seek_option( whence );

                SCOREP_IoSeek( io_handle,
                               offset,
                               seek_option,
                               pos );

                SCOREP_IoMgmt_PopHandle( io_handle );
            }

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_seek_shared )
/**
 * Measurement wrapper for MPI_File_seek_shared
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_seek_shared call with enter and exit events.
 */
int
MPI_File_seek_shared( MPI_File fh, MPI_Offset offset, int whence )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK_SHARED ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK_SHARED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_seek_shared( fh, offset, whence );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoSeekOption seek_option = mpi_io_get_seek_option( whence );

                MPI_Offset pos;
                PMPI_File_get_position( fh, &pos );

                SCOREP_IoSeek( io_handle,
                               offset,
                               seek_option,
                               pos );
                SCOREP_IoMgmt_PopHandle( io_handle );
            }

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SEEK_SHARED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_DELETE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_delete )
/**
 * Measurement wrapper for MPI_File_delete
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_delete call with enter and exit events.
 */
int
MPI_File_delete( SCOREP_MPI_CONST_DECL char* filename, MPI_Info info )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoFileHandle ifh                        = SCOREP_INVALID_IO_FILE;
    int                 return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_DELETE ] );

            ifh = SCOREP_IoMgmt_GetIoFileHandle( filename );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_DELETE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_delete( filename, info );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( ifh != SCOREP_INVALID_IO_FILE )
            {
                SCOREP_IoDeleteFile( SCOREP_IO_PARADIGM_MPI, ifh );
            }
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_DELETE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_DELETE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_AMODE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_amode )
/**
 * Measurement wrapper for MPI_File_get_amode
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_amode call with enter and exit events.
 */
int
MPI_File_get_amode( MPI_File fh, int* amode )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_AMODE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_AMODE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_amode( fh, amode );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_AMODE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_AMODE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ATOMICITY ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_atomicity )
/**
 * Measurement wrapper for MPI_File_get_atomicity
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_atomicity call with enter and exit events.
 */
int
MPI_File_get_atomicity( MPI_File fh, int* flag )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ATOMICITY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ATOMICITY ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_atomicity( fh, flag );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ATOMICITY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ATOMICITY ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_BYTE_OFFSET ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_byte_offset )
/**
 * Measurement wrapper for MPI_File_get_byte_offset
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_byte_offset call with enter and exit events.
 */
int
MPI_File_get_byte_offset( MPI_File fh, MPI_Offset offset, MPI_Offset* disp )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_BYTE_OFFSET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_BYTE_OFFSET ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_byte_offset( fh, offset, disp );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_BYTE_OFFSET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_BYTE_OFFSET ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_GROUP ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_group )
/**
 * Measurement wrapper for MPI_File_get_group
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_group call with enter and exit events.
 */
int
MPI_File_get_group( MPI_File fh, MPI_Group* group )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_GROUP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_GROUP ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_group( fh, group );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_GROUP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_GROUP ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_INFO ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_info )
/**
 * Measurement wrapper for MPI_File_get_info
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_info call with enter and exit events.
 */
int
MPI_File_get_info( MPI_File fh, MPI_Info* info_used )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_INFO ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_INFO ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_info( fh, info_used );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_INFO ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_INFO ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_position )
/**
 * Measurement wrapper for MPI_File_get_position
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_position call with enter and exit events.
 */
int
MPI_File_get_position( MPI_File fh, MPI_Offset* offset )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_position( fh, offset );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_position_shared )
/**
 * Measurement wrapper for MPI_File_get_position_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_position_shared call with enter and exit events.
 */
int
MPI_File_get_position_shared( MPI_File fh, MPI_Offset* offset )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION_SHARED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_position_shared( fh, offset );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_POSITION_SHARED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_SIZE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_size )
/**
 * Measurement wrapper for MPI_File_get_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_size call with enter and exit events.
 */
int
MPI_File_get_size( MPI_File fh, MPI_Offset* size )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_SIZE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_SIZE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_size( fh, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_SIZE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_SIZE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_TYPE_EXTENT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_type_extent )
/**
 * Measurement wrapper for MPI_File_get_type_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_type_extent call with enter and exit events.
 */
int
MPI_File_get_type_extent( MPI_File fh, MPI_Datatype datatype, MPI_Aint* extent )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_TYPE_EXTENT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_TYPE_EXTENT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_type_extent( fh, datatype, extent );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_TYPE_EXTENT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_TYPE_EXTENT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_VIEW ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_view )
/**
 * Measurement wrapper for MPI_File_get_view
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_view call with enter and exit events.
 */
int
MPI_File_get_view( MPI_File fh, MPI_Offset* disp, MPI_Datatype* etype, MPI_Datatype* filetype, char* datarep )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_VIEW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_VIEW ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_view( fh, disp, etype, filetype, datarep );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_VIEW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_VIEW ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_PREALLOCATE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_preallocate )
/**
 * Measurement wrapper for MPI_File_preallocate
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_preallocate call with enter and exit events.
 */
int
MPI_File_preallocate( MPI_File fh, MPI_Offset size )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_PREALLOCATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_PREALLOCATE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_preallocate( fh, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_PREALLOCATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_PREALLOCATE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ATOMICITY ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_atomicity )
/**
 * Measurement wrapper for MPI_File_set_atomicity
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_set_atomicity call with enter and exit events.
 */
int
MPI_File_set_atomicity( MPI_File fh, int flag )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ATOMICITY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ATOMICITY ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_set_atomicity( fh, flag );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ATOMICITY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ATOMICITY ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_INFO ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_info )
/**
 * Measurement wrapper for MPI_File_set_info
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_set_info call with enter and exit events.
 */
int
MPI_File_set_info( MPI_File fh, MPI_Info info )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_INFO ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_INFO ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_set_info( fh, info );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_INFO ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_INFO ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_SIZE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_size )
/**
 * Measurement wrapper for MPI_File_set_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_set_size call with enter and exit events.
 */
int
MPI_File_set_size( MPI_File fh, MPI_Offset size )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_SIZE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_SIZE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_set_size( fh, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_SIZE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_SIZE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_VIEW ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_view )
/**
 * Measurement wrapper for MPI_File_set_view
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_set_view call with enter and exit events.
 */
int
MPI_File_set_view( MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, SCOREP_MPI_CONST_DECL char* datarep, MPI_Info info )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_VIEW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_VIEW ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_set_view( fh, disp, etype, filetype, datarep, info );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_VIEW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_VIEW ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SYNC ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_sync )
/**
 * Measurement wrapper for MPI_File_sync
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_sync call with enter and exit events.
 */
int
MPI_File_sync( MPI_File fh )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SYNC ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SYNC ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_sync( fh );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SYNC ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SYNC ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_REGISTER_DATAREP ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Register_datarep )
/**
 * Measurement wrapper for MPI_Register_datarep
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_Register_datarep call with enter and exit events.
 */
int
MPI_Register_datarep( SCOREP_MPI_CONST_DECL char* datarep, MPI_Datarep_conversion_function* read_conversion_fn, MPI_Datarep_conversion_function* write_conversion_fn, MPI_Datarep_extent_function* dtype_file_extent_fn, void* extra_state )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REGISTER_DATAREP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REGISTER_DATAREP ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Register_datarep( datarep, read_conversion_fn, write_conversion_fn, dtype_file_extent_fn, extra_state );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REGISTER_DATAREP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REGISTER_DATAREP ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 * @name C wrappers for access functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_READ ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read )
/**
 * Measurement wrapper for MPI_File_read
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_read( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all )
/**
 * Measurement wrapper for MPI_File_read_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_read_all( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_all( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at )
/**
 * Measurement wrapper for MPI_File_read_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_read_at( MPI_File fh, MPI_Offset offset, void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_at( fh, offset, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all )
/**
 * Measurement wrapper for MPI_File_read_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_read_at_all( MPI_File fh, MPI_Offset offset, void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_at_all( fh, offset, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered )
/**
 * Measurement wrapper for MPI_File_read_ordered
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_read_ordered( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_ordered( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_shared )
/**
 * Measurement wrapper for MPI_File_read_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_read_shared( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_SHARED ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_SHARED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_shared( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_SHARED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write )
/**
 * Measurement wrapper for MPI_File_write
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_write( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all )
/**
 * Measurement wrapper for MPI_File_write_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_write_all( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_all( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at )
/**
 * Measurement wrapper for MPI_File_write_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_write_at( MPI_File fh, MPI_Offset offset, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_at( fh, offset, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all )
/**
 * Measurement wrapper for MPI_File_write_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_write_at_all( MPI_File fh, MPI_Offset offset, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_at_all( fh, offset, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered )
/**
 * Measurement wrapper for MPI_File_write_ordered
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_write_ordered( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_ordered( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_shared )
/**
 * Measurement wrapper for MPI_File_write_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoBlocking.w
 * @note C interface
 * @note Introduced with MPI 2
 * @ingroup io
 */
int
MPI_File_write_shared( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    SCOREP_MpiRequestId   req_id;
    MPI_Status            mystatus;
    int                   n_elements;
    int                   type_size;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_SHARED ] );

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );

            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                req_id    = scorep_mpi_get_request_id();
                type_size = mpi_io_get_type_size( datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );

                if ( status == MPI_STATUS_IGNORE )
                {
                    status = &mystatus;
                }
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_SHARED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_shared( fh, buf, count, datatype, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            req_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_SHARED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/* MPI Implementation which use ROMIO can have no general support for MPI_Requests
   in asynchronous file IO operations. This leads to a different signature and
   thus to compiler errors. To avoid compilation errors, the asynchronous file
   IO functions are not wrapped if ROMIO is used and they support no general
   MPI_Requests.
 */
#if !defined( ROMIO_VERSION ) || defined( MPIO_USES_MPI_REQUEST )

#if HAVE( DECL_PMPI_FILE_IREAD ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread )
/**
 * Measurement wrapper for MPI_File_iread
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iread call with enter and exit events.
 */
int
MPI_File_iread( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iread( fh, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_READ,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_all )
/**
 * Measurement wrapper for MPI_File_iread_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iread_all call with enter and exit events.
 */
int
MPI_File_iread_all( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_ALL ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iread_all( fh, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_READ,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_at )
/**
 * Measurement wrapper for MPI_File_iread_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iread_at call with enter and exit events.
 */
int
MPI_File_iread_at( MPI_File fh, MPI_Offset offset, void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iread_at( fh, offset, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_READ,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_at_all )
/**
 * Measurement wrapper for MPI_File_iread_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iread_at_all call with enter and exit events.
 */
int
MPI_File_iread_at_all( MPI_File fh, MPI_Offset offset, void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT_ALL ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iread_at_all( fh, offset, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_READ,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_AT_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_shared )
/**
 * Measurement wrapper for MPI_File_iread_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iread_shared call with enter and exit events.
 */
int
MPI_File_iread_shared( MPI_File fh, void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_SHARED ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_SHARED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iread_shared( fh, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_READ,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IREAD_SHARED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite )
/**
 * Measurement wrapper for MPI_File_iwrite
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iwrite call with enter and exit events.
 */
int
MPI_File_iwrite( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iwrite( fh, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_WRITE,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_all )
/**
 * Measurement wrapper for MPI_File_iwrite_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iwrite_all call with enter and exit events.
 */
int
MPI_File_iwrite_all( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_ALL ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iwrite_all( fh, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_WRITE,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_at )
/**
 * Measurement wrapper for MPI_File_iwrite_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iwrite_at call with enter and exit events.
 */
int
MPI_File_iwrite_at( MPI_File fh, MPI_Offset offset, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iwrite_at( fh, offset, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_WRITE,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_at_all )
/**
 * Measurement wrapper for MPI_File_iwrite_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iwrite_at_all call with enter and exit events.
 */
int
MPI_File_iwrite_at_all( MPI_File fh, MPI_Offset offset, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT_ALL ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT_ALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iwrite_at_all( fh, offset, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_WRITE,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT_ALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT_ALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_shared )
/**
 * Measurement wrapper for MPI_File_iwrite_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoNonBlocking.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_iwrite_shared call with enter and exit events.
 */
int
MPI_File_iwrite_shared( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   type_size;
    SCOREP_MpiRequestId   req_id;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_SHARED ] );

            type_size = mpi_io_get_type_size( datatype );
            req_id    = scorep_mpi_get_request_id();

            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id /* matching id */ );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_SHARED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_iwrite_shared( fh, buf, count, datatype, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                {
                    scorep_mpi_request_io_create( *request,
                                                  SCOREP_MPI_REQUEST_TYPE_IO_WRITE,
                                                  ( uint64_t )( count * type_size ),
                                                  datatype,
                                                  fh,
                                                  req_id );
                }
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_SHARED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_IWRITE_SHARED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#endif /* !defined(ROMIO_VERSION) || defined(MPIO_USES_MPI_REQUEST) */

#if HAVE( DECL_PMPI_FILE_READ_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all_begin )
/**
 * Measurement wrapper for MPI_File_read_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitBegin.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_read_all_begin call with enter and exit events.
 */
int
MPI_File_read_all_begin( MPI_File fh, void* buf, int count, MPI_Datatype datatype )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_BEGIN ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                const int                 type_size = mpi_io_get_type_size( datatype );
                const SCOREP_MpiRequestId req_id    = scorep_mpi_get_request_id();

                mpi_io_split_begin( io_handle, req_id, datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_BEGIN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_all_begin( fh, buf, count, datatype );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_IoMgmt_PopHandle( io_handle );
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_BEGIN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_BEGIN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all_begin )
/**
 * Measurement wrapper for MPI_File_read_at_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitBegin.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_read_at_all_begin call with enter and exit events.
 */
int
MPI_File_read_at_all_begin( MPI_File fh, MPI_Offset offset, void* buf, int count, MPI_Datatype datatype )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_BEGIN ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                const int                 type_size = mpi_io_get_type_size( datatype );
                const SCOREP_MpiRequestId req_id    = scorep_mpi_get_request_id();

                mpi_io_split_begin( io_handle, req_id, datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_BEGIN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_at_all_begin( fh, offset, buf, count, datatype );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_IoMgmt_PopHandle( io_handle );
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_BEGIN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_BEGIN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered_begin )
/**
 * Measurement wrapper for MPI_File_read_ordered_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitBegin.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_read_ordered_begin call with enter and exit events.
 */
int
MPI_File_read_ordered_begin( MPI_File fh, void* buf, int count, MPI_Datatype datatype )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_BEGIN ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                const int                 type_size = mpi_io_get_type_size( datatype );
                const SCOREP_MpiRequestId req_id    = scorep_mpi_get_request_id();

                mpi_io_split_begin( io_handle, req_id, datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_READ,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_BEGIN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_ordered_begin( fh, buf, count, datatype );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_IoMgmt_PopHandle( io_handle );
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_BEGIN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_BEGIN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all_begin )
/**
 * Measurement wrapper for MPI_File_write_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitBegin.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_write_all_begin call with enter and exit events.
 */
int
MPI_File_write_all_begin( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_BEGIN ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                const int                 type_size = mpi_io_get_type_size( datatype );
                const SCOREP_MpiRequestId req_id    = scorep_mpi_get_request_id();

                mpi_io_split_begin( io_handle, req_id, datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_BEGIN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_all_begin( fh, buf, count, datatype );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_IoMgmt_PopHandle( io_handle );
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_BEGIN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_BEGIN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all_begin )
/**
 * Measurement wrapper for MPI_File_write_at_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitBegin.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_write_at_all_begin call with enter and exit events.
 */
int
MPI_File_write_at_all_begin( MPI_File fh, MPI_Offset offset, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_BEGIN ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                const int                 type_size = mpi_io_get_type_size( datatype );
                const SCOREP_MpiRequestId req_id    = scorep_mpi_get_request_id();

                mpi_io_split_begin( io_handle, req_id, datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_BEGIN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_at_all_begin( fh, offset, buf, count, datatype );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_IoMgmt_PopHandle( io_handle );
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_BEGIN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_BEGIN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered_begin )
/**
 * Measurement wrapper for MPI_File_write_ordered_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitBegin.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_write_ordered_begin call with enter and exit events.
 */
int
MPI_File_write_ordered_begin( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, int count, MPI_Datatype datatype )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_BEGIN ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                const int                 type_size = mpi_io_get_type_size( datatype );
                const SCOREP_MpiRequestId req_id    = scorep_mpi_get_request_id();

                mpi_io_split_begin( io_handle, req_id, datatype );

                SCOREP_IoOperationBegin( io_handle,
                                         SCOREP_IO_OPERATION_MODE_WRITE,
                                         SCOREP_IO_OPERATION_FLAG_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )count * type_size,
                                         req_id );
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_BEGIN ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_ordered_begin( fh, buf, count, datatype );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_IoMgmt_PopHandle( io_handle );
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_BEGIN ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_BEGIN ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all_end )
/**
 * Measurement wrapper for MPI_File_read_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_read_all_end call with enter and exit events.
 */
int
MPI_File_read_all_end( MPI_File fh, void* buf, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    MPI_Status            mystatus;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_END ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( status == MPI_STATUS_IGNORE )
            {
                status = &mystatus;
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_END ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_all_end( fh, buf, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                int          n_elements;
                uint64_t     matching_id;
                MPI_Datatype datatype;

                mpi_io_split_end( io_handle, &matching_id, &datatype );

                const int type_size = mpi_io_get_type_size( datatype );
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            matching_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_END ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ALL_END ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all_end )
/**
 * Measurement wrapper for MPI_File_read_at_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_read_at_all_end call with enter and exit events.
 */
int
MPI_File_read_at_all_end( MPI_File fh, void* buf, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    MPI_Status            mystatus;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_END ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( status == MPI_STATUS_IGNORE )
            {
                status = &mystatus;
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_END ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_at_all_end( fh, buf, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                int          n_elements;
                uint64_t     matching_id;
                MPI_Datatype datatype;

                mpi_io_split_end( io_handle, &matching_id, &datatype );

                const int type_size = mpi_io_get_type_size( datatype );
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            matching_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_END ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_END ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered_end )
/**
 * Measurement wrapper for MPI_File_read_ordered_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_read_ordered_end call with enter and exit events.
 */
int
MPI_File_read_ordered_end( MPI_File fh, void* buf, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    MPI_Status            mystatus;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_END ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( status == MPI_STATUS_IGNORE )
            {
                status = &mystatus;
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_END ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_read_ordered_end( fh, buf, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                int          n_elements;
                uint64_t     matching_id;
                MPI_Datatype datatype;

                mpi_io_split_end( io_handle, &matching_id, &datatype );

                const int type_size = mpi_io_get_type_size( datatype );
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_READ,
                                            ( uint64_t )n_elements * type_size,
                                            matching_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_END ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_END ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all_end )
/**
 * Measurement wrapper for MPI_File_write_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_write_all_end call with enter and exit events.
 */
int
MPI_File_write_all_end( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    MPI_Status            mystatus;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_END ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( status == MPI_STATUS_IGNORE )
            {
                status = &mystatus;
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_END ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_all_end( fh, buf, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                int          n_elements;
                uint64_t     matching_id;
                MPI_Datatype datatype;

                mpi_io_split_end( io_handle, &matching_id, &datatype );

                const int type_size = mpi_io_get_type_size( datatype );
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            matching_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_END ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_END ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all_end )
/**
 * Measurement wrapper for MPI_File_write_at_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_write_at_all_end call with enter and exit events.
 */
int
MPI_File_write_at_all_end( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    MPI_Status            mystatus;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_END ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( status == MPI_STATUS_IGNORE )
            {
                status = &mystatus;
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_END ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_at_all_end( fh, buf, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                int          n_elements;
                uint64_t     matching_id;
                MPI_Datatype datatype;

                mpi_io_split_end( io_handle, &matching_id, &datatype );

                const int type_size = mpi_io_get_type_size( datatype );
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            matching_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_END ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_END ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered_end )
/**
 * Measurement wrapper for MPI_File_write_ordered_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io
 * Triggers an enter and exit event.
 * It wraps the MPI_File_write_ordered_end call with enter and exit events.
 */
int
MPI_File_write_ordered_end( MPI_File fh, SCOREP_MPI_CONST_DECL void* buf, MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int             event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int             event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO );
    SCOREP_IoHandleHandle io_handle                  = SCOREP_INVALID_IO_HANDLE;
    MPI_Status            mystatus;
    int                   return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_END ] );
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
            if ( status == MPI_STATUS_IGNORE )
            {
                status = &mystatus;
            }
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_END ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_write_ordered_end( fh, buf, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            if ( io_handle != SCOREP_INVALID_IO_HANDLE )
            {
                int          n_elements;
                uint64_t     matching_id;
                MPI_Datatype datatype;

                mpi_io_split_end( io_handle, &matching_id, &datatype );

                const int type_size = mpi_io_get_type_size( datatype );
                PMPI_Get_count( status, datatype, &n_elements );
                SCOREP_IoOperationComplete( io_handle,
                                            SCOREP_IO_OPERATION_MODE_WRITE,
                                            ( uint64_t )n_elements * type_size,
                                            matching_id );
            }

            SCOREP_IoMgmt_PopHandle( io_handle );

            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_END ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_END ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 * @name C wrappers for error handling functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_call_errhandler )
/**
 * Measurement wrapper for MPI_File_call_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * Triggers an enter and exit event.
 * It wraps the MPI_File_call_errhandler call with enter and exit events.
 */
int
MPI_File_call_errhandler( MPI_File fh, int errorcode )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO_ERR );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CALL_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CALL_ERRHANDLER ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_call_errhandler( fh, errorcode );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CALL_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CALL_ERRHANDLER ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_create_errhandler )
/**
 * Measurement wrapper for MPI_File_create_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * Triggers an enter and exit event.
 * It wraps the MPI_File_create_errhandler call with enter and exit events.
 */
int
MPI_File_create_errhandler( MPI_File_errhandler_fn* function, MPI_Errhandler* errhandler )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO_ERR );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CREATE_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CREATE_ERRHANDLER ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_create_errhandler( function, errhandler );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CREATE_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_CREATE_ERRHANDLER ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_get_errhandler )
/**
 * Measurement wrapper for MPI_File_get_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * Triggers an enter and exit event.
 * It wraps the MPI_File_get_errhandler call with enter and exit events.
 */
int
MPI_File_get_errhandler( MPI_File file, MPI_Errhandler* errhandler )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO_ERR );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ERRHANDLER ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_get_errhandler( file, errhandler );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_GET_ERRHANDLER ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_set_errhandler )
/**
 * Measurement wrapper for MPI_File_set_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * Triggers an enter and exit event.
 * It wraps the MPI_File_set_errhandler call with enter and exit events.
 */
int
MPI_File_set_errhandler( MPI_File file, MPI_Errhandler errhandler )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_IO_ERR );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ERRHANDLER ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_File_set_errhandler( file, errhandler );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ERRHANDLER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_FILE_SET_ERRHANDLER ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */
