#include <config.h>
void
intrinsic_function_call( int factor,
                         int r )
{
    int i, j, y;

    for ( i = 0; i < r; ++i )
    {
        for ( j = 0; j < factor; ++j )
        {
            y += sin( 1.0 * j ) + cos( 1.0 * j );
        }
    }
}
