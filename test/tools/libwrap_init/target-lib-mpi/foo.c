#include "foo.h"

#include <assert.h>

#include <mpi.h>

void
foo_mpi_broadcast_test( int* foo, int root )
{
    int err;
    err = MPI_Bcast( foo, 1, MPI_INT, root, MPI_COMM_WORLD );
    assert( err == MPI_SUCCESS );
}
