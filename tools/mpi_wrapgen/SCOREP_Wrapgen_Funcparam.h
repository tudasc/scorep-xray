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

#ifndef SCOREP_WRAPGEN_FUNCPARAM_H_
#define SCOREP_WRAPGEN_FUNCPARAM_H_

/**
 * @file       SCOREP_Wrapgen_Funcparam.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing MPI function parameters
 */

#include <string>
#include <set>

namespace SCOREP
{
namespace Wrapgen
{
enum access_type { IN, OUT, BOTH };

/**
 * Helper Class for representing function parameters.
 */
class Funcparam
{
public:
    /**
       Creates a function parameter object with full set of attributes
     */
    Funcparam( const std::string &type,
               const std::string &name,
               const std::string &suffix,
               char               atype );

    /**
       Returns the type of the parameter as a string. The type includes pointer (*) and
       reference (&) types.
     */
    std::string
    get_type
        ()   const
    {
        return m_type;
    }

    /**
       Returns the name of the parameter
     */
    std::string
    get_name
        ()   const
    {
        return m_name;
    }

    /**
       Returns any suffix for the parameter. E.g., an array specification
     */
    std::string
    get_suffix
        () const
    {
        return m_suffix;
    }

    /**
       Returns character indicating the access type. It can be 'i' for input, 'o' for
       output, or 'b' for both.
     */
    char
    get_atype
        ()  const;

    /**
       Adds a special tag to the parameter.
     */
    void
    add_special( std::string tag )
    {
        m_special.insert( tag );
    }

    /**
       Returns whether the parameter has a special tag.
     */
    bool
    has_special_tag( std::string tag ) const
    {
        return m_special.find( tag ) != m_special.end();
    }

private:
    std::string m_type;           // Type; includes pointer (*) and reference (&) types
    std::string m_name;
    std::string m_suffix;         // Type suffix; typically array specifications
    access_type m_atype;          // Type of Argument (i,o,b) Input,Ouput,Both
    /**
       Special tag for a parameter. Used to support deprecated functions.
     */
    std::set<std::string> m_special;
};
}
}

#endif
