#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

#include <config.h>

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
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "jacobi.h"

#define U( j, i ) afU[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]
#define F( j, i ) afF[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]
#define UOLD( j, i ) uold[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]

#define PRAGMA_OMP_PARALLEL_1( tpd ) _Pragma( STR( omp parallel POMP2_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_2( tpd ) _Pragma( STR( omp parallel POMP2_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_3( tpd ) _Pragma( STR( omp parallel private ( i, j, xx, yy, xx2, yy2 ) POMP2_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_4( tpd ) _Pragma( STR( omp parallel private ( j, i ) POMP2_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )

extern void
ExchangeJacobiMpiData( struct JacobiData* data,
                       double*            uold );

void
Jacobi( struct JacobiData* data )
{
    /*use local pointers for performance reasons*/
    double* afU, * afF;
    int     i, j;
    double  fLRes;

    double  ax, ay, b, residual, tmpResd;

    double* uold = ( double* )malloc(
        data->iCols * ( data->iRowLast - data->iRowFirst + 1 ) * sizeof( double ) );
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
            ExchangeJacobiMpiData( data, uold );
            {
                int pomp_num_threads = omp_get_max_threads();
                POMP2_Parallel_fork( pomp_region_1, pomp_num_threads );
#line 64 "jacobi.c"
                PRAGMA_OMP_PARALLEL_1( FORTRAN_MANGLED( pomp_tpd ) )
                {
                    POMP2_Parallel_begin( pomp_region_1 );
#line 65 "jacobi.c"
                    {
                        /* compute stencil, residual and update */
                        POMP2_For_enter( pomp_region_2 );
#line 67 "jacobi.c"
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
                        POMP2_Barrier_enter( pomp_region_2 );
#pragma omp barrier
                        POMP2_Barrier_exit( pomp_region_2 );
                        POMP2_For_exit( pomp_region_2 );
#line 83 "jacobi.c"
                    }
                    POMP2_Barrier_enter( pomp_region_1 );
#pragma omp barrier
                    POMP2_Barrier_exit( pomp_region_1 );
                    POMP2_Parallel_end( pomp_region_1 );
                }
                POMP2_Parallel_join( pomp_region_1 );
            }
#line 83 "jacobi.c"
            /* end omp parallel */
            tmpResd = residual;
            MPI_Allreduce(
                &tmpResd, &residual, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

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

void
ExchangeJacobiMpiData( struct JacobiData* data,
                       double*            uold )
{
    MPI_Request request[ 4 ];
    MPI_Status  status[ 4 ];
    double*     afU, * afF;
    int         iReqCnt = 0;
    int         i, j;
    const int   iTagMoveLeft  = 10;
    const int   iTagMoveRight = 11;
    afU = data->afU;
    afF = data->afF;

    if ( data->iMyRank != 0 )
    {
        /*  receive stripe mlo from left neighbour blocking */
        MPI_Irecv( &UOLD( data->iRowFirst, 0 ), data->iCols, MPI_DOUBLE,
                   data->iMyRank - 1, iTagMoveRight, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }

    if ( data->iMyRank != data->iNumProcs - 1 )
    {
        /* receive stripe mhi from right neighbour blocking */
        MPI_Irecv( &UOLD( data->iRowLast, 0 ), data->iCols, MPI_DOUBLE,
                   data->iMyRank + 1, iTagMoveLeft, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }

    if ( data->iMyRank != data->iNumProcs - 1 )
    {
        /* send stripe mhi-1 to right neighbour async */
        MPI_Isend( &U( data->iRowLast - 1, 0 ), data->iCols, MPI_DOUBLE,
                   data->iMyRank + 1, iTagMoveRight, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }

    if ( data->iMyRank != 0 )
    {
        /* send stripe mlo+1 to left neighbour async */
        MPI_Isend( &U( data->iRowFirst + 1, 0 ), data->iCols, MPI_DOUBLE,
                   data->iMyRank - 1, iTagMoveLeft, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }
    {
        int pomp_num_threads = omp_get_max_threads();
        POMP2_Parallel_fork( pomp_region_3, pomp_num_threads );
#line 153 "jacobi.c"
        PRAGMA_OMP_PARALLEL_4( FORTRAN_MANGLED( pomp_tpd ) )
        {
            POMP2_Parallel_begin( pomp_region_3 );
            POMP2_For_enter( pomp_region_3 );
#line 153 "jacobi.c"
#pragma omp          for               nowait
            for ( j = data->iRowFirst + 1; j <= data->iRowLast - 1; j++ )
            {
                for ( i = 0; i < data->iCols; i++ )
                {
                    UOLD( j, i ) = U( j, i );
                }
            }
            POMP2_Barrier_enter( pomp_region_3 );
#pragma omp barrier
            POMP2_Barrier_exit( pomp_region_3 );
            POMP2_For_exit( pomp_region_3 );
            POMP2_Parallel_end( pomp_region_3 );
        }
        POMP2_Parallel_join( pomp_region_3 );
    }
#line 161 "jacobi.c"

    MPI_Waitall( iReqCnt, request, status );
}
