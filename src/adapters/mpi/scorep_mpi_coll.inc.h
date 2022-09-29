/*
 * To be included from scorep_mpi_coll.h, which defines the following macros
 *
 * This file is included twice with differing defines to generate both the normal
 * and the large count interfaces.
 */
#if !defined( TYPE_SIZE_FUN ) || !defined( COUNT_T ) || !defined( COUNT_FUN )
#error "This file should only be included by scorep_mpi_coll.h"
#endif


/* -----------------------------------------------------------------------------
 * Barrier
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_barrier )( MPI_Comm  comm,
                                            uint64_t* sendbytes,
                                            uint64_t* recvbytes );


/* -----------------------------------------------------------------------------
 * All-to-one
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_gather )( COUNT_T      sendcount,
                                           MPI_Datatype sendtype,
                                           COUNT_T      recvcount,
                                           MPI_Datatype recvtype,
                                           int          root,
                                           int          inplace,
                                           MPI_Comm     comm,
                                           uint64_t*    sendbytes,
                                           uint64_t*    recvbytes );


void
COUNT_FUN( scorep_mpi_coll_bytes_reduce )( COUNT_T      count,
                                           MPI_Datatype datatype,
                                           int          root,
                                           int          inplace,
                                           MPI_Comm     comm,
                                           uint64_t*    sendbytes,
                                           uint64_t*    recvbytes );


void
COUNT_FUN( scorep_mpi_coll_bytes_gatherv )(  COUNT_T        sendcount,
                                             MPI_Datatype   sendtype,
                                             const COUNT_T* recvcounts,
                                             MPI_Datatype   recvtype,
                                             int            root,
                                             int            inplace,
                                             MPI_Comm       comm,
                                             uint64_t*      sendbytes,
                                             uint64_t*      recvbytes );


/* -----------------------------------------------------------------------------
 * One-to-all
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_bcast )(   COUNT_T      count,
                                            MPI_Datatype datatype,
                                            int          root,
                                            MPI_Comm     comm,
                                            uint64_t*    sendbytes,
                                            uint64_t*    recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_scatter )( COUNT_T      sendcount,
                                            MPI_Datatype sendtype,
                                            COUNT_T      recvcount,
                                            MPI_Datatype recvtype,
                                            int          root,
                                            int          inplace,
                                            MPI_Comm     comm,
                                            uint64_t*    sendbytes,
                                            uint64_t*    recvbytes );


void
COUNT_FUN( scorep_mpi_coll_bytes_scatterv )( const COUNT_T* sendcounts,
                                             MPI_Datatype   sendtype,
                                             COUNT_T        recvcount,
                                             MPI_Datatype   recvtype,
                                             int            root,
                                             int            inplace,
                                             MPI_Comm       comm,
                                             uint64_t*      sendbytes,
                                             uint64_t*      recvbytes );

/* -----------------------------------------------------------------------------
 * All-to-all
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_alltoall )( COUNT_T      sendcount,
                                             MPI_Datatype sendtype,
                                             COUNT_T      recvcount,
                                             MPI_Datatype recvtype,
                                             int          inplace,
                                             MPI_Comm     comm,
                                             uint64_t*    sendbytes,
                                             uint64_t*    recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_alltoallv )( const COUNT_T* sendcounts,
                                              MPI_Datatype   sendtype,
                                              const COUNT_T* recvcounts,
                                              MPI_Datatype   recvtype,
                                              int            inplace,
                                              MPI_Comm       comm,
                                              uint64_t*      sendbytes,
                                              uint64_t*      recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_alltoallw )( const COUNT_T*      sendcounts,
                                              const MPI_Datatype* sendtypes,
                                              const COUNT_T*      recvcounts,
                                              const MPI_Datatype* recvtypes,
                                              int                 inplace,
                                              MPI_Comm            comm,
                                              uint64_t*           sendbytes,
                                              uint64_t*           recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_allgather )( COUNT_T      sendcount,
                                              MPI_Datatype sendtype,
                                              COUNT_T      recvcount,
                                              MPI_Datatype recvtype,
                                              int          inplace,
                                              MPI_Comm     comm,
                                              uint64_t*    sendbytes,
                                              uint64_t*    recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_allgatherv )( COUNT_T        sendcount,
                                               MPI_Datatype   sendtype,
                                               const COUNT_T* recvcounts,
                                               MPI_Datatype   recvtype,
                                               int            inplace,
                                               MPI_Comm       comm,
                                               uint64_t*      sendbytes,
                                               uint64_t*      recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_allreduce )( COUNT_T      count,
                                              MPI_Datatype datatype,
                                              int          inplace,
                                              MPI_Comm     comm,
                                              uint64_t*    sendbytes,
                                              uint64_t*    recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_reduce_scatter_block )( COUNT_T      recvcount,
                                                         MPI_Datatype datatype,
                                                         int          inplace,
                                                         MPI_Comm     comm,
                                                         uint64_t*    sendbytes,
                                                         uint64_t*    recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_reduce_scatter )( const COUNT_T* recvcounts,
                                                   MPI_Datatype   datatype,
                                                   int            inplace,
                                                   MPI_Comm       comm,
                                                   uint64_t*      sendbytes,
                                                   uint64_t*      recvbytes );


/* -----------------------------------------------------------------------------
 * Scan
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_scan )( COUNT_T      count,
                                         MPI_Datatype datatype,
                                         int          inplace,
                                         MPI_Comm     comm,
                                         uint64_t*    sendbytes,
                                         uint64_t*    recvbytes );

void
COUNT_FUN( scorep_mpi_coll_bytes_exscan )( COUNT_T      count,
                                           MPI_Datatype datatype,
                                           int          inplace,
                                           MPI_Comm     comm,
                                           uint64_t*    sendbytes,
                                           uint64_t*    recvbytes );

#undef TYPE_SIZE_FUN
#undef COUNT_T
#undef COUNT_FUN
