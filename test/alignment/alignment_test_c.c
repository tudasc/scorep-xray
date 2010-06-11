#include <config.h>
#include <stdint.h>
#include <assert.h>


/* *INDENT-OFF* */
int64_t FORTRAN_MANGLED( foo )    = INT64_MAX;
int64_t FORTRAN_MANGLED( bar )    = INT64_MIN;
int64_t FORTRAN_MANGLED( foobar ) = 0;
int64_t FORTRAN_MANGLED( baz )    = 42;
int64_t FORTRAN_MANGLED( foobaz ) = -42;


void
FORTRAN_MANGLED(check)()
{
    assert( FORTRAN_MANGLED( foo )    == INT64_MAX - 1 );
    assert( FORTRAN_MANGLED( bar )    == INT64_MIN + 1 );
    assert( FORTRAN_MANGLED( foobar ) == -1 );
    assert( FORTRAN_MANGLED( baz )    == -1 );
    assert( FORTRAN_MANGLED( foobaz ) == 1 );
}
/* *INDENT-ON* */
