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
      include 'omp_tasks_untied.f90.opari.inc'
  write(*,*) n
  if (n == 1) then
    return
  else
      pomp2_if = .true.
      if (pomp2_if) then
      call POMP2_Untied_task_create_begin(opari2_region_1, pomp2_new_task,&
      pomp2_old_task, pomp2_if, opari2_ctc_1 )
      end if
    !$omp task untied if(pomp2_if) firstprivate(pomp2_new_task, pomp2_if)
      if (pomp2_if) then
      call POMP2_Untied_task_begin(opari2_region_1, pomp2_new_task)
      end if
      call foo(n-1)
      if (pomp2_if) then
      call POMP2_Untied_task_end(opari2_region_1)
      end if
    !$omp end task
      if (pomp2_if) then
      call POMP2_Untied_task_create_end(opari2_region_1, pomp2_old_task)
      end if
      call POMP2_Taskwait_begin(opari2_region_2,&
      pomp2_old_task, opari2_ctc_2 )
    !$omp taskwait
      call POMP2_Taskwait_end(opari2_region_2, pomp2_old_task)
  end if
END SUBROUTINE foo

PROGRAM task_untied
      include 'omp_tasks_untied.f90.opari.inc'
      pomp2_num_threads = pomp2_lib_get_max_threads()
      pomp2_if = .true.
      call POMP2_Parallel_fork(opari2_region_3,&
      pomp2_if, pomp2_num_threads, pomp2_old_task, &
      opari2_ctc_3 )
  !$omp parallel &
  !$omp firstprivate(pomp2_old_task) private(pomp2_new_task) &
  !$omp num_threads(pomp2_num_threads)
      call POMP2_Parallel_begin(opari2_region_3)
      call POMP2_Single_enter(opari2_region_4, &
     opari2_ctc_4 )
    !$omp single
      call POMP2_Single_begin(opari2_region_4)
      call foo(10)
      call POMP2_Single_end(opari2_region_4)
    !$omp end single nowait
      call POMP2_Implicit_barrier_enter(opari2_region_4,&
      pomp2_old_task)
!$omp barrier
      call POMP2_Implicit_barrier_exit(opari2_region_4, pomp2_old_task)
      call POMP2_Single_exit(opari2_region_4)
      call POMP2_Implicit_barrier_enter(opari2_region_3,&
      pomp2_old_task)
!$omp barrier
      call POMP2_Implicit_barrier_exit(opari2_region_3, pomp2_old_task)
      call POMP2_Parallel_end(opari2_region_3)
  !$omp end parallel
      call POMP2_Parallel_join(opari2_region_3, pomp2_old_task)
END PROGRAM task_untied

      subroutine pomp2_init_reg_sjwin65ixa08_3()
         include 'omp_tasks_untied.f90.opari.inc'
         call POMP2_Assign_handle( opari2_region_1, &
         opari2_ctc_1 )
         call POMP2_Assign_handle( opari2_region_2, &
         opari2_ctc_2 )
         call POMP2_Assign_handle( opari2_region_3, &
         opari2_ctc_3 )
         call POMP2_Assign_handle( opari2_region_4, &
         opari2_ctc_4 )
      end
