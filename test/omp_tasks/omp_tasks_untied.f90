!
! This file is part of the Score-P software (http://www.score-p.org)
!
! Copyright (c) 2018,
! Forschungszentrum Juelich GmbH, Germany
!
! This software may be modified and distributed under the terms of
! a BSD-style license.  See the COPYING file in the package base
! directory for details.
!

RECURSIVE SUBROUTINE foo(n)
  write(*,*) n
  if (n == 1) then
    return
  else
    !$omp task untied
      call foo(n-1)
    !$omp end task
    !$omp taskwait
  end if
END SUBROUTINE foo

PROGRAM task_untied
  !$omp parallel
    !$omp single
      call foo(10)
    !$omp end single
  !$omp end parallel
END PROGRAM task_untied
