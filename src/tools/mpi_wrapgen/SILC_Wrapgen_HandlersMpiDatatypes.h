/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2008                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SILC_WRAPGEN_HANDLERS_MPI_DATATYPES_H_
#define SILC_WRAPGEN_HANDLERS_MPI_DATATYPES_H_

#include "SILC_Wrapgen_Funcparam.h"

namespace SILC {
namespace Wrapgen {
namespace handler {
namespace mpi {
namespace datatype {
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
