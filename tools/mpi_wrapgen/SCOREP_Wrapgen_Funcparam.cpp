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
 * @file       SCOREP_Wrapgen_Funcparam.cpp
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing MPI function parameters
 */

#include <config.h>

#include <string>
using std::string;
#include "SCOREP_Wrapgen_Funcparam.h"
using namespace SCOREP::Wrapgen;

SCOREP::Wrapgen::Funcparam::Funcparam( const string& type_modifier,
                                       const string& type,
                                       const string& name,
                                       const string& suffix,
                                       char          atype ) :
    m_type_modifier( type_modifier ), m_type( type ), m_name( name ), m_suffix( suffix )
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
