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
 * @file MPIFunc.cpp
 *
 * Class representing MPI function prototypes
 */

#include <string>
#include "SILC_Wrapgen_Util.h"
#include "SILC_Wrapgen_MpiFunc.h"
using namespace SILC::Wrapgen;

SILC::Wrapgen::MPIFunc::MPIFunc( const string& rtype, const string& name,
                                 const string& group, const string& guard,
                                 const string& version,
                                 const paramlist_t& params ) :
    Func( rtype, name, group, guard, params ), m_scnt( "0" ), m_rcnt( "0" ),
    m_version( string2int( version ) )
{
    set_family( "mpi" );
};

string
SILC::Wrapgen::MPIFunc::write_conf
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
SILC::Wrapgen::MPIFunc::set_sendcount_rule
(
    const string& rule
)
{
    m_scnt = rule;
}

void
SILC::Wrapgen::MPIFunc::set_recvcount_rule
(
    const string& rule
)
{
    m_rcnt = rule;
}
