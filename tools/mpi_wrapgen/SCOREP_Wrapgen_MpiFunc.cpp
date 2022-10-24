/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file       MPIFunc.cpp
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing MPI function prototypes.
 */

#include <config.h>

#include <string>
#include <iostream>
#include "SCOREP_Wrapgen_Util.h"
#include "SCOREP_Wrapgen_MpiFunc.h"
using namespace SCOREP::Wrapgen;

namespace
{
std::pair<int, int>
version_from_string( const string& s )
{
    istringstream str( s );

    int major;
    if ( !( str >> major ) )
    {
        return std::make_pair( -1, -1 );
    }

    char dot;
    if ( !( str >> dot ) )
    {
        /* in a match rule with just the major version, minor is a wildcard */
        return std::make_pair( major, -1 );
    }
    if ( dot != '.' )
    {
        return std::make_pair( -1, -1 );
    }

    int minor;
    if ( !( str >> minor ) )
    {
        return std::make_pair( -1, -1 );
    }

    return std::make_pair( major, minor );
}

string
version_to_string( const std::pair<int, int>& v,
                   char s = '.' )
{
    ostringstream o;
    o << v.first << s << v.second;
    return o.str();
}
}

SCOREP::Wrapgen::MPIFunc::MPIFunc( const string&      rtype,
                                   const string&      name,
                                   const string&      group,
                                   const string&      guard,
                                   const string&      version,
                                   const paramlist_t& params ) :
    Func( rtype, name, group, guard, params ), m_scnt( "0" ), m_rcnt( "0" ),
    m_version( version_from_string( version ) )
{
    set_family( "mpi" );
    set_attribute( "kind", "NONE" );
    set_attribute( "operation_type", "" );
    set_attribute( "collective_type", "" );

    /* we do not accept a wildcard version */
    if ( m_version.second == -1 )
    {
        std::cerr << "ERROR: Invalid MPI version for function '"
                  << get_name() << "': " << version << std::endl;
        exit( EXIT_FAILURE );
    }
};

string
SCOREP::Wrapgen::MPIFunc::write_conf
    () const
{
    ostringstream conf;

    conf << "<prototype name=\"" << get_name() << "\" rtype=\""
         << get_rtype() << "\" group=\"" << get_group() << "\" >\n";
    for ( size_t i = 0; i < get_param_count(); ++i )
    {
        Funcparam param = get_param( i );

        conf << "    <param access=\"" << param.get_atype()
             << "\" type=\"" << param.get_type() << "\" name=\""
             << param.get_name() << "\" suffix=\"" << param.get_suffix()
             << "\" />\n";
    }
    conf << "    <version id=\"" << version_to_string( m_version ) << "\" />\n";
    conf << "    <countrules send=\"" << m_scnt << "\" recv=\""
         << m_rcnt << "\" />\n";
    conf << "<init>\n" << m_init << "</init>\n";
    conf << "<xblock>\n" << m_expr_block << "</xblock>\n";
    conf << "</prototype>\n";

    return conf.str();
}

string
SCOREP::Wrapgen::MPIFunc::get_version( char separator ) const
{
    return version_to_string( m_version, separator );
}

bool
SCOREP::Wrapgen::MPIFunc::version_equal( const string& s ) const
{
    std::pair<int, int> v = version_from_string( s );

    if ( v.first == -1 )
    {
        std::cerr << "ERROR: Invalid MPI version in match rule '"
                  << s << "'" << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( m_version.first != v.first )
    {
        return false;
    }

    if ( v.second == -1 || m_version.second == v.second )
    {
        return true;
    }

    return false;
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
