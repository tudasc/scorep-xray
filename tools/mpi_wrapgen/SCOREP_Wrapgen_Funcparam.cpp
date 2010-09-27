/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       SCOREP_Wrapgen_Funcparam.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing MPI function parameters
 */

#include <config.h>

#include <string>
using std::string;
#include "SCOREP_Wrapgen_Funcparam.h"
using namespace SCOREP::Wrapgen;

SCOREP::Wrapgen::Funcparam::Funcparam( const string& type,
                                       const string& name,
                                       const string& suffix,
                                       char          atype ) :
    m_type( type ), m_name( name ), m_suffix( suffix )
{
    switch ( atype )
    {
        case 'i':
            m_atype = IN;
            break;
        case 'o':
            m_atype = OUT;
            break;
        case 'b':
            m_atype = BOTH;
            break;
    }
}

char
SCOREP::Wrapgen::Funcparam::get_atype
    () const
{
    switch ( m_atype )
    {
        case IN:
            return 'i';
            break;
        case OUT:
            return 'o';
            break;
        case BOTH:
            return 'b';
            break;
    }

    // return 'x' when input is malformed
    return 'x';
}
