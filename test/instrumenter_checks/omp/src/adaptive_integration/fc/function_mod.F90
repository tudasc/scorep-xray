#ifndef IDLETIME
#define IDLETIME 0.01
#endif

module function_module

   use omp_lib
   implicit none
   public :: f
   integer, public :: f_count=0

 contains

 function f (x) result (f_result)

  double precision, intent (in) :: x
  double precision :: f_result
  double precision :: dt, count_rate_inv
  integer :: count1, count2, count_rate, count_max

  f_result = 4.d0 / (1.d0 + x*x)
!$omp critical
  f_count = f_count + 1
!$omp end critical
  ! spend some time here
  call system_clock(count1, count_rate, count_max)
  count_rate_inv = 1.0d0 / dble(count_rate)
  call system_clock(count1)
  do
      call system_clock(count2)
      if ( count2 < count1 ) count2 = count2 + count_max
      dt = dble(count2-count1) * count_rate_inv
      if ( dt > dble(IDLETIME) ) exit
  end do
end function f

end module function_module
