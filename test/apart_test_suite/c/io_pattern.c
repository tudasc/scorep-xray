#include <config.h>
/* ------------------------------------------------------------------------------------------ */
/* The following properties should be implemented both for MPI and OpenMP and modified later. */
/* ------------------------------------------------------------------------------------------ */

/* This property has not been implemented!
   void sequential_IO(int r) {
   int i, iam;
   int arrayA[1024];

   for (i=0; i<1024; i++)
    arrayA[i] = i;
   for (i=0; i<r; ++i) {
   #pragma omp parallel private(iam)
     {
      iam = omp_get_thread_num();
      if (iam == 0)
        printf("arrayA: %d\n", arrayA);
     }
   }
   }
 */

void
IO_in_sequential_loop( int r )
{
    int i, j, iam;
    int arrayA[ 1024 ];

    for ( i = 0; i < 1024; i++ )
    {
        arrayA[ i ] = i;
    }
    for ( i = 0; i < r; ++i )
    {
    #pragma omp parallel private(j, iam)
        {
            iam = omp_get_thread_num();
            if ( iam == 0 )
            {
                for ( j = 0; j < 1024; j++ )
                {
                    printf( "%d", arrayA[ j ] );
                }
            }
        }
    }
}

void
unbalanced_IO( distr_func_t df,
               distr_t*     dd,
               int          r )
{
    int i, j, iam, sz, factor;
    int arrayA[ 9000 ];

    for ( i = 0; i < 9000; i++ )
    {
        arrayA[ i ] = i;
    }
    for ( i = 0; i < r; ++i )
    {
    #pragma omp parallel private(j, iam, sz, factor) firstprivate(arrayA)
        {
            iam    = omp_get_thread_num();
            sz     = omp_get_num_threads();
            factor = floor( 1000 * df( iam, sz, default_sf, dd ) );
            if ( factor > 9000 )
            {
                fprintf( stderr, "The amount of work is too large!" );
            }
            else
            {
                for ( j = 0; j < factor; j++ )
                {
                    printf( "%d", arrayA[ j ] );
                }
            }
        }
    }
}
