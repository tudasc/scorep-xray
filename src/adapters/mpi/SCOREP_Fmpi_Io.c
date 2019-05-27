/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016, 2018-2019,
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
 * @brief Fortran interface wrappers for parallel I/O
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/* lowercase defines */
/** @def MPI_File_close_L
    Exchanges MPI_File_close_L by mpi_file_close.
    It is used for the Fortran wrappers of MPI_File_close.
 */
#define MPI_File_close_L mpi_file_close

/** @def MPI_File_delete_L
    Exchanges MPI_File_delete_L by mpi_file_delete.
    It is used for the Fortran wrappers of MPI_File_delete.
 */
#define MPI_File_delete_L mpi_file_delete

/** @def MPI_File_get_amode_L
    Exchanges MPI_File_get_amode_L by mpi_file_get_amode.
    It is used for the Fortran wrappers of MPI_File_get_amode.
 */
#define MPI_File_get_amode_L mpi_file_get_amode

/** @def MPI_File_get_atomicity_L
    Exchanges MPI_File_get_atomicity_L by mpi_file_get_atomicity.
    It is used for the Fortran wrappers of MPI_File_get_atomicity.
 */
#define MPI_File_get_atomicity_L mpi_file_get_atomicity

/** @def MPI_File_get_byte_offset_L
    Exchanges MPI_File_get_byte_offset_L by mpi_file_get_byte_offset.
    It is used for the Fortran wrappers of MPI_File_get_byte_offset.
 */
#define MPI_File_get_byte_offset_L mpi_file_get_byte_offset

/** @def MPI_File_get_group_L
    Exchanges MPI_File_get_group_L by mpi_file_get_group.
    It is used for the Fortran wrappers of MPI_File_get_group.
 */
#define MPI_File_get_group_L mpi_file_get_group

/** @def MPI_File_get_info_L
    Exchanges MPI_File_get_info_L by mpi_file_get_info.
    It is used for the Fortran wrappers of MPI_File_get_info.
 */
#define MPI_File_get_info_L mpi_file_get_info

/** @def MPI_File_get_position_L
    Exchanges MPI_File_get_position_L by mpi_file_get_position.
    It is used for the Fortran wrappers of MPI_File_get_position.
 */
#define MPI_File_get_position_L mpi_file_get_position

/** @def MPI_File_get_position_shared_L
    Exchanges MPI_File_get_position_shared_L by mpi_file_get_position_shared.
    It is used for the Fortran wrappers of MPI_File_get_position_shared.
 */
#define MPI_File_get_position_shared_L mpi_file_get_position_shared

/** @def MPI_File_get_size_L
    Exchanges MPI_File_get_size_L by mpi_file_get_size.
    It is used for the Fortran wrappers of MPI_File_get_size.
 */
#define MPI_File_get_size_L mpi_file_get_size

/** @def MPI_File_get_type_extent_L
    Exchanges MPI_File_get_type_extent_L by mpi_file_get_type_extent.
    It is used for the Fortran wrappers of MPI_File_get_type_extent.
 */
#define MPI_File_get_type_extent_L mpi_file_get_type_extent

/** @def MPI_File_get_view_L
    Exchanges MPI_File_get_view_L by mpi_file_get_view.
    It is used for the Fortran wrappers of MPI_File_get_view.
 */
#define MPI_File_get_view_L mpi_file_get_view

/** @def MPI_File_iread_L
    Exchanges MPI_File_iread_L by mpi_file_iread.
    It is used for the Fortran wrappers of MPI_File_iread.
 */
#define MPI_File_iread_L mpi_file_iread

/** @def MPI_File_iread_all_L
    Exchanges MPI_File_iread_all_L by mpi_file_iread_all.
    It is used for the Fortran wrappers of MPI_File_iread_all.
 */
#define MPI_File_iread_all_L mpi_file_iread_all

/** @def MPI_File_iread_at_L
    Exchanges MPI_File_iread_at_L by mpi_file_iread_at.
    It is used for the Fortran wrappers of MPI_File_iread_at.
 */
#define MPI_File_iread_at_L mpi_file_iread_at

/** @def MPI_File_iread_at_all_L
    Exchanges MPI_File_iread_at_all_L by mpi_file_iread_at_all.
    It is used for the Fortran wrappers of MPI_File_iread_at_all.
 */
#define MPI_File_iread_at_all_L mpi_file_iread_at_all

/** @def MPI_File_iread_shared_L
    Exchanges MPI_File_iread_shared_L by mpi_file_iread_shared.
    It is used for the Fortran wrappers of MPI_File_iread_shared.
 */
#define MPI_File_iread_shared_L mpi_file_iread_shared

/** @def MPI_File_iwrite_L
    Exchanges MPI_File_iwrite_L by mpi_file_iwrite.
    It is used for the Fortran wrappers of MPI_File_iwrite.
 */
#define MPI_File_iwrite_L mpi_file_iwrite

/** @def MPI_File_iwrite_all_L
    Exchanges MPI_File_iwrite_all_L by mpi_file_iwrite_all.
    It is used for the Fortran wrappers of MPI_File_iwrite_all.
 */
#define MPI_File_iwrite_all_L mpi_file_iwrite_all

/** @def MPI_File_iwrite_at_L
    Exchanges MPI_File_iwrite_at_L by mpi_file_iwrite_at.
    It is used for the Fortran wrappers of MPI_File_iwrite_at.
 */
#define MPI_File_iwrite_at_L mpi_file_iwrite_at

/** @def MPI_File_iwrite_at_all_L
    Exchanges MPI_File_iwrite_at_all_L by mpi_file_iwrite_at_all.
    It is used for the Fortran wrappers of MPI_File_iwrite_at_all.
 */
#define MPI_File_iwrite_at_all_L mpi_file_iwrite_at_all

/** @def MPI_File_iwrite_shared_L
    Exchanges MPI_File_iwrite_shared_L by mpi_file_iwrite_shared.
    It is used for the Fortran wrappers of MPI_File_iwrite_shared.
 */
#define MPI_File_iwrite_shared_L mpi_file_iwrite_shared

/** @def MPI_File_open_L
    Exchanges MPI_File_open_L by mpi_file_open.
    It is used for the Fortran wrappers of MPI_File_open.
 */
#define MPI_File_open_L mpi_file_open

/** @def MPI_File_preallocate_L
    Exchanges MPI_File_preallocate_L by mpi_file_preallocate.
    It is used for the Fortran wrappers of MPI_File_preallocate.
 */
#define MPI_File_preallocate_L mpi_file_preallocate

/** @def MPI_File_read_L
    Exchanges MPI_File_read_L by mpi_file_read.
    It is used for the Fortran wrappers of MPI_File_read.
 */
#define MPI_File_read_L mpi_file_read

/** @def MPI_File_read_all_L
    Exchanges MPI_File_read_all_L by mpi_file_read_all.
    It is used for the Fortran wrappers of MPI_File_read_all.
 */
#define MPI_File_read_all_L mpi_file_read_all

/** @def MPI_File_read_all_begin_L
    Exchanges MPI_File_read_all_begin_L by mpi_file_read_all_begin.
    It is used for the Fortran wrappers of MPI_File_read_all_begin.
 */
#define MPI_File_read_all_begin_L mpi_file_read_all_begin

/** @def MPI_File_read_all_end_L
    Exchanges MPI_File_read_all_end_L by mpi_file_read_all_end.
    It is used for the Fortran wrappers of MPI_File_read_all_end.
 */
#define MPI_File_read_all_end_L mpi_file_read_all_end

/** @def MPI_File_read_at_L
    Exchanges MPI_File_read_at_L by mpi_file_read_at.
    It is used for the Fortran wrappers of MPI_File_read_at.
 */
#define MPI_File_read_at_L mpi_file_read_at

/** @def MPI_File_read_at_all_L
    Exchanges MPI_File_read_at_all_L by mpi_file_read_at_all.
    It is used for the Fortran wrappers of MPI_File_read_at_all.
 */
#define MPI_File_read_at_all_L mpi_file_read_at_all

/** @def MPI_File_read_at_all_begin_L
    Exchanges MPI_File_read_at_all_begin_L by mpi_file_read_at_all_begin.
    It is used for the Fortran wrappers of MPI_File_read_at_all_begin.
 */
#define MPI_File_read_at_all_begin_L mpi_file_read_at_all_begin

/** @def MPI_File_read_at_all_end_L
    Exchanges MPI_File_read_at_all_end_L by mpi_file_read_at_all_end.
    It is used for the Fortran wrappers of MPI_File_read_at_all_end.
 */
#define MPI_File_read_at_all_end_L mpi_file_read_at_all_end

/** @def MPI_File_read_ordered_L
    Exchanges MPI_File_read_ordered_L by mpi_file_read_ordered.
    It is used for the Fortran wrappers of MPI_File_read_ordered.
 */
#define MPI_File_read_ordered_L mpi_file_read_ordered

/** @def MPI_File_read_ordered_begin_L
    Exchanges MPI_File_read_ordered_begin_L by mpi_file_read_ordered_begin.
    It is used for the Fortran wrappers of MPI_File_read_ordered_begin.
 */
#define MPI_File_read_ordered_begin_L mpi_file_read_ordered_begin

/** @def MPI_File_read_ordered_end_L
    Exchanges MPI_File_read_ordered_end_L by mpi_file_read_ordered_end.
    It is used for the Fortran wrappers of MPI_File_read_ordered_end.
 */
#define MPI_File_read_ordered_end_L mpi_file_read_ordered_end

/** @def MPI_File_read_shared_L
    Exchanges MPI_File_read_shared_L by mpi_file_read_shared.
    It is used for the Fortran wrappers of MPI_File_read_shared.
 */
#define MPI_File_read_shared_L mpi_file_read_shared

/** @def MPI_File_seek_L
    Exchanges MPI_File_seek_L by mpi_file_seek.
    It is used for the Fortran wrappers of MPI_File_seek.
 */
#define MPI_File_seek_L mpi_file_seek

/** @def MPI_File_seek_shared_L
    Exchanges MPI_File_seek_shared_L by mpi_file_seek_shared.
    It is used for the Fortran wrappers of MPI_File_seek_shared.
 */
#define MPI_File_seek_shared_L mpi_file_seek_shared

/** @def MPI_File_set_atomicity_L
    Exchanges MPI_File_set_atomicity_L by mpi_file_set_atomicity.
    It is used for the Fortran wrappers of MPI_File_set_atomicity.
 */
#define MPI_File_set_atomicity_L mpi_file_set_atomicity

/** @def MPI_File_set_info_L
    Exchanges MPI_File_set_info_L by mpi_file_set_info.
    It is used for the Fortran wrappers of MPI_File_set_info.
 */
#define MPI_File_set_info_L mpi_file_set_info

/** @def MPI_File_set_size_L
    Exchanges MPI_File_set_size_L by mpi_file_set_size.
    It is used for the Fortran wrappers of MPI_File_set_size.
 */
#define MPI_File_set_size_L mpi_file_set_size

/** @def MPI_File_set_view_L
    Exchanges MPI_File_set_view_L by mpi_file_set_view.
    It is used for the Fortran wrappers of MPI_File_set_view.
 */
#define MPI_File_set_view_L mpi_file_set_view

/** @def MPI_File_sync_L
    Exchanges MPI_File_sync_L by mpi_file_sync.
    It is used for the Fortran wrappers of MPI_File_sync.
 */
#define MPI_File_sync_L mpi_file_sync

/** @def MPI_File_write_L
    Exchanges MPI_File_write_L by mpi_file_write.
    It is used for the Fortran wrappers of MPI_File_write.
 */
#define MPI_File_write_L mpi_file_write

/** @def MPI_File_write_all_L
    Exchanges MPI_File_write_all_L by mpi_file_write_all.
    It is used for the Fortran wrappers of MPI_File_write_all.
 */
#define MPI_File_write_all_L mpi_file_write_all

/** @def MPI_File_write_all_begin_L
    Exchanges MPI_File_write_all_begin_L by mpi_file_write_all_begin.
    It is used for the Fortran wrappers of MPI_File_write_all_begin.
 */
#define MPI_File_write_all_begin_L mpi_file_write_all_begin

/** @def MPI_File_write_all_end_L
    Exchanges MPI_File_write_all_end_L by mpi_file_write_all_end.
    It is used for the Fortran wrappers of MPI_File_write_all_end.
 */
#define MPI_File_write_all_end_L mpi_file_write_all_end

/** @def MPI_File_write_at_L
    Exchanges MPI_File_write_at_L by mpi_file_write_at.
    It is used for the Fortran wrappers of MPI_File_write_at.
 */
#define MPI_File_write_at_L mpi_file_write_at

/** @def MPI_File_write_at_all_L
    Exchanges MPI_File_write_at_all_L by mpi_file_write_at_all.
    It is used for the Fortran wrappers of MPI_File_write_at_all.
 */
#define MPI_File_write_at_all_L mpi_file_write_at_all

/** @def MPI_File_write_at_all_begin_L
    Exchanges MPI_File_write_at_all_begin_L by mpi_file_write_at_all_begin.
    It is used for the Fortran wrappers of MPI_File_write_at_all_begin.
 */
#define MPI_File_write_at_all_begin_L mpi_file_write_at_all_begin

/** @def MPI_File_write_at_all_end_L
    Exchanges MPI_File_write_at_all_end_L by mpi_file_write_at_all_end.
    It is used for the Fortran wrappers of MPI_File_write_at_all_end.
 */
#define MPI_File_write_at_all_end_L mpi_file_write_at_all_end

/** @def MPI_File_write_ordered_L
    Exchanges MPI_File_write_ordered_L by mpi_file_write_ordered.
    It is used for the Fortran wrappers of MPI_File_write_ordered.
 */
#define MPI_File_write_ordered_L mpi_file_write_ordered

/** @def MPI_File_write_ordered_begin_L
    Exchanges MPI_File_write_ordered_begin_L by mpi_file_write_ordered_begin.
    It is used for the Fortran wrappers of MPI_File_write_ordered_begin.
 */
#define MPI_File_write_ordered_begin_L mpi_file_write_ordered_begin

/** @def MPI_File_write_ordered_end_L
    Exchanges MPI_File_write_ordered_end_L by mpi_file_write_ordered_end.
    It is used for the Fortran wrappers of MPI_File_write_ordered_end.
 */
#define MPI_File_write_ordered_end_L mpi_file_write_ordered_end

/** @def MPI_File_write_shared_L
    Exchanges MPI_File_write_shared_L by mpi_file_write_shared.
    It is used for the Fortran wrappers of MPI_File_write_shared.
 */
#define MPI_File_write_shared_L mpi_file_write_shared

/** @def MPI_Register_datarep_L
    Exchanges MPI_Register_datarep_L by mpi_register_datarep.
    It is used for the Fortran wrappers of MPI_Register_datarep.
 */
#define MPI_Register_datarep_L mpi_register_datarep



/* uppercase defines */
/** @def MPI_File_close_U
    Exchange MPI_File_close_U by MPI_FILE_CLOSE.
    It is used for the Fortran wrappers of MPI_File_close.
 */
#define MPI_File_close_U MPI_FILE_CLOSE

/** @def MPI_File_delete_U
    Exchange MPI_File_delete_U by MPI_FILE_DELETE.
    It is used for the Fortran wrappers of MPI_File_delete.
 */
#define MPI_File_delete_U MPI_FILE_DELETE

/** @def MPI_File_get_amode_U
    Exchange MPI_File_get_amode_U by MPI_FILE_GET_AMODE.
    It is used for the Fortran wrappers of MPI_File_get_amode.
 */
#define MPI_File_get_amode_U MPI_FILE_GET_AMODE

/** @def MPI_File_get_atomicity_U
    Exchange MPI_File_get_atomicity_U by MPI_FILE_GET_ATOMICITY.
    It is used for the Fortran wrappers of MPI_File_get_atomicity.
 */
#define MPI_File_get_atomicity_U MPI_FILE_GET_ATOMICITY

/** @def MPI_File_get_byte_offset_U
    Exchange MPI_File_get_byte_offset_U by MPI_FILE_GET_BYTE_OFFSET.
    It is used for the Fortran wrappers of MPI_File_get_byte_offset.
 */
#define MPI_File_get_byte_offset_U MPI_FILE_GET_BYTE_OFFSET

/** @def MPI_File_get_group_U
    Exchange MPI_File_get_group_U by MPI_FILE_GET_GROUP.
    It is used for the Fortran wrappers of MPI_File_get_group.
 */
#define MPI_File_get_group_U MPI_FILE_GET_GROUP

/** @def MPI_File_get_info_U
    Exchange MPI_File_get_info_U by MPI_FILE_GET_INFO.
    It is used for the Fortran wrappers of MPI_File_get_info.
 */
#define MPI_File_get_info_U MPI_FILE_GET_INFO

/** @def MPI_File_get_position_U
    Exchange MPI_File_get_position_U by MPI_FILE_GET_POSITION.
    It is used for the Fortran wrappers of MPI_File_get_position.
 */
#define MPI_File_get_position_U MPI_FILE_GET_POSITION

/** @def MPI_File_get_position_shared_U
    Exchange MPI_File_get_position_shared_U by MPI_FILE_GET_POSITION_SHARED.
    It is used for the Fortran wrappers of MPI_File_get_position_shared.
 */
#define MPI_File_get_position_shared_U MPI_FILE_GET_POSITION_SHARED

/** @def MPI_File_get_size_U
    Exchange MPI_File_get_size_U by MPI_FILE_GET_SIZE.
    It is used for the Fortran wrappers of MPI_File_get_size.
 */
#define MPI_File_get_size_U MPI_FILE_GET_SIZE

/** @def MPI_File_get_type_extent_U
    Exchange MPI_File_get_type_extent_U by MPI_FILE_GET_TYPE_EXTENT.
    It is used for the Fortran wrappers of MPI_File_get_type_extent.
 */
#define MPI_File_get_type_extent_U MPI_FILE_GET_TYPE_EXTENT

/** @def MPI_File_get_view_U
    Exchange MPI_File_get_view_U by MPI_FILE_GET_VIEW.
    It is used for the Fortran wrappers of MPI_File_get_view.
 */
#define MPI_File_get_view_U MPI_FILE_GET_VIEW

/** @def MPI_File_iread_U
    Exchange MPI_File_iread_U by MPI_FILE_IREAD.
    It is used for the Fortran wrappers of MPI_File_iread.
 */
#define MPI_File_iread_U MPI_FILE_IREAD

/** @def MPI_File_iread_all_U
    Exchange MPI_File_iread_all_U by MPI_FILE_IREAD_ALL.
    It is used for the Fortran wrappers of MPI_File_iread_all.
 */
#define MPI_File_iread_all_U MPI_FILE_IREAD_ALL

/** @def MPI_File_iread_at_U
    Exchange MPI_File_iread_at_U by MPI_FILE_IREAD_AT.
    It is used for the Fortran wrappers of MPI_File_iread_at.
 */
#define MPI_File_iread_at_U MPI_FILE_IREAD_AT

/** @def MPI_File_iread_at_all_U
    Exchange MPI_File_iread_at_all_U by MPI_FILE_IREAD_AT_ALL.
    It is used for the Fortran wrappers of MPI_File_iread_at_all.
 */
#define MPI_File_iread_at_all_U MPI_FILE_IREAD_AT_ALL

/** @def MPI_File_iread_shared_U
    Exchange MPI_File_iread_shared_U by MPI_FILE_IREAD_SHARED.
    It is used for the Fortran wrappers of MPI_File_iread_shared.
 */
#define MPI_File_iread_shared_U MPI_FILE_IREAD_SHARED

/** @def MPI_File_iwrite_U
    Exchange MPI_File_iwrite_U by MPI_FILE_IWRITE.
    It is used for the Fortran wrappers of MPI_File_iwrite.
 */
#define MPI_File_iwrite_U MPI_FILE_IWRITE

/** @def MPI_File_iwrite_all_U
    Exchange MPI_File_iwrite_all_U by MPI_FILE_IWRITE_ALL.
    It is used for the Fortran wrappers of MPI_File_iwrite_all.
 */
#define MPI_File_iwrite_all_U MPI_FILE_IWRITE_ALL

/** @def MPI_File_iwrite_at_U
    Exchange MPI_File_iwrite_at_U by MPI_FILE_IWRITE_AT.
    It is used for the Fortran wrappers of MPI_File_iwrite_at.
 */
#define MPI_File_iwrite_at_U MPI_FILE_IWRITE_AT

/** @def MPI_File_iwrite_at_all_U
    Exchange MPI_File_iwrite_at_all_U by MPI_FILE_IWRITE_AT_ALL.
    It is used for the Fortran wrappers of MPI_File_iwrite_at_all.
 */
#define MPI_File_iwrite_at_all_U MPI_FILE_IWRITE_AT_ALL

/** @def MPI_File_iwrite_shared_U
    Exchange MPI_File_iwrite_shared_U by MPI_FILE_IWRITE_SHARED.
    It is used for the Fortran wrappers of MPI_File_iwrite_shared.
 */
#define MPI_File_iwrite_shared_U MPI_FILE_IWRITE_SHARED

/** @def MPI_File_open_U
    Exchange MPI_File_open_U by MPI_FILE_OPEN.
    It is used for the Fortran wrappers of MPI_File_open.
 */
#define MPI_File_open_U MPI_FILE_OPEN

/** @def MPI_File_preallocate_U
    Exchange MPI_File_preallocate_U by MPI_FILE_PREALLOCATE.
    It is used for the Fortran wrappers of MPI_File_preallocate.
 */
#define MPI_File_preallocate_U MPI_FILE_PREALLOCATE

/** @def MPI_File_read_U
    Exchange MPI_File_read_U by MPI_FILE_READ.
    It is used for the Fortran wrappers of MPI_File_read.
 */
#define MPI_File_read_U MPI_FILE_READ

/** @def MPI_File_read_all_U
    Exchange MPI_File_read_all_U by MPI_FILE_READ_ALL.
    It is used for the Fortran wrappers of MPI_File_read_all.
 */
#define MPI_File_read_all_U MPI_FILE_READ_ALL

/** @def MPI_File_read_all_begin_U
    Exchange MPI_File_read_all_begin_U by MPI_FILE_READ_ALL_BEGIN.
    It is used for the Fortran wrappers of MPI_File_read_all_begin.
 */
#define MPI_File_read_all_begin_U MPI_FILE_READ_ALL_BEGIN

/** @def MPI_File_read_all_end_U
    Exchange MPI_File_read_all_end_U by MPI_FILE_READ_ALL_END.
    It is used for the Fortran wrappers of MPI_File_read_all_end.
 */
#define MPI_File_read_all_end_U MPI_FILE_READ_ALL_END

/** @def MPI_File_read_at_U
    Exchange MPI_File_read_at_U by MPI_FILE_READ_AT.
    It is used for the Fortran wrappers of MPI_File_read_at.
 */
#define MPI_File_read_at_U MPI_FILE_READ_AT

/** @def MPI_File_read_at_all_U
    Exchange MPI_File_read_at_all_U by MPI_FILE_READ_AT_ALL.
    It is used for the Fortran wrappers of MPI_File_read_at_all.
 */
#define MPI_File_read_at_all_U MPI_FILE_READ_AT_ALL

/** @def MPI_File_read_at_all_begin_U
    Exchange MPI_File_read_at_all_begin_U by MPI_FILE_READ_AT_ALL_BEGIN.
    It is used for the Fortran wrappers of MPI_File_read_at_all_begin.
 */
#define MPI_File_read_at_all_begin_U MPI_FILE_READ_AT_ALL_BEGIN

/** @def MPI_File_read_at_all_end_U
    Exchange MPI_File_read_at_all_end_U by MPI_FILE_READ_AT_ALL_END.
    It is used for the Fortran wrappers of MPI_File_read_at_all_end.
 */
#define MPI_File_read_at_all_end_U MPI_FILE_READ_AT_ALL_END

/** @def MPI_File_read_ordered_U
    Exchange MPI_File_read_ordered_U by MPI_FILE_READ_ORDERED.
    It is used for the Fortran wrappers of MPI_File_read_ordered.
 */
#define MPI_File_read_ordered_U MPI_FILE_READ_ORDERED

/** @def MPI_File_read_ordered_begin_U
    Exchange MPI_File_read_ordered_begin_U by MPI_FILE_READ_ORDERED_BEGIN.
    It is used for the Fortran wrappers of MPI_File_read_ordered_begin.
 */
#define MPI_File_read_ordered_begin_U MPI_FILE_READ_ORDERED_BEGIN

/** @def MPI_File_read_ordered_end_U
    Exchange MPI_File_read_ordered_end_U by MPI_FILE_READ_ORDERED_END.
    It is used for the Fortran wrappers of MPI_File_read_ordered_end.
 */
#define MPI_File_read_ordered_end_U MPI_FILE_READ_ORDERED_END

/** @def MPI_File_read_shared_U
    Exchange MPI_File_read_shared_U by MPI_FILE_READ_SHARED.
    It is used for the Fortran wrappers of MPI_File_read_shared.
 */
#define MPI_File_read_shared_U MPI_FILE_READ_SHARED

/** @def MPI_File_seek_U
    Exchange MPI_File_seek_U by MPI_FILE_SEEK.
    It is used for the Fortran wrappers of MPI_File_seek.
 */
#define MPI_File_seek_U MPI_FILE_SEEK

/** @def MPI_File_seek_shared_U
    Exchange MPI_File_seek_shared_U by MPI_FILE_SEEK_SHARED.
    It is used for the Fortran wrappers of MPI_File_seek_shared.
 */
#define MPI_File_seek_shared_U MPI_FILE_SEEK_SHARED

/** @def MPI_File_set_atomicity_U
    Exchange MPI_File_set_atomicity_U by MPI_FILE_SET_ATOMICITY.
    It is used for the Fortran wrappers of MPI_File_set_atomicity.
 */
#define MPI_File_set_atomicity_U MPI_FILE_SET_ATOMICITY

/** @def MPI_File_set_info_U
    Exchange MPI_File_set_info_U by MPI_FILE_SET_INFO.
    It is used for the Fortran wrappers of MPI_File_set_info.
 */
#define MPI_File_set_info_U MPI_FILE_SET_INFO

/** @def MPI_File_set_size_U
    Exchange MPI_File_set_size_U by MPI_FILE_SET_SIZE.
    It is used for the Fortran wrappers of MPI_File_set_size.
 */
#define MPI_File_set_size_U MPI_FILE_SET_SIZE

/** @def MPI_File_set_view_U
    Exchange MPI_File_set_view_U by MPI_FILE_SET_VIEW.
    It is used for the Fortran wrappers of MPI_File_set_view.
 */
#define MPI_File_set_view_U MPI_FILE_SET_VIEW

/** @def MPI_File_sync_U
    Exchange MPI_File_sync_U by MPI_FILE_SYNC.
    It is used for the Fortran wrappers of MPI_File_sync.
 */
#define MPI_File_sync_U MPI_FILE_SYNC

/** @def MPI_File_write_U
    Exchange MPI_File_write_U by MPI_FILE_WRITE.
    It is used for the Fortran wrappers of MPI_File_write.
 */
#define MPI_File_write_U MPI_FILE_WRITE

/** @def MPI_File_write_all_U
    Exchange MPI_File_write_all_U by MPI_FILE_WRITE_ALL.
    It is used for the Fortran wrappers of MPI_File_write_all.
 */
#define MPI_File_write_all_U MPI_FILE_WRITE_ALL

/** @def MPI_File_write_all_begin_U
    Exchange MPI_File_write_all_begin_U by MPI_FILE_WRITE_ALL_BEGIN.
    It is used for the Fortran wrappers of MPI_File_write_all_begin.
 */
#define MPI_File_write_all_begin_U MPI_FILE_WRITE_ALL_BEGIN

/** @def MPI_File_write_all_end_U
    Exchange MPI_File_write_all_end_U by MPI_FILE_WRITE_ALL_END.
    It is used for the Fortran wrappers of MPI_File_write_all_end.
 */
#define MPI_File_write_all_end_U MPI_FILE_WRITE_ALL_END

/** @def MPI_File_write_at_U
    Exchange MPI_File_write_at_U by MPI_FILE_WRITE_AT.
    It is used for the Fortran wrappers of MPI_File_write_at.
 */
#define MPI_File_write_at_U MPI_FILE_WRITE_AT

/** @def MPI_File_write_at_all_U
    Exchange MPI_File_write_at_all_U by MPI_FILE_WRITE_AT_ALL.
    It is used for the Fortran wrappers of MPI_File_write_at_all.
 */
#define MPI_File_write_at_all_U MPI_FILE_WRITE_AT_ALL

/** @def MPI_File_write_at_all_begin_U
    Exchange MPI_File_write_at_all_begin_U by MPI_FILE_WRITE_AT_ALL_BEGIN.
    It is used for the Fortran wrappers of MPI_File_write_at_all_begin.
 */
#define MPI_File_write_at_all_begin_U MPI_FILE_WRITE_AT_ALL_BEGIN

/** @def MPI_File_write_at_all_end_U
    Exchange MPI_File_write_at_all_end_U by MPI_FILE_WRITE_AT_ALL_END.
    It is used for the Fortran wrappers of MPI_File_write_at_all_end.
 */
#define MPI_File_write_at_all_end_U MPI_FILE_WRITE_AT_ALL_END

/** @def MPI_File_write_ordered_U
    Exchange MPI_File_write_ordered_U by MPI_FILE_WRITE_ORDERED.
    It is used for the Fortran wrappers of MPI_File_write_ordered.
 */
#define MPI_File_write_ordered_U MPI_FILE_WRITE_ORDERED

/** @def MPI_File_write_ordered_begin_U
    Exchange MPI_File_write_ordered_begin_U by MPI_FILE_WRITE_ORDERED_BEGIN.
    It is used for the Fortran wrappers of MPI_File_write_ordered_begin.
 */
#define MPI_File_write_ordered_begin_U MPI_FILE_WRITE_ORDERED_BEGIN

/** @def MPI_File_write_ordered_end_U
    Exchange MPI_File_write_ordered_end_U by MPI_FILE_WRITE_ORDERED_END.
    It is used for the Fortran wrappers of MPI_File_write_ordered_end.
 */
#define MPI_File_write_ordered_end_U MPI_FILE_WRITE_ORDERED_END

/** @def MPI_File_write_shared_U
    Exchange MPI_File_write_shared_U by MPI_FILE_WRITE_SHARED.
    It is used for the Fortran wrappers of MPI_File_write_shared.
 */
#define MPI_File_write_shared_U MPI_FILE_WRITE_SHARED

/** @def MPI_Register_datarep_U
    Exchange MPI_Register_datarep_U by MPI_REGISTER_DATAREP.
    It is used for the Fortran wrappers of MPI_Register_datarep.
 */
#define MPI_Register_datarep_U MPI_REGISTER_DATAREP



/**
 * @}
 * @name Fortran wrappers for administrative functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_CLOSE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_close )
/**
 * Measurement wrapper for MPI_File_close
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_close
 */
void
FSUB( MPI_File_close )( MPI_Fint* fh, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_close( &c_fh );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_DELETE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_delete )
/**
 * Measurement wrapper for MPI_File_delete
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_delete
 */
void
FSUB( MPI_File_delete )( char* filename, MPI_Fint* info, MPI_Fint* ierr, scorep_fortran_charlen_t filename_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_filename = NULL;
    c_filename = scorep_mpi_f2c_string( filename, filename_len );



    *ierr = MPI_File_delete( c_filename, PMPI_Info_f2c( *info ) );

    free( c_filename );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_AMODE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_amode )
/**
 * Measurement wrapper for MPI_File_get_amode
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_amode
 */
void
FSUB( MPI_File_get_amode )( MPI_Fint* fh, MPI_Fint* amode, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_amode( PMPI_File_f2c( *fh ), amode );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_ATOMICITY ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_atomicity )
/**
 * Measurement wrapper for MPI_File_get_atomicity
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_atomicity
 */
void
FSUB( MPI_File_get_atomicity )( MPI_Fint* fh, MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_atomicity( PMPI_File_f2c( *fh ), flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_BYTE_OFFSET ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_byte_offset )
/**
 * Measurement wrapper for MPI_File_get_byte_offset
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_byte_offset
 */
void
FSUB( MPI_File_get_byte_offset )( MPI_Fint* fh, MPI_Offset* offset, MPI_Offset* disp, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_byte_offset( PMPI_File_f2c( *fh ), *offset, disp );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_GROUP ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_group )
/**
 * Measurement wrapper for MPI_File_get_group
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_group
 */
void
FSUB( MPI_File_get_group )( MPI_Fint* fh, MPI_Fint* group, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Group c_group;


    *ierr = MPI_File_get_group( PMPI_File_f2c( *fh ), &c_group );

    *group = PMPI_Group_c2f( c_group );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_INFO ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_info )
/**
 * Measurement wrapper for MPI_File_get_info
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_info
 */
void
FSUB( MPI_File_get_info )( MPI_Fint* fh, MPI_Fint* info_used, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_info_used;


    *ierr = MPI_File_get_info( PMPI_File_f2c( *fh ), &c_info_used );

    *info_used = PMPI_Info_c2f( c_info_used );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_position )
/**
 * Measurement wrapper for MPI_File_get_position
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_position
 */
void
FSUB( MPI_File_get_position )( MPI_Fint* fh, MPI_Offset* offset, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_position( PMPI_File_f2c( *fh ), offset );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_position_shared )
/**
 * Measurement wrapper for MPI_File_get_position_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_position_shared
 */
void
FSUB( MPI_File_get_position_shared )( MPI_Fint* fh, MPI_Offset* offset, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_position_shared( PMPI_File_f2c( *fh ), offset );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_SIZE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_size )
/**
 * Measurement wrapper for MPI_File_get_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_size
 */
void
FSUB( MPI_File_get_size )( MPI_Fint* fh, MPI_Offset* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_size( PMPI_File_f2c( *fh ), size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_TYPE_EXTENT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_type_extent )
/**
 * Measurement wrapper for MPI_File_get_type_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_type_extent
 */
void
FSUB( MPI_File_get_type_extent )( MPI_Fint* fh, MPI_Fint* datatype, MPI_Aint* extent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_File_get_type_extent( PMPI_File_f2c( *fh ), PMPI_Type_f2c( *datatype ), extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_GET_VIEW ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_view )
/**
 * Measurement wrapper for MPI_File_get_view
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_get_view
 */
void
FSUB( MPI_File_get_view )( MPI_Fint* fh, MPI_Offset* disp, MPI_Fint* etype, MPI_Fint* filetype, char* datarep, MPI_Fint* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_etype;
    MPI_Datatype c_filetype;
    char*        c_datarep     = NULL;
    size_t       c_datarep_len = 0;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_File_get_view( PMPI_File_f2c( *fh ), disp, &c_etype, &c_filetype, c_datarep );

    *etype    = PMPI_Type_c2f( c_etype );
    *filetype = PMPI_Type_c2f( c_filetype );

    c_datarep_len = strlen( c_datarep );
    strncpy( datarep, c_datarep, c_datarep_len );
    memset( datarep + c_datarep_len, ' ', datarep_len - c_datarep_len );
    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_OPEN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_open )
/**
 * Measurement wrapper for MPI_File_open
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_open
 */
void
FSUB( MPI_File_open )( MPI_Fint* comm, char* filename, MPI_Fint* amode, MPI_Fint* info, MPI_Fint* fh, MPI_Fint* ierr, scorep_fortran_charlen_t filename_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*    c_filename = NULL;
    MPI_File c_fh;
    c_filename = scorep_mpi_f2c_string( filename, filename_len );



    *ierr = MPI_File_open( PMPI_Comm_f2c( *comm ), c_filename, *amode, PMPI_Info_f2c( *info ), &c_fh );

    free( c_filename );
    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_PREALLOCATE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_preallocate )
/**
 * Measurement wrapper for MPI_File_preallocate
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_preallocate
 */
void
FSUB( MPI_File_preallocate )( MPI_Fint* fh, MPI_Offset* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_preallocate( c_fh, *size );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_seek )
/**
 * Measurement wrapper for MPI_File_seek
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_seek
 */
void
FSUB( MPI_File_seek )( MPI_Fint* fh, MPI_Offset* offset, MPI_Fint* whence, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_seek( c_fh, *offset, *whence );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SEEK_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_seek_shared )
/**
 * Measurement wrapper for MPI_File_seek_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_seek_shared
 */
void
FSUB( MPI_File_seek_shared )( MPI_Fint* fh, MPI_Offset* offset, MPI_Fint* whence, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_seek_shared( c_fh, *offset, *whence );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_ATOMICITY ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_atomicity )
/**
 * Measurement wrapper for MPI_File_set_atomicity
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_atomicity
 */
void
FSUB( MPI_File_set_atomicity )( MPI_Fint* fh, MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_set_atomicity( c_fh, *flag );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_INFO ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_info )
/**
 * Measurement wrapper for MPI_File_set_info
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_info
 */
void
FSUB( MPI_File_set_info )( MPI_Fint* fh, MPI_Fint* info, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_set_info( c_fh, PMPI_Info_f2c( *info ) );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_SIZE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_size )
/**
 * Measurement wrapper for MPI_File_set_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_size
 */
void
FSUB( MPI_File_set_size )( MPI_Fint* fh, MPI_Offset* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_set_size( c_fh, *size );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SET_VIEW ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_view )
/**
 * Measurement wrapper for MPI_File_set_view
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_set_view
 */
void
FSUB( MPI_File_set_view )( MPI_Fint* fh, MPI_Offset* disp, MPI_Fint* etype, MPI_Fint* filetype, char* datarep, MPI_Fint* info, MPI_Fint* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh      = PMPI_File_f2c( *fh );
    char*    c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    *ierr = MPI_File_set_view( c_fh, *disp, PMPI_Type_f2c( *etype ), PMPI_Type_f2c( *filetype ), c_datarep, PMPI_Info_f2c( *info ) );

    *fh = PMPI_File_c2f( c_fh );
    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_SYNC ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_sync )
/**
 * Measurement wrapper for MPI_File_sync
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_sync
 */
void
FSUB( MPI_File_sync )( MPI_Fint* fh, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    *ierr = MPI_File_sync( c_fh );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_REGISTER_DATAREP ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Register_datarep )
/**
 * Measurement wrapper for MPI_Register_datarep
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_Register_datarep
 */
void
FSUB( MPI_Register_datarep )( char* datarep, void* read_conversion_fn, void* write_conversion_fn, void* dtype_file_extent_fn, void* extra_state, MPI_Fint* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    *ierr = MPI_Register_datarep( c_datarep, ( MPI_Datarep_conversion_function* )read_conversion_fn, ( MPI_Datarep_conversion_function* )write_conversion_fn, ( MPI_Datarep_extent_function* )dtype_file_extent_fn, extra_state );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif


/**
 * @}
 * @name Fortran wrappers for access functions
 * @{
 */

#if HAVE( DECL_PMPI_FILE_IREAD ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread )
/**
 * Measurement wrapper for MPI_File_iread
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iread
 */
void
FSUB( MPI_File_iread )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iread( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_all )
/**
 * Measurement wrapper for MPI_File_iread_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup io
 * For the order of events see @ref MPI_File_iread_all
 */
void
FSUB( MPI_File_iread_all )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iread_all( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_at )
/**
 * Measurement wrapper for MPI_File_iread_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iread_at
 */
void
FSUB( MPI_File_iread_at )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iread_at( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_at_all )
/**
 * Measurement wrapper for MPI_File_iread_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup io
 * For the order of events see @ref MPI_File_iread_at_all
 */
void
FSUB( MPI_File_iread_at_all )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iread_at_all( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_shared )
/**
 * Measurement wrapper for MPI_File_iread_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iread_shared
 */
void
FSUB( MPI_File_iread_shared )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iread_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite )
/**
 * Measurement wrapper for MPI_File_iwrite
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite
 */
void
FSUB( MPI_File_iwrite )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iwrite( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_all )
/**
 * Measurement wrapper for MPI_File_iwrite_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite_all
 */
void
FSUB( MPI_File_iwrite_all )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iwrite_all( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_at )
/**
 * Measurement wrapper for MPI_File_iwrite_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite_at
 */
void
FSUB( MPI_File_iwrite_at )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iwrite_at( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_at_all )
/**
 * Measurement wrapper for MPI_File_iwrite_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite_at_all
 */
void
FSUB( MPI_File_iwrite_at_all )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iwrite_at_all( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_shared )
/**
 * Measurement wrapper for MPI_File_iwrite_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_iwrite_shared
 */
void
FSUB( MPI_File_iwrite_shared )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_iwrite_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), &c_request );

    *fh      = PMPI_File_c2f( c_fh );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read )
/**
 * Measurement wrapper for MPI_File_read
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read
 */
void
FSUB( MPI_File_read )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all )
/**
 * Measurement wrapper for MPI_File_read_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_all
 */
void
FSUB( MPI_File_read_all )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_all( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all_begin )
/**
 * Measurement wrapper for MPI_File_read_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_all_begin
 */
void
FSUB( MPI_File_read_all_begin )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_all_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all_end )
/**
 * Measurement wrapper for MPI_File_read_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_all_end
 */
void
FSUB( MPI_File_read_all_end )( MPI_Fint* fh, void* buf, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_all_end( c_fh, buf, c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at )
/**
 * Measurement wrapper for MPI_File_read_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at
 */
void
FSUB( MPI_File_read_at )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_at( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all )
/**
 * Measurement wrapper for MPI_File_read_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at_all
 */
void
FSUB( MPI_File_read_at_all )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_at_all( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all_begin )
/**
 * Measurement wrapper for MPI_File_read_at_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at_all_begin
 */
void
FSUB( MPI_File_read_at_all_begin )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_at_all_begin( PMPI_File_f2c( *fh ), *offset, buf, *count, PMPI_Type_f2c( *datatype ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all_end )
/**
 * Measurement wrapper for MPI_File_read_at_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_at_all_end
 */
void
FSUB( MPI_File_read_at_all_end )( MPI_Fint* fh, void* buf, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_at_all_end( PMPI_File_f2c( *fh ), buf, c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered )
/**
 * Measurement wrapper for MPI_File_read_ordered
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_ordered
 */
void
FSUB( MPI_File_read_ordered )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_ordered( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered_begin )
/**
 * Measurement wrapper for MPI_File_read_ordered_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_ordered_begin
 */
void
FSUB( MPI_File_read_ordered_begin )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_ordered_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered_end )
/**
 * Measurement wrapper for MPI_File_read_ordered_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_ordered_end
 */
void
FSUB( MPI_File_read_ordered_end )( MPI_Fint* fh, void* buf, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_ordered_end( c_fh, buf, c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_READ_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_shared )
/**
 * Measurement wrapper for MPI_File_read_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_read_shared
 */
void
FSUB( MPI_File_read_shared )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_read_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write )
/**
 * Measurement wrapper for MPI_File_write
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write
 */
void
FSUB( MPI_File_write )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all )
/**
 * Measurement wrapper for MPI_File_write_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_all
 */
void
FSUB( MPI_File_write_all )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_all( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all_begin )
/**
 * Measurement wrapper for MPI_File_write_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_all_begin
 */
void
FSUB( MPI_File_write_all_begin )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_all_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all_end )
/**
 * Measurement wrapper for MPI_File_write_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_all_end
 */
void
FSUB( MPI_File_write_all_end )( MPI_Fint* fh, void* buf, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_all_end( c_fh, buf, c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at )
/**
 * Measurement wrapper for MPI_File_write_at
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at
 */
void
FSUB( MPI_File_write_at )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_at( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all )
/**
 * Measurement wrapper for MPI_File_write_at_all
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at_all
 */
void
FSUB( MPI_File_write_at_all )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_at_all( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all_begin )
/**
 * Measurement wrapper for MPI_File_write_at_all_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at_all_begin
 */
void
FSUB( MPI_File_write_at_all_begin )( MPI_Fint* fh, MPI_Offset* offset, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_at_all_begin( c_fh, *offset, buf, *count, PMPI_Type_f2c( *datatype ) );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all_end )
/**
 * Measurement wrapper for MPI_File_write_at_all_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_at_all_end
 */
void
FSUB( MPI_File_write_at_all_end )( MPI_Fint* fh, void* buf, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_at_all_end( c_fh, buf, c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered )
/**
 * Measurement wrapper for MPI_File_write_ordered
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_ordered
 */
void
FSUB( MPI_File_write_ordered )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_ordered( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered_begin )
/**
 * Measurement wrapper for MPI_File_write_ordered_begin
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_ordered_begin
 */
void
FSUB( MPI_File_write_ordered_begin )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File c_fh = PMPI_File_f2c( *fh );


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_ordered_begin( c_fh, buf, *count, PMPI_Type_f2c( *datatype ) );

    *fh = PMPI_File_c2f( c_fh );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered_end )
/**
 * Measurement wrapper for MPI_File_write_ordered_end
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_ordered_end
 */
void
FSUB( MPI_File_write_ordered_end )( MPI_Fint* fh, void* buf, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_ordered_end( c_fh, buf, c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_shared )
/**
 * Measurement wrapper for MPI_File_write_shared
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup io
 * For the order of events see @ref MPI_File_write_shared
 */
void
FSUB( MPI_File_write_shared )( MPI_Fint* fh, void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_File    c_fh = PMPI_File_f2c( *fh );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_File_write_shared( c_fh, buf, *count, PMPI_Type_f2c( *datatype ), c_status_ptr );

    *fh = PMPI_File_c2f( c_fh );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */
/* Do not wrap group io_err */

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */


/**
 * @}
 */
