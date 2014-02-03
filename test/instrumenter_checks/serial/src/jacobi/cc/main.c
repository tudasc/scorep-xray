#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/time.h>
#include "jacobi.h"

#include <scorep/SCOREP_User.h>

#define U( j, i ) data->afU[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]
#define F( j, i ) data->afF[ ( ( j ) - data->iRowFirst ) * data->iCols + ( i ) ]

SCOREP_USER_GLOBAL_REGION_DEFINE( scorep_region2 );
SCOREP_USER_METRIC_GLOBAL( scorep_globalMetric );

/*
 * setting values, init mpi, omp etc
 */
void
Init( struct JacobiData* data, int* argc, char** argv )
{
    int i;
    int block_lengths[ 8 ];

    int   ITERATIONS = 1000;
    char* env        = getenv( "ITERATIONS" );
    if ( env )
    {
        int iterations = atoi( env );
        if ( iterations > 0 )
        {
            ITERATIONS = iterations;
        }
        else
        {
            printf( "Ignoring invalid ITERATIONS=%s!\n", env );
        }
    }
    printf( "Jacobi (serial)\n" );


/* default medium */
    data->iCols      = 2000;
    data->iRows      = 2000;
    data->fAlpha     = 0.8;
    data->fRelax     = 1.0;
    data->fTolerance = 1e-10;
    data->iIterMax   = ITERATIONS;
    printf( "\n-> matrix size: %dx%d"
            "\n-> alpha: %f"
            "\n-> relax: %f"
            "\n-> tolerance: %f"
            "\n-> iterations: %d \n\n",
            data->iCols, data->iRows, data->fAlpha, data->fRelax,
            data->fTolerance, data->iIterMax );


    /* MPI values, set to defaults to avoid data inconsistency */
    data->iMyRank   = 0;
    data->iNumProcs = 1;
    data->iRowFirst = 0;
    data->iRowLast  = data->iRows - 1;

    /* memory allocation for serial & omp */
    data->afU = ( double* )malloc( data->iRows * data->iCols * sizeof( double ) );
    data->afF = ( double* )malloc( data->iRows * data->iCols * sizeof( double ) );

    /* calculate dx and dy */
    data->fDx = 2.0 / ( data->iCols - 1 );
    data->fDy = 2.0 / ( data->iRows - 1 );

    data->iIterCount = 0;

    return;
}

/*
 * final cleanup routines
 */
void
Finish( struct JacobiData* data )
{
    free( data->afU );
    free( data->afF );

    return;
}

/*
 * print result summary
 */
void
PrintResults( const struct JacobiData* data )
{
    if ( data->iMyRank == 0 )
    {
        printf( " Number of iterations : %d\n", data->iIterCount );
        printf( " Residual             : %le\n", data->fResidual );
        printf( " Solution Error       : %1.12lf\n", data->fError );
        printf( " Elapsed Time         : %5.7lf\n",
                data->fTimeStop - data->fTimeStart );
        printf( " MFlops               : %6.6lf\n",
                0.000013 * data->iIterCount * ( data->iCols - 2 ) * ( data->iRows - 2 )
                / ( data->fTimeStop - data->fTimeStart ) );
    }
    return;
}

/*
 * Initializes matrix
 * Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
 */
void
InitializeMatrix( struct JacobiData* data )
{
    int i, j, xx, yy, xx2, yy2;
    /* Initialize initial condition and RHS */
    for ( j = data->iRowFirst; j <= data->iRowLast; j++ )
    {
        for ( i = 0; i < data->iCols; i++ )
        {
            /* TODO: check if this values have to be ints or doubles */
            xx = ( int )( -1.0 + data->fDx * i );
            yy = ( int )( -1.0 + data->fDy * j );

            xx2 = xx * xx;
            yy2 = yy * yy;

            U( j, i ) = 0.0;
            F( j, i ) = -data->fAlpha * ( 1.0 - xx2 ) * ( 1.0 - yy2 )
                        + 2.0 * ( -2.0 + xx2 + yy2 );
        }
    }
}

/*
 * Checks error between numerical and exact solution
 */
void
CheckError( struct JacobiData* data )
{
    double error = 0.0;
    int    i, j;
    double xx, yy, temp;

    for ( j = data->iRowFirst; j <= data->iRowLast; j++ )
    {
        if ( ( data->iMyRank != 0 && j == data->iRowFirst ) ||
             ( data->iMyRank != data->iNumProcs - 1 && j == data->iRowLast ) )
        {
            continue;
        }

        for ( i = 0; i < data->iCols; i++ )
        {
            xx     = -1.0 + data->fDx * i;
            yy     = -1.0 + data->fDy * j;
            temp   = U( j, i ) - ( 1.0 - xx * xx ) * ( 1.0 - yy * yy );
            error += temp * temp;
        }
    }

    data->fError = sqrt( error ) / ( data->iCols * data->iRows );

    return;
}

double
get_wtime()
{
    struct timeval tp;
    gettimeofday( &tp, 0 );
    return tp.tv_sec + ( tp.tv_usec * 1.0e-6 );
}


int
main( int argc, char** argv )
{
    int retVal = 0;    /* return value */

    struct JacobiData myData;

    SCOREP_USER_METRIC_LOCAL( scorep_localMetric );

    SCOREP_USER_METRIC_INIT( scorep_globalMetric, "GLOBAL_METRIC", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );
    SCOREP_USER_METRIC_INIT( scorep_localMetric, "LOCAL_METRIC", "s", SCOREP_USER_METRIC_TYPE_DOUBLE,
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL );
    SCOREP_USER_METRIC_DOUBLE( scorep_localMetric, 3.0 );


    /* sets default values or reads from stdin
     * inits MPI and OpenMP if needed
     * distribute MPI data, calculate MPI bounds
     */
    Init( &myData, &argc, argv );

    if ( myData.afU && myData.afF )
    {
        /* matrix init */
        InitializeMatrix( &myData );

        /* starting timer */
        myData.fTimeStart = get_wtime();

        /* running calculations */
        Jacobi( &myData );

        /* stopping timer */
        myData.fTimeStop = get_wtime();

        /* error checking */
        CheckError( &myData );

        /* print result summary */
        PrintResults( &myData );
    }
    else
    {
        printf( " Memory allocation failed ...\n" );
        retVal = -1;
    }

    /* cleanup */
    Finish( &myData );

    return retVal;
}
