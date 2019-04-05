/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SCORE_EVENTLIST_HPP
#define SCOREP_SCORE_EVENTLIST_HPP

/**
 * @file
 *
 * @brief      Defines name lists for determine which events are created by which region.
 */

#define SCOREP_SCORE_EVENT_MPI_SEND \
    SCOREP_SCORE_EVENT( "MPI_Bsend" ) \
    SCOREP_SCORE_EVENT( "MPI_Rsend" ) \
    SCOREP_SCORE_EVENT( "MPI_Ssend" ) \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv" ) \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv_replace" ) \
    SCOREP_SCORE_EVENT( "MPI_Send" )

#define SCOREP_SCORE_EVENT_MPI_ISEND \
    SCOREP_SCORE_EVENT( "MPI_Ibsend" ) \
    SCOREP_SCORE_EVENT( "MPI_Irsend" ) \
    SCOREP_SCORE_EVENT( "MPI_Issend" ) \
    SCOREP_SCORE_EVENT( "MPI_Isend" ) \
    SCOREP_SCORE_EVENT( "MPI_Bsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Rsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Send_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Ssend_init" )

#define SCOREP_SCORE_EVENT_MPI_ISENDCOMPLETE \
    SCOREP_SCORE_EVENT( "MPI_Ibsend" ) \
    SCOREP_SCORE_EVENT( "MPI_Irsend" ) \
    SCOREP_SCORE_EVENT( "MPI_Issend" ) \
    SCOREP_SCORE_EVENT( "MPI_Isend" ) \
    SCOREP_SCORE_EVENT( "MPI_Bsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Rsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Send_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Ssend_init" )

#define SCOREP_SCORE_EVENT_MPI_IRECVREQUEST \
    SCOREP_SCORE_EVENT( "MPI_Irecv" ) \
    SCOREP_SCORE_EVENT( "MPI_Irecv_init" )

#define SCOREP_SCORE_EVENT_MPI_RECV \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv" ) \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv_replace" ) \
    SCOREP_SCORE_EVENT( "MPI_Recv" )

#define SCOREP_SCORE_EVENT_MPI_IRECV \
    SCOREP_SCORE_EVENT( "MPI_Irecv" ) \
    SCOREP_SCORE_EVENT( "MPI_Irecv_init" )

#define SCOREP_SCORE_EVENT_MPI_COLLECTIVE \
    SCOREP_SCORE_EVENT( "MPI_Allgather" ) \
    SCOREP_SCORE_EVENT( "MPI_Allgatherv" ) \
    SCOREP_SCORE_EVENT( "MPI_Allreduce" ) \
    SCOREP_SCORE_EVENT( "MPI_Alltoall" ) \
    SCOREP_SCORE_EVENT( "MPI_Alltoallv" ) \
    SCOREP_SCORE_EVENT( "MPI_Alltoallw" ) \
    SCOREP_SCORE_EVENT( "MPI_Barrier" ) \
    SCOREP_SCORE_EVENT( "MPI_Bcast" ) \
    SCOREP_SCORE_EVENT( "MPI_Exscan" ) \
    SCOREP_SCORE_EVENT( "MPI_Gather" ) \
    SCOREP_SCORE_EVENT( "MPI_Gatherv" ) \
    SCOREP_SCORE_EVENT( "MPI_Reduce" ) \
    SCOREP_SCORE_EVENT( "MPI_Reduce_scatter" ) \
    SCOREP_SCORE_EVENT( "MPI_Reduce_scatter_block" ) \
    SCOREP_SCORE_EVENT( "MPI_Scan" ) \
    SCOREP_SCORE_EVENT( "MPI_Scatter" ) \
    SCOREP_SCORE_EVENT( "MPI_Scatterv" )

#define SCOREP_SCORE_EVENT_CUDASTREAMCREATE \
    SCOREP_SCORE_EVENT( "cuStreamCreate" ) \
    SCOREP_SCORE_EVENT( "cudaStreamCreate" ) \

#define SCOREP_SCORE_EVENT_CUDAMEMCPY \
    SCOREP_SCORE_EVENT( "cuMemcpy" ) \
    SCOREP_SCORE_EVENT( "cudaMemcpy" ) \

/* Regions which trigger an an RmaWinCreate event */
#define SCOREP_SCORE_EVENT_RMA_WIN_CREATE

/* Regions which trigger an an RmaWinDestroy event */
#define SCOREP_SCORE_EVENT_RMA_WIN_DESTROY

/*
 * Regions which trigger an an Rma(Put|Get) and RmaOpComplete* event combination.
 * Note that RmaPut and RmaGet have the same estimate, and all of the
 * RmaOpComplete have the same estimate too.
 */
#define SCOREP_SCORE_EVENT_RMA_OP \
    SCOREP_SCORE_EVENT( "shmem_char_p" ) \
    SCOREP_SCORE_EVENT( "shmem_short_p" ) \
    SCOREP_SCORE_EVENT( "shmem_int_p" ) \
    SCOREP_SCORE_EVENT( "shmem_long_p" ) \
    SCOREP_SCORE_EVENT( "shmem_float_p" ) \
    SCOREP_SCORE_EVENT( "shmem_double_p" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_p" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_p" ) \
    SCOREP_SCORE_EVENT( "shmem_char_put" ) \
    SCOREP_SCORE_EVENT( "shmem_short_put" ) \
    SCOREP_SCORE_EVENT( "shmem_int_put" ) \
    SCOREP_SCORE_EVENT( "shmem_long_put" ) \
    SCOREP_SCORE_EVENT( "shmem_float_put" ) \
    SCOREP_SCORE_EVENT( "shmem_double_put" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_put" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_put" ) \
    SCOREP_SCORE_EVENT( "shmem_put16" ) \
    SCOREP_SCORE_EVENT( "shmem_put32" ) \
    SCOREP_SCORE_EVENT( "shmem_put64" ) \
    SCOREP_SCORE_EVENT( "shmem_put128" ) \
    SCOREP_SCORE_EVENT( "shmem_putmem" ) \
    SCOREP_SCORE_EVENT( "shmem_short_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_int_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_float_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_long_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_double_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_iput" ) \
    SCOREP_SCORE_EVENT( "shmem_iput16" ) \
    SCOREP_SCORE_EVENT( "shmem_iput32" ) \
    SCOREP_SCORE_EVENT( "shmem_iput64" ) \
    SCOREP_SCORE_EVENT( "shmem_iput128" ) \
    SCOREP_SCORE_EVENT( "shmem_char_g" ) \
    SCOREP_SCORE_EVENT( "shmem_short_g" ) \
    SCOREP_SCORE_EVENT( "shmem_int_g" ) \
    SCOREP_SCORE_EVENT( "shmem_long_g" ) \
    SCOREP_SCORE_EVENT( "shmem_float_g" ) \
    SCOREP_SCORE_EVENT( "shmem_double_g" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_g" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_g" ) \
    SCOREP_SCORE_EVENT( "shmem_char_get" ) \
    SCOREP_SCORE_EVENT( "shmem_short_get" ) \
    SCOREP_SCORE_EVENT( "shmem_int_get" ) \
    SCOREP_SCORE_EVENT( "shmem_long_get" ) \
    SCOREP_SCORE_EVENT( "shmem_float_get" ) \
    SCOREP_SCORE_EVENT( "shmem_double_get" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_get" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_get" ) \
    SCOREP_SCORE_EVENT( "shmem_get16" ) \
    SCOREP_SCORE_EVENT( "shmem_get32" ) \
    SCOREP_SCORE_EVENT( "shmem_get64" ) \
    SCOREP_SCORE_EVENT( "shmem_get128" ) \
    SCOREP_SCORE_EVENT( "shmem_getmem" ) \
    SCOREP_SCORE_EVENT( "shmem_short_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_int_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_float_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_long_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_double_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_iget" ) \
    SCOREP_SCORE_EVENT( "shmem_iget16" ) \
    SCOREP_SCORE_EVENT( "shmem_iget32" ) \
    SCOREP_SCORE_EVENT( "shmem_iget64" ) \
    SCOREP_SCORE_EVENT( "shmem_iget128" )

/*
 * Regions which trigger an an RmaOpCompleteRemote event.
 */
#define SCOREP_SCORE_EVENT_RMA_OP_COMPLETE_REMOTE \
    SCOREP_SCORE_EVENT( "shmem_barrier_all" )

/*
 * Regions which trigger an RmaAtomic and RmaOpComplete* event combination.
 * Note that all of the RmaOpComplete* events have the same estimate.
 */
#define SCOREP_SCORE_EVENT_RMA_ATOMIC \
    SCOREP_SCORE_EVENT( "shmem_short_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_int_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_long_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_float_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_double_swap" ) \
    SCOREP_SCORE_EVENT( "shmem_short_cswap" ) \
    SCOREP_SCORE_EVENT( "shmem_int_cswap" ) \
    SCOREP_SCORE_EVENT( "shmem_long_cswap" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_cswap" ) \
    SCOREP_SCORE_EVENT( "shmem_short_fadd" ) \
    SCOREP_SCORE_EVENT( "shmem_int_fadd" ) \
    SCOREP_SCORE_EVENT( "shmem_long_fadd" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_fadd" ) \
    SCOREP_SCORE_EVENT( "shmem_short_finc" ) \
    SCOREP_SCORE_EVENT( "shmem_int_finc" ) \
    SCOREP_SCORE_EVENT( "shmem_long_finc" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_finc" ) \
    SCOREP_SCORE_EVENT( "shmem_short_add" ) \
    SCOREP_SCORE_EVENT( "shmem_int_add" ) \
    SCOREP_SCORE_EVENT( "shmem_long_add" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_add" ) \
    SCOREP_SCORE_EVENT( "shmem_short_inc" ) \
    SCOREP_SCORE_EVENT( "shmem_int_inc" ) \
    SCOREP_SCORE_EVENT( "shmem_long_inc" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_inc" )

/* Regions which trigger an RmaCollectiveBegin and RmaCollectiveEnd event combination */
#define SCOREP_SCORE_EVENT_RMA_COLLECTIVE \
    SCOREP_SCORE_EVENT( "shmem_barrier_all" ) \
    SCOREP_SCORE_EVENT( "shmem_barrier" ) \
    SCOREP_SCORE_EVENT( "shmem_broadcast32" ) \
    SCOREP_SCORE_EVENT( "shmem_broadcast64" ) \
    SCOREP_SCORE_EVENT( "shmem_collect32" ) \
    SCOREP_SCORE_EVENT( "shmem_collect64" ) \
    SCOREP_SCORE_EVENT( "shmem_fcollect32" ) \
    SCOREP_SCORE_EVENT( "shmem_fcollect64" ) \
    SCOREP_SCORE_EVENT( "shmem_short_and_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_and_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_and_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_and_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_short_or_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_or_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_or_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_or_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_short_xor_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_xor_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_xor_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_xor_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_short_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_float_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_double_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_max_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_short_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_float_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_double_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_min_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_short_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_float_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_double_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_complexf_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_complexd_sum_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_short_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_int_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_long_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_float_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_double_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_longdouble_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_complexf_prod_to_all" ) \
    SCOREP_SCORE_EVENT( "shmem_complexd_prod_to_all" )

/* Regions which trigger an RmaWaitChange event */
#define SCOREP_SCORE_EVENT_RMA_WAIT_CHANGE \
    SCOREP_SCORE_EVENT( "shmem_short_wait" ) \
    SCOREP_SCORE_EVENT( "shmem_int_wait" ) \
    SCOREP_SCORE_EVENT( "shmem_long_wait" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_wait" ) \
    SCOREP_SCORE_EVENT( "shmem_wait" ) \
    SCOREP_SCORE_EVENT( "shmem_short_wait_until" ) \
    SCOREP_SCORE_EVENT( "shmem_int_wait_until" ) \
    SCOREP_SCORE_EVENT( "shmem_long_wait_until" ) \
    SCOREP_SCORE_EVENT( "shmem_longlong_wait_until" ) \
    SCOREP_SCORE_EVENT( "shmem_wait_until" )

/* Regions which trigger an Rma(Request|Acquire|Try)Lock event */
#define SCOREP_SCORE_EVENT_RMA_LOCK \
    SCOREP_SCORE_EVENT( "shmem_set_lock" ) \
    SCOREP_SCORE_EVENT( "shmem_test_lock" )

/* Regions which trigger an RmaReleaseLock event */
#define SCOREP_SCORE_EVENT_RMA_RELEASE_LOCK \
    SCOREP_SCORE_EVENT( "shmem_clear_lock" )

/*
 * RMA events which are not handled yet
 *  RmaGroupSync
 *  RmaOpTest
 *  RmaSync
 */

#define SCOREP_SCORE_EVENT_THREAD_FORK \
    SCOREP_SCORE_EVENT( "!$omp parallel" )

#define SCOREP_SCORE_EVENT_THREAD_JOIN \
    SCOREP_SCORE_EVENT( "!$omp parallel" )

#define SCOREP_SCORE_EVENT_THREAD_TEAM \
    SCOREP_SCORE_EVENT( "!$omp parallel" )

#define SCOREP_SCORE_EVENT_THREAD_TASK_CREATE \
    SCOREP_SCORE_EVENT( "!$omp task " )

// Should include taskwaits and task regions
// create task regions are not counted.
#define SCOREP_SCORE_EVENT_THREAD_TASK_SWITCH \
    SCOREP_SCORE_EVENT( "!$omp task" )

// In case of "pthread_mutex_trylock"
// it is unknown how many times locks were locked
// but amount of unlocks is reliable
#define SCOREP_SCORE_EVENT_THREAD_ACQUIRELOCK \
    SCOREP_SCORE_EVENT( "omp_set_lock" ) \
    SCOREP_SCORE_EVENT( "omp_set_nest_lock" ) \
    SCOREP_SCORE_EVENT( "pthread_mutex_unlock" ) \
    SCOREP_SCORE_EVENT( "pthread_cond_wait" ) \
    SCOREP_SCORE_EVENT( "pthread_cond_timedwait" )

#define SCOREP_SCORE_EVENT_THREAD_RELEASELOCK \
    SCOREP_SCORE_EVENT( "omp_unset_lock" ) \
    SCOREP_SCORE_EVENT( "omp_unset_nest_lock" ) \
    SCOREP_SCORE_EVENT( "pthread_mutex_unlock" ) \
    SCOREP_SCORE_EVENT( "pthread_cond_wait" ) \
    SCOREP_SCORE_EVENT( "pthread_cond_timedwait" )

#define SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_CREATE \
    SCOREP_SCORE_EVENT( "pthread_create" )

#define SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_BEGIN \
    SCOREP_SCORE_EVENT( "pthread_create" )

#define SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_WAIT \
    SCOREP_SCORE_EVENT( "pthread_join" )

#define SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_END \
    SCOREP_SCORE_EVENT( "pthread_create" )

#define SCOREP_SCORE_EVENT_IO_CREATE \
    SCOREP_SCORE_EVENT( "MPI_File_open" ) \
    SCOREP_SCORE_EVENT( "creat" ) \
    SCOREP_SCORE_EVENT( "creat64" ) \
    SCOREP_SCORE_EVENT( "open" ) \
    SCOREP_SCORE_EVENT( "open64" ) \
    SCOREP_SCORE_EVENT( "openat" ) \
    SCOREP_SCORE_EVENT( "fdopen" ) \
    SCOREP_SCORE_EVENT( "fopen" ) \
    SCOREP_SCORE_EVENT( "fopen64" ) \
    SCOREP_SCORE_EVENT( "freopen" )

#define SCOREP_SCORE_EVENT_IO_DELETE \
    SCOREP_SCORE_EVENT( "MPI_File_close" ) \
    SCOREP_SCORE_EVENT( "MPI_File_delete" ) \
    SCOREP_SCORE_EVENT( "unlink" ) \
    SCOREP_SCORE_EVENT( "unlinkat" )

#define SCOREP_SCORE_EVENT_IO_SEEK \
    SCOREP_SCORE_EVENT( "MPI_File_seek" ) \
    SCOREP_SCORE_EVENT( "MPI_File_seek_shared" ) \
    SCOREP_SCORE_EVENT( "lseek" ) \
    SCOREP_SCORE_EVENT( "lseek64" ) \
    SCOREP_SCORE_EVENT( "fseek" ) \
    SCOREP_SCORE_EVENT( "fseeko" ) \
    SCOREP_SCORE_EVENT( "fseeko64" ) \
    SCOREP_SCORE_EVENT( "fsetpos" ) \
    SCOREP_SCORE_EVENT( "rewind" )

#define SCOREP_SCORE_EVENT_IO_BLOCKING_TRANSFER \
    SCOREP_SCORE_EVENT( "MPI_File_read" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_at" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_at_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_ordered" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_shared" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_at" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_at_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_ordered" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_shared" ) \
    SCOREP_SCORE_EVENT( "fdatasync" ) \
    SCOREP_SCORE_EVENT( "fsync" ) \
    SCOREP_SCORE_EVENT( "pread" ) \
    SCOREP_SCORE_EVENT( "pread64" ) \
    SCOREP_SCORE_EVENT( "pwrite" ) \
    SCOREP_SCORE_EVENT( "pwrite64" ) \
    SCOREP_SCORE_EVENT( "read" ) \
    SCOREP_SCORE_EVENT( "syncfs" ) \
    SCOREP_SCORE_EVENT( "write" ) \
    SCOREP_SCORE_EVENT( "preadv" ) \
    SCOREP_SCORE_EVENT( "pwritev" ) \
    SCOREP_SCORE_EVENT( "readv" ) \
    SCOREP_SCORE_EVENT( "writev" ) \
    SCOREP_SCORE_EVENT( "lio_listio" ) \
    SCOREP_SCORE_EVENT( "fflush" ) \
    SCOREP_SCORE_EVENT( "fgetc" ) \
    SCOREP_SCORE_EVENT( "fgets" ) \
    SCOREP_SCORE_EVENT( "fputc" ) \
    SCOREP_SCORE_EVENT( "fprintf" ) \
    SCOREP_SCORE_EVENT( "fputs" ) \
    SCOREP_SCORE_EVENT( "fread" ) \
    SCOREP_SCORE_EVENT( "fscanf" ) \
    SCOREP_SCORE_EVENT( "fwrite" ) \
    SCOREP_SCORE_EVENT( "getc" ) \
    SCOREP_SCORE_EVENT( "getchar" ) \
    SCOREP_SCORE_EVENT( "gets" ) \
    SCOREP_SCORE_EVENT( "printf" ) \
    SCOREP_SCORE_EVENT( "putchar" ) \
    SCOREP_SCORE_EVENT( "puts" ) \
    SCOREP_SCORE_EVENT( "scanf" ) \
    SCOREP_SCORE_EVENT( "vfprintf" ) \
    SCOREP_SCORE_EVENT( "vfscanf" ) \
    SCOREP_SCORE_EVENT( "vscanf" ) \
    SCOREP_SCORE_EVENT( "vprintf" )

#define SCOREP_SCORE_EVENT_IO_NONBLOCKING_TRANSFER_BEGIN \
    SCOREP_SCORE_EVENT( "MPI_File_iread" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iread_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iread_at" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iread_at_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iread_shared" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iwrite" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iwrite_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iwrite_at" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iwrite_at_all" ) \
    SCOREP_SCORE_EVENT( "MPI_File_iwrite_shared" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_all_begin" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_at_all_begin" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_ordered_begin" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_all_begin" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_at_all_begin" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_ordered_begin" ) \
    SCOREP_SCORE_EVENT( "aio_read" ) \
    SCOREP_SCORE_EVENT( "aio_write" )

#define SCOREP_SCORE_EVENT_IO_NONBLOCKING_TRANSFER_END \
    SCOREP_SCORE_EVENT( "MPI_File_read_all_end" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_at_all_end" ) \
    SCOREP_SCORE_EVENT( "MPI_File_read_ordered_end" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_all_end" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_at_all_end" ) \
    SCOREP_SCORE_EVENT( "MPI_File_write_ordered_end" ) \
    SCOREP_SCORE_EVENT( "sync" ) \
    SCOREP_SCORE_EVENT( "aio_error" ) \
    SCOREP_SCORE_EVENT( "aio_return" )

#define SCOREP_SCORE_EVENT_IO_CLOSE \
    SCOREP_SCORE_EVENT( "close" ) \
    SCOREP_SCORE_EVENT( "closedir" ) \
    SCOREP_SCORE_EVENT( "fclose" )

#define SCOREP_SCORE_EVENT_IO_DUPLICATE \
    SCOREP_SCORE_EVENT( "dup" ) \
    SCOREP_SCORE_EVENT( "dup2" ) \
    SCOREP_SCORE_EVENT( "dup3" ) \
    SCOREP_SCORE_EVENT( "fcntl" )

#define SCOREP_SCORE_EVENT_IO_ACQUIRE_LOCK \
    SCOREP_SCORE_EVENT( "lockf" ) \
    SCOREP_SCORE_EVENT( "flockfile" ) \
    SCOREP_SCORE_EVENT( "ftrylockfile" )

#define SCOREP_SCORE_EVENT_IO_OPERATION_CANCELLED \
    SCOREP_SCORE_EVENT( "aio_cancel" )

#define SCOREP_SCORE_EVENT_IO_RELEASE_LOCK \
    SCOREP_SCORE_EVENT( "funlockfile" )

#define SCOREP_SCORE_EVENT_MEASUREMENT_BUFFER_FLUSH \
    SCOREP_SCORE_EVENT( "TRACE BUFFER FLUSH" )

#define SCOREP_SCORE_EVENT_MEASUREMENT_ON_OFF \
    SCOREP_SCORE_EVENT( "MEASUREMENT OFF" )

#endif // SCOREP_SCORE_EVENTLIST_HPP
