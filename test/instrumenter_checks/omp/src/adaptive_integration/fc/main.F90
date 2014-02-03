#ifndef TOLERANCE
#define TOLERANCE 0.000001
#endif

program integrate

   use omp_lib
   use function_module
   use integral_module
   use math_module, only : pi

   double precision :: x_min, x_max, tolerance=TOLERANCE
   double precision :: answer
   double precision :: t0, t1, f_result

   x_min = 0.0d0
   x_max = 1.0d0

   t0 = omp_get_wtime()
!$omp parallel
!$omp single
   answer = integral (f, x_min, x_max, tolerance)
!$omp end single
!$omp end parallel
   t1 = omp_get_wtime()

   print "(a, f10.5)", &
         "Total time used for integral()    ",  &
         (t1-t0)
   print "(a, f20.12)",  &
         "The integral is approximately  ",  &
         answer
   print "(a, f20.12)",  &
         "The exact answer is            ",  &
         pi
   print "(a, f20.12)",  &
         "The error is                   ",  &
          abs(answer-pi)
   print "(a, i10)", &
         "The total num of func eval    ",  &
         f_count
end program integrate
