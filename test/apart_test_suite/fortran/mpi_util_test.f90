PROGRAM MPI_PATTERN_TEST

USE MPI_PATTERN

IMPLICIT NONE

INTEGER :: ierror, sz
TYPE(val_distr_t) :: dl
TYPE(mpi_buf_t), POINTER :: sbuf, rbuf
TYPE(mpi_vbuf_t), POINTER :: vbuf

CALL MPI_INIT(ierror)
CALL MPI_COMM_SIZE(MPI_COMM_WORLD, sz, ierror)
dl%low = 1
dl%high = sz

CALL alloc_mpi_buf(sbuf, MPI_DOUBLE_PRECISION, 117)
CALL alloc_mpi_buf(rbuf, MPI_DOUBLE_PRECISION, 117)
CALL alloc_mpi_vbuf(vbuf, MPI_INTEGER, df_linear, dl, 0, MPI_COMM_WORLD)
CALL MPI_GATHERV(vbuf%buf, vbuf%cnt, vbuf%type, vbuf%rootbuf,           &
	vbuf%rootcnt, vbuf%rootdispl, vbuf%type, 0, MPI_COMM_WORLD, ierror)

CALL mpi_commpattern_sendrecv(sbuf, DIR_UP, 0, 0, MPI_COMM_WORLD);
CALL mpi_commpattern_sendrecv(sbuf, DIR_DOWN, 0, 0, MPI_COMM_WORLD);
CALL mpi_commpattern_sendrecv(sbuf, DIR_DOWN, 1, 0, MPI_COMM_WORLD);
CALL mpi_commpattern_sendrecv(sbuf, DIR_DOWN, 0, 1, MPI_COMM_WORLD);
CALL mpi_commpattern_sendrecv(sbuf, DIR_DOWN, 1, 1, MPI_COMM_WORLD);

CALL mpi_commpattern_shift(sbuf, rbuf, DIR_UP, 0, 0, MPI_COMM_WORLD);
CALL mpi_commpattern_shift(sbuf, rbuf, DIR_DOWN, 0, 0, MPI_COMM_WORLD);
CALL mpi_commpattern_shift(sbuf, rbuf, DIR_DOWN, 1, 0, MPI_COMM_WORLD);
CALL mpi_commpattern_shift(sbuf, rbuf, DIR_DOWN, 0, 1, MPI_COMM_WORLD);
CALL mpi_commpattern_shift(sbuf, rbuf, DIR_DOWN, 1, 1, MPI_COMM_WORLD);

CALL free_mpi_buf(sbuf)
CALL free_mpi_buf(rbuf)
CALL free_mpi_vbuf(vbuf)
CALL MPI_FINALIZE(ierror)

END PROGRAM MPI_PATTERN_TEST
