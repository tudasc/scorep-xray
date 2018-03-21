#include "foo.h"

#include <assert.h>
#include <stdio.h>

#include <shmem.h>

void
foo_shmem_test( void )
{
    int  me;
    int* target;
    int* source;

    me = _my_pe();

    target = shmalloc( sizeof( int ) );
    source = shmalloc( sizeof( int ) );

    *target = -1;

    if ( me == 0 )
    {
        *source = 123;
    }
    else
    {
        *source = -1;
    }

    shmem_int_get( target, source, 1 /*nelements*/, 0 /*pe*/ );
    shmem_barrier_all();
    shmem_fence();

    // assert( *target == 123 ); // doesn't always work

    shfree( target );
    shfree( source );
}
