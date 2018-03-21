#include "foo.h"

namespace ns
{
/* *** class 1 ***/

class_1::class_1()
    : m_var( 0 )
{
}

/* *** class 2 ***/

class_2::type_1
class_2::operator()() const
{
    return m_var;
}

void
class_2::operator()( type_1 m )
{
    m_var = m;
}

void
class_2::operator()( type_1 m, class_1::type_1 n )
{
    m_var = m + static_cast<type_1>( n );
}

class_1
class_2::fn_3_return_class_1() const
{
    return class_1( static_cast<class_1::type_1>( m_var ) );
}

void
class_2::fn_4_pointer_arg( type_1* x )
{
    *x = m_var;
}

void
class_2::fn_5_ref_arg( type_1& y )
{
    y = m_var;
}

void
class_2::fn_6_static()
{
}

void
class_2::fn_7_private()
{
    m_var = 0;
}

/* *** global ***/

void
fn_8_global_in_namespace( int n, class_1& c )
{
    c.fn_1b_set( n );
}
}; // namespace ns

void
fn_9_global_use_class_1( int n, ns::class_1& c )
{
    c.fn_1b_set( n );
}

void
fn_10_global_use_class_2( double d )
{
    ns::class_2 c( d );
}

void
fn_11_duplicate_decl()
{
}

extern "C"
{
void
fn_12_extern_c()
{
}

void
fn_13_extern_c_ellipsis( int, ... )
{
}
};

template<>
void
fn_14_template_int<0>( std::ostream& stream )
{
    stream << "null";
}

template<>
void
fn_17_class_template_specialization<int>::func()
{
}

/* TODO doesn't work: see declaration
   static int
   dummy(double d) {
    return (int) d;
   }

   class_3::fn_t
   class_3::fn_18_scoped_typedeffed_function_pointer_return()
   {
    return dummy;
   }*/

int
class_3::fn_19_scoped_typedeffed_function_pointer_arg( fn_t f, double d )
{
    return ( *f )( d );
}
