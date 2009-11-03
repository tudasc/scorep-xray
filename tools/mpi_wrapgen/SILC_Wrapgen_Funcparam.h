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
 * @file SILC_Wrapgen_Funcparam.h
 *
 * Class representing MPI function parameters
 */
#ifndef SILC_WRAPGEN_FUNCPARAM_H_
#define SILC_WRAPGEN_FUNCPARAM_H_

#include <string>

namespace SILC {
namespace Wrapgen {
enum access_type { IN, OUT, BOTH };

/**
 * Helper Class for representing function parameters.
 */
class Funcparam {
public:
    /** Create a function parameter object with full set of attributes */
    Funcparam( const string &type, const string &name,
               const string &suffix, char atype );

    /** Returns the type of the parameter as a string. The type includes pointer (*) and
        reference (&) types.
     */
    string
    get_type
        ()   const
    {
        return m_type;
    }

    /** Returns the name of the parameter */
    string
    get_name
        ()   const
    {
        return m_name;
    }

    /** Returns any suffix for the parameter. E.g., an array specification */
    string
    get_suffix
        () const
    {
        return m_suffix;
    }

    /** Returns character indicating the access type. It can be 'i' for input, 'o' for
        output, or 'b' for both.
     */
    char
    get_atype
        ()  const;

private:
    string      m_type;      // Type; includes pointer (*) and reference (&) types
    string      m_name;
    string      m_suffix;    // Type suffix; typically array specifications
    access_type m_atype;     // Type of Argument (i,o,b) Input,Ouput,Both
};
}
}

#endif
