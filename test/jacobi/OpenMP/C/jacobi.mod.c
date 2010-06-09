#ifdef _POMP
#  undef _POMP
#endif
#define _POMP 200110

#include "jacobi.c.opari.inc"
#line 1 "jacobi.c"
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "jacobi.h"

#define U( j, i ) afU[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]
#define F( j, i ) afF[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]
#define UOLD( j, i ) uold[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]

#define PRAGMA_OMP_PARALLEL_1( tpd ) _Pragma( STR( omp parallel POMP_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_2( tpd ) _Pragma( STR( omp parallel POMP_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_3( tpd ) _Pragma( STR( omp parallel private ( i, j, xx, yy, xx2, yy2 ) POMP_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_4( tpd ) _Pragma( STR( omp parallel private ( j, i ) POMP_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )

void
Jacobi( struct JacobiData* data )
{
    /*use local pointers for performance reasons*/
    double* afU, * afF;
    int     i, j;
    double  fLRes;

    double  ax, ay, b, residual, tmpResd;

    double* uold = ( double* )malloc( data->iCols * data->iRows * sizeof( double ) );
    afU = data->afU;
    afF = data->afF;

    if ( uold )
    {
        ax       = 1.0 / ( data->fDx * data->fDx );   /* X-direction coef */
        ay       = 1.0 / ( data->fDy * data->fDy );   /* Y_direction coef */
        b        = -2.0 * ( ax + ay ) - data->fAlpha; /* Central coeff */
        residual = 10.0 * data->fTolerance;

        while ( data->iIterCount < data->iIterMax && residual > data->fTolerance )
        {
            residual = 0.0;

            /* copy new solution into old */
            {
                int pomp_num_threads = omp_get_max_threads();
                POMP_Parallel_fork( pomp_region_1, pomp_num_threads );
#line 58 "jacobi.c"
                PRAGMA_OMP_PARALLEL_1( POMP_TPD_MANGLED )
                {
                    POMP_Parallel_begin( pomp_region_1 );
#line 59 "jacobi.c"
                    {
                        POMP_For_enter( pomp_region_2 );
#line 60 "jacobi.c"
#pragma omp for private(j, i) nowait
                        for ( j = 1; j < data->iRows - 1; j++ )
                        {
                            for ( i = 1; i < data->iCols - 1; i++ )
                            {
                                UOLD( j, i ) = U( j, i );
                            }
                        }
                        POMP_Barrier_enter( pomp_region_2 );
#pragma omp barrier
                        POMP_Barrier_exit( pomp_region_2 );
                        POMP_For_exit( pomp_region_2 );
#line 68 "jacobi.c"


                        /* compute stencil, residual and update */
                        POMP_For_enter( pomp_region_3 );
#line 71 "jacobi.c"
#pragma omp for private(j, i, fLRes) reduction(+:residual) nowait
                        for ( j = data->iRowFirst + 1; j <= data->iRowLast - 1; j++ )
                        {
                            for ( i = 1; i <= data->iCols - 2; i++ )
                            {
                                fLRes = ( ax * ( UOLD( j, i - 1 ) + UOLD( j, i + 1 ) )
                                          + ay * ( UOLD( j - 1, i ) + UOLD( j + 1, i ) )
                                          +  b * UOLD( j, i ) - F( j, i ) ) / b;

                                /* update solution */
                                U( j, i ) = UOLD( j, i ) - data->fRelax * fLRes;

                                /* accumulate residual error */
                                residual += fLRes * fLRes;
                            }
                        }
                        POMP_Barrier_enter( pomp_region_3 );
#pragma omp barrier
                        POMP_Barrier_exit( pomp_region_3 );
                        POMP_For_exit( pomp_region_3 );
#line 87 "jacobi.c"
                    }
                    POMP_Barrier_enter( pomp_region_1 );
#pragma omp barrier
                    POMP_Barrier_exit( pomp_region_1 );
                    POMP_Parallel_end( pomp_region_1 );
                }
                POMP_Parallel_join( pomp_region_1 );
            }
#line 87 "jacobi.c"
            /* end omp parallel */

            /* error check */
            ( data->iIterCount )++;
            residual = sqrt( residual ) / ( data->iCols * data->iRows );
        } /* while */

        data->fResidual = residual;
        free( uold );
    }
    else
    {
        fprintf( stderr, "Error: cant allocate memory\n" );
        Finish( data );
        exit( 1 );
    }
}
