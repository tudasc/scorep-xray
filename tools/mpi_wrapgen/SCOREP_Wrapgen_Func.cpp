/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Wrapgen_Func.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing a function prototype.
 */

#include <config.h>

#include <string>
using std::string;
#include "SCOREP_Wrapgen_Util.h"
#include "SCOREP_Wrapgen_Func.h"
using namespace SCOREP::Wrapgen;

SCOREP::Wrapgen::Func::Func( const string&      rtype,
                             const string&      name,
                             const string&      group,
                             const string&      guard,
                             const paramlist_t& params ) :
    m_id( int2string( num_func++ ) ), m_rtype( rtype ), m_name( name ),
    m_group( group ), m_guard( guard ), m_family( "default" ), m_params( params )
{
}

SCOREP::Wrapgen::Func::Func( const string& rtype,
                             const string& name,
                             const string& group,
                             const string& guard ) :
    m_id( int2string( num_func++ ) ), m_rtype( rtype ), m_name( name ),
    m_group( group ), m_guard( guard )
{
}

string
SCOREP::Wrapgen::Func::generate_wrapper
(
    const string& filename
) const
{
    return "";
}

string
SCOREP::Wrapgen::Func::write_conf
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
SCOREP::Wrapgen::Func::set_params
(
    const paramlist_t& params
)
{
    m_params = params;
}

SCOREP::Wrapgen::Func::~Func()
{
}

int SCOREP::Wrapgen::Func::num_func = 0;

void
SCOREP::Wrapgen::Func::add_param
(
    const string& type,
    const string& name,
    const string& suffix
)
{
    m_params.push_back( Funcparam( type, name, suffix, 'i' ) );
}
