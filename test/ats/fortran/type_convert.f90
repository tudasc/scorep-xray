MODULE TYPE_CONVERT

IMPLICIT NONE
PRIVATE
INCLUDE 'mpif.h'

PUBLIC :: CONVERT_REAL, CONVERT_DOUBLE, CONVERT_INTEGER

CONTAINS
! ---------------------------------------------------------------------------

SUBROUTINE CONVERT_REAL(buf, cnt, errno)

IMPLICIT NONE

CHARACTER(1), POINTER, DIMENSION(:) :: buf
INTEGER, INTENT(IN) :: cnt, errno
REAL, DIMENSION(:), ALLOCATABLE :: array_real
INTEGER :: ierror

ALLOCATE(array_real(cnt), STAT=ierror)
IF (ierror .NE. 0) THEN
	DEALLOCATE(array_real)
	PRINT *, '+++ ERROR2: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, errno, ierror)
END IF
CALL MAP(buf, transfer(array_real,(/"1"/)), errno)
DEALLOCATE(array_real)

END SUBROUTINE CONVERT_REAL

! ---------------------------------------------------------------------------

SUBROUTINE CONVERT_DOUBLE(buf, cnt, errno)

IMPLICIT NONE

CHARACTER(1), POINTER, DIMENSION(:) :: buf
INTEGER, INTENT(IN) :: cnt, errno
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: array_double
INTEGER :: ierror

ALLOCATE(array_double(cnt), STAT=ierror)
IF (ierror .NE. 0) THEN
	DEALLOCATE(array_double)
	PRINT *, '+++ ERROR2: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, errno, ierror)
END IF
CALL MAP(buf, transfer(array_double,(/"1"/)), errno)
DEALLOCATE(array_double)

END SUBROUTINE CONVERT_DOUBLE

! ---------------------------------------------------------------------------

SUBROUTINE CONVERT_INTEGER(buf, cnt, errno)

IMPLICIT NONE

CHARACTER(1), POINTER, DIMENSION(:) :: buf
INTEGER, INTENT(IN) :: cnt, errno
INTEGER, DIMENSION(:), ALLOCATABLE :: array_integer
INTEGER :: ierror

ALLOCATE(array_integer(cnt), STAT=ierror)
IF (ierror .NE. 0) THEN
	DEALLOCATE(array_integer)
	PRINT *, '+++ ERROR2: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, errno, ierror)
END IF
CALL MAP(buf, transfer(array_integer,(/"1"/)), errno)
DEALLOCATE(array_integer)

END SUBROUTINE CONVERT_INTEGER

! ---------------------------------------------------------------------------

SUBROUTINE MAP(buf, char, errno)

IMPLICIT NONE

CHARACTER(1), POINTER, DIMENSION(:) :: buf
CHARACTER(1), DIMENSION(:), INTENT(IN) :: char
INTEGER, INTENT(IN) :: errno
INTEGER :: ierror

ALLOCATE(buf(SIZE(char)), STAT=ierror)
IF (ierror .NE. 0) THEN
	DEALLOCATE(buf)
	PRINT *, '+++ ERROR2: allocation of MPI buffer failed!!!'
	CALL MPI_ABORT(MPI_COMM_WORLD, errno, ierror)
END IF

END SUBROUTINE MAP

! ---------------------------------------------------------------------------

END MODULE TYPE_CONVERT









