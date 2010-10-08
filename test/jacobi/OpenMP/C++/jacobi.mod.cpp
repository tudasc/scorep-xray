#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

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

#define PRAGMA_OMP_PARALLEL_1( tpd ) _Pragma( STR( omp parallel POMP2_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_2( tpd ) _Pragma( STR( omp parallel POMP2_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_3( tpd ) _Pragma( STR( omp parallel private ( i, j, xx, yy, xx2, yy2 ) POMP2_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_4( tpd ) _Pragma( STR( omp parallel private ( j, i ) POMP2_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )

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
                int pomp_num_threads = omp_get_max_threads();
                POMP2_Parallel_fork( &pomp_region_1, pomp_num_threads );
#line 55 "jacobi.cpp"
                PRAGMA_OMP_PARALLEL_1( FORTRAN_MANGLED( pomp_tpd ) )
                {
                    POMP2_Parallel_begin( &pomp_region_1 );
#line 56 "jacobi.cpp"
                    {
                        POMP2_For_enter( &pomp_region_2 );
#line 57 "jacobi.cpp"
#pragma omp for nowait
                        for ( int j = 1; j < data.iRows - 1; j++ )
                        {
                            for ( int i = 1; i < data.iCols - 1; i++ )
                            {
                                UOLD( j, i ) = U( j, i );
                            }
                        }
                        POMP2_Barrier_enter( &pomp_region_2 );
#pragma omp barrier
                        POMP2_Barrier_exit( &pomp_region_2 );
                        POMP2_For_exit( &pomp_region_2 );
#line 65 "jacobi.cpp"

                        double fLRes;

                        /* compute stencil, residual and update */
                        POMP2_For_enter( &pomp_region_3 );
#line 69 "jacobi.cpp"
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
                        POMP2_Barrier_enter( &pomp_region_3 );
#pragma omp barrier
                        POMP2_Barrier_exit( &pomp_region_3 );
                        POMP2_For_exit( &pomp_region_3 );
#line 85 "jacobi.cpp"
                    }
                    POMP2_Barrier_enter( &pomp_region_1 );
#pragma omp barrier
                    POMP2_Barrier_exit( &pomp_region_1 );
                    POMP2_Parallel_end( &pomp_region_1 );
                }
                POMP2_Parallel_join( &pomp_region_1 );
            }
#line 85 "jacobi.cpp"
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
