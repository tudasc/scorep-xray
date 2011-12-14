#include <scorep/SCOREP_User.inc>

PROGRAM MPI_PATT_ALL_TEST

USE MPI_PATTERN

IMPLICIT NONE

INTEGER :: ierror,k
TYPE(val_distr_t) :: dd
SCOREP_USER_REGION_DEFINE(mainRegion)

CALL MPI_INIT(ierror)
CALL set_base_comm(MPI_INTEGER, 20000)


do k=1,1


SCOREP_USER_OA_PHASE_BEGIN(mainRegion,"mainRegion",SCOREP_USER_REGION_TYPE_COMMON)



dd = val_distr_t(0D0, 0.5D0, 1D0, 0D0, 0)
CALL imbalance_at_mpi_barrier(df_block2, dd, 1, MPI_COMM_WORLD)
CALL late_sender(0D0, 0.5D0, 1, MPI_COMM_WORLD)
CALL late_broadcast(0D0, 0.5D0, 1, 1, MPI_COMM_WORLD)
CALL late_scatter(0D0, 0.5D0, 1, 1, MPI_COMM_WORLD)
CALL imbalance_at_mpi_alltoall(df_cyclic2, dd, 1, MPI_COMM_WORLD)
CALL early_reduce(0D0, 0.5D0, 1, 1, MPI_COMM_WORLD)
CALL early_gather(0D0, 0.5D0, 1, 1, MPI_COMM_WORLD)
CALL late_scatterv(0.2D0, 0.7D0, 3, 3, MPI_COMM_WORLD)
CALL early_gatherv(0.3D0, 0.2D0, 6, 7, MPI_COMM_WORLD)


SCOREP_USER_OA_PHASE_END(mainRegion)

enddo
CALL MPI_FINALIZE(ierror)

END PROGRAM MPI_PATT_ALL_TEST
