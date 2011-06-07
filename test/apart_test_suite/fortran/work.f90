MODULE WORK

USE CALIBRATE

IMPLICIT NONE
PRIVATE

PUBLIC :: do_work

INTEGER, PARAMETER :: ARR_MAX = 1024
INTEGER :: my_r = 1676

CONTAINS
! ---------------------------------------------------------------------------

INTEGER FUNCTION myrand(my_r)

IMPLICIT NONE

INTEGER, INTENT(IN OUT) :: my_r


my_r = MOD((my_r * 9631), 21599)
myrand = my_r

END FUNCTION myrand

! ---------------------------------------------------------------------------

SUBROUTINE do_work(secs)

IMPLICIT NONE

REAL(KIND=8), INTENT(IN) :: secs

INTEGER, DIMENSION(ARR_MAX) :: arrA, arrB
INTEGER :: i, N


N = N_PER_SEC * secs
arrB = 1
DO i=1, N
	arrA(MOD(myrand(my_r), ARR_MAX)+1) = arrB(MOD(myrand(my_r), ARR_MAX)+1)
END DO

END SUBROUTINE do_work

! ---------------------------------------------------------------------------

END MODULE WORK










