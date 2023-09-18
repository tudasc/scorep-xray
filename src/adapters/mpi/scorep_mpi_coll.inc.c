#if !defined( TYPE_SIZE_FUN ) || !defined( COUNT_T ) || !defined( COUNT_FUN )
#error "This file should only be included by scorep_mpi_coll.c"
#endif

/* -----------------------------------------------------------------------------
 * Barrier
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_barrier )( MPI_Comm  comm,
                                            uint64_t* sendbytes,
                                            uint64_t* recvbytes )
{
    *sendbytes = 0;
    *recvbytes = 0;
}


/* -----------------------------------------------------------------------------
 * All-to-one
 * -------------------------------------------------------------------------- */
static inline void
COUNT_FUN( coll_bytes_all_to_one )( COUNT_T      sendcount,
                                    MPI_Datatype sendtype,
                                    COUNT_T      recvcount,
                                    MPI_Datatype recvtype,
                                    int          root,
                                    int          inplace,
                                    MPI_Comm     comm,
                                    uint64_t*    sendbytes,
                                    uint64_t*    recvbytes )
{
    if ( is_intracomm( comm ) )
    {
        int me;
        PMPI_Comm_rank( comm, &me );
        if ( me == root )
        {
            int num_ranks;
            PMPI_Comm_size( comm, &num_ranks );

            /*
             * At the root, recvcount, recvtype are always significant.
             * sendcount, sendtype are ignored if the in-place option is given
             */
            COUNT_T recvsize;
            TYPE_SIZE_FUN( recvtype, &recvsize );
            /*
             * We rely on the equality of type signatures, i.e.:
             * (recvcount * recvsize) = (sendcount * sendsize)
             */
            const uint64_t blockbytes = recvcount * recvsize;

            if ( inplace )
            {
                *sendbytes = 0;
                *recvbytes = ( num_ranks - 1 ) * blockbytes;
            }
            else
            {
                *sendbytes = blockbytes;
                *recvbytes = num_ranks * blockbytes;
            }
        }
        else
        {
            /*
             * At the other ranks, sendcount, sendsize are always significant
             */
            COUNT_T sendsize;
            TYPE_SIZE_FUN( sendtype, &sendsize );
            *sendbytes = sendcount * sendsize;
            *recvbytes = 0;
        }
    }
    else /* Intercomm */
    {
        if ( root == MPI_ROOT )
        {
            int num_remote_ranks;
            PMPI_Comm_remote_size( comm, &num_remote_ranks );

            COUNT_T recvsize;
            TYPE_SIZE_FUN( recvtype, &recvsize );

            *sendbytes = 0;
            *recvbytes = num_remote_ranks * recvcount * recvsize;
        }
        else if ( root != MPI_PROC_NULL )
        {
            COUNT_T sendsize;
            TYPE_SIZE_FUN( sendtype, &sendsize );

            *sendbytes = 1 * sendcount * sendsize;
            *recvbytes = 0;
        }
        else
        {
            *sendbytes = 0;
            *recvbytes = 0;
        }
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_gather )( COUNT_T      sendcount,
                                           MPI_Datatype sendtype,
                                           COUNT_T      recvcount,
                                           MPI_Datatype recvtype,
                                           int          root,
                                           int          inplace,
                                           MPI_Comm     comm,
                                           uint64_t*    sendbytes,
                                           uint64_t*    recvbytes )
{
    COUNT_FUN( coll_bytes_all_to_one )( sendcount,
                                        sendtype,
                                        recvcount,
                                        recvtype,
                                        root,
                                        inplace,
                                        comm,
                                        sendbytes,
                                        recvbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_reduce )( COUNT_T      count,
                                           MPI_Datatype datatype,
                                           int          root,
                                           int          inplace,
                                           MPI_Comm     comm,
                                           uint64_t*    sendbytes,
                                           uint64_t*    recvbytes )
{
    /*
     * Reduce takes the same (count, datatype) for both send and receive
     */
    COUNT_FUN( coll_bytes_all_to_one )( count,
                                        datatype,
                                        count,
                                        datatype,
                                        root,
                                        inplace,
                                        comm,
                                        sendbytes,
                                        recvbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_gatherv )( COUNT_T        sendcount,
                                            MPI_Datatype   sendtype,
                                            const COUNT_T* recvcounts,
                                            MPI_Datatype   recvtype,
                                            int            root,
                                            int            inplace,
                                            MPI_Comm       comm,
                                            uint64_t*      sendbytes,
                                            uint64_t*      recvbytes )
{
    if ( is_intracomm( comm ) )
    {
        int me;
        PMPI_Comm_rank( comm, &me );

        if ( me == root )
        {
            int num_ranks;
            PMPI_Comm_size( comm, &num_ranks );
            /*
             * At the root, recvcounts, recvtype are always significant.
             * sendcount, sendtype are ignored if the in-place option is given
             */
            COUNT_T recvsize;
            TYPE_SIZE_FUN( recvtype, &recvsize );

            *recvbytes = 0;
            for ( int rank = 0; rank < num_ranks; ++rank )
            {
                *recvbytes += recvcounts[ rank ] * recvsize;
            }

            if ( inplace )
            {
                *sendbytes  = 0;
                *recvbytes -= recvcounts[ me ] * recvsize;
            }
            else
            {
                /*
                 * We rely on the equality of type signatures, i.e.:
                 * (recvcounts[me] * recvsize) = (sendcount * sendsize)
                 */
                *sendbytes = recvcounts[ me ] * recvsize;
                /* recvbytes already computed above */
            }
        }
        else
        {
            /*
             * At the other ranks, sendcount, sendsize are always significant
             */
            COUNT_T sendsize;
            TYPE_SIZE_FUN( sendtype, &sendsize );
            *sendbytes = sendcount * sendsize;
            *recvbytes = 0;
        }
    }
    else /* Intercomm */
    {
        if ( root == MPI_ROOT )
        {
            int num_remote_ranks;
            PMPI_Comm_remote_size( comm, &num_remote_ranks );

            COUNT_T recvsize;
            TYPE_SIZE_FUN( recvtype, &recvsize );

            *sendbytes = 0;
            *recvbytes = 0;
            for ( int rank = 0; rank < num_remote_ranks; ++rank )
            {
                *recvbytes += recvcounts[ rank ] * recvsize;
            }
        }
        else if ( root != MPI_PROC_NULL )
        {
            COUNT_T sendsize;
            TYPE_SIZE_FUN( sendtype, &sendsize );

            *sendbytes = 1 * sendcount * sendsize;
            *recvbytes = 0;
        }
        else
        {
            *sendbytes = 0;
            *recvbytes = 0;
        }
    }
}


/* -----------------------------------------------------------------------------
 * One-to-all
 * -------------------------------------------------------------------------- */
static inline void
COUNT_FUN( scorep_mpi_coll_bytes_one_to_all )( COUNT_T      sendcount,
                                               MPI_Datatype sendtype,
                                               COUNT_T      recvcount,
                                               MPI_Datatype recvtype,
                                               int          root,
                                               int          inplace,
                                               MPI_Comm     comm,
                                               uint64_t*    sendbytes,
                                               uint64_t*    recvbytes )
{
    /*
     * Same as All-to-one with the meaning of send and receive exchanged
     */
    COUNT_FUN( coll_bytes_all_to_one )( recvcount,
                                        recvtype,
                                        sendcount,
                                        sendtype,
                                        root,
                                        inplace,
                                        comm,
                                        recvbytes,
                                        sendbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_bcast )(   COUNT_T      count,
                                            MPI_Datatype datatype,
                                            int          root,
                                            MPI_Comm     comm,
                                            uint64_t*    sendbytes,
                                            uint64_t*    recvbytes )
{
    /*
     * Bcast takes the same (count, datatype) for both send and receive
     * Also, the inplace option does not make sense here
     */
    COUNT_FUN( scorep_mpi_coll_bytes_one_to_all )( count,
                                                   datatype,
                                                   count,
                                                   datatype,
                                                   root,
                                                   0,
                                                   comm,
                                                   sendbytes,
                                                   recvbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_scatter )( COUNT_T      sendcount,
                                            MPI_Datatype sendtype,
                                            COUNT_T      recvcount,
                                            MPI_Datatype recvtype,
                                            int          root,
                                            int          inplace,
                                            MPI_Comm     comm,
                                            uint64_t*    sendbytes,
                                            uint64_t*    recvbytes )
{
    COUNT_FUN( scorep_mpi_coll_bytes_one_to_all )( sendcount,
                                                   sendtype,
                                                   recvcount,
                                                   recvtype,
                                                   root,
                                                   inplace,
                                                   comm,
                                                   sendbytes,
                                                   recvbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_scatterv )( const COUNT_T* sendcounts,
                                             MPI_Datatype   sendtype,
                                             COUNT_T        recvcount,
                                             MPI_Datatype   recvtype,
                                             int            root,
                                             int            inplace,
                                             MPI_Comm       comm,
                                             uint64_t*      sendbytes,
                                             uint64_t*      recvbytes )
{
    /*
     * Same as gatherv with the meaning of send and receive exchanged
     */
    COUNT_FUN( scorep_mpi_coll_bytes_gatherv )( recvcount,
                                                recvtype,
                                                sendcounts,
                                                sendtype,
                                                root,
                                                inplace,
                                                comm,
                                                recvbytes,
                                                sendbytes );
}


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
                                             uint64_t*    recvbytes )
{
    int num_ranks;
    if ( is_intracomm( comm ) )
    {
        PMPI_Comm_size( comm, &num_ranks );
    }
    else /* Intercomm */
    {
        PMPI_Comm_remote_size( comm, &num_ranks );
    }

    /*
     * recvcounts, recvtype are always significant.
     * sendcount, sendtype are ignored if the in-place option is given
     */
    COUNT_T recvsize;
    TYPE_SIZE_FUN( recvtype, &recvsize );

    if ( inplace )
    {
        *sendbytes = ( num_ranks - 1 ) * recvcount * recvsize;
        *recvbytes = *sendbytes;
    }
    else /* either Intracomm without inplace or Intercomm */
    {
        COUNT_T sendsize;
        TYPE_SIZE_FUN( sendtype, &sendsize );

        *sendbytes = num_ranks * sendcount * sendsize;
        *recvbytes = num_ranks * recvcount * recvsize;
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_alltoallv )( const COUNT_T* sendcounts,
                                              MPI_Datatype   sendtype,
                                              const COUNT_T* recvcounts,
                                              MPI_Datatype   recvtype,
                                              int            inplace,
                                              MPI_Comm       comm,
                                              uint64_t*      sendbytes,
                                              uint64_t*      recvbytes )
{
    int num_ranks;
    if ( is_intracomm( comm ) )
    {
        PMPI_Comm_size( comm, &num_ranks );
    }
    else /* Intercomm */
    {
        PMPI_Comm_remote_size( comm, &num_ranks );
    }

    /*
     * recvcounts, recvtype are always significant.
     * sendcount, sendtype are ignored if the in-place option is given
     */
    COUNT_T recvsize;
    TYPE_SIZE_FUN( recvtype, &recvsize );

    *sendbytes = 0;
    *recvbytes = 0;

    if ( inplace )
    {
        int me;
        PMPI_Comm_rank( comm, &me );

        for ( int rank = 0; rank < num_ranks; ++rank )
        {
            *recvbytes += recvcounts[ rank ] * recvsize;
        }
        *recvbytes -= recvcounts[ me ] * recvsize;
        /*
         * The same amount of data is exchanged between any two processes.
         * Therefore, the total amount sent is equal to the total amount received
         * at each process.
         */
        *sendbytes = *recvbytes;
    }
    else /* either Intracomm without inplace or Intercomm */
    {
        COUNT_T sendsize;
        TYPE_SIZE_FUN( sendtype, &sendsize );

        for ( int rank = 0; rank < num_ranks; ++rank )
        {
            *sendbytes += sendcounts[ rank ] * sendsize;
            *recvbytes += recvcounts[ rank ] * recvsize;
        }
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_alltoallw )( const COUNT_T*      sendcounts,
                                              const MPI_Datatype* sendtypes,
                                              const COUNT_T*      recvcounts,
                                              const MPI_Datatype* recvtypes,
                                              int                 inplace,
                                              MPI_Comm            comm,
                                              uint64_t*           sendbytes,
                                              uint64_t*           recvbytes )
{
    int num_ranks;
    if ( is_intracomm( comm ) )
    {
        PMPI_Comm_size( comm, &num_ranks );
    }
    else /* Intercomm */
    {
        PMPI_Comm_remote_size( comm, &num_ranks );
    }

    *sendbytes = 0;
    *recvbytes = 0;

    if ( inplace )
    {
        int me;
        PMPI_Comm_rank( comm, &me );

        COUNT_T recvsize;
        for ( int rank = 0; rank < num_ranks; ++rank )
        {
            TYPE_SIZE_FUN( recvtypes[ rank ], &recvsize );
            *recvbytes += recvcounts[ rank ] * recvsize;
        }
        TYPE_SIZE_FUN( recvtypes[ me ], &recvsize );
        *recvbytes -= recvcounts[ me ] * recvsize;
        /*
         * The same amount of data is exchanged between any two processes.
         * Therefore, the total amount sent is equal to the total amount received
         * at each process.
         */
        *sendbytes = *recvbytes;
    }
    else /* either Intracomm without inplace or Intercomm */
    {
        COUNT_T sendsize, recvsize;
        for ( int rank = 0; rank < num_ranks; ++rank )
        {
            TYPE_SIZE_FUN( sendtypes[ rank ], &sendsize );
            TYPE_SIZE_FUN( recvtypes[ rank ], &recvsize );
            *sendbytes += sendcounts[ rank ] * sendsize;
            *recvbytes += recvcounts[ rank ] * recvsize;
        }
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_allgather )( COUNT_T      sendcount,
                                              MPI_Datatype sendtype,
                                              COUNT_T      recvcount,
                                              MPI_Datatype recvtype,
                                              int          inplace,
                                              MPI_Comm     comm,
                                              uint64_t*    sendbytes,
                                              uint64_t*    recvbytes )
{
    COUNT_FUN( scorep_mpi_coll_bytes_alltoall )( sendcount,
                                                 sendtype,
                                                 recvcount,
                                                 recvtype,
                                                 inplace,
                                                 comm,
                                                 sendbytes,
                                                 recvbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_allgatherv )( COUNT_T        sendcount,
                                               MPI_Datatype   sendtype,
                                               const COUNT_T* recvcounts,
                                               MPI_Datatype   recvtype,
                                               int            inplace,
                                               MPI_Comm       comm,
                                               uint64_t*      sendbytes,
                                               uint64_t*      recvbytes )
{
    int num_ranks;
    if ( is_intracomm( comm ) )
    {
        PMPI_Comm_size( comm, &num_ranks );
    }
    else /* Intercomm */
    {
        PMPI_Comm_remote_size( comm, &num_ranks );
    }

    /*
     * recvcounts, recvtype are always significant.
     * sendcount, sendtype are ignored if the in-place option is given
     */
    COUNT_T recvsize;
    TYPE_SIZE_FUN( recvtype, &recvsize );

    *sendbytes = 0;
    *recvbytes = 0;

    if ( inplace )
    {
        int me;
        PMPI_Comm_rank( comm, &me );

        for ( int rank = 0; rank < num_ranks; ++rank )
        {
            *recvbytes += recvcounts[ rank ] * recvsize;
        }
        *recvbytes -= recvcounts[ me ] * recvsize;

        /*
         * We rely on the equality of type signatures, i.e.:
         * (recvcounts[me] * recvsize) = (sendcount * sendsize)
         */
        *sendbytes = ( num_ranks - 1 ) * ( recvcounts[ me ] * recvsize );
    }
    else /* either Intracomm without inplace or Intercomm */
    {
        COUNT_T sendsize;
        TYPE_SIZE_FUN( sendtype, &sendsize );

        *sendbytes = num_ranks * ( sendcount * sendsize );
        for ( int rank = 0; rank < num_ranks; ++rank )
        {
            *recvbytes += recvcounts[ rank ] * recvsize;
        }
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_allreduce )( COUNT_T      count,
                                              MPI_Datatype datatype,
                                              int          inplace,
                                              MPI_Comm     comm,
                                              uint64_t*    sendbytes,
                                              uint64_t*    recvbytes )
{
    COUNT_FUN( scorep_mpi_coll_bytes_alltoall )( count,
                                                 datatype,
                                                 count,
                                                 datatype,
                                                 inplace,
                                                 comm,
                                                 sendbytes,
                                                 recvbytes );
}

void
COUNT_FUN( scorep_mpi_coll_bytes_reduce_scatter_block )( COUNT_T      recvcount,
                                                         MPI_Datatype datatype,
                                                         int          inplace,
                                                         MPI_Comm     comm,
                                                         uint64_t*    sendbytes,
                                                         uint64_t*    recvbytes )
{
    int num_local_ranks;
    PMPI_Comm_size( comm, &num_local_ranks );

    COUNT_T typesize;
    TYPE_SIZE_FUN( datatype, &typesize );

    if ( inplace )
    {
        *sendbytes = ( num_local_ranks - 1 ) * recvcount * typesize;
        *recvbytes = *sendbytes;
    }
    else /* either Intracomm without inplace or Intercomm */
    {
        *sendbytes = num_local_ranks * recvcount * typesize;
        *recvbytes = *sendbytes;
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_reduce_scatter )( const COUNT_T* recvcounts,
                                                   MPI_Datatype   datatype,
                                                   int            inplace,
                                                   MPI_Comm       comm,
                                                   uint64_t*      sendbytes,
                                                   uint64_t*      recvbytes )
{
    int num_local_ranks;
    PMPI_Comm_size( comm, &num_local_ranks );

    COUNT_T typesize;
    TYPE_SIZE_FUN( datatype, &typesize );

    int me;
    PMPI_Comm_rank( comm, &me );

    uint64_t sendcount = 0;
    for ( int rank = 0; rank < num_local_ranks; ++rank )
    {
        sendcount += recvcounts[ rank ];
    }

    if ( is_intracomm( comm ) )
    {
        if ( inplace )
        {
            sendcount -= recvcounts[ me ];
            *sendbytes = sendcount * typesize;
            *recvbytes = ( num_local_ranks - 1 ) * recvcounts[ me ] * typesize;
        }
        else
        {
            *sendbytes = sendcount * typesize;
            *recvbytes = num_local_ranks * recvcounts[ me ] * typesize;
        }
    }
    else /* Intercomm */
    {
        int num_remote_ranks;
        PMPI_Comm_remote_size( comm, &num_remote_ranks );

        *sendbytes = sendcount * typesize;
        *recvbytes = num_remote_ranks * recvcounts[ me ] * typesize;
    }
}



/* -----------------------------------------------------------------------------
 * Scan
 * -------------------------------------------------------------------------- */
void
COUNT_FUN( scorep_mpi_coll_bytes_scan )( COUNT_T      count,
                                         MPI_Datatype datatype,
                                         int          inplace,
                                         MPI_Comm     comm,
                                         uint64_t*    sendbytes,
                                         uint64_t*    recvbytes )
{
    int me;
    PMPI_Comm_rank( comm, &me );

    int num_ranks;
    PMPI_Comm_size( comm, &num_ranks );

    COUNT_T size;
    TYPE_SIZE_FUN( datatype, &size );
    const uint64_t blockbytes = count * size;

    if ( inplace )
    {
        *sendbytes = ( num_ranks - me - 1 ) * blockbytes;
        *recvbytes = me * blockbytes;
    }
    else
    {
        *sendbytes = ( num_ranks - me ) * blockbytes;
        *recvbytes = ( me + 1 ) * blockbytes;
    }
}

void
COUNT_FUN( scorep_mpi_coll_bytes_exscan )( COUNT_T      count,
                                           MPI_Datatype datatype,
                                           int          inplace,
                                           MPI_Comm     comm,
                                           uint64_t*    sendbytes,
                                           uint64_t*    recvbytes )
{
    /*
     * Exscan never sends data to itself,
     * the byte calculation is the same as for inclusive scan with inplace option
     */
    COUNT_FUN( scorep_mpi_coll_bytes_scan )( count,
                                             datatype,
                                             1,
                                             comm,
                                             sendbytes,
                                             recvbytes );
}

#undef TYPE_SIZE_FUN
#undef COUNT_T
#undef COUNT_FUN
