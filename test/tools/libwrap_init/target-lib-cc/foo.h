#include <iostream>

namespace ns
{
class class_1
{
public:
    typedef int type_1;

    class_1();
    class_1( type_1 n ) : m_var( n )
    {
    }

    type_1
    fn_1_get()  const
    {
        return m_var;
    }
    void
    fn_1b_set( int n )
    {
        m_var = static_cast<type_1>( n );
    }

private:
    type_1 m_var;
};

class class_2
{
public:
    typedef double type_1;     // same name as above typedef, but different type

    class_2( const class_1& c )
    {
        m_var = static_cast<type_1>( c.fn_1_get() );
        if ( m_var < 0 )
        {
            m_var = -m_var;
        }
    }

    type_1
    operator()() const;
    void
    operator()( type_1 );
    void
    operator()( type_1,
                class_1::type_1 );

    void
    fn_2_header_defined()
    {
        m_var = 0;
    }
    class_1
    fn_3_return_class_1() const;
    void
    fn_4_pointer_arg( type_1* x );
    void
    fn_5_ref_arg( type_1& y );

    static void
    fn_6_static();

private:
    void
    fn_7_private();

    type_1 m_var;
};

void
fn_8_global_in_namespace( int,
                          class_1& );
}

void
fn_9_global_use_class_1( int,
                         ns::class_1& );

void
fn_10_global_use_class_2( double );

void
fn_11_duplicate_decl();

void
fn_11_duplicate_decl();

extern "C"
{
void
fn_12_extern_c( void );

void
fn_13_extern_c_ellipsis( int,
                         ... );
};

template<int N>
void
fn_14_template_int( std::ostream& stream )
{
    stream << N;
}

template<>
void
fn_14_template_int<0>( std::ostream& stream ); // template specialization

template<typename T>
void
fn_15_template_func();

template <typename T>
class fn_16_class_template
{
public:
    void func( T );
};

template <typename T>
class fn_17_class_template_specialization
{
public:
    void
    func();
};

template<>
void
fn_17_class_template_specialization<int>::func();

class class_3
{
public:
    typedef int (* fn_t)( double );

    /* TODO doesn't work: scoped typedefs are generally a problem.
     *                    For arguments we use the canonical type,
     *                    instead of the given type.
     *                    But if we did that here, we would run into
     *                    the issue of fn_x_return_function_pointer
     *                    in target-lib-c. So we leave the typedeffed
     *                    types that contain function pointers as is.
       fn_t
       fn_18_scoped_typedeffed_function_pointer_return();*/

    int fn_19_scoped_typedeffed_function_pointer_arg( fn_t, double );
};
