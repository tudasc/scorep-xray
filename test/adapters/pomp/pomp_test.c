#include <config.h>

#include <omp.h>

int
main( int   argc,
      char* argv[] )
{
    int a;
  #pragma omp parallel
    {
        int i;
    #pragma omp for
        for ( i = 0; i < 1000; i++ )
        {
            a++;
        }

    #pragma omp master
        {
            a++;
        }

    #pragma omp barrier

    #pragma omp critical(test1)
        {
            a++;
        }

    #pragma omp atomic
        a++;

    #pragma omp sections
        {
      #pragma omp section
            {
                a++;
            }
      #pragma omp section
            {
                a += 2;
            }
        }
    }
    return 0;
}
