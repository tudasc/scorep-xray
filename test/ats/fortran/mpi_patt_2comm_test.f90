PROGRAM MPI_PATT_2COMM_TEST

USE MPI_PATTERN

IMPLICIT NONE

INTEGER :: id, numprocs, lsize, ierror
INTEGER :: color, local
TYPE(val_distr_t) :: dd = val_distr_t(0D0, 0.5D0, 1D0, 0D0, 0)

CALL MPI_INIT(ierror)
CALL MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierror)
CALL MPI_COMM_RANK(MPI_COMM_WORLD, id, ierror)
CALL set_base_comm(MPI_INTEGER, 20000)

IF (id .GE. (numprocs/2)) THEN
	color = 1
ELSE
	color = 0
END IF
CALL MPI_COMM_SPLIT(MPI_COMM_WORLD, color, id, local, ierror)
CALL MPI_COMM_SIZE(local, lsize, ierror)

CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)
CALL do_work(0.2D0)
IF (color .EQ. 1) THEN
	CALL late_sender(0.1D0, 0.2D0, 20, local)
	CALL imbalance_at_mpi_barrier(df_block2, dd, 4, local)
	CALL late_broadcast(0.2D0, 0.3D0, 1, 13, local)
	CALL early_reduce(0.1D0, 0.5D0, 4, 9, local)
ELSE
	CALL late_receiver(0.1D0, 0.2D0, 20, local)
	CALL imbalance_at_mpi_alltoall(df_cyclic2, dd, 7, local)
	!CALL late_scatter(0.4D0, 0.1D0, 2, 5, local)
	CALL early_gather(0.2D0, 0.3D0, 5, 4, local)
END IF
CALL do_work(0.2D0)
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

CALL MPI_COMM_FREE(local, ierror)
CALL MPI_FINALIZE(ierror)

END PROGRAM MPI_PATT_2COMM_TEST







