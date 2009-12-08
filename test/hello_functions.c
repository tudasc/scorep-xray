#include <stdio.h>

int
foo_area( int length,
          int width )
{
    printf( " in function foo_area \n" );
    return length * width;
}

int
foo_quadrat( int number )
{
    printf( " in function foo_quadrat \n" );
    return foo_area( number, number );
}

int
foo_volume( int length,
            int width,
            int height )
{
    printf( " in function foo_volume \n" );
    return foo_area( length, foo_area( width, height ) );
}

int
main( int   argc,
      char* argv[] )
{
    //MPI_Init( &argc, &argv );

    printf( "hello, world\n" );

    printf( " the quadrat of 5       : %i \n", foo_quadrat( 5 ) );
    printf( " the area of 5 x 4      : %i \n", foo_area( 5, 4 ) );
    printf( " the volume of 5 x 4 x 3: %i \n", foo_volume( 5, 4, 3 ) );

    //MPI_Finalize();

    return 0;
}
