/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
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
 * @file       MPIFunc.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing MPI function prototypes.
 */

#include <config.h>

#include <string>
#include "SCOREP_Wrapgen_Util.h"
#include "SCOREP_Wrapgen_MpiFunc.h"
using namespace SCOREP::Wrapgen;

SCOREP::Wrapgen::MPIFunc::MPIFunc( const string&      rtype,
                                   const string&      name,
                                   const string&      group,
                                   const string&      guard,
                                   const string&      version,
                                   const paramlist_t& params ) :
    Func( rtype, name, group, guard, params ), m_scnt( "0" ), m_rcnt( "0" ),
    m_version( string2int( version ) )
{
    set_family( "mpi" );
};

string
SCOREP::Wrapgen::MPIFunc::write_conf
    () const
{
    ostringstream conf;

    conf << "<prototype name=\"" << get_name() << "\" rtype=\"" <<
    get_rtype() << "\" group=\"" << get_group() << "\" >\n";
    for ( size_t i = 0; i < get_param_count(); ++i )
    {
        Funcparam param = get_param( i );

        conf << "    <param access=\"" << param.get_atype() <<
        "\" type=\"" << param.get_type() << "\" name=\"" <<
        param.get_name() << "\" suffix=\"" << param.get_suffix() <<
        "\" />\n";
    }
    conf << "    <version id=\"" << m_version << "\" />\n";
    conf << "    <countrules send=\"" << m_scnt << "\" recv=\""
         << m_rcnt << "\" />\n";
    conf << "<init>\n" << m_init << "</init>\n";
    conf << "<xblock>\n" << m_expr_block << "</xblock>\n";
    conf << "</prototype>\n";

    return conf.str();
}

void
SCOREP::Wrapgen::MPIFunc::set_sendcount_rule
(
    const string& rule
)
{
    m_scnt = rule;
}

void
SCOREP::Wrapgen::MPIFunc::set_recvcount_rule
(
    const string& rule
)
{
    m_rcnt = rule;
}
