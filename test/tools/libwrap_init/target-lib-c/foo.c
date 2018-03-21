#include "foo.h"
#include "foo2.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

struct some_struct
{
    int i;
};

struct some_struct*
fn_1_new( void )
{
    struct some_struct* s = malloc( sizeof( *s ) );
    s->i = 0;
    return s;
}

int
fn_2_get( struct some_struct* s )
{
    return s->i;
}

void
fn_3_set( struct some_struct* s, int i )
{
    s->i = i;
}

void
fn_4_ellipsis( int x, ... )
{
    ( void )x;
}

void
vfn_4_ellipsis_va_list_variant( int x, va_list vl )
{
    ( void )x;
}

int
fn_6_function_pointer_arg( int ( * f )( double ) )
{
    return f( 3.6 );
}

void*
fn_6_function_pointer_arg_pointer_return( void* ( *f )( double ) )
{
    return f( 3.6 );
}

int
fn_7_function_pointer_without_star_arg( int( f )( double ) )
{
    return f( 3.6 );
}

void*
fn_7_function_pointer_without_star_arg_pointer_return( void* ( f )( double ) )
{
    return f( 3.6 );
}

int
fn_8_array_of_function_pointers_arg( int( *fs[] )( double ), int len )
{
    int ret = 0;
    int i;
    for ( i = 0; i < len; i += 1 )
    {
        ret += fs[ i ]( 1.0 );
    }
    return ret;
}

void*
fn_8_array_of_function_pointers_arg_pointer_return( void* ( *fs[]  )( double ), int len )
{
    int i;
    for ( i = 0; i < len; i += 1 )
    {
        fs[ i ]( 1.0 );
    }
    return NULL;
}

static int
return_func( double d )
{
    return ( int )d;
}

static void*
return_func_pointer_return( double d )
{
    ( void )d;
    return NULL;
}

void
fn_9_function_pointer_out_arg( int( **f )( double ) )
{
    *f = return_func;
}

void*
fn_9_function_pointer_out_arg_pointer_return( void* ( **f )( double ) )
{
    *f = return_func_pointer_return;
    return NULL;
}

int( *fn_x_return_function_pointer( void ) )( double )
{
    return return_func;
}

int
fn_10_function_pointer_typedef( fn_t f )
{
    return fn_6_function_pointer_arg( f );
}

fn_t
fn_11_return_function_pointer_typedef( void )
{
    return fn_x_return_function_pointer();
}

int
fn_12_array_arg( int a[], int len )
{
    int ret = 0;
    int i;
    for ( i = 0; i < len; i += 1 )
    {
        ret += a[ i ];
    }
    return ret;
}

int
fn_12_array_to_pointer_arg( int* in[], int len )
{
    int ret = 0;
    int i, j;
    for ( i = 0; i < len; i += 1 )
    {
        int len2 = in[ i ][ 0 ];
        for ( j = 1; j < len2; j += 1 )
        {
            ret += in[ i ][ j ];
        }
    }
    return ret;
}

int
fn_13_array_const_size_arg( int a[ 2 ] )
{
    return a[ 0 ] + a[ 1 ];
}

int
fn_14_array_var_size_arg( int len, int a[ len ] )
{
    return fn_12_array_arg( a, len );
}

int
fn_15_unknown_arg_list( void )
{
    return 23;
}

int
fn_16_const_arg( const int* i )
{
    assert( i != 0 );
    return *i;
}

int
fn_17_arg_const( int* const* i )
{
    assert( i != 0 );
    assert( *i != 0 );
    return **i;
}

#if __STDC_VERSION__ >= 199901L
int
fn_18_restrict_arg( int* restrict i )
{
    assert( i != 0 );
    return *i;
}
#endif

const void*
fn_19_const_ret( void )
{
    return ( const void* )0;
}

int
fn_20_two_dim_array__arg( int a[][ 2 ], int len )
{
    int ret = 0;
    int i;
    for ( i = 0; i < len; i += 1 )
    {
        int j;
        for ( j = 0; j < 2; j += 1 )
        {
            ret += a[ i ][ j ];
        }
    }
    return ret;
}

int
fn_21_filter_test( void )
{
    return 123;
}

int
fn_23_ellipsis_without_wrapping_va_list_variant( const char* msg, ... )
{
    va_list vl;
    int     ret;
    va_start( vl, msg );
    ret = vprintf( msg, vl );
    va_end( vl );
    return ret;
}

int
fn_24a_typedef_array_arg( t_24* t )
{
    return ( *t )[ 0 ];
}

t_24*
fn_24b_typedef_array_return( void )
{
    return NULL;
}
