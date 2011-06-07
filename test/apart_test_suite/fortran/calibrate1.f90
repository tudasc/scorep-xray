PROGRAM CALIBRATE1

IMPLICIT NONE

INTEGER, PARAMETER :: C_N = 100000000
INTEGER, PARAMETER :: MAX = 13
INTEGER, PARAMETER :: ARR_MAX = 1024

INTEGER :: my_r = 1676

DOUBLE PRECISION :: t1, t2
INTEGER :: i, n_middle, ios
INTEGER, DIMENSION(MAX) :: n

PRINT *, 'calibration1:'
DO i = 1, MAX
  PRINT *, '.'
  t1 = secs()
  CALL do_work_n(C_N)
  t2 = secs()
  n(i) = C_N / (t2 - t1)
END DO

CALL sort_int(MAX, n)
n_middle = n(MAX/2)
PRINT *, 'using ', n(MAX/2)

OPEN (UNIT=10, IOSTAT=ios, FILE='calibrate.f90', STATUS='REPLACE')
IF (ios .NE. 0) THEN
  PRINT *, 'error: failed to open the file calibrate.f90.'
  STOP
END IF
WRITE (10, '(A)') 'MODULE CALIBRATE'
WRITE (10, *) 'INTEGER, PARAMETER :: N_PER_SEC = ', n_middle
WRITE (10, '(A)') 'END MODULE CALIBRATE'

CLOSE (UNIT=10)

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

INTEGER FUNCTION myrand(my_r)

IMPLICIT NONE

INTEGER, INTENT(IN OUT) :: my_r


my_r = MOD((my_r * 9631), 21599)
myrand = my_r

END FUNCTION myrand

! ---------------------------------------------------------------------------

SUBROUTINE do_work_n(N)

IMPLICIT NONE

INTEGER, INTENT(IN) :: N

INTEGER, DIMENSION(ARR_MAX) :: arrA, arrB
INTEGER :: i

DO i = 1, N
  arrA(MOD(myrand(my_r), ARR_MAX)) = arrB(MOD(myrand(my_r), ARR_MAX))
END DO

END SUBROUTINE do_work_n

! ---------------------------------------------------------------------------

SUBROUTINE sort_int(n, array)

IMPLICIT NONE

INTEGER, INTENT(IN) :: n
INTEGER, DIMENSION(n), INTENT(IN OUT) :: array

LOGICAL :: no_exchange
INTEGER :: i, temp

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

END SUBROUTINE sort_int

! ---------------------------------------------------------------------------

END PROGRAM CALIBRATE1







