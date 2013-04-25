/* *INDENT-OFF* */
#include <config.h>
#include "jacobi.cpp.opari.inc"
#line 1 "jacobi.cpp"
/*
 ******************************************************************
 * Subroutine HelmholtzJ
 * Solves poisson equation on rectangular grid assuming :
 * (1) Uniform discretization in each direction, and
 * (2) Dirichlect boundary conditions
 *
 * Jacobi method is used in this routine
 *
 * Input : n,m   Number of grid points in the X/Y directions
 *         dx,dy Grid spacing in the X/Y directions
 *         alpha Helmholtz eqn. coefficient
 *         omega Relaxation factor
 *         f(n,m) Right hand side function
 *         u(n,m) Dependent variable/Solution
 *         tolerance Tolerance for iterative solver
 *         maxit  Maximum number of iterations
 *
 * Output : u(n,m) - Solution
 *****************************************************************
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include "jacobi.h"

using namespace std;

#define U( j, i ) afU[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define F( j, i ) afF[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define UOLD( j, i ) uold[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]


void
Jacobi( JacobiData &data )
{
    /*use local pointers for performance reasons*/
    double* afU, * afF;
    afU = data.afU;
    afF = data.afF;
    double* uold = new double[ data.iCols * data.iRows ];

    if ( uold )
    {
        double ax       = 1.0 / ( data.fDx * data.fDx );    /* X-direction coef */
        double ay       = 1.0 / ( data.fDy * data.fDy );    /* Y_direction coef */
        double b        = -2.0 * ( ax + ay ) - data.fAlpha; /* Central coeff */
        double residual = 10.0 * data.fTolerance;

        while ( data.iIterCount < data.iIterMax && residual > data.fTolerance )
        {
            residual = 0.0;

            /* copy new solution into old */
{
  int pomp2_num_threads = omp_get_max_threads();
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_1, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_1 );
#line 57 "jacobi.cpp"
#pragma omp parallel POMP2_DLIST_00001 firstprivate(pomp2_old_task) if(pomp2_if) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
{   POMP2_Parallel_begin( &pomp2_region_1 );
#line 58 "jacobi.cpp"
            {
{   POMP2_For_enter( &pomp2_region_2, pomp2_ctc_2  );
#line 59 "jacobi.cpp"
#pragma omp for nowait
                for ( int j = 1; j < data.iRows - 1; j++ )
                {
                    for ( int i = 1; i < data.iCols - 1; i++ )
                    {
                        UOLD( j, i ) = U( j, i );
                    }
                }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_2, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_2, pomp2_old_task ); }
  POMP2_For_exit( &pomp2_region_2 );
 }
#line 67 "jacobi.cpp"

                double fLRes;

                /* compute stencil, residual and update */
{   POMP2_For_enter( &pomp2_region_3, pomp2_ctc_3  );
#line 71 "jacobi.cpp"
#pragma omp for reduction(+:residual) nowait
                for ( int j = data.iRowFirst + 1; j <= data.iRowLast - 1; j++ )
                {
                    for ( int i = 1; i < data.iCols - 1; i++ )
                    {
                        fLRes = ( ax * ( UOLD( j, i - 1 ) + UOLD( j, i + 1 ) )
                                  + ay * ( UOLD( j - 1, i ) + UOLD( j + 1, i ) )
                                  +  b * UOLD( j, i ) - F( j, i ) ) / b;

                        /* update solution */
                        U( j, i ) = UOLD( j, i ) - data.fRelax * fLRes;

                        /* accumulate residual error */
                        residual += fLRes * fLRes;
                    }
                }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_3, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_3, pomp2_old_task ); }
  POMP2_For_exit( &pomp2_region_3 );
 }
#line 87 "jacobi.cpp"
            }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_1, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_1, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_1 ); }
  POMP2_Parallel_join( &pomp2_region_1, pomp2_old_task ); }
#line 87 "jacobi.cpp"
              /* end omp parallel */

            /* error check */
            data.iIterCount++;
            residual = sqrt( residual ) / ( data.iCols * data.iRows );
        } /* while */

        data.fResidual = residual;
        delete[] uold;
    }
    else
    {
        cerr << "Error: cant allocate memory\n";
        Finish( data );
        exit( 1 );
    }
}
