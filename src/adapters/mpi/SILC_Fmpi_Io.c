/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */



/**
 * @file  SILC_Fmpi_Io.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for parallel I/O
 */

#include "SILC_Fmpi.h"
#include "config.h"

/* lowercase defines */
/** @def MPI_File_close_L
    Exchanges MPI_File_close_L by mpi_file_close.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_close_L mpi_file_close

/** @def MPI_File_delete_L
    Exchanges MPI_File_delete_L by mpi_file_delete.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_delete_L mpi_file_delete

/** @def MPI_File_get_amode_L
    Exchanges MPI_File_get_amode_L by mpi_file_get_amode.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_amode_L mpi_file_get_amode

/** @def MPI_File_get_atomicity_L
    Exchanges MPI_File_get_atomicity_L by mpi_file_get_atomicity.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_atomicity_L mpi_file_get_atomicity

/** @def MPI_File_get_byte_offset_L
    Exchanges MPI_File_get_byte_offset_L by mpi_file_get_byte_offset.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_byte_offset_L mpi_file_get_byte_offset

/** @def MPI_File_get_group_L
    Exchanges MPI_File_get_group_L by mpi_file_get_group.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_group_L mpi_file_get_group

/** @def MPI_File_get_info_L
    Exchanges MPI_File_get_info_L by mpi_file_get_info.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_info_L mpi_file_get_info

/** @def MPI_File_get_position_L
    Exchanges MPI_File_get_position_L by mpi_file_get_position.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_position_L mpi_file_get_position

/** @def MPI_File_get_position_shared_L
    Exchanges MPI_File_get_position_shared_L by mpi_file_get_position_shared.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_position_shared_L mpi_file_get_position_shared

/** @def MPI_File_get_size_L
    Exchanges MPI_File_get_size_L by mpi_file_get_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_size_L mpi_file_get_size

/** @def MPI_File_get_type_extent_L
    Exchanges MPI_File_get_type_extent_L by mpi_file_get_type_extent.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_type_extent_L mpi_file_get_type_extent

/** @def MPI_File_get_view_L
    Exchanges MPI_File_get_view_L by mpi_file_get_view.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_get_view_L mpi_file_get_view

/** @def MPI_File_iread_L
    Exchanges MPI_File_iread_L by mpi_file_iread.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_iread_L mpi_file_iread

/** @def MPI_File_iread_at_L
    Exchanges MPI_File_iread_at_L by mpi_file_iread_at.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_iread_at_L mpi_file_iread_at

/** @def MPI_File_iread_shared_L
    Exchanges MPI_File_iread_shared_L by mpi_file_iread_shared.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_iread_shared_L mpi_file_iread_shared

/** @def MPI_File_iwrite_L
    Exchanges MPI_File_iwrite_L by mpi_file_iwrite.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_iwrite_L mpi_file_iwrite

/** @def MPI_File_iwrite_at_L
    Exchanges MPI_File_iwrite_at_L by mpi_file_iwrite_at.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_iwrite_at_L mpi_file_iwrite_at

/** @def MPI_File_iwrite_shared_L
    Exchanges MPI_File_iwrite_shared_L by mpi_file_iwrite_shared.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_iwrite_shared_L mpi_file_iwrite_shared

/** @def MPI_File_open_L
    Exchanges MPI_File_open_L by mpi_file_open.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_open_L mpi_file_open

/** @def MPI_File_preallocate_L
    Exchanges MPI_File_preallocate_L by mpi_file_preallocate.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_preallocate_L mpi_file_preallocate

/** @def MPI_File_read_L
    Exchanges MPI_File_read_L by mpi_file_read.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_L mpi_file_read

/** @def MPI_File_read_all_L
    Exchanges MPI_File_read_all_L by mpi_file_read_all.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_all_L mpi_file_read_all

/** @def MPI_File_read_all_begin_L
    Exchanges MPI_File_read_all_begin_L by mpi_file_read_all_begin.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_all_begin_L mpi_file_read_all_begin

/** @def MPI_File_read_all_end_L
    Exchanges MPI_File_read_all_end_L by mpi_file_read_all_end.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_all_end_L mpi_file_read_all_end

/** @def MPI_File_read_at_L
    Exchanges MPI_File_read_at_L by mpi_file_read_at.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_at_L mpi_file_read_at

/** @def MPI_File_read_at_all_L
    Exchanges MPI_File_read_at_all_L by mpi_file_read_at_all.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_at_all_L mpi_file_read_at_all

/** @def MPI_File_read_at_all_begin_L
    Exchanges MPI_File_read_at_all_begin_L by mpi_file_read_at_all_begin.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_at_all_begin_L mpi_file_read_at_all_begin

/** @def MPI_File_read_at_all_end_L
    Exchanges MPI_File_read_at_all_end_L by mpi_file_read_at_all_end.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_at_all_end_L mpi_file_read_at_all_end

/** @def MPI_File_read_ordered_L
    Exchanges MPI_File_read_ordered_L by mpi_file_read_ordered.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_ordered_L mpi_file_read_ordered

/** @def MPI_File_read_ordered_begin_L
    Exchanges MPI_File_read_ordered_begin_L by mpi_file_read_ordered_begin.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_ordered_begin_L mpi_file_read_ordered_begin

/** @def MPI_File_read_ordered_end_L
    Exchanges MPI_File_read_ordered_end_L by mpi_file_read_ordered_end.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_ordered_end_L mpi_file_read_ordered_end

/** @def MPI_File_read_shared_L
    Exchanges MPI_File_read_shared_L by mpi_file_read_shared.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_read_shared_L mpi_file_read_shared

/** @def MPI_File_seek_L
    Exchanges MPI_File_seek_L by mpi_file_seek.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_seek_L mpi_file_seek

/** @def MPI_File_seek_shared_L
    Exchanges MPI_File_seek_shared_L by mpi_file_seek_shared.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_seek_shared_L mpi_file_seek_shared

/** @def MPI_File_set_atomicity_L
    Exchanges MPI_File_set_atomicity_L by mpi_file_set_atomicity.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_set_atomicity_L mpi_file_set_atomicity

/** @def MPI_File_set_info_L
    Exchanges MPI_File_set_info_L by mpi_file_set_info.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_set_info_L mpi_file_set_info

/** @def MPI_File_set_size_L
    Exchanges MPI_File_set_size_L by mpi_file_set_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_set_size_L mpi_file_set_size

/** @def MPI_File_set_view_L
    Exchanges MPI_File_set_view_L by mpi_file_set_view.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_set_view_L mpi_file_set_view

/** @def MPI_File_sync_L
    Exchanges MPI_File_sync_L by mpi_file_sync.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_sync_L mpi_file_sync

/** @def MPI_File_write_L
    Exchanges MPI_File_write_L by mpi_file_write.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_L mpi_file_write

/** @def MPI_File_write_all_L
    Exchanges MPI_File_write_all_L by mpi_file_write_all.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_all_L mpi_file_write_all

/** @def MPI_File_write_all_begin_L
    Exchanges MPI_File_write_all_begin_L by mpi_file_write_all_begin.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_all_begin_L mpi_file_write_all_begin

/** @def MPI_File_write_all_end_L
    Exchanges MPI_File_write_all_end_L by mpi_file_write_all_end.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_all_end_L mpi_file_write_all_end

/** @def MPI_File_write_at_L
    Exchanges MPI_File_write_at_L by mpi_file_write_at.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_at_L mpi_file_write_at

/** @def MPI_File_write_at_all_L
    Exchanges MPI_File_write_at_all_L by mpi_file_write_at_all.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_at_all_L mpi_file_write_at_all

/** @def MPI_File_write_at_all_begin_L
    Exchanges MPI_File_write_at_all_begin_L by mpi_file_write_at_all_begin.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_at_all_begin_L mpi_file_write_at_all_begin

/** @def MPI_File_write_at_all_end_L
    Exchanges MPI_File_write_at_all_end_L by mpi_file_write_at_all_end.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_at_all_end_L mpi_file_write_at_all_end

/** @def MPI_File_write_ordered_L
    Exchanges MPI_File_write_ordered_L by mpi_file_write_ordered.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_ordered_L mpi_file_write_ordered

/** @def MPI_File_write_ordered_begin_L
    Exchanges MPI_File_write_ordered_begin_L by mpi_file_write_ordered_begin.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_ordered_begin_L mpi_file_write_ordered_begin

/** @def MPI_File_write_ordered_end_L
    Exchanges MPI_File_write_ordered_end_L by mpi_file_write_ordered_end.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_ordered_end_L mpi_file_write_ordered_end

/** @def MPI_File_write_shared_L
    Exchanges MPI_File_write_shared_L by mpi_file_write_shared.
    It is used for the Forran wrappers of me).
 */
#define MPI_File_write_shared_L mpi_file_write_shared

/** @def MPI_Register_datarep_L
    Exchanges MPI_Register_datarep_L by mpi_register_datarep.
    It is used for the Forran wrappers of me).
 */
#define MPI_Register_datarep_L mpi_register_datarep




/* uppercase defines */
/** @def MPI_File_close_U
    Exchange MPI_File_close_U by MPI_FILE_CLOSE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_close_U MPI_FILE_CLOSE

/** @def MPI_File_delete_U
    Exchange MPI_File_delete_U by MPI_FILE_DELETE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_delete_U MPI_FILE_DELETE

/** @def MPI_File_get_amode_U
    Exchange MPI_File_get_amode_U by MPI_FILE_GET_AMODE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_amode_U MPI_FILE_GET_AMODE

/** @def MPI_File_get_atomicity_U
    Exchange MPI_File_get_atomicity_U by MPI_FILE_GET_ATOMICITY.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_atomicity_U MPI_FILE_GET_ATOMICITY

/** @def MPI_File_get_byte_offset_U
    Exchange MPI_File_get_byte_offset_U by MPI_FILE_GET_BYTE_OFFSET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_byte_offset_U MPI_FILE_GET_BYTE_OFFSET

/** @def MPI_File_get_group_U
    Exchange MPI_File_get_group_U by MPI_FILE_GET_GROUP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_group_U MPI_FILE_GET_GROUP

/** @def MPI_File_get_info_U
    Exchange MPI_File_get_info_U by MPI_FILE_GET_INFO.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_info_U MPI_FILE_GET_INFO

/** @def MPI_File_get_position_U
    Exchange MPI_File_get_position_U by MPI_FILE_GET_POSITION.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_position_U MPI_FILE_GET_POSITION

/** @def MPI_File_get_position_shared_U
    Exchange MPI_File_get_position_shared_U by MPI_FILE_GET_POSITION_SHARED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_position_shared_U MPI_FILE_GET_POSITION_SHARED

/** @def MPI_File_get_size_U
    Exchange MPI_File_get_size_U by MPI_FILE_GET_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_size_U MPI_FILE_GET_SIZE

/** @def MPI_File_get_type_extent_U
    Exchange MPI_File_get_type_extent_U by MPI_FILE_GET_TYPE_EXTENT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_type_extent_U MPI_FILE_GET_TYPE_EXTENT

/** @def MPI_File_get_view_U
    Exchange MPI_File_get_view_U by MPI_FILE_GET_VIEW.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_get_view_U MPI_FILE_GET_VIEW

/** @def MPI_File_iread_U
    Exchange MPI_File_iread_U by MPI_FILE_IREAD.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_iread_U MPI_FILE_IREAD

/** @def MPI_File_iread_at_U
    Exchange MPI_File_iread_at_U by MPI_FILE_IREAD_AT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_iread_at_U MPI_FILE_IREAD_AT

/** @def MPI_File_iread_shared_U
    Exchange MPI_File_iread_shared_U by MPI_FILE_IREAD_SHARED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_iread_shared_U MPI_FILE_IREAD_SHARED

/** @def MPI_File_iwrite_U
    Exchange MPI_File_iwrite_U by MPI_FILE_IWRITE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_iwrite_U MPI_FILE_IWRITE

/** @def MPI_File_iwrite_at_U
    Exchange MPI_File_iwrite_at_U by MPI_FILE_IWRITE_AT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_iwrite_at_U MPI_FILE_IWRITE_AT

/** @def MPI_File_iwrite_shared_U
    Exchange MPI_File_iwrite_shared_U by MPI_FILE_IWRITE_SHARED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_iwrite_shared_U MPI_FILE_IWRITE_SHARED

/** @def MPI_File_open_U
    Exchange MPI_File_open_U by MPI_FILE_OPEN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_open_U MPI_FILE_OPEN

/** @def MPI_File_preallocate_U
    Exchange MPI_File_preallocate_U by MPI_FILE_PREALLOCATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_preallocate_U MPI_FILE_PREALLOCATE

/** @def MPI_File_read_U
    Exchange MPI_File_read_U by MPI_FILE_READ.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_U MPI_FILE_READ

/** @def MPI_File_read_all_U
    Exchange MPI_File_read_all_U by MPI_FILE_READ_ALL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_all_U MPI_FILE_READ_ALL

/** @def MPI_File_read_all_begin_U
    Exchange MPI_File_read_all_begin_U by MPI_FILE_READ_ALL_BEGIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_all_begin_U MPI_FILE_READ_ALL_BEGIN

/** @def MPI_File_read_all_end_U
    Exchange MPI_File_read_all_end_U by MPI_FILE_READ_ALL_END.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_all_end_U MPI_FILE_READ_ALL_END

/** @def MPI_File_read_at_U
    Exchange MPI_File_read_at_U by MPI_FILE_READ_AT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_at_U MPI_FILE_READ_AT

/** @def MPI_File_read_at_all_U
    Exchange MPI_File_read_at_all_U by MPI_FILE_READ_AT_ALL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_at_all_U MPI_FILE_READ_AT_ALL

/** @def MPI_File_read_at_all_begin_U
    Exchange MPI_File_read_at_all_begin_U by MPI_FILE_READ_AT_ALL_BEGIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_at_all_begin_U MPI_FILE_READ_AT_ALL_BEGIN

/** @def MPI_File_read_at_all_end_U
    Exchange MPI_File_read_at_all_end_U by MPI_FILE_READ_AT_ALL_END.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_at_all_end_U MPI_FILE_READ_AT_ALL_END

/** @def MPI_File_read_ordered_U
    Exchange MPI_File_read_ordered_U by MPI_FILE_READ_ORDERED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_ordered_U MPI_FILE_READ_ORDERED

/** @def MPI_File_read_ordered_begin_U
    Exchange MPI_File_read_ordered_begin_U by MPI_FILE_READ_ORDERED_BEGIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_ordered_begin_U MPI_FILE_READ_ORDERED_BEGIN

/** @def MPI_File_read_ordered_end_U
    Exchange MPI_File_read_ordered_end_U by MPI_FILE_READ_ORDERED_END.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_ordered_end_U MPI_FILE_READ_ORDERED_END

/** @def MPI_File_read_shared_U
    Exchange MPI_File_read_shared_U by MPI_FILE_READ_SHARED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_read_shared_U MPI_FILE_READ_SHARED

/** @def MPI_File_seek_U
    Exchange MPI_File_seek_U by MPI_FILE_SEEK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_seek_U MPI_FILE_SEEK

/** @def MPI_File_seek_shared_U
    Exchange MPI_File_seek_shared_U by MPI_FILE_SEEK_SHARED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_seek_shared_U MPI_FILE_SEEK_SHARED

/** @def MPI_File_set_atomicity_U
    Exchange MPI_File_set_atomicity_U by MPI_FILE_SET_ATOMICITY.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_set_atomicity_U MPI_FILE_SET_ATOMICITY

/** @def MPI_File_set_info_U
    Exchange MPI_File_set_info_U by MPI_FILE_SET_INFO.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_set_info_U MPI_FILE_SET_INFO

/** @def MPI_File_set_size_U
    Exchange MPI_File_set_size_U by MPI_FILE_SET_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_set_size_U MPI_FILE_SET_SIZE

/** @def MPI_File_set_view_U
    Exchange MPI_File_set_view_U by MPI_FILE_SET_VIEW.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_set_view_U MPI_FILE_SET_VIEW

/** @def MPI_File_sync_U
    Exchange MPI_File_sync_U by MPI_FILE_SYNC.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_sync_U MPI_FILE_SYNC

/** @def MPI_File_write_U
    Exchange MPI_File_write_U by MPI_FILE_WRITE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_U MPI_FILE_WRITE

/** @def MPI_File_write_all_U
    Exchange MPI_File_write_all_U by MPI_FILE_WRITE_ALL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_all_U MPI_FILE_WRITE_ALL

/** @def MPI_File_write_all_begin_U
    Exchange MPI_File_write_all_begin_U by MPI_FILE_WRITE_ALL_BEGIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_all_begin_U MPI_FILE_WRITE_ALL_BEGIN

/** @def MPI_File_write_all_end_U
    Exchange MPI_File_write_all_end_U by MPI_FILE_WRITE_ALL_END.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_all_end_U MPI_FILE_WRITE_ALL_END

/** @def MPI_File_write_at_U
    Exchange MPI_File_write_at_U by MPI_FILE_WRITE_AT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_at_U MPI_FILE_WRITE_AT

/** @def MPI_File_write_at_all_U
    Exchange MPI_File_write_at_all_U by MPI_FILE_WRITE_AT_ALL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_at_all_U MPI_FILE_WRITE_AT_ALL

/** @def MPI_File_write_at_all_begin_U
    Exchange MPI_File_write_at_all_begin_U by MPI_FILE_WRITE_AT_ALL_BEGIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_at_all_begin_U MPI_FILE_WRITE_AT_ALL_BEGIN

/** @def MPI_File_write_at_all_end_U
    Exchange MPI_File_write_at_all_end_U by MPI_FILE_WRITE_AT_ALL_END.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_at_all_end_U MPI_FILE_WRITE_AT_ALL_END

/** @def MPI_File_write_ordered_U
    Exchange MPI_File_write_ordered_U by MPI_FILE_WRITE_ORDERED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_ordered_U MPI_FILE_WRITE_ORDERED

/** @def MPI_File_write_ordered_begin_U
    Exchange MPI_File_write_ordered_begin_U by MPI_FILE_WRITE_ORDERED_BEGIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_ordered_begin_U MPI_FILE_WRITE_ORDERED_BEGIN

/** @def MPI_File_write_ordered_end_U
    Exchange MPI_File_write_ordered_end_U by MPI_FILE_WRITE_ORDERED_END.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_ordered_end_U MPI_FILE_WRITE_ORDERED_END

/** @def MPI_File_write_shared_U
    Exchange MPI_File_write_shared_U by MPI_FILE_WRITE_SHARED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_File_write_shared_U MPI_FILE_WRITE_SHARED

/** @def MPI_Register_datarep_U
    Exchange MPI_Register_datarep_U by MPI_REGISTER_DATAREP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Register_datarep_U MPI_REGISTER_DATAREP





#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

/**
 * @name Fortran wrappers for administrative functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_CLOSE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_close )
/**
 * Measurement wrapper for MPI_File_close
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_close )( MPI_File* fh,
                        int*      ierr )
{
    *ierr = MPI_File_close( fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_DELETE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_delete )
/**
 * Measurement wrapper for MPI_File_delete
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_delete )( char*     filename,
                         MPI_Info* info,
                         int*      ierr,
                         int       filename_len )
{
    char* c_filename = NULL;
    c_filename = ( char* )malloc( ( filename_len + 1 ) * sizeof( char ) );
    if ( !c_filename )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_filename, filename, filename_len );
    c_filename[ filename_len ] = '\0';


    *ierr = MPI_File_delete( c_filename, *info );

    free( c_filename );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_AMODE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_amode )
/**
 * Measurement wrapper for MPI_File_get_amode
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_amode )( MPI_File* fh,
                            int*      amode,
                            int*      ierr )
{
    *ierr = MPI_File_get_amode( *fh, amode );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ATOMICITY ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_atomicity )
/**
 * Measurement wrapper for MPI_File_get_atomicity
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_atomicity )( MPI_File* fh,
                                int*      flag,
                                int*      ierr )
{
    *ierr = MPI_File_get_atomicity( *fh, flag );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_BYTE_OFFSET ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_byte_offset )
/**
 * Measurement wrapper for MPI_File_get_byte_offset
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_byte_offset )( MPI_File*   fh,
                                  MPI_Offset* offset,
                                  MPI_Offset* disp,
                                  int*        ierr )
{
    *ierr = MPI_File_get_byte_offset( *fh, *offset, disp );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_GROUP ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_group )
/**
 * Measurement wrapper for MPI_File_get_group
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_group )( MPI_File*  fh,
                            MPI_Group* group,
                            int*       ierr )
{
    *ierr = MPI_File_get_group( *fh, group );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_INFO ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_info )
/**
 * Measurement wrapper for MPI_File_get_info
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_info )( MPI_File* fh,
                           MPI_Info* info_used,
                           int*      ierr )
{
    *ierr = MPI_File_get_info( *fh, info_used );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_position )
/**
 * Measurement wrapper for MPI_File_get_position
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_position )( MPI_File*   fh,
                               MPI_Offset* offset,
                               int*        ierr )
{
    *ierr = MPI_File_get_position( *fh, offset );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_position_shared )
/**
 * Measurement wrapper for MPI_File_get_position_shared
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_position_shared )( MPI_File*   fh,
                                      MPI_Offset* offset,
                                      int*        ierr )
{
    *ierr = MPI_File_get_position_shared( *fh, offset );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_SIZE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_size )
/**
 * Measurement wrapper for MPI_File_get_size
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_size )( MPI_File*   fh,
                           MPI_Offset* size,
                           int*        ierr )
{
    *ierr = MPI_File_get_size( *fh, size );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_TYPE_EXTENT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_type_extent )
/**
 * Measurement wrapper for MPI_File_get_type_extent
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_type_extent )( MPI_File*     fh,
                                  MPI_Datatype* datatype,
                                  MPI_Aint*     extent,
                                  int*          ierr )
{
    *ierr = MPI_File_get_type_extent( *fh, *datatype, extent );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_VIEW ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_view )
/**
 * Measurement wrapper for MPI_File_get_view
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_get_view )( MPI_File*     fh,
                           MPI_Offset*   disp,
                           MPI_Datatype* etype,
                           MPI_Datatype* filetype,
                           char*         datarep,
                           int*          ierr,
                           int           datarep_len )
{
    char* c_datarep     = NULL;
    int   c_datarep_len = 0;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }


    *ierr = MPI_File_get_view( *fh, disp, etype, filetype, c_datarep );


    c_datarep_len = strlen( c_datarep );
    strncpy( datarep, c_datarep, c_datarep_len );
    memset( datarep + c_datarep_len, ' ', datarep_len - c_datarep_len );
    free( c_datarep );
}
#endif
#if HAVE( DECL_PMPI_FILE_OPEN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_open )
/**
 * Measurement wrapper for MPI_File_open
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_open )( MPI_Comm* comm,
                       char*     filename,
                       int*      amode,
                       MPI_Info* info,
                       MPI_File* fh,
                       int*      ierr,
                       int       filename_len )
{
    char* c_filename = NULL;
    c_filename = ( char* )malloc( ( filename_len + 1 ) * sizeof( char ) );
    if ( !c_filename )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_filename, filename, filename_len );
    c_filename[ filename_len ] = '\0';


    *ierr = MPI_File_open( *comm, c_filename, *amode, *info, fh );

    free( c_filename );
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_seek )
/**
 * Measurement wrapper for MPI_File_seek
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_seek )( MPI_File*   fh,
                       MPI_Offset* offset,
                       int*        whence,
                       int*        ierr )
{
    *ierr = MPI_File_seek( *fh, *offset, *whence );
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_seek_shared )
/**
 * Measurement wrapper for MPI_File_seek_shared
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_seek_shared )( MPI_File*   fh,
                              MPI_Offset* offset,
                              int*        whence,
                              int*        ierr )
{
    *ierr = MPI_File_seek_shared( *fh, *offset, *whence );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ATOMICITY ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_atomicity )
/**
 * Measurement wrapper for MPI_File_set_atomicity
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_set_atomicity )( MPI_File* fh,
                                int*      flag,
                                int*      ierr )
{
    *ierr = MPI_File_set_atomicity( *fh, *flag );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_INFO ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_info )
/**
 * Measurement wrapper for MPI_File_set_info
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_set_info )( MPI_File* fh,
                           MPI_Info* info,
                           int*      ierr )
{
    *ierr = MPI_File_set_info( *fh, *info );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_SIZE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_size )
/**
 * Measurement wrapper for MPI_File_set_size
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_set_size )( MPI_File*   fh,
                           MPI_Offset* size,
                           int*        ierr )
{
    *ierr = MPI_File_set_size( *fh, *size );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_VIEW ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_view )
/**
 * Measurement wrapper for MPI_File_set_view
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_set_view )( MPI_File*     fh,
                           MPI_Offset*   disp,
                           MPI_Datatype* etype,
                           MPI_Datatype* filetype,
                           char*         datarep,
                           MPI_Info*     info,
                           int*          ierr,
                           int           datarep_len )
{
    char* c_datarep = NULL;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';


    *ierr = MPI_File_set_view( *fh, *disp, *etype, *filetype, c_datarep, *info );

    free( c_datarep );
}
#endif
#if HAVE( DECL_PMPI_FILE_SYNC ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_sync )
/**
 * Measurement wrapper for MPI_File_sync
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_sync )( MPI_File* fh,
                       int*      ierr )
{
    *ierr = MPI_File_sync( *fh );
}
#endif
#if HAVE( DECL_PMPI_REGISTER_DATAREP ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Register_datarep )
/**
 * Measurement wrapper for MPI_Register_datarep
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_Register_datarep )( char*                            datarep,
                              MPI_Datarep_conversion_function* read_conversion_fn,
                              MPI_Datarep_conversion_function* write_conversion_fn,
                              MPI_Datarep_extent_function*     dtype_file_extent_fn,
                              void*                            extra_state,
                              int*                             ierr,
                              int                              datarep_len )
{
    char* c_datarep = NULL;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';


    *ierr = MPI_Register_datarep( c_datarep, read_conversion_fn, write_conversion_fn, dtype_file_extent_fn, extra_state );

    free( c_datarep );
}
#endif

/**
 * @}
 * @name Fortran wrappers for access functions
 * @{
 */

/* MPI Implementation which use ROMIO can have no general support for MPI_Requests
   in asynchronous file IO operations. This leads to a different signature and
   thus to compiler errors. To avoid compilation errors, the asynchronous file
   IO functions are not wrapped if ROMIO is used and they support no general
   MPI_Requests.
 */
#if !defined( ROMIO_VERSION ) || defined( MPIO_USES_MPI_REQUEST )

#if HAVE( DECL_PMPI_FILE_IREAD ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread )
/**
 * Measurement wrapper for MPI_File_iread
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_iread )( MPI_File*     fh,
                        void*         buf,
                        int*          count,
                        MPI_Datatype* datatype,
                        MPI_Request*  request,
                        int*          ierr )
{
    *ierr = MPI_File_iread( *fh, buf, *count, *datatype, request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread_at )
/**
 * Measurement wrapper for MPI_File_iread_at
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_iread_at )( MPI_File*     fh,
                           MPI_Offset*   offset,
                           void*         buf,
                           int*          count,
                           MPI_Datatype* datatype,
                           MPI_Request*  request,
                           int*          ierr )
{
    *ierr = MPI_File_iread_at( *fh, *offset, buf, *count, *datatype, request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread_shared )
/**
 * Measurement wrapper for MPI_File_iread_shared
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_iread_shared )( MPI_File*     fh,
                               void*         buf,
                               int*          count,
                               MPI_Datatype* datatype,
                               MPI_Request*  request,
                               int*          ierr )
{
    *ierr = MPI_File_iread_shared( *fh, buf, *count, *datatype, request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite )
/**
 * Measurement wrapper for MPI_File_iwrite
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_iwrite )( MPI_File*     fh,
                         void*         buf,
                         int*          count,
                         MPI_Datatype* datatype,
                         MPI_Request*  request,
                         int*          ierr )
{
    *ierr = MPI_File_iwrite( *fh, buf, *count, *datatype, request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite_at )
/**
 * Measurement wrapper for MPI_File_iwrite_at
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_iwrite_at )( MPI_File*     fh,
                            MPI_Offset*   offset,
                            void*         buf,
                            int*          count,
                            MPI_Datatype* datatype,
                            MPI_Request*  request,
                            int*          ierr )
{
    *ierr = MPI_File_iwrite_at( *fh, *offset, buf, *count, *datatype, request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite_shared )
/**
 * Measurement wrapper for MPI_File_iwrite_shared
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_iwrite_shared )( MPI_File*     fh,
                                void*         buf,
                                int*          count,
                                MPI_Datatype* datatype,
                                MPI_Request*  request,
                                int*          ierr )
{
    *ierr = MPI_File_iwrite_shared( *fh, buf, *count, *datatype, request );
}
#endif

#endif /* !defined(ROMIO_VERSION) || defined(MPIO_USES_MPI_REQUEST) */

#if HAVE( DECL_PMPI_FILE_READ ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read )
/**
 * Measurement wrapper for MPI_File_read
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read )( MPI_File*     fh,
                       void*         buf,
                       int*          count,
                       MPI_Datatype* datatype,
                       MPI_Status*   status,
                       int*          ierr )
{
    *ierr = MPI_File_read( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all )
/**
 * Measurement wrapper for MPI_File_read_all
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_all )( MPI_File*     fh,
                           void*         buf,
                           int*          count,
                           MPI_Datatype* datatype,
                           MPI_Status*   status,
                           int*          ierr )
{
    *ierr = MPI_File_read_all( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all_begin )
/**
 * Measurement wrapper for MPI_File_read_all_begin
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_all_begin )( MPI_File*     fh,
                                 void*         buf,
                                 int*          count,
                                 MPI_Datatype* datatype,
                                 int*          ierr )
{
    *ierr = MPI_File_read_all_begin( *fh, buf, *count, *datatype );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all_end )
/**
 * Measurement wrapper for MPI_File_read_all_end
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_all_end )( MPI_File*   fh,
                               void*       buf,
                               MPI_Status* status,
                               int*        ierr )
{
    *ierr = MPI_File_read_all_end( *fh, buf, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at )
/**
 * Measurement wrapper for MPI_File_read_at
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_at )( MPI_File*     fh,
                          MPI_Offset*   offset,
                          void*         buf,
                          int*          count,
                          MPI_Datatype* datatype,
                          MPI_Status*   status,
                          int*          ierr )
{
    *ierr = MPI_File_read_at( *fh, *offset, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all )
/**
 * Measurement wrapper for MPI_File_read_at_all
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_at_all )( MPI_File*     fh,
                              MPI_Offset*   offset,
                              void*         buf,
                              int*          count,
                              MPI_Datatype* datatype,
                              MPI_Status*   status,
                              int*          ierr )
{
    *ierr = MPI_File_read_at_all( *fh, *offset, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all_begin )
/**
 * Measurement wrapper for MPI_File_read_at_all_begin
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_at_all_begin )( MPI_File*     fh,
                                    MPI_Offset*   offset,
                                    void*         buf,
                                    int*          count,
                                    MPI_Datatype* datatype,
                                    int*          ierr )
{
    *ierr = MPI_File_read_at_all_begin( *fh, *offset, buf, *count, *datatype );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all_end )
/**
 * Measurement wrapper for MPI_File_read_at_all_end
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_at_all_end )( MPI_File*   fh,
                                  void*       buf,
                                  MPI_Status* status,
                                  int*        ierr )
{
    *ierr = MPI_File_read_at_all_end( *fh, buf, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered )
/**
 * Measurement wrapper for MPI_File_read_ordered
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_ordered )( MPI_File*     fh,
                               void*         buf,
                               int*          count,
                               MPI_Datatype* datatype,
                               MPI_Status*   status,
                               int*          ierr )
{
    *ierr = MPI_File_read_ordered( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered_begin )
/**
 * Measurement wrapper for MPI_File_read_ordered_begin
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_ordered_begin )( MPI_File*     fh,
                                     void*         buf,
                                     int*          count,
                                     MPI_Datatype* datatype,
                                     int*          ierr )
{
    *ierr = MPI_File_read_ordered_begin( *fh, buf, *count, *datatype );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered_end )
/**
 * Measurement wrapper for MPI_File_read_ordered_end
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_ordered_end )( MPI_File*   fh,
                                   void*       buf,
                                   MPI_Status* status,
                                   int*        ierr )
{
    *ierr = MPI_File_read_ordered_end( *fh, buf, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_shared )
/**
 * Measurement wrapper for MPI_File_read_shared
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_read_shared )( MPI_File*     fh,
                              void*         buf,
                              int*          count,
                              MPI_Datatype* datatype,
                              MPI_Status*   status,
                              int*          ierr )
{
    *ierr = MPI_File_read_shared( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write )
/**
 * Measurement wrapper for MPI_File_write
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write )( MPI_File*     fh,
                        void*         buf,
                        int*          count,
                        MPI_Datatype* datatype,
                        MPI_Status*   status,
                        int*          ierr )
{
    *ierr = MPI_File_write( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all )
/**
 * Measurement wrapper for MPI_File_write_all
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_all )( MPI_File*     fh,
                            void*         buf,
                            int*          count,
                            MPI_Datatype* datatype,
                            MPI_Status*   status,
                            int*          ierr )
{
    *ierr = MPI_File_write_all( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all_begin )
/**
 * Measurement wrapper for MPI_File_write_all_begin
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_all_begin )( MPI_File*     fh,
                                  void*         buf,
                                  int*          count,
                                  MPI_Datatype* datatype,
                                  int*          ierr )
{
    *ierr = MPI_File_write_all_begin( *fh, buf, *count, *datatype );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all_end )
/**
 * Measurement wrapper for MPI_File_write_all_end
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_all_end )( MPI_File*   fh,
                                void*       buf,
                                MPI_Status* status,
                                int*        ierr )
{
    *ierr = MPI_File_write_all_end( *fh, buf, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at )
/**
 * Measurement wrapper for MPI_File_write_at
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_at )( MPI_File*     fh,
                           MPI_Offset*   offset,
                           void*         buf,
                           int*          count,
                           MPI_Datatype* datatype,
                           MPI_Status*   status,
                           int*          ierr )
{
    *ierr = MPI_File_write_at( *fh, *offset, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all )
/**
 * Measurement wrapper for MPI_File_write_at_all
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_at_all )( MPI_File*     fh,
                               MPI_Offset*   offset,
                               void*         buf,
                               int*          count,
                               MPI_Datatype* datatype,
                               MPI_Status*   status,
                               int*          ierr )
{
    *ierr = MPI_File_write_at_all( *fh, *offset, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all_begin )
/**
 * Measurement wrapper for MPI_File_write_at_all_begin
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_at_all_begin )( MPI_File*     fh,
                                     MPI_Offset*   offset,
                                     void*         buf,
                                     int*          count,
                                     MPI_Datatype* datatype,
                                     int*          ierr )
{
    *ierr = MPI_File_write_at_all_begin( *fh, *offset, buf, *count, *datatype );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all_end )
/**
 * Measurement wrapper for MPI_File_write_at_all_end
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_at_all_end )( MPI_File*   fh,
                                   void*       buf,
                                   MPI_Status* status,
                                   int*        ierr )
{
    *ierr = MPI_File_write_at_all_end( *fh, buf, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered )
/**
 * Measurement wrapper for MPI_File_write_ordered
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_ordered )( MPI_File*     fh,
                                void*         buf,
                                int*          count,
                                MPI_Datatype* datatype,
                                MPI_Status*   status,
                                int*          ierr )
{
    *ierr = MPI_File_write_ordered( *fh, buf, *count, *datatype, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered_begin )
/**
 * Measurement wrapper for MPI_File_write_ordered_begin
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_ordered_begin )( MPI_File*     fh,
                                      void*         buf,
                                      int*          count,
                                      MPI_Datatype* datatype,
                                      int*          ierr )
{
    *ierr = MPI_File_write_ordered_begin( *fh, buf, *count, *datatype );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered_end )
/**
 * Measurement wrapper for MPI_File_write_ordered_end
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_ordered_end )( MPI_File*   fh,
                                    void*       buf,
                                    MPI_Status* status,
                                    int*        ierr )
{
    *ierr = MPI_File_write_ordered_end( *fh, buf, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_shared )
/**
 * Measurement wrapper for MPI_File_write_shared
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 */
void
FSUB( MPI_File_write_shared )( MPI_File*     fh,
                               void*         buf,
                               int*          count,
                               MPI_Datatype* datatype,
                               MPI_Status*   status,
                               int*          ierr )
{
    *ierr = MPI_File_write_shared( *fh, buf, *count, *datatype, status );
}
#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_CALL_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_call_errhandler )
/**
 * Measurement wrapper for MPI_File_call_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 */
void
FSUB( MPI_File_call_errhandler )( MPI_File* fh,
                                  int*      errorcode,
                                  int*      ierr )
{
    *ierr = MPI_File_call_errhandler( *fh, *errorcode );
}
#endif
#if HAVE( DECL_PMPI_FILE_CREATE_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_create_errhandler )
/**
 * Measurement wrapper for MPI_File_create_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 */
void
FSUB( MPI_File_create_errhandler )( MPI_File_errhandler_fn* function,
                                    MPI_Errhandler*         errhandler,
                                    int*                    ierr )
{
    *ierr = MPI_File_create_errhandler( function, errhandler );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_get_errhandler )
/**
 * Measurement wrapper for MPI_File_get_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 */
void
FSUB( MPI_File_get_errhandler )( MPI_File*       file,
                                 MPI_Errhandler* errhandler,
                                 int*            ierr )
{
    *ierr = MPI_File_get_errhandler( *file, errhandler );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_set_errhandler )
/**
 * Measurement wrapper for MPI_File_set_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 */
void
FSUB( MPI_File_set_errhandler )( MPI_File*       file,
                                 MPI_Errhandler* errhandler,
                                 int*            ierr )
{
    *ierr = MPI_File_set_errhandler( *file, *errhandler );
}
#endif

#else /* !NEED_F2C_CONV */

/**
 * @}
 * @name Fortran wrappers for administrative functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_CLOSE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_close )
/**
 * Measurement wrapper for MPI_File_close
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_close
 */
void
FSUB( MPI_File_close )( MPI_Fint* fh,
                        int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_close( &c_fh );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_DELETE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_delete )
/**
 * Measurement wrapper for MPI_File_delete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_delete
 */
void
FSUB( MPI_File_delete )( char*     filename,
                         MPI_Fint* info,
                         int*      ierr,
                         int       filename_len )
{
    char* c_filename = NULL;
    c_filename = ( char* )malloc( ( filename_len + 1 ) * sizeof( char ) );
    if ( !c_filename )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_filename, filename, filename_len );
    c_filename[ filename_len ] = '\0';

    *ierr = MPI_File_delete( c_filename, PMPI_Info_f2c( *info ) );
    free( c_filename );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_AMODE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_amode )
/**
 * Measurement wrapper for MPI_File_get_amode
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_amode
 */
void
FSUB( MPI_File_get_amode )( MPI_Fint* fh,
                            MPI_Fint* amode,
                            int*      ierr )
{
    *ierr = MPI_File_get_amode( PMPI_File_f2c( *fh ), amode );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ATOMICITY ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_atomicity )
/**
 * Measurement wrapper for MPI_File_get_atomicity
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_atomicity
 */
void
FSUB( MPI_File_get_atomicity )( MPI_Fint* fh,
                                MPI_Fint* flag,
                                int*      ierr )
{
    *ierr = MPI_File_get_atomicity( PMPI_File_f2c( *fh ), flag );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_BYTE_OFFSET ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_byte_offset )
/**
 * Measurement wrapper for MPI_File_get_byte_offset
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_byte_offset
 */
void
FSUB( MPI_File_get_byte_offset )( MPI_Fint*   fh,
                                  MPI_Offset* offset,
                                  MPI_Offset* disp,
                                  int*        ierr )
{
    *ierr = MPI_File_get_byte_offset( PMPI_File_f2c( *fh ), *offset, disp );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_GROUP ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_group )
/**
 * Measurement wrapper for MPI_File_get_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_group
 */
void
FSUB( MPI_File_get_group )( MPI_Fint* fh,
                            MPI_Fint* group,
                            int*      ierr )
{
    MPI_Group c_group;
    *ierr  = MPI_File_get_group( PMPI_File_f2c( *fh ), &c_group );
    *group = PMPI_Group_c2f( c_group );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_INFO ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_info )
/**
 * Measurement wrapper for MPI_File_get_info
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_info
 */
void
FSUB( MPI_File_get_info )( MPI_Fint* fh,
                           MPI_Fint* info_used,
                           int*      ierr )
{
    MPI_Info c_info_used;
    *ierr      = MPI_File_get_info( PMPI_File_f2c( *fh ), &c_info_used );
    *info_used = PMPI_Info_c2f( c_info_used );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_position )
/**
 * Measurement wrapper for MPI_File_get_position
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_position
 */
void
FSUB( MPI_File_get_position )( MPI_Fint*   fh,
                               MPI_Offset* offset,
                               int*        ierr )
{
    *ierr = MPI_File_get_position( PMPI_File_f2c( *fh ), offset );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_position_shared )
/**
 * Measurement wrapper for MPI_File_get_position_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_position_shared
 */
void
FSUB( MPI_File_get_position_shared )( MPI_Fint*   fh,
                                      MPI_Offset* offset,
                                      int*        ierr )
{
    *ierr = MPI_File_get_position_shared( PMPI_File_f2c( *fh ), offset );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_SIZE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_size )
/**
 * Measurement wrapper for MPI_File_get_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_size
 */
void
FSUB( MPI_File_get_size )( MPI_Fint*   fh,
                           MPI_Offset* size,
                           int*        ierr )
{
    *ierr = MPI_File_get_size( PMPI_File_f2c( *fh ), size );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_TYPE_EXTENT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_type_extent )
/**
 * Measurement wrapper for MPI_File_get_type_extent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_type_extent
 */
void
FSUB( MPI_File_get_type_extent )( MPI_Fint* fh,
                                  MPI_Fint* datatype,
                                  MPI_Aint* extent,
                                  int*      ierr )
{
    *ierr = MPI_File_get_type_extent( PMPI_File_f2c( *fh ), PMPI_Type_f2c( *datatype ), extent );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_VIEW ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_view )
/**
 * Measurement wrapper for MPI_File_get_view
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_view
 */
void
FSUB( MPI_File_get_view )( MPI_Fint*   fh,
                           MPI_Offset* disp,
                           MPI_Fint*   etype,
                           MPI_Fint*   filetype,
                           char*       datarep,
                           int*        ierr,
                           int         datarep_len )
{
    char*        c_datarep     = NULL;
    int          c_datarep_len = 0;
    MPI_Datatype c_etype;
    MPI_Datatype c_filetype;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }

    *ierr = MPI_File_get_view( PMPI_File_f2c( *fh ), disp, &c_etype, &c_filetype, c_datarep );

    c_datarep_len = strlen( c_datarep );
    strncpy( datarep, c_datarep, c_datarep_len );
    memset( datarep + c_datarep_len, ' ', datarep_len - c_datarep_len );
    free( c_datarep );
    *etype    = PMPI_Type_c2f( c_etype );
    *filetype = PMPI_Type_c2f( c_filetype );
}
#endif
#if HAVE( DECL_PMPI_FILE_OPEN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_open )
/**
 * Measurement wrapper for MPI_File_open
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_open
 */
void
FSUB( MPI_File_open )( MPI_Fint* comm,
                       char*     filename,
                       MPI_Fint* amode,
                       MPI_Fint* info,
                       MPI_Fint* fh,
                       int*      ierr,
                       int       filename_len )
{
    char* c_filename = NULL;
    c_filename = ( char* )malloc( ( filename_len + 1 ) * sizeof( char ) );
    if ( !c_filename )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_filename, filename, filename_len );
    c_filename[ filename_len ] = '\0';

    MPI_File c_fh;
    *ierr = MPI_File_open( PMPI_Comm_f2c( *comm ), c_filename, *amode, PMPI_Info_f2c( *info ), &c_fh );
    free( c_filename );
    *fh = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_seek )
/**
 * Measurement wrapper for MPI_File_seek
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_seek
 */
void
FSUB( MPI_File_seek )( MPI_Fint*   fh,
                       MPI_Offset* offset,
                       MPI_Fint*   whence,
                       int*        ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_seek( c_fh, *offset, *whence );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_seek_shared )
/**
 * Measurement wrapper for MPI_File_seek_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_seek_shared
 */
void
FSUB( MPI_File_seek_shared )( MPI_Fint*   fh,
                              MPI_Offset* offset,
                              MPI_Fint*   whence,
                              int*        ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_seek_shared( c_fh, *offset, *whence );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ATOMICITY ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_atomicity )
/**
 * Measurement wrapper for MPI_File_set_atomicity
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_atomicity
 */
void
FSUB( MPI_File_set_atomicity )( MPI_Fint* fh,
                                MPI_Fint* flag,
                                int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_set_atomicity( c_fh, *flag );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_INFO ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_info )
/**
 * Measurement wrapper for MPI_File_set_info
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_info
 */
void
FSUB( MPI_File_set_info )( MPI_Fint* fh,
                           MPI_Fint* info,
                           int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_set_info( c_fh, PMPI_Info_f2c( *info ) );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_SIZE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_size )
/**
 * Measurement wrapper for MPI_File_set_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_size
 */
void
FSUB( MPI_File_set_size )( MPI_Fint*   fh,
                           MPI_Offset* size,
                           int*        ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_set_size( c_fh, *size );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_VIEW ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_view )
/**
 * Measurement wrapper for MPI_File_set_view
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_view
 */
void
FSUB( MPI_File_set_view )( MPI_Fint*   fh,
                           MPI_Offset* disp,
                           MPI_Fint*   etype,
                           MPI_Fint*   filetype,
                           char*       datarep,
                           MPI_Fint*   info,
                           int*        ierr,
                           int         datarep_len )
{
    char*    c_datarep = NULL;
    MPI_File c_fh      = PMPI_File_f2c( *fh );
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';

    *ierr = MPI_File_set_view( c_fh, *disp, PMPI_Type_f2c( *etype ), PMPI_Type_f2c( *filetype ), c_datarep, PMPI_Info_f2c( *info ) );
    free( c_datarep );
    *fh = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_SYNC ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_sync )
/**
 * Measurement wrapper for MPI_File_sync
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_sync
 */
void
FSUB( MPI_File_sync )( MPI_Fint* fh,
                       int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_sync( c_fh );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_REGISTER_DATAREP ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Register_datarep )
/**
 * Measurement wrapper for MPI_Register_datarep
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_Register_datarep
 */
void
FSUB( MPI_Register_datarep )( char* datarep,
                              void* read_conversion_fn,
                              void* write_conversion_fn,
                              void* dtype_file_extent_fn,
                              void* extra_state,
                              int*  ierr,
                              int   datarep_len )
{
    char* c_datarep = NULL;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';

    *ierr = MPI_Register_datarep( c_datarep, ( MPI_Datarep_conversion_function* )read_conversion_fn, ( MPI_Datarep_conversion_function* )write_conversion_fn, ( MPI_Datarep_extent_function* )dtype_file_extent_fn, extra_state );
    free( c_datarep );
}
#endif


/**
 * @}
 * @name Fortran wrappers for access functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_IREAD ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread )
/**
 * Measurement wrapper for MPI_File_iread
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iread
 */
void
FSUB( MPI_File_iread )( MPI_Fint* fh,
                        void*     buf,
                        MPI_Fint* count,
                        MPI_Fint* datatype,
                        MPI_Fint* request,
                        int*      ierr )
{
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;
    *ierr    = MPI_File_iread( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );
    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread_at )
/**
 * Measurement wrapper for MPI_File_iread_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iread_at
 */
void
FSUB( MPI_File_iread_at )( MPI_Fint*   fh,
                           MPI_Offset* offset,
                           void*       buf,
                           MPI_Fint*   count,
                           MPI_Fint*   datatype,
                           MPI_Fint*   request,
                           int*        ierr )
{
    MPI_Request c_request;
    *ierr    = MPI_File_iread_at( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread_shared )
/**
 * Measurement wrapper for MPI_File_iread_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iread_shared
 */
void
FSUB( MPI_File_iread_shared )( MPI_Fint* fh,
                               void*     buf,
                               MPI_Fint* count,
                               MPI_Fint* datatype,
                               MPI_Fint* request,
                               int*      ierr )
{
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;
    *ierr    = MPI_File_iread_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );
    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite )
/**
 * Measurement wrapper for MPI_File_iwrite
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite
 */
void
FSUB( MPI_File_iwrite )( MPI_Fint* fh,
                         void*     buf,
                         MPI_Fint* count,
                         MPI_Fint* datatype,
                         MPI_Fint* request,
                         int*      ierr )
{
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;
    *ierr    = MPI_File_iwrite( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );
    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite_at )
/**
 * Measurement wrapper for MPI_File_iwrite_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite_at
 */
void
FSUB( MPI_File_iwrite_at )( MPI_Fint*   fh,
                            MPI_Offset* offset,
                            void*       buf,
                            MPI_Fint*   count,
                            MPI_Fint*   datatype,
                            MPI_Fint*   request,
                            int*        ierr )
{
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;
    *ierr    = MPI_File_iwrite_at( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );
    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite_shared )
/**
 * Measurement wrapper for MPI_File_iwrite_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite_shared
 */
void
FSUB( MPI_File_iwrite_shared )( MPI_Fint* fh,
                                void*     buf,
                                MPI_Fint* count,
                                MPI_Fint* datatype,
                                MPI_Fint* request,
                                int*      ierr )
{
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;
    *ierr    = MPI_File_iwrite_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );
    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read )
/**
 * Measurement wrapper for MPI_File_read
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read
 */
void
FSUB( MPI_File_read )( MPI_Fint* fh,
                       void*     buf,
                       MPI_Fint* count,
                       MPI_Fint* datatype,
                       MPI_Fint* status,
                       int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_read( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all )
/**
 * Measurement wrapper for MPI_File_read_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_all
 */
void
FSUB( MPI_File_read_all )( MPI_Fint* fh,
                           void*     buf,
                           MPI_Fint* count,
                           MPI_Fint* datatype,
                           MPI_Fint* status,
                           int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_read_all( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all_begin )
/**
 * Measurement wrapper for MPI_File_read_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_all_begin
 */
void
FSUB( MPI_File_read_all_begin )( MPI_Fint* fh,
                                 void*     buf,
                                 MPI_Fint* count,
                                 MPI_Fint* datatype,
                                 int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_read_all_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all_end )
/**
 * Measurement wrapper for MPI_File_read_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_all_end
 */
void
FSUB( MPI_File_read_all_end )( MPI_Fint* fh,
                               void*     buf,
                               MPI_Fint* status,
                               int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_read_all_end( c_fh, buf, &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at )
/**
 * Measurement wrapper for MPI_File_read_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at
 */
void
FSUB( MPI_File_read_at )( MPI_Fint*   fh,
                          MPI_Offset* offset,
                          void*       buf,
                          MPI_Fint*   count,
                          MPI_Fint*   datatype,
                          MPI_Fint*   status,
                          int*        ierr )
{
    MPI_Status c_status;
    *ierr = MPI_File_read_at( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all )
/**
 * Measurement wrapper for MPI_File_read_at_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at_all
 */
void
FSUB( MPI_File_read_at_all )( MPI_Fint*   fh,
                              MPI_Offset* offset,
                              void*       buf,
                              MPI_Fint*   count,
                              MPI_Fint*   datatype,
                              MPI_Fint*   status,
                              int*        ierr )
{
    MPI_Status c_status;
    *ierr = MPI_File_read_at_all( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all_begin )
/**
 * Measurement wrapper for MPI_File_read_at_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at_all_begin
 */
void
FSUB( MPI_File_read_at_all_begin )( MPI_Fint*   fh,
                                    MPI_Offset* offset,
                                    void*       buf,
                                    MPI_Fint*   count,
                                    MPI_Fint*   datatype,
                                    int*        ierr )
{
    *ierr = MPI_File_read_at_all_begin( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ) );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all_end )
/**
 * Measurement wrapper for MPI_File_read_at_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at_all_end
 */
void
FSUB( MPI_File_read_at_all_end )( MPI_Fint* fh,
                                  void*     buf,
                                  MPI_Fint* status,
                                  int*      ierr )
{
    MPI_Status c_status;
    *ierr = MPI_File_read_at_all_end( PMPI_File_f2c( *fh ), buf, &c_status );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered )
/**
 * Measurement wrapper for MPI_File_read_ordered
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_ordered
 */
void
FSUB( MPI_File_read_ordered )( MPI_Fint* fh,
                               void*     buf,
                               MPI_Fint* count,
                               MPI_Fint* datatype,
                               MPI_Fint* status,
                               int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_read_ordered( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered_begin )
/**
 * Measurement wrapper for MPI_File_read_ordered_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_ordered_begin
 */
void
FSUB( MPI_File_read_ordered_begin )( MPI_Fint* fh,
                                     void*     buf,
                                     MPI_Fint* count,
                                     MPI_Fint* datatype,
                                     int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_read_ordered_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered_end )
/**
 * Measurement wrapper for MPI_File_read_ordered_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_ordered_end
 */
void
FSUB( MPI_File_read_ordered_end )( MPI_Fint* fh,
                                   void*     buf,
                                   MPI_Fint* status,
                                   int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_read_ordered_end( c_fh, buf, &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_shared )
/**
 * Measurement wrapper for MPI_File_read_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_shared
 */
void
FSUB( MPI_File_read_shared )( MPI_Fint* fh,
                              void*     buf,
                              MPI_Fint* count,
                              MPI_Fint* datatype,
                              MPI_Fint* status,
                              int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_read_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write )
/**
 * Measurement wrapper for MPI_File_write
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write
 */
void
FSUB( MPI_File_write )( MPI_Fint* fh,
                        void*     buf,
                        MPI_Fint* count,
                        MPI_Fint* datatype,
                        MPI_Fint* status,
                        int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all )
/**
 * Measurement wrapper for MPI_File_write_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_all
 */
void
FSUB( MPI_File_write_all )( MPI_Fint* fh,
                            void*     buf,
                            MPI_Fint* count,
                            MPI_Fint* datatype,
                            MPI_Fint* status,
                            int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_all( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all_begin )
/**
 * Measurement wrapper for MPI_File_write_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_all_begin
 */
void
FSUB( MPI_File_write_all_begin )( MPI_Fint* fh,
                                  void*     buf,
                                  MPI_Fint* count,
                                  MPI_Fint* datatype,
                                  int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_write_all_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all_end )
/**
 * Measurement wrapper for MPI_File_write_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_all_end
 */
void
FSUB( MPI_File_write_all_end )( MPI_Fint* fh,
                                void*     buf,
                                MPI_Fint* status,
                                int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_all_end( c_fh, buf, &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at )
/**
 * Measurement wrapper for MPI_File_write_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at
 */
void
FSUB( MPI_File_write_at )( MPI_Fint*   fh,
                           MPI_Offset* offset,
                           void*       buf,
                           MPI_Fint*   count,
                           MPI_Fint*   datatype,
                           MPI_Fint*   status,
                           int*        ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_at( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all )
/**
 * Measurement wrapper for MPI_File_write_at_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at_all
 */
void
FSUB( MPI_File_write_at_all )( MPI_Fint*   fh,
                               MPI_Offset* offset,
                               void*       buf,
                               MPI_Fint*   count,
                               MPI_Fint*   datatype,
                               MPI_Fint*   status,
                               int*        ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_at_all( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all_begin )
/**
 * Measurement wrapper for MPI_File_write_at_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at_all_begin
 */
void
FSUB( MPI_File_write_at_all_begin )( MPI_Fint*   fh,
                                     MPI_Offset* offset,
                                     void*       buf,
                                     MPI_Fint*   count,
                                     MPI_Fint*   datatype,
                                     int*        ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_write_at_all_begin( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ) );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all_end )
/**
 * Measurement wrapper for MPI_File_write_at_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at_all_end
 */
void
FSUB( MPI_File_write_at_all_end )( MPI_Fint* fh,
                                   void*     buf,
                                   MPI_Fint* status,
                                   int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_at_all_end( c_fh, buf, &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered )
/**
 * Measurement wrapper for MPI_File_write_ordered
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_ordered
 */
void
FSUB( MPI_File_write_ordered )( MPI_Fint* fh,
                                void*     buf,
                                MPI_Fint* count,
                                MPI_Fint* datatype,
                                MPI_Fint* status,
                                int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_ordered( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered_begin )
/**
 * Measurement wrapper for MPI_File_write_ordered_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_ordered_begin
 */
void
FSUB( MPI_File_write_ordered_begin )( MPI_Fint* fh,
                                      void*     buf,
                                      MPI_Fint* count,
                                      MPI_Fint* datatype,
                                      int*      ierr )
{
    MPI_File c_fh = PMPI_File_f2c( *fh );
    *ierr = MPI_File_write_ordered_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );
    *fh   = PMPI_File_c2f( c_fh );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered_end )
/**
 * Measurement wrapper for MPI_File_write_ordered_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_ordered_end
 */
void
FSUB( MPI_File_write_ordered_end )( MPI_Fint* fh,
                                    void*     buf,
                                    MPI_Fint* status,
                                    int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_ordered_end( c_fh, buf, &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_shared )
/**
 * Measurement wrapper for MPI_File_write_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_shared
 */
void
FSUB( MPI_File_write_shared )( MPI_Fint* fh,
                               void*     buf,
                               MPI_Fint* count,
                               MPI_Fint* datatype,
                               MPI_Fint* status,
                               int*      ierr )
{
    MPI_File   c_fh = PMPI_File_f2c( *fh );
    MPI_Status c_status;
    *ierr = MPI_File_write_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_status );
    *fh   = PMPI_File_c2f( c_fh );
    MPI_Status_c2f( &c_status, status );
}
#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_CALL_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_call_errhandler )
/**
 * Measurement wrapper for MPI_File_call_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * For the order of events see @ref MPI_File_call_errhandler
 */
void
FSUB( MPI_File_call_errhandler )( MPI_Fint* fh,
                                  MPI_Fint* errorcode,
                                  int*      ierr )
{
    *ierr = MPI_File_call_errhandler( PMPI_File_f2c( *fh ), *errorcode );
}
#endif
#if HAVE( DECL_PMPI_FILE_CREATE_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_create_errhandler )
/**
 * Measurement wrapper for MPI_File_create_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * For the order of events see @ref MPI_File_create_errhandler
 */
void
FSUB( MPI_File_create_errhandler )( void* function,
                                    void* errhandler,
                                    int*  ierr )
{
    *ierr = MPI_File_create_errhandler( ( MPI_File_errhandler_fn* )function, ( MPI_Errhandler* )errhandler );
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_get_errhandler )
/**
 * Measurement wrapper for MPI_File_get_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * For the order of events see @ref MPI_File_get_errhandler
 */
void
FSUB( MPI_File_get_errhandler )( MPI_Fint* file,
                                 void*     errhandler,
                                 int*      ierr )
{
    *ierr = MPI_File_get_errhandler( PMPI_File_f2c( *file ), ( MPI_Errhandler* )errhandler );
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_set_errhandler )
/**
 * Measurement wrapper for MPI_File_set_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io_err
 * For the order of events see @ref MPI_File_set_errhandler
 */
void
FSUB( MPI_File_set_errhandler )( MPI_Fint* file,
                                 void*     errhandler,
                                 int*      ierr )
{
    MPI_File c_file = PMPI_File_f2c( *file );
    *ierr = MPI_File_set_errhandler( c_file, *( ( MPI_Errhandler* )errhandler ) );
    *file = PMPI_File_c2f( c_file );
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */


#endif /* !NEED_F2C_CONV */

/**
 * @}
 */
