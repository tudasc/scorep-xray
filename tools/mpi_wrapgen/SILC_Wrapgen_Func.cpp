/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Wrapgen_Func.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing a function prototype.
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
    m_rtype << "\" group=\"" << m_group << "\" guard=\"" << m_guard << "\" >\n";
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

void
SILC::Wrapgen::Func::add_param
(
    const string& type,
    const string& name,
    const string& suffix
)
{
    m_params.push_back( Funcparam( type, name, suffix, 'i' ) );
}
