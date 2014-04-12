PROGRAM CALIBRATE2

USE CALIBRATE
USE WORK

IMPLICIT NONE

INTEGER, PARAMETER :: MAX = 13

DOUBLE PRECISION :: t1, t2, f
INTEGER :: i, N, ios
DOUBLE PRECISION, DIMENSION(MAX) :: d

PRINT *, 'calibration:'
DO i = 1, MAX
  PRINT *, '.'
  t1 = secs()
  CALL do_work(3.42D0)
  t2 = secs()
  d(i) = 3.42D0 / (t2 - t1)
END DO

CALL sort_double(MAX, d)
f = d(MAX/2)
IF (ABS(f-1.0) .LT. 5.0E-4) THEN
  PRINT *, f, 'wrong => close enough'
  STOP
ELSE
  N = f * N_PER_SEC
  PRINT *, f, 'wrong => using now ', N
END IF

OPEN (UNIT=11, IOSTAT=ios, FILE='calibrate.f90', STATUS='REPLACE')
IF (ios .NE. 0) THEN
  PRINT *, 'error: failed to open the file calibrate.f90.'
  STOP
END IF
WRITE (11, '(A)') 'MODULE CALIBRATE'
WRITE (11, *) 'INTEGER, PARAMETER :: N_PER_SEC = ', N
WRITE (11, '(A)') 'END MODULE CALIBRATE'

CLOSE (UNIT=11)

CONTAINS
! ---------------------------------------------------------------------------

FUNCTION secs()

IMPLICIT NONE

DOUBLE PRECISION :: secs
INTEGER count, count_rate, count_max

CALL system_clock(count, count_rate, count_max)
secs = (count * 1D0) / count_rate

END FUNCTION secs

! ---------------------------------------------------------------------------

SUBROUTINE sort_double(n, array)

IMPLICIT NONE

INTEGER, INTENT(IN) :: n
DOUBLE PRECISION, DIMENSION(n), INTENT(IN OUT) :: array

LOGICAL :: no_exchange
INTEGER :: i
DOUBLE PRECISION :: temp

DO
  no_exchange = .TRUE.
  DO i = 1, n-1
    IF (array(i) > array(i+1)) THEN
      temp = array(i)
      array(i) = array(i+1)
      array(i+1) = temp
      no_exchange = .FALSE.
    END IF
  END DO
  IF (no_exchange) EXIT
END DO

END SUBROUTINE sort_double

! ---------------------------------------------------------------------------

END PROGRAM CALIBRATE2

