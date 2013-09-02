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
 * @file stringutil.h
 *
 * Strings utility functions
 */


#ifndef STRUTIL_H_INCLUDED
#define STRUTIL_H_INCLUDED


#include <string>
#include <iostream>
#include <utility>


size_t
strskip_ws( std::string str,
            int         pos );

size_t
get_token( std::string  str,
           int          pos,
           std::string  delim,
           std::string& tok );

size_t
get_key_value_pair( std::string str,
                    int pos,
                    std::pair< std::string, std::string >& kvpair );


#endif /* STRUTIL_H_INCLUDED */
