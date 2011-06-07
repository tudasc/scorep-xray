#include <config.h>
#include "mpi_pattern.h"

int
main( int   argc,
      char* argv[] )
{
    val2_distr_t dl;
    int          sz;
    mpi_buf_t*   sbuf, * rbuf;
    mpi_vbuf_t*  vbuf;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &sz );
    dl.low  = 1;
    dl.high = sz;

    sbuf = alloc_mpi_buf( MPI_DOUBLE, 117 );
    rbuf = alloc_mpi_buf( MPI_DOUBLE, 117 );
    vbuf = alloc_mpi_vbuf( MPI_INT, df_linear, &dl, 0, MPI_COMM_WORLD );
    MPI_Gatherv( vbuf->buf, vbuf->cnt, vbuf->type,
                 vbuf->rootbuf, vbuf->rootcnt, vbuf->rootdispl, vbuf->type,
                 0, MPI_COMM_WORLD );

    mpi_commpattern_sendrecv( sbuf, DIR_UP, 0, 0, MPI_COMM_WORLD );
    mpi_commpattern_sendrecv( sbuf, DIR_DOWN, 0, 0, MPI_COMM_WORLD );
    mpi_commpattern_sendrecv( sbuf, DIR_DOWN, 1, 0, MPI_COMM_WORLD );
    mpi_commpattern_sendrecv( sbuf, DIR_DOWN, 0, 1, MPI_COMM_WORLD );
    mpi_commpattern_sendrecv( sbuf, DIR_DOWN, 1, 1, MPI_COMM_WORLD );

    mpi_commpattern_shift( sbuf, rbuf, DIR_UP, 0, 0, MPI_COMM_WORLD );
    mpi_commpattern_shift( sbuf, rbuf, DIR_DOWN, 0, 0, MPI_COMM_WORLD );
    mpi_commpattern_shift( sbuf, rbuf, DIR_DOWN, 1, 0, MPI_COMM_WORLD );
    mpi_commpattern_shift( sbuf, rbuf, DIR_DOWN, 0, 1, MPI_COMM_WORLD );
    mpi_commpattern_shift( sbuf, rbuf, DIR_DOWN, 1, 1, MPI_COMM_WORLD );

    free_mpi_buf( sbuf );
    free_mpi_buf( rbuf );
    free_mpi_vbuf( vbuf );
    MPI_Finalize();
    return 0;
}
