/*
 * This file is part of the SILC project (http://www.silc.de)
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

#ifndef SILC_WRAPGEN_HANDLERS_MPI_DATATYPES_H_
#define SILC_WRAPGEN_HANDLERS_MPI_DATATYPES_H_

/**
 * @file       SILC_Wrapgen_Handlers_MpiDatatypes.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Helper functions for MPI handlers.
 */


#include "SILC_Wrapgen_Funcparam.h"

namespace SILC
{
namespace Wrapgen
{
namespace handler
{
namespace mpi
{
namespace datatype
{
/**
 * Check for pointers to transfer buffers
 */
bool
is_pointer_to_buffer
(
    const Funcparam& param
)
{
    return param.get_type() == "void*";
}

bool
is_char_pointer
(
    const Funcparam& param
)
{
    return param.get_type() == "char*";
}

bool
is_handler_function
(
    const Funcparam& param
)
{
    return ( param.get_type().find( "_function" ) != string::npos ) ||
           ( param.get_type().find( "handler" )   != string::npos );
}

bool
is_pointer
(
    const Funcparam& param
)
{
    return ( param.get_type().find( "*" )     != string::npos ) ||
           ( param.get_name().find( "array" ) != string::npos ) ||
           ( param.get_suffix().find( "[]" )  != string::npos );
}

bool
is_special_int
(
    const Funcparam& param
)
{
    return ( param.get_type().find( "MPI_Aint" )   != string::npos ) ||
           ( param.get_type().find( "MPI_Offset" ) != string::npos );
}

bool
is_input_param
(
    const Funcparam& param
)
{
    return param.get_atype() == 'i';
}

bool
is_output_param
(
    const Funcparam& param
)
{
    return param.get_atype() == 'o';
}

bool
is_input_output_param
(
    const Funcparam& param
)
{
    return param.get_atype() == 'b';
}

bool
is_range_triplet
(
    const Funcparam& param
)
{
    return param.get_name() == "ranges";
}

bool
is_array
(
    const Funcparam& param
)
{
    return param.get_name().find( "array" ) != string::npos;
}

bool
needs_output_handling
(
    const Funcparam& param
)
{
    return is_output_param( param ) ||
           is_input_output_param( param );
}

bool
needs_cast
(
    const Funcparam& param
)
{
    return is_handler_function( param ) ||
           is_range_triplet( param ) ||
           param.get_name().find( "Request" ) != string::npos;
}
}               // namespace datatypes
}               // namespace mpi
}               // namespace handler
}               // namespace wrapgen
}               // namespace SILC

#endif
