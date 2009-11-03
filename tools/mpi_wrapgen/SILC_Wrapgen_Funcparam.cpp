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
 * @file SILC_Wrapgen_Funcparam.cpp
 *
 * Class representing MPI function parameters
 */

#include <string>
using std::string;
#include "SILC_Wrapgen_Funcparam.h"
using namespace SILC::Wrapgen;

SILC::Wrapgen::Funcparam::Funcparam( const string& type, const string& name,
                                     const string& suffix, char atype ) :
    m_type( type ), m_name( name ), m_suffix( suffix )
{
    switch ( atype )
    {
        case 'i': m_atype = IN;
            break;
        case 'o': m_atype = OUT;
            break;
        case 'b': m_atype = BOTH;
            break;
    }
}

char
SILC::Wrapgen::Funcparam::get_atype
    () const
{
    switch ( m_atype )
    {
        case IN:   return 'i';
            break;
        case OUT:  return 'o';
            break;
        case BOTH: return 'b';
            break;
    }

    // return 'x' when input is malformed
    return 'x';
}
