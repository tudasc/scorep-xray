/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
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
 * @brief Fortran interface wrappers for collective communication
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/* uppercase defines */

/** @def MPI_Allgather_U
    Exchanges MPI_Allgather_U with MPI_ALLGATHER.
    It is used for the Fortran wrappers of MPI_Allgather.
 */
#define MPI_Allgather_U MPI_ALLGATHER

/** @def MPI_Allgather_init_U
    Exchanges MPI_Allgather_init_U with MPI_ALLGATHER_INIT.
    It is used for the Fortran wrappers of MPI_Allgather_init.
 */
#define MPI_Allgather_init_U MPI_ALLGATHER_INIT

/** @def MPI_Allgatherv_U
    Exchanges MPI_Allgatherv_U with MPI_ALLGATHERV.
    It is used for the Fortran wrappers of MPI_Allgatherv.
 */
#define MPI_Allgatherv_U MPI_ALLGATHERV

/** @def MPI_Allgatherv_init_U
    Exchanges MPI_Allgatherv_init_U with MPI_ALLGATHERV_INIT.
    It is used for the Fortran wrappers of MPI_Allgatherv_init.
 */
#define MPI_Allgatherv_init_U MPI_ALLGATHERV_INIT

/** @def MPI_Allreduce_U
    Exchanges MPI_Allreduce_U with MPI_ALLREDUCE.
    It is used for the Fortran wrappers of MPI_Allreduce.
 */
#define MPI_Allreduce_U MPI_ALLREDUCE

/** @def MPI_Allreduce_init_U
    Exchanges MPI_Allreduce_init_U with MPI_ALLREDUCE_INIT.
    It is used for the Fortran wrappers of MPI_Allreduce_init.
 */
#define MPI_Allreduce_init_U MPI_ALLREDUCE_INIT

/** @def MPI_Alltoall_U
    Exchanges MPI_Alltoall_U with MPI_ALLTOALL.
    It is used for the Fortran wrappers of MPI_Alltoall.
 */
#define MPI_Alltoall_U MPI_ALLTOALL

/** @def MPI_Alltoall_init_U
    Exchanges MPI_Alltoall_init_U with MPI_ALLTOALL_INIT.
    It is used for the Fortran wrappers of MPI_Alltoall_init.
 */
#define MPI_Alltoall_init_U MPI_ALLTOALL_INIT

/** @def MPI_Alltoallv_U
    Exchanges MPI_Alltoallv_U with MPI_ALLTOALLV.
    It is used for the Fortran wrappers of MPI_Alltoallv.
 */
#define MPI_Alltoallv_U MPI_ALLTOALLV

/** @def MPI_Alltoallv_init_U
    Exchanges MPI_Alltoallv_init_U with MPI_ALLTOALLV_INIT.
    It is used for the Fortran wrappers of MPI_Alltoallv_init.
 */
#define MPI_Alltoallv_init_U MPI_ALLTOALLV_INIT

/** @def MPI_Alltoallw_U
    Exchanges MPI_Alltoallw_U with MPI_ALLTOALLW.
    It is used for the Fortran wrappers of MPI_Alltoallw.
 */
#define MPI_Alltoallw_U MPI_ALLTOALLW

/** @def MPI_Alltoallw_init_U
    Exchanges MPI_Alltoallw_init_U with MPI_ALLTOALLW_INIT.
    It is used for the Fortran wrappers of MPI_Alltoallw_init.
 */
#define MPI_Alltoallw_init_U MPI_ALLTOALLW_INIT

/** @def MPI_Barrier_U
    Exchanges MPI_Barrier_U with MPI_BARRIER.
    It is used for the Fortran wrappers of MPI_Barrier.
 */
#define MPI_Barrier_U MPI_BARRIER

/** @def MPI_Barrier_init_U
    Exchanges MPI_Barrier_init_U with MPI_BARRIER_INIT.
    It is used for the Fortran wrappers of MPI_Barrier_init.
 */
#define MPI_Barrier_init_U MPI_BARRIER_INIT

/** @def MPI_Bcast_U
    Exchanges MPI_Bcast_U with MPI_BCAST.
    It is used for the Fortran wrappers of MPI_Bcast.
 */
#define MPI_Bcast_U MPI_BCAST

/** @def MPI_Bcast_init_U
    Exchanges MPI_Bcast_init_U with MPI_BCAST_INIT.
    It is used for the Fortran wrappers of MPI_Bcast_init.
 */
#define MPI_Bcast_init_U MPI_BCAST_INIT

/** @def MPI_Exscan_U
    Exchanges MPI_Exscan_U with MPI_EXSCAN.
    It is used for the Fortran wrappers of MPI_Exscan.
 */
#define MPI_Exscan_U MPI_EXSCAN

/** @def MPI_Exscan_init_U
    Exchanges MPI_Exscan_init_U with MPI_EXSCAN_INIT.
    It is used for the Fortran wrappers of MPI_Exscan_init.
 */
#define MPI_Exscan_init_U MPI_EXSCAN_INIT

/** @def MPI_Gather_U
    Exchanges MPI_Gather_U with MPI_GATHER.
    It is used for the Fortran wrappers of MPI_Gather.
 */
#define MPI_Gather_U MPI_GATHER

/** @def MPI_Gather_init_U
    Exchanges MPI_Gather_init_U with MPI_GATHER_INIT.
    It is used for the Fortran wrappers of MPI_Gather_init.
 */
#define MPI_Gather_init_U MPI_GATHER_INIT

/** @def MPI_Gatherv_U
    Exchanges MPI_Gatherv_U with MPI_GATHERV.
    It is used for the Fortran wrappers of MPI_Gatherv.
 */
#define MPI_Gatherv_U MPI_GATHERV

/** @def MPI_Gatherv_init_U
    Exchanges MPI_Gatherv_init_U with MPI_GATHERV_INIT.
    It is used for the Fortran wrappers of MPI_Gatherv_init.
 */
#define MPI_Gatherv_init_U MPI_GATHERV_INIT

/** @def MPI_Iallgather_U
    Exchanges MPI_Iallgather_U with MPI_IALLGATHER.
    It is used for the Fortran wrappers of MPI_Iallgather.
 */
#define MPI_Iallgather_U MPI_IALLGATHER

/** @def MPI_Iallgatherv_U
    Exchanges MPI_Iallgatherv_U with MPI_IALLGATHERV.
    It is used for the Fortran wrappers of MPI_Iallgatherv.
 */
#define MPI_Iallgatherv_U MPI_IALLGATHERV

/** @def MPI_Iallreduce_U
    Exchanges MPI_Iallreduce_U with MPI_IALLREDUCE.
    It is used for the Fortran wrappers of MPI_Iallreduce.
 */
#define MPI_Iallreduce_U MPI_IALLREDUCE

/** @def MPI_Ialltoall_U
    Exchanges MPI_Ialltoall_U with MPI_IALLTOALL.
    It is used for the Fortran wrappers of MPI_Ialltoall.
 */
#define MPI_Ialltoall_U MPI_IALLTOALL

/** @def MPI_Ialltoallv_U
    Exchanges MPI_Ialltoallv_U with MPI_IALLTOALLV.
    It is used for the Fortran wrappers of MPI_Ialltoallv.
 */
#define MPI_Ialltoallv_U MPI_IALLTOALLV

/** @def MPI_Ialltoallw_U
    Exchanges MPI_Ialltoallw_U with MPI_IALLTOALLW.
    It is used for the Fortran wrappers of MPI_Ialltoallw.
 */
#define MPI_Ialltoallw_U MPI_IALLTOALLW

/** @def MPI_Ibarrier_U
    Exchanges MPI_Ibarrier_U with MPI_IBARRIER.
    It is used for the Fortran wrappers of MPI_Ibarrier.
 */
#define MPI_Ibarrier_U MPI_IBARRIER

/** @def MPI_Ibcast_U
    Exchanges MPI_Ibcast_U with MPI_IBCAST.
    It is used for the Fortran wrappers of MPI_Ibcast.
 */
#define MPI_Ibcast_U MPI_IBCAST

/** @def MPI_Iexscan_U
    Exchanges MPI_Iexscan_U with MPI_IEXSCAN.
    It is used for the Fortran wrappers of MPI_Iexscan.
 */
#define MPI_Iexscan_U MPI_IEXSCAN

/** @def MPI_Igather_U
    Exchanges MPI_Igather_U with MPI_IGATHER.
    It is used for the Fortran wrappers of MPI_Igather.
 */
#define MPI_Igather_U MPI_IGATHER

/** @def MPI_Igatherv_U
    Exchanges MPI_Igatherv_U with MPI_IGATHERV.
    It is used for the Fortran wrappers of MPI_Igatherv.
 */
#define MPI_Igatherv_U MPI_IGATHERV

/** @def MPI_Ireduce_U
    Exchanges MPI_Ireduce_U with MPI_IREDUCE.
    It is used for the Fortran wrappers of MPI_Ireduce.
 */
#define MPI_Ireduce_U MPI_IREDUCE

/** @def MPI_Ireduce_scatter_U
    Exchanges MPI_Ireduce_scatter_U with MPI_IREDUCE_SCATTER.
    It is used for the Fortran wrappers of MPI_Ireduce_scatter.
 */
#define MPI_Ireduce_scatter_U MPI_IREDUCE_SCATTER

/** @def MPI_Ireduce_scatter_block_U
    Exchanges MPI_Ireduce_scatter_block_U with MPI_IREDUCE_SCATTER_BLOCK.
    It is used for the Fortran wrappers of MPI_Ireduce_scatter_block.
 */
#define MPI_Ireduce_scatter_block_U MPI_IREDUCE_SCATTER_BLOCK

/** @def MPI_Iscan_U
    Exchanges MPI_Iscan_U with MPI_ISCAN.
    It is used for the Fortran wrappers of MPI_Iscan.
 */
#define MPI_Iscan_U MPI_ISCAN

/** @def MPI_Iscatter_U
    Exchanges MPI_Iscatter_U with MPI_ISCATTER.
    It is used for the Fortran wrappers of MPI_Iscatter.
 */
#define MPI_Iscatter_U MPI_ISCATTER

/** @def MPI_Iscatterv_U
    Exchanges MPI_Iscatterv_U with MPI_ISCATTERV.
    It is used for the Fortran wrappers of MPI_Iscatterv.
 */
#define MPI_Iscatterv_U MPI_ISCATTERV

/** @def MPI_Reduce_U
    Exchanges MPI_Reduce_U with MPI_REDUCE.
    It is used for the Fortran wrappers of MPI_Reduce.
 */
#define MPI_Reduce_U MPI_REDUCE

/** @def MPI_Reduce_init_U
    Exchanges MPI_Reduce_init_U with MPI_REDUCE_INIT.
    It is used for the Fortran wrappers of MPI_Reduce_init.
 */
#define MPI_Reduce_init_U MPI_REDUCE_INIT

/** @def MPI_Reduce_local_U
    Exchanges MPI_Reduce_local_U with MPI_REDUCE_LOCAL.
    It is used for the Fortran wrappers of MPI_Reduce_local.
 */
#define MPI_Reduce_local_U MPI_REDUCE_LOCAL

/** @def MPI_Reduce_scatter_U
    Exchanges MPI_Reduce_scatter_U with MPI_REDUCE_SCATTER.
    It is used for the Fortran wrappers of MPI_Reduce_scatter.
 */
#define MPI_Reduce_scatter_U MPI_REDUCE_SCATTER

/** @def MPI_Reduce_scatter_block_U
    Exchanges MPI_Reduce_scatter_block_U with MPI_REDUCE_SCATTER_BLOCK.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_block.
 */
#define MPI_Reduce_scatter_block_U MPI_REDUCE_SCATTER_BLOCK

/** @def MPI_Reduce_scatter_block_init_U
    Exchanges MPI_Reduce_scatter_block_init_U with MPI_REDUCE_SCATTER_BLOCK_INIT.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_block_init.
 */
#define MPI_Reduce_scatter_block_init_U MPI_REDUCE_SCATTER_BLOCK_INIT

/** @def MPI_Reduce_scatter_init_U
    Exchanges MPI_Reduce_scatter_init_U with MPI_REDUCE_SCATTER_INIT.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_init.
 */
#define MPI_Reduce_scatter_init_U MPI_REDUCE_SCATTER_INIT

/** @def MPI_Scan_U
    Exchanges MPI_Scan_U with MPI_SCAN.
    It is used for the Fortran wrappers of MPI_Scan.
 */
#define MPI_Scan_U MPI_SCAN

/** @def MPI_Scan_init_U
    Exchanges MPI_Scan_init_U with MPI_SCAN_INIT.
    It is used for the Fortran wrappers of MPI_Scan_init.
 */
#define MPI_Scan_init_U MPI_SCAN_INIT

/** @def MPI_Scatter_U
    Exchanges MPI_Scatter_U with MPI_SCATTER.
    It is used for the Fortran wrappers of MPI_Scatter.
 */
#define MPI_Scatter_U MPI_SCATTER

/** @def MPI_Scatter_init_U
    Exchanges MPI_Scatter_init_U with MPI_SCATTER_INIT.
    It is used for the Fortran wrappers of MPI_Scatter_init.
 */
#define MPI_Scatter_init_U MPI_SCATTER_INIT

/** @def MPI_Scatterv_U
    Exchanges MPI_Scatterv_U with MPI_SCATTERV.
    It is used for the Fortran wrappers of MPI_Scatterv.
 */
#define MPI_Scatterv_U MPI_SCATTERV

/** @def MPI_Scatterv_init_U
    Exchanges MPI_Scatterv_init_U with MPI_SCATTERV_INIT.
    It is used for the Fortran wrappers of MPI_Scatterv_init.
 */
#define MPI_Scatterv_init_U MPI_SCATTERV_INIT

/* lowercase defines */

/** @def MPI_Allgather_L
    Exchanges MPI_Allgather_L with mpi_allgather.
    It is used for the Fortran wrappers of MPI_Allgather.
 */
#define MPI_Allgather_L mpi_allgather

/** @def MPI_Allgather_init_L
    Exchanges MPI_Allgather_init_L with mpi_allgather_init.
    It is used for the Fortran wrappers of MPI_Allgather_init.
 */
#define MPI_Allgather_init_L mpi_allgather_init

/** @def MPI_Allgatherv_L
    Exchanges MPI_Allgatherv_L with mpi_allgatherv.
    It is used for the Fortran wrappers of MPI_Allgatherv.
 */
#define MPI_Allgatherv_L mpi_allgatherv

/** @def MPI_Allgatherv_init_L
    Exchanges MPI_Allgatherv_init_L with mpi_allgatherv_init.
    It is used for the Fortran wrappers of MPI_Allgatherv_init.
 */
#define MPI_Allgatherv_init_L mpi_allgatherv_init

/** @def MPI_Allreduce_L
    Exchanges MPI_Allreduce_L with mpi_allreduce.
    It is used for the Fortran wrappers of MPI_Allreduce.
 */
#define MPI_Allreduce_L mpi_allreduce

/** @def MPI_Allreduce_init_L
    Exchanges MPI_Allreduce_init_L with mpi_allreduce_init.
    It is used for the Fortran wrappers of MPI_Allreduce_init.
 */
#define MPI_Allreduce_init_L mpi_allreduce_init

/** @def MPI_Alltoall_L
    Exchanges MPI_Alltoall_L with mpi_alltoall.
    It is used for the Fortran wrappers of MPI_Alltoall.
 */
#define MPI_Alltoall_L mpi_alltoall

/** @def MPI_Alltoall_init_L
    Exchanges MPI_Alltoall_init_L with mpi_alltoall_init.
    It is used for the Fortran wrappers of MPI_Alltoall_init.
 */
#define MPI_Alltoall_init_L mpi_alltoall_init

/** @def MPI_Alltoallv_L
    Exchanges MPI_Alltoallv_L with mpi_alltoallv.
    It is used for the Fortran wrappers of MPI_Alltoallv.
 */
#define MPI_Alltoallv_L mpi_alltoallv

/** @def MPI_Alltoallv_init_L
    Exchanges MPI_Alltoallv_init_L with mpi_alltoallv_init.
    It is used for the Fortran wrappers of MPI_Alltoallv_init.
 */
#define MPI_Alltoallv_init_L mpi_alltoallv_init

/** @def MPI_Alltoallw_L
    Exchanges MPI_Alltoallw_L with mpi_alltoallw.
    It is used for the Fortran wrappers of MPI_Alltoallw.
 */
#define MPI_Alltoallw_L mpi_alltoallw

/** @def MPI_Alltoallw_init_L
    Exchanges MPI_Alltoallw_init_L with mpi_alltoallw_init.
    It is used for the Fortran wrappers of MPI_Alltoallw_init.
 */
#define MPI_Alltoallw_init_L mpi_alltoallw_init

/** @def MPI_Barrier_L
    Exchanges MPI_Barrier_L with mpi_barrier.
    It is used for the Fortran wrappers of MPI_Barrier.
 */
#define MPI_Barrier_L mpi_barrier

/** @def MPI_Barrier_init_L
    Exchanges MPI_Barrier_init_L with mpi_barrier_init.
    It is used for the Fortran wrappers of MPI_Barrier_init.
 */
#define MPI_Barrier_init_L mpi_barrier_init

/** @def MPI_Bcast_L
    Exchanges MPI_Bcast_L with mpi_bcast.
    It is used for the Fortran wrappers of MPI_Bcast.
 */
#define MPI_Bcast_L mpi_bcast

/** @def MPI_Bcast_init_L
    Exchanges MPI_Bcast_init_L with mpi_bcast_init.
    It is used for the Fortran wrappers of MPI_Bcast_init.
 */
#define MPI_Bcast_init_L mpi_bcast_init

/** @def MPI_Exscan_L
    Exchanges MPI_Exscan_L with mpi_exscan.
    It is used for the Fortran wrappers of MPI_Exscan.
 */
#define MPI_Exscan_L mpi_exscan

/** @def MPI_Exscan_init_L
    Exchanges MPI_Exscan_init_L with mpi_exscan_init.
    It is used for the Fortran wrappers of MPI_Exscan_init.
 */
#define MPI_Exscan_init_L mpi_exscan_init

/** @def MPI_Gather_L
    Exchanges MPI_Gather_L with mpi_gather.
    It is used for the Fortran wrappers of MPI_Gather.
 */
#define MPI_Gather_L mpi_gather

/** @def MPI_Gather_init_L
    Exchanges MPI_Gather_init_L with mpi_gather_init.
    It is used for the Fortran wrappers of MPI_Gather_init.
 */
#define MPI_Gather_init_L mpi_gather_init

/** @def MPI_Gatherv_L
    Exchanges MPI_Gatherv_L with mpi_gatherv.
    It is used for the Fortran wrappers of MPI_Gatherv.
 */
#define MPI_Gatherv_L mpi_gatherv

/** @def MPI_Gatherv_init_L
    Exchanges MPI_Gatherv_init_L with mpi_gatherv_init.
    It is used for the Fortran wrappers of MPI_Gatherv_init.
 */
#define MPI_Gatherv_init_L mpi_gatherv_init

/** @def MPI_Iallgather_L
    Exchanges MPI_Iallgather_L with mpi_iallgather.
    It is used for the Fortran wrappers of MPI_Iallgather.
 */
#define MPI_Iallgather_L mpi_iallgather

/** @def MPI_Iallgatherv_L
    Exchanges MPI_Iallgatherv_L with mpi_iallgatherv.
    It is used for the Fortran wrappers of MPI_Iallgatherv.
 */
#define MPI_Iallgatherv_L mpi_iallgatherv

/** @def MPI_Iallreduce_L
    Exchanges MPI_Iallreduce_L with mpi_iallreduce.
    It is used for the Fortran wrappers of MPI_Iallreduce.
 */
#define MPI_Iallreduce_L mpi_iallreduce

/** @def MPI_Ialltoall_L
    Exchanges MPI_Ialltoall_L with mpi_ialltoall.
    It is used for the Fortran wrappers of MPI_Ialltoall.
 */
#define MPI_Ialltoall_L mpi_ialltoall

/** @def MPI_Ialltoallv_L
    Exchanges MPI_Ialltoallv_L with mpi_ialltoallv.
    It is used for the Fortran wrappers of MPI_Ialltoallv.
 */
#define MPI_Ialltoallv_L mpi_ialltoallv

/** @def MPI_Ialltoallw_L
    Exchanges MPI_Ialltoallw_L with mpi_ialltoallw.
    It is used for the Fortran wrappers of MPI_Ialltoallw.
 */
#define MPI_Ialltoallw_L mpi_ialltoallw

/** @def MPI_Ibarrier_L
    Exchanges MPI_Ibarrier_L with mpi_ibarrier.
    It is used for the Fortran wrappers of MPI_Ibarrier.
 */
#define MPI_Ibarrier_L mpi_ibarrier

/** @def MPI_Ibcast_L
    Exchanges MPI_Ibcast_L with mpi_ibcast.
    It is used for the Fortran wrappers of MPI_Ibcast.
 */
#define MPI_Ibcast_L mpi_ibcast

/** @def MPI_Iexscan_L
    Exchanges MPI_Iexscan_L with mpi_iexscan.
    It is used for the Fortran wrappers of MPI_Iexscan.
 */
#define MPI_Iexscan_L mpi_iexscan

/** @def MPI_Igather_L
    Exchanges MPI_Igather_L with mpi_igather.
    It is used for the Fortran wrappers of MPI_Igather.
 */
#define MPI_Igather_L mpi_igather

/** @def MPI_Igatherv_L
    Exchanges MPI_Igatherv_L with mpi_igatherv.
    It is used for the Fortran wrappers of MPI_Igatherv.
 */
#define MPI_Igatherv_L mpi_igatherv

/** @def MPI_Ireduce_L
    Exchanges MPI_Ireduce_L with mpi_ireduce.
    It is used for the Fortran wrappers of MPI_Ireduce.
 */
#define MPI_Ireduce_L mpi_ireduce

/** @def MPI_Ireduce_scatter_L
    Exchanges MPI_Ireduce_scatter_L with mpi_ireduce_scatter.
    It is used for the Fortran wrappers of MPI_Ireduce_scatter.
 */
#define MPI_Ireduce_scatter_L mpi_ireduce_scatter

/** @def MPI_Ireduce_scatter_block_L
    Exchanges MPI_Ireduce_scatter_block_L with mpi_ireduce_scatter_block.
    It is used for the Fortran wrappers of MPI_Ireduce_scatter_block.
 */
#define MPI_Ireduce_scatter_block_L mpi_ireduce_scatter_block

/** @def MPI_Iscan_L
    Exchanges MPI_Iscan_L with mpi_iscan.
    It is used for the Fortran wrappers of MPI_Iscan.
 */
#define MPI_Iscan_L mpi_iscan

/** @def MPI_Iscatter_L
    Exchanges MPI_Iscatter_L with mpi_iscatter.
    It is used for the Fortran wrappers of MPI_Iscatter.
 */
#define MPI_Iscatter_L mpi_iscatter

/** @def MPI_Iscatterv_L
    Exchanges MPI_Iscatterv_L with mpi_iscatterv.
    It is used for the Fortran wrappers of MPI_Iscatterv.
 */
#define MPI_Iscatterv_L mpi_iscatterv

/** @def MPI_Reduce_L
    Exchanges MPI_Reduce_L with mpi_reduce.
    It is used for the Fortran wrappers of MPI_Reduce.
 */
#define MPI_Reduce_L mpi_reduce

/** @def MPI_Reduce_init_L
    Exchanges MPI_Reduce_init_L with mpi_reduce_init.
    It is used for the Fortran wrappers of MPI_Reduce_init.
 */
#define MPI_Reduce_init_L mpi_reduce_init

/** @def MPI_Reduce_local_L
    Exchanges MPI_Reduce_local_L with mpi_reduce_local.
    It is used for the Fortran wrappers of MPI_Reduce_local.
 */
#define MPI_Reduce_local_L mpi_reduce_local

/** @def MPI_Reduce_scatter_L
    Exchanges MPI_Reduce_scatter_L with mpi_reduce_scatter.
    It is used for the Fortran wrappers of MPI_Reduce_scatter.
 */
#define MPI_Reduce_scatter_L mpi_reduce_scatter

/** @def MPI_Reduce_scatter_block_L
    Exchanges MPI_Reduce_scatter_block_L with mpi_reduce_scatter_block.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_block.
 */
#define MPI_Reduce_scatter_block_L mpi_reduce_scatter_block

/** @def MPI_Reduce_scatter_block_init_L
    Exchanges MPI_Reduce_scatter_block_init_L with mpi_reduce_scatter_block_init.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_block_init.
 */
#define MPI_Reduce_scatter_block_init_L mpi_reduce_scatter_block_init

/** @def MPI_Reduce_scatter_init_L
    Exchanges MPI_Reduce_scatter_init_L with mpi_reduce_scatter_init.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_init.
 */
#define MPI_Reduce_scatter_init_L mpi_reduce_scatter_init

/** @def MPI_Scan_L
    Exchanges MPI_Scan_L with mpi_scan.
    It is used for the Fortran wrappers of MPI_Scan.
 */
#define MPI_Scan_L mpi_scan

/** @def MPI_Scan_init_L
    Exchanges MPI_Scan_init_L with mpi_scan_init.
    It is used for the Fortran wrappers of MPI_Scan_init.
 */
#define MPI_Scan_init_L mpi_scan_init

/** @def MPI_Scatter_L
    Exchanges MPI_Scatter_L with mpi_scatter.
    It is used for the Fortran wrappers of MPI_Scatter.
 */
#define MPI_Scatter_L mpi_scatter

/** @def MPI_Scatter_init_L
    Exchanges MPI_Scatter_init_L with mpi_scatter_init.
    It is used for the Fortran wrappers of MPI_Scatter_init.
 */
#define MPI_Scatter_init_L mpi_scatter_init

/** @def MPI_Scatterv_L
    Exchanges MPI_Scatterv_L with mpi_scatterv.
    It is used for the Fortran wrappers of MPI_Scatterv.
 */
#define MPI_Scatterv_L mpi_scatterv

/** @def MPI_Scatterv_init_L
    Exchanges MPI_Scatterv_init_L with mpi_scatterv_init.
    It is used for the Fortran wrappers of MPI_Scatterv_init.
 */
#define MPI_Scatterv_init_L mpi_scatterv_init

/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLGATHER )
/**
 * Measurement wrapper for MPI_Allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Allgather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Allgather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLGATHER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgather_init )
/**
 * Measurement wrapper for MPI_Allgather_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Allgather_init )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Allgather_init( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv )
/**
 * Measurement wrapper for MPI_Allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Allgatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Allgatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLGATHERV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv_init )
/**
 * Measurement wrapper for MPI_Allgatherv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Allgatherv_init )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int recvcounts[], int displs[], MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Allgatherv_init( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce )
/**
 * Measurement wrapper for MPI_Allreduce
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Allreduce )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Allreduce( sendbuf, recvbuf, *count, *datatype, *op, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLREDUCE_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce_init )
/**
 * Measurement wrapper for MPI_Allreduce_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Allreduce_init )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Allreduce_init( sendbuf, recvbuf, *count, *datatype, *op, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall )
/**
 * Measurement wrapper for MPI_Alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoall )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Alltoall( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLTOALL_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall_init )
/**
 * Measurement wrapper for MPI_Alltoall_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoall_init )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Alltoall_init( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv )
/**
 * Measurement wrapper for MPI_Alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallv )( void* sendbuf, int* sendcounts, int* sdispls, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* rdispls, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Alltoallv( sendbuf, sendcounts, sdispls, *sendtype, recvbuf, recvcounts, rdispls, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLTOALLV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv_init )
/**
 * Measurement wrapper for MPI_Alltoallv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallv_init )( void* sendbuf, int sendcounts[], int sdispls[], MPI_Datatype* sendtype, void* recvbuf, int recvcounts[], int rdispls[], MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Alltoallv_init( sendbuf, sendcounts, sdispls, *sendtype, recvbuf, recvcounts, rdispls, *recvtype, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_ALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallw )
/**
 * Measurement wrapper for MPI_Alltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallw )( void* sendbuf, int sendcounts[], int sdispls[], MPI_Datatype sendtypes[], void* recvbuf, int recvcounts[], int rdispls[], MPI_Datatype recvtypes[], MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Alltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLTOALLW_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallw_init )
/**
 * Measurement wrapper for MPI_Alltoallw_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallw_init )( void* sendbuf, int sendcounts[], int sdispls[], MPI_Datatype sendtypes[], void* recvbuf, int recvcounts[], int rdispls[], MPI_Datatype recvtypes[], MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Alltoallw_init( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_BARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier )
/**
 * Measurement wrapper for MPI_Barrier
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Barrier )( MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Barrier( *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_BARRIER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier_init )
/**
 * Measurement wrapper for MPI_Barrier_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Barrier_init )( MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Barrier_init( *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_BCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast )
/**
 * Measurement wrapper for MPI_Bcast
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Bcast )( void* buffer, int* count, MPI_Datatype* datatype, int* root, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buffer == scorep_mpi_fortran_bottom )
    {
        buffer = MPI_BOTTOM;
    }


    *ierr = MPI_Bcast( buffer, *count, *datatype, *root, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_BCAST_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast_init )
/**
 * Measurement wrapper for MPI_Bcast_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Bcast_init )( void* buffer, int* count, MPI_Datatype* datatype, int* root, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Bcast_init( buffer, *count, *datatype, *root, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_EXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan )
/**
 * Measurement wrapper for MPI_Exscan
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup coll
 */
void
FSUB( MPI_Exscan )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Exscan( sendbuf, recvbuf, *count, *datatype, *op, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_EXSCAN_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan_init )
/**
 * Measurement wrapper for MPI_Exscan_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Exscan_init )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Exscan_init( sendbuf, recvbuf, *count, *datatype, *op, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_GATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather )
/**
 * Measurement wrapper for MPI_Gather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Gather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Gather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_GATHER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather_init )
/**
 * Measurement wrapper for MPI_Gather_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Gather_init )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Gather_init( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_GATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv )
/**
 * Measurement wrapper for MPI_Gatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Gatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Gatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *root, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_GATHERV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv_init )
/**
 * Measurement wrapper for MPI_Gatherv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Gatherv_init )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int recvcounts[], int displs[], MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Gatherv_init( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *root, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLGATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iallgather )
/**
 * Measurement wrapper for MPI_Iallgather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iallgather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iallgather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iallgatherv )
/**
 * Measurement wrapper for MPI_Iallgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iallgatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iallgatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iallreduce )
/**
 * Measurement wrapper for MPI_Iallreduce
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iallreduce )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iallreduce( sendbuf, recvbuf, *count, *datatype, *op, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ialltoall )
/**
 * Measurement wrapper for MPI_Ialltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ialltoall )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ialltoall( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ialltoallv )
/**
 * Measurement wrapper for MPI_Ialltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ialltoallv )( void* sendbuf, int* sendcounts, int* sdispls, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* rdispls, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ialltoallv( sendbuf, sendcounts, sdispls, *sendtype, recvbuf, recvcounts, rdispls, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ialltoallw )
/**
 * Measurement wrapper for MPI_Ialltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ialltoallw )( void* sendbuf, int sendcounts[], int sdispls[], MPI_Datatype sendtypes[], void* recvbuf, int recvcounts[], int rdispls[], MPI_Datatype recvtypes[], MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ialltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IBARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ibarrier )
/**
 * Measurement wrapper for MPI_Ibarrier
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ibarrier )( MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Ibarrier( *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IBCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ibcast )
/**
 * Measurement wrapper for MPI_Ibcast
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ibcast )( void* buffer, int* count, MPI_Datatype* datatype, int* root, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buffer == scorep_mpi_fortran_bottom )
    {
        buffer = MPI_BOTTOM;
    }


    *ierr = MPI_Ibcast( buffer, *count, *datatype, *root, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IEXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iexscan )
/**
 * Measurement wrapper for MPI_Iexscan
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iexscan )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iexscan( sendbuf, recvbuf, *count, *datatype, *op, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IGATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Igather )
/**
 * Measurement wrapper for MPI_Igather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Igather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Igather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Igatherv )
/**
 * Measurement wrapper for MPI_Igatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Igatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Igatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *root, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ireduce )
/**
 * Measurement wrapper for MPI_Ireduce
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ireduce )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, int* root, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ireduce( sendbuf, recvbuf, *count, *datatype, *op, *root, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IREDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ireduce_scatter )
/**
 * Measurement wrapper for MPI_Ireduce_scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ireduce_scatter )( void* sendbuf, void* recvbuf, int* recvcounts, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ireduce_scatter( sendbuf, recvbuf, recvcounts, *datatype, *op, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IREDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ireduce_scatter_block )
/**
 * Measurement wrapper for MPI_Ireduce_scatter_block
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ireduce_scatter_block )( void* sendbuf, void* recvbuf, int* recvcount, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ireduce_scatter_block( sendbuf, recvbuf, *recvcount, *datatype, *op, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_ISCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iscan )
/**
 * Measurement wrapper for MPI_Iscan
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iscan )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iscan( sendbuf, recvbuf, *count, *datatype, *op, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_ISCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iscatter )
/**
 * Measurement wrapper for MPI_Iscatter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iscatter )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iscatter( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_ISCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iscatterv )
/**
 * Measurement wrapper for MPI_Iscatterv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Iscatterv )( void* sendbuf, int* sendcounts, int* displs, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iscatterv( sendbuf, sendcounts, displs, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_REDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce )
/**
 * Measurement wrapper for MPI_Reduce
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Reduce )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, int* root, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Reduce( sendbuf, recvbuf, *count, *datatype, *op, *root, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_REDUCE_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_init )
/**
 * Measurement wrapper for MPI_Reduce_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_init )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, int* root, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Reduce_init( sendbuf, recvbuf, *count, *datatype, *op, *root, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_2_SYMBOL_PMPI_REDUCE_LOCAL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_local )
/**
 * Measurement wrapper for MPI_Reduce_local
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.2
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_local )( void* inbuf, void* inoutbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Reduce_local( inbuf, inoutbuf, *count, *datatype, *op );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_REDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter )
/**
 * Measurement wrapper for MPI_Reduce_scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_scatter )( void* sendbuf, void* recvbuf, int* recvcounts, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Reduce_scatter( sendbuf, recvbuf, recvcounts, *datatype, *op, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_2_SYMBOL_PMPI_REDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block )
/**
 * Measurement wrapper for MPI_Reduce_scatter_block
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.2
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_scatter_block )( void* sendbuf, void* recvbuf, int* recvcount, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Reduce_scatter_block( sendbuf, recvbuf, *recvcount, *datatype, *op, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_REDUCE_SCATTER_BLOCK_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block_init )
/**
 * Measurement wrapper for MPI_Reduce_scatter_block_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_scatter_block_init )( void* sendbuf, void* recvbuf, int* recvcount, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Reduce_scatter_block_init( sendbuf, recvbuf, *recvcount, *datatype, *op, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_REDUCE_SCATTER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_init )
/**
 * Measurement wrapper for MPI_Reduce_scatter_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_scatter_init )( void* sendbuf, void* recvbuf, int recvcounts[], MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Reduce_scatter_init( sendbuf, recvbuf, recvcounts, *datatype, *op, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan )
/**
 * Measurement wrapper for MPI_Scan
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Scan )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Scan( sendbuf, recvbuf, *count, *datatype, *op, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SCAN_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan_init )
/**
 * Measurement wrapper for MPI_Scan_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Scan_init )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Scan_init( sendbuf, recvbuf, *count, *datatype, *op, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter )
/**
 * Measurement wrapper for MPI_Scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Scatter )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Scatter( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SCATTER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter_init )
/**
 * Measurement wrapper for MPI_Scatter_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Scatter_init )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Scatter_init( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv )
/**
 * Measurement wrapper for MPI_Scatterv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Scatterv )( void* sendbuf, int* sendcounts, int* displs, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Scatterv( sendbuf, sendcounts, displs, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SCATTERV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv_init )
/**
 * Measurement wrapper for MPI_Scatterv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 */
void
FSUB( MPI_Scatterv_init )( void* sendbuf, int sendcounts[], int displs[], MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Scatterv_init( sendbuf, sendcounts, displs, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLGATHER )
/**
 * Measurement wrapper for MPI_Allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Allgather
 */
void
FSUB( MPI_Allgather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Allgather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLGATHER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgather_init )
/**
 * Measurement wrapper for MPI_Allgather_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Allgather_init
 */
void
FSUB( MPI_Allgather_init )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Allgather_init( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv )
/**
 * Measurement wrapper for MPI_Allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Allgatherv
 */
void
FSUB( MPI_Allgatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Allgatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLGATHERV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv_init )
/**
 * Measurement wrapper for MPI_Allgatherv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Allgatherv_init
 */
void
FSUB( MPI_Allgatherv_init )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Allgatherv_init( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce )
/**
 * Measurement wrapper for MPI_Allreduce
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Allreduce
 */
void
FSUB( MPI_Allreduce )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Allreduce( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLREDUCE_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce_init )
/**
 * Measurement wrapper for MPI_Allreduce_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Allreduce_init
 */
void
FSUB( MPI_Allreduce_init )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Allreduce_init( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall )
/**
 * Measurement wrapper for MPI_Alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Alltoall
 */
void
FSUB( MPI_Alltoall )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Alltoall( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLTOALL_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall_init )
/**
 * Measurement wrapper for MPI_Alltoall_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Alltoall_init
 */
void
FSUB( MPI_Alltoall_init )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Alltoall_init( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv )
/**
 * Measurement wrapper for MPI_Alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Alltoallv
 */
void
FSUB( MPI_Alltoallv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Alltoallv( sendbuf, sendcounts, sdispls, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, rdispls, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ALLTOALLV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv_init )
/**
 * Measurement wrapper for MPI_Alltoallv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Alltoallv_init
 */
void
FSUB( MPI_Alltoallv_init )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Alltoallv_init( sendbuf, sendcounts, sdispls, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, rdispls, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_BARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier )
/**
 * Measurement wrapper for MPI_Barrier
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Barrier
 */
void
FSUB( MPI_Barrier )( MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Barrier( PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_BARRIER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier_init )
/**
 * Measurement wrapper for MPI_Barrier_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Barrier_init
 */
void
FSUB( MPI_Barrier_init )( MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Barrier_init( PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_BCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast )
/**
 * Measurement wrapper for MPI_Bcast
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Bcast
 */
void
FSUB( MPI_Bcast )( void* buffer, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buffer == scorep_mpi_fortran_bottom )
    {
        buffer = MPI_BOTTOM;
    }


    *ierr = MPI_Bcast( buffer, *count, PMPI_Type_f2c( *datatype ), *root, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_BCAST_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast_init )
/**
 * Measurement wrapper for MPI_Bcast_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Bcast_init
 */
void
FSUB( MPI_Bcast_init )( void* buffer, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Bcast_init( buffer, *count, PMPI_Type_f2c( *datatype ), *root, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_EXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan )
/**
 * Measurement wrapper for MPI_Exscan
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup coll
 * For the order of events see @ref MPI_Exscan
 */
void
FSUB( MPI_Exscan )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Exscan( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_EXSCAN_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan_init )
/**
 * Measurement wrapper for MPI_Exscan_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Exscan_init
 */
void
FSUB( MPI_Exscan_init )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Exscan_init( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_GATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather )
/**
 * Measurement wrapper for MPI_Gather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Gather
 */
void
FSUB( MPI_Gather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Gather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_GATHER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather_init )
/**
 * Measurement wrapper for MPI_Gather_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Gather_init
 */
void
FSUB( MPI_Gather_init )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Gather_init( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_GATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv )
/**
 * Measurement wrapper for MPI_Gatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Gatherv
 */
void
FSUB( MPI_Gatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Gatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_GATHERV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv_init )
/**
 * Measurement wrapper for MPI_Gatherv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Gatherv_init
 */
void
FSUB( MPI_Gatherv_init )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Gatherv_init( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLGATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iallgather )
/**
 * Measurement wrapper for MPI_Iallgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iallgather
 */
void
FSUB( MPI_Iallgather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iallgather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iallgatherv )
/**
 * Measurement wrapper for MPI_Iallgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iallgatherv
 */
void
FSUB( MPI_Iallgatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iallgatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iallreduce )
/**
 * Measurement wrapper for MPI_Iallreduce
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iallreduce
 */
void
FSUB( MPI_Iallreduce )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iallreduce( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ialltoall )
/**
 * Measurement wrapper for MPI_Ialltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ialltoall
 */
void
FSUB( MPI_Ialltoall )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ialltoall( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ialltoallv )
/**
 * Measurement wrapper for MPI_Ialltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ialltoallv
 */
void
FSUB( MPI_Ialltoallv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ialltoallv( sendbuf, sendcounts, sdispls, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, rdispls, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IBARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ibarrier )
/**
 * Measurement wrapper for MPI_Ibarrier
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ibarrier
 */
void
FSUB( MPI_Ibarrier )( MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Ibarrier( PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IBCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ibcast )
/**
 * Measurement wrapper for MPI_Ibcast
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ibcast
 */
void
FSUB( MPI_Ibcast )( void* buffer, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buffer == scorep_mpi_fortran_bottom )
    {
        buffer = MPI_BOTTOM;
    }


    *ierr = MPI_Ibcast( buffer, *count, PMPI_Type_f2c( *datatype ), *root, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IEXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iexscan )
/**
 * Measurement wrapper for MPI_Iexscan
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iexscan
 */
void
FSUB( MPI_Iexscan )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iexscan( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IGATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Igather )
/**
 * Measurement wrapper for MPI_Igather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Igather
 */
void
FSUB( MPI_Igather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Igather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Igatherv )
/**
 * Measurement wrapper for MPI_Igatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Igatherv
 */
void
FSUB( MPI_Igatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Igatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ireduce )
/**
 * Measurement wrapper for MPI_Ireduce
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ireduce
 */
void
FSUB( MPI_Ireduce )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ireduce( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), *root, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IREDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ireduce_scatter )
/**
 * Measurement wrapper for MPI_Ireduce_scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ireduce_scatter
 */
void
FSUB( MPI_Ireduce_scatter )( void* sendbuf, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ireduce_scatter( sendbuf, recvbuf, recvcounts, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IREDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Ireduce_scatter_block )
/**
 * Measurement wrapper for MPI_Ireduce_scatter_block
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Ireduce_scatter_block
 */
void
FSUB( MPI_Ireduce_scatter_block )( void* sendbuf, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Ireduce_scatter_block( sendbuf, recvbuf, *recvcount, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_ISCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iscan )
/**
 * Measurement wrapper for MPI_Iscan
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iscan
 */
void
FSUB( MPI_Iscan )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iscan( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_ISCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iscatter )
/**
 * Measurement wrapper for MPI_Iscatter
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iscatter
 */
void
FSUB( MPI_Iscatter )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iscatter( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_ISCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Iscatterv )
/**
 * Measurement wrapper for MPI_Iscatterv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 * For the order of events see @ref MPI_Iscatterv
 */
void
FSUB( MPI_Iscatterv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* displs, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Iscatterv( sendbuf, sendcounts, displs, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_REDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce )
/**
 * Measurement wrapper for MPI_Reduce
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce
 */
void
FSUB( MPI_Reduce )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Reduce( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), *root, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_REDUCE_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_init )
/**
 * Measurement wrapper for MPI_Reduce_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_init
 */
void
FSUB( MPI_Reduce_init )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Reduce_init( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), *root, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_2_SYMBOL_PMPI_REDUCE_LOCAL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_local )
/**
 * Measurement wrapper for MPI_Reduce_local
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.2
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_local
 */
void
FSUB( MPI_Reduce_local )( void* inbuf, void* inoutbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Reduce_local( inbuf, inoutbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_REDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter )
/**
 * Measurement wrapper for MPI_Reduce_scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_scatter
 */
void
FSUB( MPI_Reduce_scatter )( void* sendbuf, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Reduce_scatter( sendbuf, recvbuf, recvcounts, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_2_SYMBOL_PMPI_REDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block )
/**
 * Measurement wrapper for MPI_Reduce_scatter_block
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.2
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_scatter_block
 */
void
FSUB( MPI_Reduce_scatter_block )( void* sendbuf, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Reduce_scatter_block( sendbuf, recvbuf, *recvcount, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_REDUCE_SCATTER_BLOCK_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block_init )
/**
 * Measurement wrapper for MPI_Reduce_scatter_block_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_scatter_block_init
 */
void
FSUB( MPI_Reduce_scatter_block_init )( void* sendbuf, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Reduce_scatter_block_init( sendbuf, recvbuf, *recvcount, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_REDUCE_SCATTER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_init )
/**
 * Measurement wrapper for MPI_Reduce_scatter_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_scatter_init
 */
void
FSUB( MPI_Reduce_scatter_init )( void* sendbuf, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Reduce_scatter_init( sendbuf, recvbuf, recvcounts, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan )
/**
 * Measurement wrapper for MPI_Scan
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Scan
 */
void
FSUB( MPI_Scan )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Scan( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SCAN_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan_init )
/**
 * Measurement wrapper for MPI_Scan_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Scan_init
 */
void
FSUB( MPI_Scan_init )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Scan_init( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter )
/**
 * Measurement wrapper for MPI_Scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Scatter
 */
void
FSUB( MPI_Scatter )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Scatter( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SCATTER_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter_init )
/**
 * Measurement wrapper for MPI_Scatter_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Scatter_init
 */
void
FSUB( MPI_Scatter_init )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Scatter_init( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv )
/**
 * Measurement wrapper for MPI_Scatterv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 * For the order of events see @ref MPI_Scatterv
 */
void
FSUB( MPI_Scatterv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* displs, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }


    *ierr = MPI_Scatterv( sendbuf, sendcounts, displs, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SCATTERV_INIT ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv_init )
/**
 * Measurement wrapper for MPI_Scatterv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup coll
 * For the order of events see @ref MPI_Scatterv_init
 */
void
FSUB( MPI_Scatterv_init )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* displs, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Scatterv_init( sendbuf, sendcounts, displs, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_ALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL )
/**
 * Measurement wrapper for MPI_Alltoallw
 * @note Manually created wrapper
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallw )( void*     sendbuf,
                       MPI_Fint* sendcounts,
                       MPI_Fint* sdispls,
                       MPI_Fint* sendtypes,
                       void*     recvbuf,
                       MPI_Fint* recvcounts,
                       MPI_Fint* rdispls,
                       MPI_Fint* recvtypes,
                       MPI_Fint* comm,
                       MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype* csendtypes;
    MPI_Datatype* crecvtypes;
    MPI_Comm      ccomm;
    int           size;

    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }

    ccomm = PMPI_Comm_f2c( *comm );
    PMPI_Comm_size( ccomm, &size );

    if ( sendbuf != MPI_IN_PLACE )
    {
        csendtypes = malloc( size * sizeof( MPI_Datatype ) );
    }
    crecvtypes = malloc( size * sizeof( MPI_Datatype ) );

    while ( size > 0 )
    {
        if ( sendbuf != MPI_IN_PLACE )
        {
            csendtypes[ size - 1 ] = PMPI_Type_f2c( sendtypes[ size - 1 ] );
        }
        crecvtypes[ size - 1 ] = PMPI_Type_f2c( recvtypes[ size - 1 ] );
        --size;
    }

    *ierr = MPI_Alltoallw( sendbuf, sendcounts, sdispls, csendtypes, recvbuf,
                           recvcounts, rdispls, crecvtypes, ccomm );

    if ( sendbuf != MPI_IN_PLACE )
    {
        free( csendtypes );
    }
    free( crecvtypes );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_3_0_SYMBOL_PMPI_IALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL )
/**
 * Measurement wrapper for MPI_Ialltoallw
 * @note Manually created wrapper
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup coll
 */
void
FSUB( MPI_Ialltoallw )( void*     sendbuf,
                        MPI_Fint* sendcounts,
                        MPI_Fint* sdispls,
                        MPI_Fint* sendtypes,
                        void*     recvbuf,
                        MPI_Fint* recvcounts,
                        MPI_Fint* rdispls,
                        MPI_Fint* recvtypes,
                        MPI_Fint* comm,
                        MPI_Fint* request,
                        MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype* csendtypes;
    MPI_Datatype* crecvtypes;
    MPI_Comm      ccomm;
    MPI_Request   crequest;
    int           size;

    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }

    ccomm = PMPI_Comm_f2c( *comm );
    PMPI_Comm_size( ccomm, &size );

    if ( sendbuf != MPI_IN_PLACE )
    {
        csendtypes = malloc( size * sizeof( MPI_Datatype ) );
    }
    crecvtypes = malloc( size * sizeof( MPI_Datatype ) );

    while ( size > 0 )
    {
        if ( sendbuf != MPI_IN_PLACE )
        {
            csendtypes[ size - 1 ] = PMPI_Type_f2c( sendtypes[ size - 1 ] );
        }
        crecvtypes[ size - 1 ] = PMPI_Type_f2c( recvtypes[ size - 1 ] );
        --size;
    }

    *ierr = MPI_Ialltoallw( sendbuf, sendcounts, sdispls, csendtypes, recvbuf,
                            recvcounts, rdispls, crecvtypes, ccomm, &crequest );

    *request = PMPI_Request_c2f( crequest );

    if ( sendbuf != MPI_IN_PLACE )
    {
        free( csendtypes );
    }
    free( crecvtypes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif

/**
 * @}
 */
