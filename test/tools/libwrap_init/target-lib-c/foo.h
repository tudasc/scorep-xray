#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct some_struct;

struct some_struct*
fn_1_new( void );

int
fn_2_get( struct some_struct* );

void
fn_3_set( struct some_struct*, int );

void
fn_4_ellipsis( int, ... );

void
vfn_4_ellipsis_va_list_variant( int x, va_list vl );

static int
fn_5_static( void )
{
    return 23;
}

int
fn_6_function_pointer_arg( int ( * f )( double ) );

void *
fn_6_function_pointer_arg_pointer_return( void * ( * f )( double ) );

int
fn_7_function_pointer_without_star_arg( int ( f )( double ) );

void *
fn_7_function_pointer_without_star_arg_pointer_return( void * ( f )( double ) );

int
fn_8_array_of_function_pointers_arg( int( *fs[] )( double ), int len );

void *
fn_8_array_of_function_pointers_arg_pointer_return( void * ( * fs [ ] ) ( double ), int len );

void
fn_9_function_pointer_out_arg( int ( * * f )( double ) );

void *
fn_9_function_pointer_out_arg_pointer_return( void * ( * * f )( double ) );

/* TODO doesn't work
int (*fn_x_return_function_pointer( void ))(double);
*/

typedef int (*fn_t)( double );

int
fn_10_function_pointer_typedef( fn_t f );

fn_t
fn_11_return_function_pointer_typedef( void );

int
fn_12_array_arg( int[], int len );

int
fn_12_array_to_pointer_arg( int *in[], int len );

int
fn_13_array_const_size_arg(int a[2]);

#if __STDC_VERSION__ >= 199901L && !defined(__PGI)
int
fn_14_array_var_size_arg(int len, int a[len]);
#endif

/*
 * TODO this is a GCC extension, as it will be preprocessed it will be input to
 *      clang, but he will bark on this. this is a general issue.
 */
/* #if defined( __GNUC__ ) && !defined( __clang__ )
 * void
 * arr_as_arg_var_size2(int s; int foo[s], int s);
 * #endif
 */

/* This is also a variadic function, as the argument list is unknown, not "void" */
int
fn_15_unknown_arg_list();

int
fn_16_const_arg( const int* i );

int
fn_17_arg_const( int* const* i );

#if __STDC_VERSION__ >= 199901L
int
fn_18_restrict_arg( int* restrict i );
#endif

const void*
fn_19_const_ret( void );

int
fn_20_two_dim_array__arg( int a[][2], int len );

/* duplicate decl, i.e., a forward decl, should only be wrapped once */
int
fn_20_two_dim_array__arg( int a[][2], int len );

/* Body is not accepted by Clang in C++, but we skip it anyway. */
static int
fn_22_skip_body( void )
{
    /* TODO: IIRC the idea was to have something which libclang does not accept
             but other compilers do, but when clang is that 'other' compiler
             than it already cannot build the target lib */
    int __builtin_LINE( void );
    return __builtin_LINE();
}

int
fn_23_ellipsis_without_wrapping_va_list_variant( const char*, ... );

typedef int t_24[2];

int
fn_24a_typedef_array_arg(t_24*);

// the canonical type here is int (*)[2], which is not allowed as a return type (I think).
// There we have to preserve the typedef
t_24*
fn_24b_typedef_array_return(void);

#ifdef __cplusplus
};
#endif
