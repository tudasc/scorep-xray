MODULE DISTRIBUTION

IMPLICIT NONE
PRIVATE

PUBLIC :: df_same, df_cyclic2, df_block2, df_linear, df_peak, df_cyclic3
PUBLIC :: df_block3

DOUBLE PRECISION, PUBLIC :: default_sf = 1.0

TYPE, PUBLIC :: val_distr_t
	DOUBLE PRECISION :: val
	DOUBLE PRECISION :: low
	DOUBLE PRECISION :: high
	DOUBLE PRECISION :: med
	INTEGER :: n
END TYPE val_distr_t

!data structure for task.
!define the type of task (tied or untied task)

TYPE, PUBLIC :: task_type
        INTEGER :: tied_task
        INTEGER :: untied_task
END TYPE task_type


CONTAINS
! ---------------------------------------------------------------------------

FUNCTION df_same(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_same
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE(val_distr_t) :: dd


df_same = dd%val * scale

END FUNCTION df_same

! ---------------------------------------------------------------------------

FUNCTION df_cyclic2(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_cyclic2
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE(val_distr_t) :: dd


IF ( MOD(me, 2) .EQ. 0) THEN
	df_cyclic2 = dd%low * scale
ELSE
	df_cyclic2 = dd%high * scale
END IF

END FUNCTION df_cyclic2

! ---------------------------------------------------------------------------

FUNCTION df_block2(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_block2
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE(val_distr_t) :: dd


IF ( me .LT. (sz/2) ) THEN
	df_block2 = dd%low * scale
ELSE
	df_block2 = dd%high * scale
END IF

END FUNCTION df_block2

! ---------------------------------------------------------------------------

FUNCTION df_linear(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_linear
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE(val_distr_t) :: dd

DOUBLE PRECISION :: step


step = (dd%high - dd%low) / (sz -1)
df_linear = (dd%low + me * step) * scale

END FUNCTION df_linear

! ---------------------------------------------------------------------------

FUNCTION df_peak(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_peak
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE(val_distr_t) :: dd


IF ( me .EQ. dd%n ) THEN
	df_peak = dd%high * scale
ELSE
	df_peak = dd%low * scale
END IF

END FUNCTION df_peak

! ---------------------------------------------------------------------------

FUNCTION df_cyclic3(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_cyclic3
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE(val_distr_t) :: dd


SELECT CASE (MOD(me, 3))
CASE (0)
	df_cyclic3 = dd%low * scale
CASE (1)
	df_cyclic3 = dd%med * scale
CASE (2)
	df_cyclic3 = dd%high * scale
CASE DEFAULT
	df_cyclic3 = 0.0
END SELECT

END FUNCTION df_cyclic3

! ---------------------------------------------------------------------------

FUNCTION df_block3(me, sz, scale, dd)

IMPLICIT NONE

DOUBLE PRECISION :: df_block3
INTEGER, INTENT(IN) :: me, sz
DOUBLE PRECISION, INTENT(IN) :: scale
TYPE (val_distr_t) :: dd


IF ( me .LT. (sz/3) ) THEN
	df_block3 = dd%low * scale
ELSE IF ( me .LT. (2*sz/3) ) THEN
	df_block3 = dd%med * scale
ELSE
	df_block3 = dd%high * scale
END IF

END FUNCTION df_block3

! ---------------------------------------------------------------------------

END MODULE DISTRIBUTION
















