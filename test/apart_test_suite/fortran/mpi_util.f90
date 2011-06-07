MODULE MPI_UTIL

USE DISTRIBUTION
USE WORK

IMPLICIT NONE
INCLUDE 'mpif.h'

PUBLIC :: par_do_mpi_work, alloc_mpi_buf, free_mpi_buf, alloc_mpi_vbuf
PUBLIC :: free_mpi_vbuf, mpi_commpattern_sendrecv, mpi_commpattern_shift
PUBLIC :: mpi_buf_t, mpi_vbuf_t

INTEGER, PUBLIC, PARAMETER :: P_SR_TAG = 42, P_SFT_TAG = 43
INTEGER, PUBLIC, PARAMETER :: DIR_UP = 1, DIR_DOWN = -1

TYPE mpi_buf_t
	CHARACTER(1), POINTER, DIMENSION(:) :: buf
	INTEGER :: type
	INTEGER :: cnt
END TYPE mpi_buf_t

TYPE mpi_vbuf_t
	CHARACTER(1), POINTER, DIMENSION(:) :: buf
	CHARACTER(1), POINTER, DIMENSION(:) :: rootbuf
	INTEGER :: type
	INTEGER :: cnt
	INTEGER, POINTER, DIMENSION(:) :: rootcnt
	INTEGER, POINTER, DIMENSION(:) :: rootdispl
	INTEGER :: isroot
END TYPE mpi_vbuf_t

INTERFACE ALLOC_BUF
      MODULE PROCEDURE ALLOC_BUF_CHAR
      MODULE PROCEDURE ALLOC_BUF_INTEGER
END INTERFACE

CONTAINS
! ---------------------------------------------------------------------------

SUBROUTINE par_do_mpi_work(df, dd, sf, c)

IMPLICIT NONE

DOUBLE PRECISION, EXTERNAL :: df
TYPE(val_distr_t) :: dd
DOUBLE PRECISION, INTENT(IN) :: sf
INTEGER, INTENT(IN) :: c

INTEGER :: me, sz, ierror


CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)

CALL do_work(df(me, sz, sf, dd))

END SUBROUTINE par_do_mpi_work

! ---------------------------------------------------------------------------

SUBROUTINE alloc_mpi_buf(buf, type, cnt)

IMPLICIT NONE

TYPE(mpi_buf_t), POINTER :: buf
INTEGER, INTENT(IN) :: type, cnt

INTEGER :: ierror, fullsize, extend

ALLOCATE (buf, STAT=ierror)
IF (ierror .NE. 0) THEN
	PRINT *, '+++ ERROR1: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, 1, ierror)
END IF

call MPI_TYPE_EXTENT(type, extend, ierror)
fullsize=extend*cnt
ALLOCATE(buf%buf(1:fullsize), STAT=ierror)
IF (ierror .NE. 0) THEN
	DEALLOCATE(buf)
	PRINT *, '+++ ERROR2: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, 2, ierror)
END IF

buf%type = type
buf%cnt = cnt

END SUBROUTINE alloc_mpi_buf

! ---------------------------------------------------------------------------

SUBROUTINE free_mpi_buf(buf)

IMPLICIT NONE

TYPE(mpi_buf_t), POINTER :: buf

IF ( ASSOCIATED(buf) ) THEN
	IF ( ASSOCIATED(buf%buf) ) THEN
		DEALLOCATE(buf%buf)
	END IF
	DEALLOCATE(buf)
END IF

END SUBROUTINE free_mpi_buf

! ---------------------------------------------------------------------------

SUBROUTINE ALLOC_BUF_CHAR(buf, type, res, sz, errno)

IMPLICIT NONE

CHARACTER(1), POINTER, DIMENSION(:) :: buf
TYPE(mpi_vbuf_t), POINTER :: res
INTEGER, INTENT(IN) :: type, sz, errno

INTEGER :: ierror, fullsize, extend

IF ( sz .LE. 0) THEN
	RETURN
END IF

call MPI_TYPE_EXTENT(type, extend, ierror)
fullsize=extend*sz
ALLOCATE(buf(1:fullsize), STAT=ierror)
IF (ierror .NE. 0) THEN
	PRINT *, '+++ ERROR: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, errno, ierror)
END IF

END SUBROUTINE ALLOC_BUF_CHAR

! ---------------------------------------------------------------------------

SUBROUTINE ALLOC_BUF_INTEGER(buf, res, sz, errno)

IMPLICIT NONE

INTEGER, POINTER, DIMENSION(:) :: buf
TYPE(mpi_vbuf_t), POINTER :: res
INTEGER, INTENT(IN) :: sz, errno

INTEGER :: ierror

IF ( sz .LE. 0) THEN
	RETURN
END IF

ALLOCATE ( buf(sz), STAT=ierror )
IF ( ierror .NE. 0 ) THEN
	CALL free_mpi_vbuf(res)
	PRINT *, '+++ ERROR', errno, ': allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, errno, ierror)
END IF
END SUBROUTINE ALLOC_BUF_INTEGER

! ---------------------------------------------------------------------------

SUBROUTINE alloc_mpi_vbuf(res, type, df, dd, root, c)

IMPLICIT NONE

INTEGER, INTENT(IN) :: type, root, c
TYPE(val_distr_t) :: dd
DOUBLE PRECISION, EXTERNAL :: df
TYPE(mpi_vbuf_t), POINTER :: res

INTEGER :: i, me, sz, tcnt, ierror

ALLOCATE (res, STAT=ierror)
IF (ierror .NE. 0) THEN
	PRINT *, '+++ ERROR3: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, 3, ierror)
END IF

CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)

IF (me .EQ. root) THEN
	res%isroot = 1
	CALL ALLOC_BUF(res%rootcnt, res, sz, 4)
	CALL ALLOC_BUF(res%rootdispl, res, sz, 5)

	tcnt = 0
	DO i = 1, sz
		res%rootdispl(i) = tcnt
		res%rootcnt(i) = df(i, sz, 1D0, dd)
		tcnt = tcnt + res%rootcnt(i)
	END DO
      CALL ALLOC_BUF(res%rootbuf, type, res, tcnt, 6)
ELSE
	res%isroot = 0
      ALLOCATE(res%rootcnt(0))
      ALLOCATE(res%rootdispl(0))
      ALLOCATE(res%rootbuf(0))
END IF

res%cnt = df(me, sz, 1D0, dd)
res%type = type
CALL ALLOC_BUF(res%buf, type, res, res%cnt, 7)
END SUBROUTINE alloc_mpi_vbuf

! ---------------------------------------------------------------------------

SUBROUTINE free_mpi_vbuf(buf)

IMPLICIT NONE

TYPE(mpi_vbuf_t), POINTER :: buf

IF ( ASSOCIATED(buf) ) THEN
	IF ( buf%isroot .EQ. 1) THEN
		IF ( ASSOCIATED(buf%rootbuf) ) THEN
			DEALLOCATE(buf%rootbuf)
		END IF
		IF ( ASSOCIATED(buf%rootcnt) ) THEN
			DEALLOCATE(buf%rootcnt)
		END IF
		IF ( ASSOCIATED(buf%rootdispl) ) THEN
			DEALLOCATE(buf%rootdispl)
		END IF
	END IF
	IF ( ASSOCIATED(buf%buf) ) THEN
		DEALLOCATE(buf%buf)
	END IF
	DEALLOCATE(buf)
END IF

END SUBROUTINE free_mpi_vbuf

! ---------------------------------------------------------------------------

SUBROUTINE mpi_commpattern_sendrecv(buf, dir, use_isend, use_irecv, c)

TYPE(mpi_buf_t), POINTER :: buf
INTEGER, INTENT(IN) :: dir, use_isend, use_irecv, c

INTEGER :: me, sz, req, ierror
INTEGER :: stat(MPI_STATUS_SIZE)

CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)

!...if odd number of processors, get rid of last rank
IF ( MOD(sz, 2) .EQ. 1 ) THEN
	IF ( me == sz-1 ) THEN
		RETURN
	ELSE
		sz=sz-1
	END IF
END IF

IF ( MOD(me, 2) .EQ. 0 ) THEN
	!...sender
	IF ( use_isend .EQ. 1 ) THEN
		CALL MPI_ISEND(buf%buf, buf%cnt, buf%type,                  &
                MOD((me+dir+sz), sz), P_SR_TAG, c, req, ierror)
		CALL MPI_WAIT(req, stat, ierror)
	ELSE
		CALL MPI_SSEND(buf%buf, buf%cnt, buf%type,                  &
                MOD((me+dir+sz), sz), P_SR_TAG, c, ierror)
	END IF
ELSE
	!...receiver
	IF ( use_irecv .EQ. 1 ) THEN
		CALL MPI_IRECV(buf%buf, buf%cnt, buf%type,                  &
                        MOD((me-dir+sz), sz), P_SR_TAG, c, req, ierror)
		CALL MPI_WAIT(req, stat, ierror)
	ELSE
		CALL MPI_RECV(buf%buf, buf%cnt, buf%type,                   &
                MOD((me-dir+sz), sz), P_SR_TAG, c, stat, ierror)
	END IF
END IF
END SUBROUTINE mpi_commpattern_sendrecv

! ---------------------------------------------------------------------------

SUBROUTINE mpi_commpattern_shift(sbuf, rbuf, dir, use_isend, use_irecv, c)

TYPE(mpi_buf_t), POINTER :: sbuf, rbuf
INTEGER, INTENT(IN) :: dir, use_isend, use_irecv, c

INTEGER :: me, sz, sreq, rreq, ierror
INTEGER :: stat(MPI_STATUS_SIZE)

CALL MPI_COMM_RANK(c, me, ierror)
CALL MPI_COMM_SIZE(c, sz, ierror)

IF ( use_isend==1 ) THEN
	!...use separate send and recv
	CALL MPI_ISEND(sbuf%buf, sbuf%cnt, sbuf%type, MOD((me+dir+sz), sz), &
        P_SFT_TAG, c, sreq, ierror)

	IF ( use_irecv==1 ) THEN
		CALL MPI_IRECV(rbuf%buf, rbuf%cnt, rbuf%type,               &
                MOD((me-dir+sz), sz), P_SFT_TAG, c, rreq, ierror)
                CALL MPI_WAIT(rreq, stat, ierror)
	ELSE
		CALL MPI_RECV(rbuf%buf, rbuf%cnt, rbuf%type,                &
                MOD((me-dir+sz), sz), P_SFT_TAG, c, stat, ierror)
	END IF
	CALL MPI_WAIT(sreq, stat, ierror)
ELSE
	!...use sendrecv
  	CALL MPI_Sendrecv(sbuf%buf, sbuf%cnt, sbuf%type,                    &
        MOD((me+dir+sz), sz), P_SFT_TAG, rbuf%buf, rbuf%cnt, rbuf%type,     &
	MOD((me-dir+sz), sz), P_SFT_TAG, c, stat, ierror)
END IF
END SUBROUTINE mpi_commpattern_shift

! ---------------------------------------------------------------------------

END MODULE MPI_UTIL









