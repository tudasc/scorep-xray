#ifdef _POMP
#  undef _POMP
#endif
#define _POMP 200110

#line 1 "jacobi.F90"
module JacobiMod
    use VariableDef
    implicit none 

    contains
   
    subroutine Jacobi(myData)
        implicit none
      include 'jacobi.F90.opari.inc'
        include 'mpif.h'
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
        type(JacobiData), intent(inout) :: myData 
         
        !.. Local Scalars .. 
        integer :: i, j, iErr
        double precision :: ax, ay, b, residual, fLRes, tmpResd
         
        !.. Local Arrays .. 
        double precision, allocatable :: uold(:,:)
         
        !.. Intrinsic Functions .. 
        intrinsic DBLE, SQRT

        allocate(uold (0 : myData%iCols -1, myData%iRowFirst : myData%iRowLast))

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
                call ExchangeJacobiMpiData(myData, uold)
pomp_num_threads = omp_get_max_threads();
      call POMP_Parallel_fork(pomp_region_1,pomp_num_threads)
#line 58 "jacobi.F90"
!$omp parallel private(flres, tmpresd, i) num_threads(pomp_num_threads) copyin(pomp_tpd)
      call POMP_Parallel_begin(pomp_region_1)
#line 59 "jacobi.F90"
      call POMP_Do_enter(pomp_region_2)
#line 59 "jacobi.F90"
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
#line 75 "jacobi.F90"
!$omp end do nowait
      call POMP_Barrier_enter(pomp_region_2)
!$omp barrier
      call POMP_Barrier_exit(pomp_region_2)
      call POMP_Do_exit(pomp_region_2)
#line 76 "jacobi.F90"
      call POMP_Barrier_enter(pomp_region_1)
!$omp barrier
      call POMP_Barrier_exit(pomp_region_1)
      call POMP_Parallel_end(pomp_region_1)
#line 76 "jacobi.F90"
!$omp end parallel
      call POMP_Parallel_join(pomp_region_1)
#line 77 "jacobi.F90"
                  tmpResd = residual
                  call MPI_Allreduce(tmpResd, residual, 1, MPI_DOUBLE_PRECISION, &
                                MPI_SUM, MPI_COMM_WORLD, Ierr)
          
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

    subroutine ExchangeJacobiMpiData (myData, uold)
    !    use VariableDef
        implicit none
      include 'jacobi.F90.opari.inc'
        include 'mpif.h'
        type(JacobiData), intent(inout) :: myData
        double precision, intent(inout) :: uold(0: myData%iCols -1, myData%iRowFirst : myData%iRowLast)
        integer :: request(4), status(MPI_STATUS_SIZE, 4)
        integer, parameter :: iTagMoveLeft = 10, iTagMoveRight = 11
        integer i, j, iErr, iReqCnt
 
        iReqCnt = 0
        if (myData%iMyRank /= 0) then
!!           /*  receive stripe mlo from left neighbour blocking */
            iReqCnt = iReqCnt + 1
            call MPI_Irecv( uold(0, myData%iRowFirst), myData%iCols, &
                           MPI_DOUBLE_PRECISION, myData%iMyRank - 1, &
                           iTagMoveRight, MPI_COMM_WORLD, request(iReqCnt), iErr)
        end if
 
        if (myData%iMyRank /= myData%iNumProcs - 1) then
!!           /* receive stripe mhi from right neighbour blocking */
            iReqCnt = iReqCnt + 1
            call MPI_Irecv( uold(0, myData%iRowLast), myData%iCols,  &
                           MPI_DOUBLE_PRECISION, myData%iMyRank + 1, &
                           iTagMoveLeft, MPI_COMM_WORLD, request(iReqCnt), iErr)
        end if
 
        if (myData%iMyRank /= myData%iNumProcs - 1) then
!!           /* send stripe mhi-1 to right neighbour async */
            iReqCnt = iReqCnt + 1
            call MPI_Isend( myData%afU(0, myData%iRowLast - 1), myData%iCols, &
                            MPI_DOUBLE_PRECISION, myData%iMyRank + 1,         &
                            iTagMoveRight, MPI_COMM_WORLD, request(iReqCnt), iErr)
        end if
 
        if (myData%iMyRank /= 0) then
!!           /* send stripe mlo+1 to left neighbour async */
            iReqCnt = iReqCnt + 1
            call MPI_Isend( myData%afU(0, myData%iRowFirst + 1), myData%iCols, &
                           MPI_DOUBLE_PRECISION, myData%iMyRank - 1,           &
                           iTagMoveLeft, MPI_COMM_WORLD, request(iReqCnt), iErr)
        end if
pomp_num_threads = omp_get_max_threads();
      call POMP_Parallel_fork(pomp_region_3,pomp_num_threads)
#line 138 "jacobi.F90"
!$omp parallel    num_threads(pomp_num_threads) copyin(pomp_tpd)
      call POMP_Parallel_begin(pomp_region_3)
      call POMP_Do_enter(pomp_region_3)
#line 138 "jacobi.F90"
!$omp          do
        do j = myData%iRowFirst + 1, myData%iRowLast - 1
            do i = 0, myData%iCols - 1
                uold(i, j) = myData%afU(i, j)
            end do
        end do
!$omp end do nowait
      call POMP_Barrier_enter(pomp_region_3)
!$omp barrier
      call POMP_Barrier_exit(pomp_region_3)
      call POMP_Do_exit(pomp_region_3)
      call POMP_Parallel_end(pomp_region_3)
!$omp end parallel
      call POMP_Parallel_join(pomp_region_3)
#line 145 "jacobi.F90"

        call MPI_Waitall(iReqCnt, request, status, iErr)
    end subroutine ExchangeJacobiMpiData
end module JacobiMod

      subroutine POMP_Init_regions_1276000468650595_3()
         include 'jacobi.F90.opari.inc'
         call POMP_Assign_handle( pomp_region_1, "66*regionType=parallel*sscl=jacobi.F90:58:58*escl=jacobi.F90:76:76**" );
         call POMP_Assign_handle( pomp_region_2, "60*regionType=do*sscl=jacobi.F90:59:59*escl=jacobi.F90:75:75**" );
         call POMP_Assign_handle( pomp_region_3, "72*regionType=paralleldo*sscl=jacobi.F90:138:138*escl=jacobi.F90:144:144**" );
      end subroutine
