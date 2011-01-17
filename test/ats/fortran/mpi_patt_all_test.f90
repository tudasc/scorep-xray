PROGRAM MPI_PATT_ALL_TEST

USE MPI_PATTERN

IMPLICIT NONE

INTEGER :: ierror,k
TYPE(val_distr_t) :: dd

CALL MPI_INIT(ierror)
CALL set_base_comm(MPI_INTEGER, 20000)


do k=1,10
!$MON USER REGION
dd = val_distr_t(0D0, 0.5D0, 1D0, 0D0, 0)
CALL imbalance_at_mpi_barrier(df_block2, dd, 2, MPI_COMM_WORLD)
!CALL late_sender(0.1D0, 0.2D0, 14, MPI_COMM_WORLD)
!CALL late_broadcast(0.2D0, 0.3D0, 1, 13, MPI_COMM_WORLD)
!CALL late_receiver(0.1D0, 0.2D0, 15, MPI_COMM_WORLD)
!CALL late_scatter(0.4D0, 0.1D0, 2, 5, MPI_COMM_WORLD)
!CALL late_scatterv(0.2D0, 0.7D0, 3, 3, MPI_COMM_WORLD)
!CALL imbalance_at_mpi_alltoall(df_cyclic2, dd, 7, MPI_COMM_WORLD)
!CALL early_reduce(0.1D0, 0.5D0, 4, 9, MPI_COMM_WORLD)
!CALL early_gather(0.2D0, 0.3D0, 5, 4, MPI_COMM_WORLD)
!CALL early_gatherv(0.3D0, 0.2D0, 6, 7, MPI_COMM_WORLD)
!$MON END USER REGION
enddo
CALL MPI_FINALIZE(ierror)

END PROGRAM MPI_PATT_ALL_TEST
