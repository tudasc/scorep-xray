#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

#line 1 "jacobi.F90"
module JacobiMod
    use VariableDef
    implicit none 

    contains
   
    subroutine Jacobi(myData)
        implicit none
        !********************************************************************
        ! Subroutine HelmholtzJ                                             *
        ! Solves poisson equation on rectangular grid assuming :            *
        ! (1) Uniform discretization in each direction, and                 *
        ! (2) Dirichlect boundary conditions                                *
        ! Jacobi method is used in this routine                             *
        !                                                                   *
        ! Input : n,m   Number of grid points in the X/Y directions         *
        !         dx,dy Grid spacing in the X/Y directions                  *
        !         alpha Helmholtz eqn. coefficient                          *
        !         omega Relaxation factor                                   *
        !         myData%afF(n,m) Right hand side function                  *
        !         myData%afU(n,m) Dependent variable/Solution               *
        !         tol    Tolerance for iterative solver                     *
        !         maxit  Maximum number of iterations                       *
        !                                                                   *
        ! Output : myData%afU(n,m) - Solution                               *
        !******************************************************************** 
  
        !.. Formal Arguments .. 
      include 'jacobi.F90.opari.inc'
        type(JacobiData), intent(inout) :: myData 
         
        !.. Local Scalars .. 
        integer :: i, j, iErr
        double precision :: ax, ay, b, residual, fLRes, tmpResd
         
        !.. Local Arrays .. 
        double precision, allocatable :: uold(:,:)
         
        !.. Intrinsic Functions .. 
        intrinsic DBLE, SQRT

        allocate(uold (0 : myData%iCols -1, 0 : myData%iRows -1))

        ! ... Executable Statements ...
        ! Initialize coefficients 
        
        if (allocated(uold)) then    
            ax = 1.0d0 / (myData%fDx * myData%fDx)      ! X-direction coef 
            ay = 1.0d0 / (myData%fDx * myData%fDx)      ! Y-direction coef
            b = -2.0d0 * (ax + ay) - myData%fAlpha      ! Central coeff  
            residual = 10.0d0 * myData%fTolerance
        
            do while (myData%iIterCount < myData%iIterMax .and. residual > myData%fTolerance)
                residual = 0.0d0
        
            ! Copy new solution into old
      pomp_num_threads = pomp_get_max_threads1287559604408488();
      call POMP2_Parallel_fork(pomp2_region_1,pomp_num_threads)
#line 56 "jacobi.F90"
!$omp parallel private(flres, tmpresd, i) num_threads(pomp_num_threads) 
      call POMP2_Parallel_begin(pomp2_region_1)
#line 57 "jacobi.F90"
      call POMP2_Do_enter(pomp2_region_2)
#line 57 "jacobi.F90"
!$omp do
                   do j = 1, myData%iRows - 2
                       do i = 1, myData%iCols - 2
                           uold(i, j) = myData%afU(i, j)
                       end do
                   end do
#line 63 "jacobi.F90"
!$omp end do nowait
      call POMP2_Barrier_enter(pomp2_region_2)
!$omp barrier
      call POMP2_Barrier_exit(pomp2_region_2)
      call POMP2_Do_exit(pomp2_region_2)
#line 64 "jacobi.F90"
      call POMP2_Do_enter(pomp2_region_3)
#line 64 "jacobi.F90"
!$omp do reduction(+:residual)
                  ! Compute stencil, residual, & update
                   do j = myData%iRowFirst + 1, myData%iRowLast - 1
                       do i = 1, myData%iCols - 2
                           ! Evaluate residual 
                           fLRes = (ax * (uold(i-1, j) + uold(i+1, j)) &
                                  + ay * (uold(i, j-1) + uold(i, j+1)) &
                                  + b * uold(i, j) - myData%afF(i, j)) / b
                    
                           ! Update solution 
                           myData%afU(i, j) = uold(i, j) - myData%fRelax * fLRes
                    
                           ! Accumulate residual error
                           residual = residual + fLRes * fLRes
                       end do
                   end do
#line 80 "jacobi.F90"
!$omp end do nowait
      call POMP2_Barrier_enter(pomp2_region_3)
!$omp barrier
      call POMP2_Barrier_exit(pomp2_region_3)
      call POMP2_Do_exit(pomp2_region_3)
#line 81 "jacobi.F90"
      call POMP2_Barrier_enter(pomp2_region_1)
!$omp barrier
      call POMP2_Barrier_exit(pomp2_region_1)
      call POMP2_Parallel_end(pomp2_region_1)
#line 81 "jacobi.F90"
!$omp end parallel
      call POMP2_Parallel_join(pomp2_region_1)
#line 82 "jacobi.F90"
          
                 ! Error check 
                 myData%iIterCount = myData%iIterCount + 1      
                 residual = SQRT(residual) / DBLE(myData%iCols * myData%iRows)
             
            ! End iteration loop 
            end do
            myData%fResidual = residual
            deallocate(uold)
        else
           write (*,*) 'Error: cant allocate memory'
           call Finish(myData)
           stop
        end if
    end subroutine Jacobi

end module JacobiMod

      integer function pomp_get_max_threads1287559604408488()
         integer omp_get_max_threads
         pomp_get_max_threads1287559604408488=omp_get_max_threads()
         return
      end function

      subroutine POMP2_Init_regions_1287559604408488_3()
         include 'jacobi.F90.opari.inc'
         call POMP2_Assign_handle( pomp2_region_1, "66*regionType=parallel*sscl=jacobi.F90:56:56*escl=jacobi.F90:81:81**" )
         call POMP2_Assign_handle( pomp2_region_2, "60*regionType=do*sscl=jacobi.F90:57:57*escl=jacobi.F90:63:63**" )
         call POMP2_Assign_handle( pomp2_region_3, "60*regionType=do*sscl=jacobi.F90:64:64*escl=jacobi.F90:80:80**" )
      end subroutine
