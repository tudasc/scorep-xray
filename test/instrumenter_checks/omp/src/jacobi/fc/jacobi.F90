#include <scorep/SCOREP_User.inc>
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
        type(JacobiData), intent(inout) :: myData

        !.. Local Scalars ..
        integer :: i, j, iErr
        integer * 8 :: scorep_metric_int_val
        double precision :: ax, ay, b, residual, fLRes, tmpResd,  scorep_metric_double_val

        !.. Local Arrays ..
        double precision, allocatable :: uold(:,:)

        !.. Pomp user region handle to test on the fly registration
        integer( kind=8 ) :: pomp_user_region_handle = 0;

        !.. Intrinsic Functions ..
        intrinsic DBLE, SQRT

		SCOREP_USER_REGION_DEFINE(region1)
		SCOREP_USER_REGION_DEFINE(region2)
		SCOREP_USER_REGION_DEFINE(region3)
		SCOREP_USER_FUNC_DEFINE()
		SCOREP_USER_METRIC_LOCAL(local2)
		SCOREP_USER_METRIC_LOCAL(local3)
		SCOREP_USER_PARAMETER_DEFINE(iparam)
		SCOREP_USER_PARAMETER_DEFINE(uparam)
		SCOREP_USER_PARAMETER_DEFINE(sparam)

		scorep_metric_double_val=3.0;
		scorep_metric_int_val=-1;

        allocate(uold (0 : myData%iCols -1, 0 : myData%iRows -1))

        ! ... Executable Statements ...
        ! Initialize coefficients

        if (allocated(uold)) then
            ax = 1.0d0 / (myData%fDx * myData%fDx)      ! X-direction coef
            ay = 1.0d0 / (myData%fDx * myData%fDx)      ! Y-direction coef
            b = -2.0d0 * (ax + ay) - myData%fAlpha      ! Central coeff
            residual = 10.0d0 * myData%fTolerance

            SCOREP_USER_METRIC_INIT(local2,"local2","s",SCOREP_USER_METRIC_TYPE_INT64,SCOREP_USER_METRIC_CONTEXT_GLOBAL)
            SCOREP_USER_METRIC_INIT(local3,"local3","s",SCOREP_USER_METRIC_TYPE_DOUBLE,SCOREP_USER_METRIC_CONTEXT_GLOBAL)
            SCOREP_USER_FUNC_BEGIN("ScorepTest")
            SCOREP_USER_REGION_INIT(region2,"Region2",SCOREP_USER_REGION_TYPE_COMMON)
            SCOREP_USER_REGION_ENTER(region2)
            SCOREP_USER_PARAMETER_INT64(iparam,"iparam",scorep_metric_int_val)
            SCOREP_USER_PARAMETER_STRING(sparam,"sparam","hello")

            SCOREP_USER_METRIC_INT64(local2, scorep_metric_int_val)
            SCOREP_USER_METRIC_DOUBLE(local3, scorep_metric_double_val)

#ifdef SCOREP_POMP_USER
            ! POMP2 user instrumentation
            ! Not inserted as pragma to test on-the-fly registration.
            ! With Pragmas, the instrumenter would create initialization time
            ! initialization.
	    call POMP2_Begin( pomp_user_region_handle, &
	      "90*regionType=userRegion*sscl=jacobi.F90:63:63*escl=jacobi.F90:102:102*userRegionName=userloop**" )
#endif
            do while (myData%iIterCount < myData%iIterMax .and. residual > myData%fTolerance)

                SCOREP_USER_REGION_BEGIN(region3,"Iteration",SCOREP_USER_REGION_TYPE_DYNAMIC)

                residual = 0.0d0

            ! Copy new solution into old
!$omp parallel private(fLRes, tmpResd, i)
                    SCOREP_USER_REGION_BEGIN(region1,"Region1",SCOREP_USER_REGION_TYPE_PHASE)
                    SCOREP_USER_PARAMETER_INT64(iparam,"iparam",scorep_metric_int_val)
                    SCOREP_USER_PARAMETER_STRING(sparam,"sparam","hello")
                    SCOREP_USER_METRIC_INT64(local2, scorep_metric_int_val)
                    SCOREP_USER_METRIC_DOUBLE(local3, scorep_metric_double_val)

!$omp do
                   do j = 1, myData%iRows - 2
                       do i = 1, myData%iCols - 2
                           uold(i, j) = myData%afU(i, j)
                       end do
                   end do
!$omp end do

					SCOREP_USER_REGION_END(region1)

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
!$omp end do
!$omp end parallel

                 ! Error check
                 myData%iIterCount = myData%iIterCount + 1
                 residual = SQRT(residual) / DBLE(myData%iCols * myData%iRows)

				 SCOREP_USER_REGION_END(region3)

            ! End iteration loop
            end do
#ifdef SCOREP_POMP_USER
	    call POMP2_End( pomp_user_region_handle );
#endif

            SCOREP_USER_REGION_END(region2)
            SCOREP_USER_FUNC_END()

            myData%fResidual = residual
            deallocate(uold)
        else
           write (*,*) 'Error: cant allocate memory'
           call Finish(myData)
           stop
        end if
    end subroutine Jacobi

end module JacobiMod
