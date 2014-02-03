module integral_module
   use function_module
   implicit none
   public :: integral
 
 contains

 function integral (f, a, b, tolerance)  &
       result (integral_result)
  double precision :: integral_result
  double precision :: f, a, b, fa, fb, tolerance
      external f
  fa = f(a)
  fb = f(b)
  integral_result = integral_rec (f, a, fa, b, fb, tolerance)

end function integral


recursive function integral_rec (f, a, fa, b, fb, tolerance)  &
  result (integral_result)
  interface
  function f (x) result (f_result)
       double precision, intent (in) :: x
       double precision :: f_result
  end function f
  end interface

  double precision, intent (in) :: a, b, tolerance
  double precision :: fa, fb, fmid
  double precision :: integral_result
  double precision :: h, mid
  double precision :: one_trapezoid_area, two_trapezoid_area
  double precision :: left_area, right_area

  h = b - a
  mid = (a + b) /2
  fmid = f(mid)
  one_trapezoid_area = h * (fa + fb) / 2.0
  two_trapezoid_area = h/2 * (fa + fmid) / 2.0 + &
  h/2 * (fmid + fb) / 2.0
  if (abs(one_trapezoid_area - two_trapezoid_area)  &
        < 3.0d0 * tolerance) then
       integral_result = two_trapezoid_area
  else
!$omp task shared(left_area)
       left_area = integral_rec (f, a, fa, mid, fmid, tolerance / 2)
!$omp end task
!$omp task shared(right_area)
       right_area = integral_rec (f, mid, fmid, b, fb, tolerance / 2)
!$omp end task
!$omp taskwait
       integral_result = left_area + right_area
  end if

end function integral_rec

end module integral_module
