MODULE MPI_PATTERN

USE MPI_UTIL
USE DISTRIBUTION

IMPLICIT NONE

PUBLIC :: set_base_comm, late_sender, late_receiver, imbalance_at_mpi_barrier
PUBLIC :: imbalance_at_mpi_alltoall, late_broadcast, late_scatter
PUBLIC :: late_scatterv, early_reduce, early_gather, early_gatherv

INTEGER, PUBLIC :: base_type = MPI_DOUBLE_PRECISION
INTEGER, PUBLIC :: base_cnt = 1

CONTAINS
! ---------------------------------------------------------------------------

SUBROUTINE set_base_comm(type, cnt)

IMPLICIT NONE

INTEGER, INTENT(IN) :: type, cnt

base_type = type
base_cnt = cnt

END SUBROUTINE set_base_comm

! ---------------------------------------------------------------------------

SUBROUTINE late_sender(basework, extrawork, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: basework, extrawork
INTEGER, INTENT(IN) :: r, c

TYPE(val_distr_t) :: dd
INTEGER :: i
TYPE(mpi_buf_t), POINTER :: buf

CALL alloc_mpi_buf(buf, base_type, base_cnt)

dd%low = basework + extrawork
dd%high = basework

DO i = 1, r
  CALL par_do_mpi_work(df_cyclic2, dd, default_sf, c)
  CALL mpi_commpattern_sendrecv(buf, DIR_UP, 0, 1, c)
END DO

CALL free_mpi_buf(buf)


END SUBROUTINE late_sender

! ---------------------------------------------------------------------------

SUBROUTINE late_receiver(basework, extrawork, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: basework, extrawork
INTEGER, INTENT(IN) :: r, c

TYPE(val_distr_t) :: dd
INTEGER :: i
TYPE(mpi_buf_t), POINTER :: buf

CALL alloc_mpi_buf(buf, base_type, base_cnt)

dd%low = basework
dd%high = basework + extrawork

DO i = 1, r
  CALL par_do_mpi_work(df_cyclic2, dd, default_sf, c)
  CALL mpi_commpattern_sendrecv(buf, DIR_UP, 0, 0, c)
END DO

CALL free_mpi_buf(buf)

END SUBROUTINE late_receiver

! ---------------------------------------------------------------------------

SUBROUTINE imbalance_at_mpi_barrier(df, dd, r, c)

IMPLICIT NONE

TYPE(val_distr_t) :: dd
INTEGER, INTENT(IN) :: r, c
DOUBLE PRECISION, EXTERNAL :: df

INTEGER :: i, ierror

DO i = 1, r
  CALL par_do_mpi_work(df, dd, default_sf, c)
  CALL MPI_BARRIER(c, ierror)
END DO

END SUBROUTINE imbalance_at_mpi_barrier

! ---------------------------------------------------------------------------

SUBROUTINE imbalance_at_mpi_alltoall(df, dd, r, c)

IMPLICIT NONE

TYPE(val_distr_t) :: dd
INTEGER, INTENT(IN) :: r, c
DOUBLE PRECISION, EXTERNAL :: df

TYPE(mpi_buf_t), POINTER :: sbuf, rbuf
INTEGER :: i, sz, ierror

CALL MPI_COMM_SIZE(c, sz, ierror)
CALL alloc_mpi_buf(sbuf, base_type, base_cnt*sz)
CALL alloc_mpi_buf(rbuf, base_type, base_cnt*sz)

DO i = 1, r
 	CALL par_do_mpi_work(df, dd, default_sf, c)
	CALL MPI_ALLTOALL(sbuf%buf, sbuf%cnt / sz, sbuf%type,             &
           rbuf%buf, rbuf%cnt / sz, rbuf%type, c, ierror)
END DO

CALL free_mpi_buf(sbuf)
CALL free_mpi_buf(rbuf)

END SUBROUTINE imbalance_at_mpi_alltoall

! ---------------------------------------------------------------------------

SUBROUTINE late_broadcast(basework, rootextrawork, root, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: basework, rootextrawork
INTEGER, INTENT(IN) :: root, r, c

TYPE(mpi_buf_t), POINTER :: buf

TYPE(val_distr_t) :: dd
INTEGER :: i, sz, ierror

CALL alloc_mpi_buf(buf, base_type, base_cnt)
CALL MPI_COMM_SIZE(c, sz, ierror)
dd%low = basework
dd%high = basework + rootextrawork
dd%n = MOD(root, sz)

DO i = 1, r
  	CALL par_do_mpi_work(df_peak, dd, default_sf, c)
	CALL MPI_BCAST(buf%buf, buf%cnt, buf%type, dd%n, c, ierror)
END DO

CALL free_mpi_buf(buf)

END SUBROUTINE late_broadcast

! ---------------------------------------------------------------------------

SUBROUTINE late_scatter(basework, rootextrawork, root, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: basework, rootextrawork
INTEGER, INTENT(IN) :: root, r, c

TYPE(mpi_buf_t), POINTER :: sbuf, rbuf
TYPE(val_distr_t) :: dd
INTEGER :: i, sz, me, me_result, ierror

CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)
dd%low = basework
dd%high = basework + rootextrawork
dd%n = MOD(root, sz)

IF (me == dd % n) THEN
	me_result = base_cnt * sz
ELSE
	me_result = 0
END IF
CALL alloc_mpi_buf(sbuf, base_type, me_result)
CALL alloc_mpi_buf(rbuf, base_type, base_cnt)

DO i = 1, r
 	CALL par_do_mpi_work(df_peak, dd, default_sf, c)
	CALL MPI_SCATTER(sbuf%buf, sbuf%cnt / sz, sbuf%type,              &
           rbuf%buf, rbuf%cnt, rbuf%type,dd%n, c, ierror)
END DO
CALL free_mpi_buf(sbuf)
CALL free_mpi_buf(rbuf)

END SUBROUTINE late_scatter

! ---------------------------------------------------------------------------

SUBROUTINE late_scatterv(basework, rootextrawork, root, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: basework, rootextrawork
INTEGER, INTENT(IN) :: root, r, c

TYPE(val_distr_t) :: de, dd
TYPE(mpi_vbuf_t), POINTER :: vbuf
INTEGER :: i, sz, ierror

CALL MPI_COMM_SIZE(c, sz, ierror)
dd%low = basework
dd%high = basework + rootextrawork
dd%n = MOD(root, sz)
de%val = base_cnt
CALL alloc_mpi_vbuf(vbuf, base_type, df_same, de, dd%n, c)

DO i = 1, r
  	CALL par_do_mpi_work(df_peak, dd, default_sf, c)
	CALL MPI_SCATTERV(vbuf%rootbuf, vbuf%rootcnt,                     &
           vbuf%rootdispl, vbuf%type, vbuf%buf, vbuf%cnt,               &
           vbuf%type, dd%n, c, ierror)
END DO

CALL free_mpi_vbuf(vbuf)

END SUBROUTINE late_scatterv

! ---------------------------------------------------------------------------

SUBROUTINE early_reduce(rootwork, baseextrawork, root, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: rootwork, baseextrawork
INTEGER, INTENT(IN) :: root, r, c

TYPE(mpi_buf_t), POINTER :: sbuf, rbuf
INTEGER :: i, sz, me, me_result, ierror
TYPE(val_distr_t) :: dd

CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)
dd%low = rootwork + baseextrawork
dd%high = rootwork
dd%n = MOD(root, sz)

If (me == dd % n) then
	me_result = base_cnt
Else
	me_result = 0
End If
CALL alloc_mpi_buf(sbuf, base_type, base_cnt)
CALL alloc_mpi_buf(rbuf, base_type, me_result)

DO i = 1, r
  	CALL par_do_mpi_work(df_peak, dd, default_sf, c)
	CALL mpi_reduce(sbuf%buf, rbuf%buf, sbuf%cnt, sbuf%type,          &
	     MPI_MIN, dd%n, c, ierror)
END DO

CALL free_mpi_buf(sbuf)
CALL free_mpi_buf(rbuf)

END SUBROUTINE early_reduce

! ---------------------------------------------------------------------------

SUBROUTINE early_gather(rootwork, baseextrawork, root, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: rootwork, baseextrawork
INTEGER, INTENT(IN) :: root, r, c

TYPE(mpi_buf_t), POINTER :: sbuf, rbuf
INTEGER :: i, sz, me, me_result, ierror
TYPE(val_distr_t) :: dd

CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)
dd%low = rootwork + baseextrawork
dd%high = rootwork
dd%n = MOD(root, sz)

If (me == dd % n) then
	me_result = base_cnt * sz
Else
	me_result = 0
End If
CALL alloc_mpi_buf(sbuf, base_type, base_cnt)
CALL alloc_mpi_buf(rbuf, base_type, me_result)

DO i = 1, r
  	CALL par_do_mpi_work(df_peak, dd, default_sf, c)
	CALL MPI_GATHER(sbuf%buf, sbuf%cnt, sbuf%type, rbuf%buf,         &
           rbuf%cnt / sz, rbuf%type, dd%n, c, ierror)
END DO

CALL free_mpi_buf(sbuf)
CALL free_mpi_buf(rbuf)

END SUBROUTINE early_gather

! ---------------------------------------------------------------------------

SUBROUTINE early_gatherv(rootwork, baseextrawork, root, r, c)

IMPLICIT NONE

DOUBLE PRECISION, INTENT(IN) :: rootwork, baseextrawork
INTEGER, INTENT(IN) :: root, r, c

TYPE(val_distr_t) :: de, dd
TYPE(mpi_vbuf_t), POINTER :: vbuf
INTEGER :: i, sz, ierror

CALL MPI_COMM_SIZE(c, sz, ierror)
dd%low = rootwork + baseextrawork
dd%high = rootwork
dd%n = MOD(root, sz)
de%val = base_cnt
CALL alloc_mpi_vbuf(vbuf, base_type, df_same, de, dd%n, c)

DO i = 1, r
  	CALL par_do_mpi_work(df_peak, dd, default_sf, c)
	CALL MPI_GATHERV(vbuf%buf, vbuf%cnt, vbuf%type, vbuf%rootbuf,    &
           vbuf%rootcnt, vbuf%rootdispl, vbuf%type, dd%n, c, ierror)
END DO

CALL free_mpi_vbuf(vbuf)

END SUBROUTINE early_gatherv

! ---------------------------------------------------------------------------

END MODULE MPI_PATTERN


