#include <stdio.h>

int
foo_quadrat
(
    int number
)
{
    return number * number;
}

int
foo_area
(
    int length,
    int width
)
{
    return length * width;
}

int
foo_volume
(
    int length,
    int width,
    int height
)
{
    return length * width * height;
}


main( int argc, char* argv[] )
{
    printf( "hello, world\n" );

    printf( " the quadrat of 5       : %i \n", foo_quadrat( 5 ) );
    printf( " the area of 5 x 4      : %i \n", foo_area( 5, 4 ) );
    printf( " the volume of 5 x 4 x 3: %i \n", foo_volume( 5, 4, 3 ) );

    return 0;
}
