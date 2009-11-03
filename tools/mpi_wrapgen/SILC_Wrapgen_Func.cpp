/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
 * @file Func.cpp
 *
 * Class representing MPI function prototypes
 */

#include <string>
using std::string;
#include "SILC_Wrapgen_Util.h"
#include "SILC_Wrapgen_Func.h"
using namespace SILC::Wrapgen;

SILC::Wrapgen::Func::Func( const string& rtype, const string& name,
                           const string& group, const string& guard, const paramlist_t& params ) :
    m_id( int2string( num_func++ ) ), m_rtype( rtype ), m_name( name ),
    m_group( group ), m_guard( guard ), m_family( "default" ), m_decl_block( "" ),
    m_expr_block( "" ), m_params( params )
{
}

SILC::Wrapgen::Func::Func( const string& rtype, const string& name,
                           const string& group, const string& guard ) :
    m_id( int2string( num_func++ ) ), m_rtype( rtype ), m_name( name ),
    m_group( group ), m_guard( guard )
{
}

string
SILC::Wrapgen::Func::generate_wrapper
(
    const string& filename
) const
{
    return "";
}

string
SILC::Wrapgen::Func::write_conf
    () const
{
    ostringstream conf;

    conf << "<prototype name=\"" << m_name << "\" rtype=\"" <<
    m_rtype << "\" group=\"" << m_group << "\" >\n";
    for ( size_t i = 0; i < get_param_count(); ++i )
    {
        Funcparam param = get_param( i );

        conf << "    <param access=\"" << param.get_atype() <<
        "\" type=\"" << param.get_type() << "\" name=\"" <<
        param.get_name() << "\" suffix=\"" << param.get_suffix() <<
        "\" />\n";
    }
    conf << "</prototype>\n";

    return conf.str();
}

void
SILC::Wrapgen::Func::set_params
(
    const paramlist_t& params
)
{
    m_params = params;
}

SILC::Wrapgen::Func::~Func()
{
}

int SILC::Wrapgen::Func::num_func = 0;
