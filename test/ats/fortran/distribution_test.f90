PROGRAM DISTRIBUTION_TEST

USE DISTRIBUTION

IMPLICIT NONE
INCLUDE 'mpif.h'

INTEGER :: sz, me, ierror
DOUBLE PRECISION :: scale, temp
TYPE (val_distr_t) :: dd

CALL MPI_INIT(ierror)

CALL MPI_COMM_RANK(MPI_COMM_WORLD, me, ierror)
CALL MPI_COMM_SIZE(MPI_COMM_WORLD, sz, ierror)

scale=2.1D0
!allocate (dd)
dd=val_distr_t(3D0, 1D0, 5D0, 3D0, 2)

! test df_same
temp=df_same(me, sz, scale, dd)
print *, "myrank", me, "df_same:", temp
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

! test df_cyclic2
temp=df_cyclic2(me, sz, scale, dd)
print *, "myrank", me, "df_cyclic2:", temp
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

! test df_block2
temp=df_block2(me, sz, scale, dd)
print *, "myrank", me, "df_block2:", temp
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

! test df_linear
temp=df_linear(me, sz, scale, dd)
print *, "myrank", me, "df_linear:", temp
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

! test df_peak
temp=df_peak(me, sz, scale, dd)
print *, "myrank", me, "df_peak:", temp
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

! test df_cyclic3
temp=df_cyclic3(me, sz, scale, dd)
print *, "myrank", me, "df_cyclic3:", temp
CALL MPI_BARRIER(MPI_COMM_WORLD, ierror)

! test df_block3
temp=df_block3(me, sz, scale, dd)
print *, "myrank", me, "df_block3:", temp


CALL MPI_FINALIZE(ierror)

END PROGRAM DISTRIBUTION_TEST











