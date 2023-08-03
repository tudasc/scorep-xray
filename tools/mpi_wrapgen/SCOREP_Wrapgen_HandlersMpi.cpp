/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2018-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
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
 * @file       SCOREP_Wrapgen_HandlersMpi.cpp
 * @ingroup    Wrapgen_module
 *
 * @brief Output handlers for template variables in MPI wrappers.
 */

#include <config.h>

#include <string>
using std::string;
#include "SCOREP_Wrapgen_Func.h"
#include "SCOREP_Wrapgen_Funcparam.h"
#include "SCOREP_Wrapgen_MpiFunc.h"
#include "SCOREP_Wrapgen_Handlers.h"
#include "SCOREP_Wrapgen_HandlersMpi.h"
#include "SCOREP_Wrapgen_HandlersMpiDatatypes.h"
#include "SCOREP_Wrapgen_Util.h"

#include <cstdlib>
#include <iostream>

using namespace SCOREP::Wrapgen;
using namespace SCOREP::Wrapgen::handler;
using namespace SCOREP::Wrapgen::handler::mpi;

/* static Variables */

/** @internal
 * Map to enable MPI-Type to MPI-F2C-call conversion. It is needed
 * as the type MPI_Datatype is handled by MPI_Type_* function calls
 */
static handler::info_t f2c_types;

/**
 * Initialize and assign handlers for MPI family.
 * These include:
 */
void
SCOREP::Wrapgen::handler::mpi::_initialize
    ()
{
    /** - Func-object template handlers */
    funcarg_handlers[ "attribute" ]     = handler::mpi::attribute;
    funcarg_handlers[ "proto:c" ]       = handler::mpi::proto_c;
    funcarg_handlers[ "proto:fortran" ] = handler::mpi::proto_fortran;
    funcarg_handlers[ "proto:f2c_c2f" ] = handler::mpi::proto_f2c_c2f;

    func_handlers[ "call:f2c_c2f" ]    = handler::mpi::call_f2c_c2f;
    func_handlers[ "call:fortran" ]    = handler::mpi::call_fortran;
    func_handlers[ "call:pmpi" ]       = handler::mpi::call_pmpi;
    func_handlers[ "cleanup" ]         = handler::mpi::cleanup;
    func_handlers[ "cleanup:f2c_c2f" ] = handler::mpi::cleanup_f2c_c2f;
    func_handlers[ "cleanup:fortran" ] = handler::mpi::cleanup_fortran;
    func_handlers[ "decl" ]            = handler::mpi::decl;
    func_handlers[ "decl:f2c_c2f" ]    = handler::mpi::decl_f2c_c2f;
    func_handlers[ "decl:fortran" ]    = handler::mpi::decl_fortran;
    func_handlers[ "group" ]           = handler::mpi::group;
    func_handlers[ "id" ]              = handler::mpi::id;
    func_handlers[ "init" ]            = handler::mpi::init;
    func_handlers[ "init:f2c_c2f" ]    = handler::mpi::init_f2c_c2f;
    func_handlers[ "init:fortran" ]    = handler::mpi::init_fortran;
    func_handlers[ "mpi:sendcount" ]   = handler::mpi::send_rule;
    func_handlers[ "mpi:recvcount" ]   = handler::mpi::recv_rule;
    func_handlers[ "mpi:version" ]     = handler::mpi::version;
    func_handlers[ "name" ]            = handler::mpi::name;
    func_handlers[ "proto:c" ]         = handler::mpi::proto_c;
    func_handlers[ "proto:fortran" ]   = handler::mpi::proto_fortran;
    func_handlers[ "proto:f2c_c2f" ]   = handler::mpi::proto_f2c_c2f;
    func_handlers[ "rtype" ]           = handler::mpi::rtype;
    func_handlers[ "xblock" ]          = handler::mpi::xblock;
    func_handlers[ "xblock:pre" ]      = handler::mpi::xblock_pre;
    func_handlers[ "xblock:post" ]     = handler::mpi::xblock_post;
    func_handlers[ "xblock:fortran" ]  = handler::mpi::xblock_fortran;
    func_handlers[ "xblock:f2c_c2f" ]  = handler::mpi::xblock_f2c_c2f;
    func_handlers[ "guard:start" ]     = handler::mpi::guard_start;
    func_handlers[ "guard:end" ]       = handler::mpi::guard_end;
    func_handlers[ "comm:new" ]        = handler::mpi::comm_new;
    func_handlers[ "comm:parent" ]     = handler::mpi::comm_parent;
    func_handlers[ "group:new" ]       = handler::mpi::group_new;

    /** - Fortran<->C conversion types */
    f2c_types[ "MPI_Status" ]   = "PMPI_Status";
    f2c_types[ "MPI_Comm" ]     = "PMPI_Comm";
    f2c_types[ "MPI_Group" ]    = "PMPI_Group";
    f2c_types[ "MPI_Request" ]  = "PMPI_Request";
    f2c_types[ "MPI_File" ]     = "PMPI_File";
    f2c_types[ "MPI_Win" ]      = "PMPI_Win";
    f2c_types[ "MPI_Op" ]       = "PMPI_Op";
    f2c_types[ "MPI_Datatype" ] = "PMPI_Type";
    f2c_types[ "MPI_Info" ]     = "PMPI_Info";
    f2c_types[ "MPI_Message" ]  = "PMPI_Message";
    f2c_types[ "MPI_Session" ]  = "PMPI_Session";
}

string
SCOREP::Wrapgen::handler::mpi::attribute
(
    const Func&   func,
    const string& attribute
)
{
    return func.get_attribute( attribute );
}

string
SCOREP::Wrapgen::handler::mpi::call_f2c_c2f
(
    const Func& func
)
{
    /*
     * In Fortran all values are given by reference. These need
     * to be dereferenced for the C call.
     * Additionally, some types need to be handled by appropriate
     * C <-> Fortran handlers
     */
    string str      = func.get_name() + "(";
    bool   f2c_used = false;

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        /* add a comma, if this is not the first parameter */
        if ( i )
        {
            str += ", ";
        }
        /* initialize flag */
        f2c_used = false;

        const Funcparam&       arg          = func.get_param( i );
        string                 arg_type     = arg.get_type();
        string                 arg_name     = arg.get_name();
        string                 striped_type = arg_type.substr( 0, arg_type.find_first_of( "*" ) );
        info_t::const_iterator it           = f2c_types.find( striped_type );

        /* do we need to handle the parameter with f2c conversion? */
        if ( it != f2c_types.end() )
        {
            if ( datatype::is_array( arg ) )
            {
                /* parameter is a local array */
                str     += "c_" + arg_name;
                f2c_used = true;
            }
            else if ( datatype::is_input_param( arg ) &&
                      !( striped_type == "MPI_Status" ) )
            {
                /* wrap argument with f2c call */
                str     += it->second + "_f2c(*" + arg_name + ")";
                f2c_used = true;
            }
            /* output parameter needs a local variable */
            else
            {
                if ( ( datatype::is_pointer( arg ) )
                     && !( striped_type == "MPI_Status" ) )
                {
                    str += "&";
                }

                str += "c_" + arg_name;

                if ( ( datatype::is_pointer( arg ) )
                     && ( striped_type == "MPI_Status" ) )
                {
                    str += "_ptr";
                }
                f2c_used = true;
            }
        }
        //if no F2C used, we still need to handle the argument
        if ( !f2c_used )
        {
            // create appropriate cast if needed
            if ( datatype::needs_cast( arg ) )
            {
                /* ranges on MPI_Group_incl/excl need special cast */
                if ( datatype::is_range_triplet( arg ) )
                {
                    str += "(int(*)[3])";
                }
                else if ( datatype::is_handler_function( arg ) )
                {
                    if ( datatype::is_pointer( arg ) )
                    {
                        str += "(" + arg_type + ")";
                    }
                    else
                    {
                        str += "*((" + arg_type + "*)" + arg_name + ")";
                    }
                }
                else
                {
                    if ( datatype::is_pointer( arg ) )
                    {
                        str += "*(" + arg_type + ")";
                    }
                    else
                    {
                        str += "(" + arg_type + ")";
                    }
                }
            }
            // dereference non-pointer arguments which are not casted
            else if ( !datatype::is_pointer( arg ) )
            {
                if ( arg.has_special_tag( "noaint" ) )
                {
                    if ( datatype::is_input_param( arg ) )
                    {
                        str += "c_";
                    }
                    else
                    {
                        str += "&c_";
                    }
                }
                else
                {
                    str += "*";
                }
            }
            else if ( datatype::is_char_pointer( arg ) )
            {
                // character arrays from Fortran are not null-terminated
                // thus we have to use our own internal character array,
                // initialized in a preceding declaration and init block
                str += "c_";
            }
            else if ( arg.has_special_tag( "noaint" ) )
            {
                if ( datatype::is_pointer( arg )
                     && datatype::is_input_param( arg ) )
                {
                    str += "c_";
                }
                else
                {
                    str += "&c_";
                }
            }

            if ( !( datatype::needs_cast( arg ) &&
                    datatype::is_handler_function( arg ) &&
                    !datatype::is_pointer( arg ) ) )
            {
                str += arg_name;
            }
        }
    }
    str +=  ");";

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::call_fortran
(
    const Func& func
)
{
    // In Fortran all values are given by reference. These need
    // to be dereferenced for the C call.
    string str = func.get_name() + "(";

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg      = func.get_param( i );
        string           arg_name = arg.get_name();
        string           arg_type = arg.get_type();

        if ( i != 0 )
        {
            str += ", ";
        }

        if ( !( datatype::is_pointer( arg ) || datatype::is_range_triplet( arg ) ) )
        {
            // de-reference argument
            str += '*';
        }

        if ( datatype::is_char_pointer( arg ) )
        {
            // character arrays from Fortran are not null-terminated
            // thus we have to use our own internal character array,
            // initialized in a preceding declaration and init block
            str += "c_" + arg_name;
        }
        else if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "c_" + arg_name;
            }
            else
            {
                str += "&c_" + arg_name;
            }
        }
        else
        {
            // argument is a pointer in the original definition,
            // thus use it as it is
            str += arg_name;
        }
    }
    str += ");";

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::call_pmpi
(
    const Func& func
)
{
    string str = "P" + func.get_name() + "(";

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg = func.get_param( i );

        if ( i )
        {
            str += ", ";
        }

        str += arg.get_name();
    }
    str += ")";

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::cleanup
(
    const Func& func
)
{
    return func.get_cleanup_block();
}

string
SCOREP::Wrapgen::handler::mpi::cleanup_fortran
(
    const Func& func
)
{
    string str = func.get_cleanup_block( "fortran" );

    // Fortran wrappers need additional cleanup
    // depending on call parameters
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg      = func.get_param( i );
        string           arg_name = arg.get_name();

        if ( datatype::is_char_pointer( arg ) )
        {
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            // Output parameters need to be copied and padded with spaces
            if ( datatype::is_output_param( arg ) )
            {
                str += "\n  c_" + arg_name + "_len = strlen(c_" + arg_name + ");\n  " +
                       "memcpy(" + arg_name + ", c_" + arg_name + ", c_" + arg_name + "_len);\n  " +
                       "memset(" + arg_name + " + c_" + arg_name + "_len, ' ', " + arg_name + "_len - c_" + arg_name + "_len);\n";
            }
            str += "free(c_" + arg_name + ");";
        }

        /* do we need special handling based on tags? */
        if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_output_param( arg ) )
            {
                str += "*" + arg_name + " = (int) c_" + arg_name + ";\n";
                str += "if (*" + arg_name + " != c_" + arg_name + ")\n";
                str += "  UTILS_DEBUG_PRINTF(SCOREP_DEBUG_MPI, "
                       "\"Value truncated in \\\"" + func.get_name() +
                       "\\\". Function is deprecated due to mismatching parameter types! "
                       "Consult the MPI Standard for more details.\");\n";
            }

            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "free(c_" + arg_name + ");\n";
            }
        }
    }

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::cleanup_f2c_c2f
(
    const Func& func
)
{
    string str = func.get_cleanup_block( "f2c_c2f" );

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam&       arg          = func.get_param( i );
        string                 arg_type     = arg.get_type();
        string                 arg_name     = arg.get_name();
        string                 striped_type = arg_type.substr( 0, arg_type.find_first_of( "*" ) );
        info_t::const_iterator it           = f2c_types.find( striped_type );

        if ( datatype::is_char_pointer( arg ) )
        {
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            // Output parameters need to be copied and padded with spaces
            if ( datatype::is_output_param( arg ) )
            {
                str += "\n  c_" + arg_name + "_len = strlen(c_" + arg_name + ");\n  " +
                       "memcpy(" + arg_name + ", c_" + arg_name + ", c_" + arg_name + "_len);\n  " +
                       "memset(" + arg_name + " + c_" + arg_name + "_len, ' ', " + arg_name + "_len - c_" + arg_name + "_len);\n";
            }
            str += "free(c_" + arg_name + ");";
        }

        /* do we need special handling based on tags? */
        if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_output_param( arg ) )
            {
                str += "*" + arg_name + " = (MPI_Fint) c_" + arg_name + ";\n";
                str += "if (*" + arg_name + " != c_" + arg_name + ")\n";
                str += "  UTILS_DEBUG_PRINTF(SCOREP_DEBUG_MPI, "
                       "\"Value truncated in \\\"" + func.get_name() +
                       "\\\". Function is deprecated due to mismatching parameter types! "
                       "Consult the MPI Standard for more details.\");\n";
            }
            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "free(c_" + arg_name + ");\n";
            }
        }

        /* do we need to handle the parameter with f2c conversion? */
        if ( it != f2c_types.end() &&
             datatype::needs_output_handling( arg ) )
        {
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            if ( striped_type == "MPI_Status" )
            {
                str += "    if (c_" + arg_name + "_ptr != MPI_STATUS_IGNORE)\n";
                str += "    {\n";
                str += "      PMPI_Status_c2f(c_" + arg.get_name() + "_ptr, " + arg.get_name() + ");";
                str += "    }\n";
            }
            else
            {
                str += "*" + arg_name + " = " + it->second + "_c2f(c_" + arg_name + ");";
            }
        }
    }
    return str;
}

string
SCOREP::Wrapgen::handler::mpi::decl
(
    const Func& func
)
{
    return func.get_decl_block();
}

string
SCOREP::Wrapgen::handler::mpi::decl_f2c_c2f
(
    const Func& func
)
{
    string str;
    // Fortran wrappers might need additional declarations,
    // depending on call parameters
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam&       arg          = func.get_param( i );
        string                 arg_name     = arg.get_name();
        string                 arg_type     = arg.get_type();
        string                 striped_type = arg_type.substr( 0, arg_type.find_first_of( "*" ) );
        info_t::const_iterator it           = f2c_types.find( striped_type );


        if ( datatype::is_char_pointer( arg ) )
        {
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            str += "char* c_" + arg_name + " = NULL;";
            if ( datatype::is_output_param( arg ) )
            {
                str += "\nsize_t c_" + arg_name + "_len = 0;";
            }
        }
        else if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "int i;\n";
                str += "MPI_Aint* c_" + arg_name + ";\n";
            }
            else
            {
                if ( datatype::is_input_param( arg )
                     || datatype::is_input_output_param( arg ) )
                {
                    str += "MPI_Aint c_" + arg_name + " = *" + arg_name + ";\n";
                }
                else
                {
                    str += "MPI_Aint c_" + arg_name + ";";
                }
            }
        }

        if ( it != f2c_types.end() )
        {
            if ( ( datatype::needs_input_handling( arg ) &&
                   striped_type == "MPI_Status" )
                 || datatype::needs_output_handling( arg ) )
            {
                str += striped_type + " c_" + arg.get_name();
            }

            if ( datatype::is_input_output_param( arg )
                 && striped_type != "MPI_Status" )
            {
                // initialize with f2c'ed input value
                str += " = " + it->second + "_f2c(*" + arg.get_name() + ");";
            }
            else
            {
                if ( ( datatype::needs_input_handling( arg ) &&
                       striped_type == "MPI_Status" )
                     || datatype::needs_output_handling( arg ) )
                {
                    str += ";";
                }
            }
        }
    }

    str += func.get_decl_block( "f2c_c2f" );

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::decl_fortran
(
    const Func& func
)
{
    string str;

    // Fortran wrappers might need additional declarations,
    // depending on call parameters
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg      = func.get_param( i );
        string           arg_name = arg.get_name();
        string           arg_type = arg.get_type();

        if ( datatype::is_char_pointer( arg ) )
        {
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            str += "char* c_" + arg_name + " = NULL;";
            if ( datatype::is_output_param( arg ) )
            {
                str += "\nsize_t c_" + arg_name + "_len = 0;";
            }
        }
        else if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "MPI_Aint* c_" + arg_name + ";\n";
            }
            else
            {
                str += "MPI_Aint c_" + arg_name;
                if ( datatype::is_input_param( arg ) )
                {
                    str += " = *" + arg_name;
                }
                str += ";\n";
            }
        }
    }

    str += func.get_decl_block( "fortran" );

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::group
(
    const Func& func
)
{
    return func.get_group();
}

string
SCOREP::Wrapgen::handler::mpi::id
(
    const Func& func
)
{
    return func.get_id();
}

string
SCOREP::Wrapgen::handler::mpi::init
(
    const Func& func
)
{
    return func.get_init_block();
}

string
SCOREP::Wrapgen::handler::mpi::init_f2c_c2f
(
    const Func& func
)
{
    string str;

    // Fortran wrappers might need additional initializations,
    // depending on call parameters
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam&       arg          = func.get_param( i );
        string                 arg_name     = arg.get_name();
        string                 arg_type     = arg.get_type();
        string                 striped_type = arg_type.substr( 0, arg_type.find_first_of( "*" ) );
        info_t::const_iterator it           = f2c_types.find( striped_type );

        if ( it != f2c_types.end() )
        {
            // MPI_Status needs to be handled seperately as the
            // conversion is a subroutine, not a function
            if ( striped_type == "MPI_Status" )
            {
                if ( str != "" )
                {
                    str += "\n    ";
                }
                // declare pointer to C status for subsequent use
                str += striped_type + "* c_" + arg.get_name()
                       + "_ptr = &c_" + arg.get_name() + ";";

                if ( datatype::is_input_output_param( arg ) ||
                     datatype::is_input_param( arg ) )
                {
                    str += "\n    ";
                    str += it->second + "_f2c(" + arg.get_name()
                           + ", c_" + arg.get_name() + "_ptr);";
                }
            }
        }

        if ( datatype::is_char_pointer( arg ) )
        {
            // start new line if we already handled other parameters
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            if ( datatype::is_input_param( arg ) )
            {
                // char* with input need copy and trim
                str += "c_" + arg_name + " = scorep_mpi_f2c_string( " + arg_name
                       + ", " + arg_name + "_len );\n    ";
            }
            else
            {
                // char* need allocation
                str += "c_" + arg_name + " = (char*) malloc((" + arg_name
                       + "_len + 1) * sizeof(char));\n  "
                       + "if (!c_" + arg_name + ") exit(EXIT_FAILURE);\n  ";
            }
        }

        if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "c_" + arg_name + " = (" + arg_type + ") malloc(*count * sizeof(MPI_Aint));\n";
                str += "for(i = 0; i < *count; ++i)\n"
                       "  c_" + arg_name + "[i] = " + arg_name + "[i];\n";
            }
        }
    }

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::init_fortran
(
    const Func& func
)
{
    string str;

    // Fortran wrappers might need additional initializations,
    // depending on call parameters
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg      = func.get_param( i );
        string           arg_name = arg.get_name();
        string           arg_type = arg.get_type();

        if ( datatype::is_char_pointer( arg ) )
        {
            // start new line if we already handled other parameters
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            if ( datatype::is_input_param( arg ) )
            {
                // char* with input need copy and trim
                str += "c_" + arg_name + " = scorep_mpi_f2c_string( " + arg_name
                       + ", " + arg_name + "_len );\n    ";
            }
            else
            {
                // char* need allocation
                str += "c_" + arg_name + " = (char*) malloc((" + arg_name
                       + "_len + 1) * sizeof(char));\n  "
                       + "if (!c_" + arg_name + ") exit(EXIT_FAILURE);\n  ";
            }
        }

        if ( arg.has_special_tag( "noaint" ) )
        {
            if ( datatype::is_pointer( arg )
                 && datatype::is_input_param( arg ) )
            {
                str += "c_" + arg_name + " = (" + arg_type + ") malloc(*count * sizeof(MPI_Aint));\n";
                str += "for(int i = 0; i < *count; ++i)\n"
                       "  c_" + arg_name + "[i] = " + arg_name + "[i];\n";
            }
        }
    }

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::name
(
    const Func& func
)
{
    return func.get_name();
}

string
SCOREP::Wrapgen::handler::mpi::proto_c
(
    const Func&        func,
    const std::string& name_prefix
)
{
    string str = func.get_rtype() + " " + name_prefix + func.get_name() + "(";

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        // add comma, if it is not the first parameter
        if ( i )
        {
            str += ", ";
        }

        const Funcparam& arg = func.get_param( i );

        const string& type_modififier_as_string = arg.get_type_modifier();
        str += type_modififier_as_string;
        if ( type_modififier_as_string.length() == 0 )
        {
            str += "";
        }
        else
        {
            str += " ";
        }
        str += arg.get_type() + " " + arg.get_name() + arg.get_suffix();
    }
    str += ")";
    return str;
}

string
SCOREP::Wrapgen::handler::mpi::proto_c
(
    const Func& func
)
{
    return proto_c( func, "" );
}

string
SCOREP::Wrapgen::handler::mpi::proto_fortran
(
    const Func&        func,
    const std::string& name_suffix
)
{
    string str = "void FSUB(" + func.get_name() + name_suffix + ")(";
    //string str = "void " + func.get_name() + "(";
    string c_str_lengths;

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg      = func.get_param( i );
        string           arg_type = arg.get_type();
        string           arg_name = arg.get_name();

        // add comma, if it is not the first parameter
        if ( i )
        {
            str += ", ";
        }

        if ( datatype::is_char_pointer( arg ) )
        {
            // most compilers add an additional 'int' parameter for each
            // char-pointer in the parameter list
            c_str_lengths += ", scorep_fortran_charlen_t " + arg_name + "_len";
            str           += arg_type + " ";
        }
#if 0
        else if ( datatype::is_range_triplet( arg ) )
        {
            str += "MPI_Fint ";
        }
        else if ( datatype::is_special_int( arg ) )
        {
            if ( datatype::is_pointer( arg ) )
            {
                str += arg_type + " ";
            }
            else
            {
                str += arg_type + "* ";
            }
        }
        else if ( datatype::is_handler_function( arg ) ||
                  datatype::is_pointer_to_buffer( arg ) )
        {
            str += "void* ";
        }
#endif
        else
        {
            /*
               str += "MPI_Fint* ";
             */
            if ( arg_type.find( "MPI_Aint" ) != string::npos &&
                 arg.has_special_tag( "noaint" ) )
            {
                str += "int* ";
            }
            else
            {
                str += arg_type;

                if ( datatype::is_pointer( arg ) )
                {
                    str += " ";
                }
                else
                {
                    str += "* ";
                }
            }
        }

        str += arg.get_name() + arg.get_suffix();
        /*
           str += arg_name;
           if (datatype::is_range_triplet(arg) ||
           (datatype::is_special_int(arg) && datatype::is_pointer(arg)))
           {
            str += arg.get_suffix();
           }
         */
    }
    if ( func.get_param_count() != 0 )
    {
        str += ", ";
    }
    str += "int* ierr" + c_str_lengths + ")";

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::proto_fortran
(
    const Func& func
)
{
    return proto_fortran( func, "" );
}

string
SCOREP::Wrapgen::handler::mpi::proto_f2c_c2f
(
    const Func&        func,
    const std::string& name_suffix
)
{
    string str = "void FSUB(" + func.get_name() + name_suffix + ")(";
    //string str = "void " + func.get_name() + "(";
    string c_str_lengths;

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg      = func.get_param( i );
        string           arg_type = arg.get_type();
        string           arg_name = arg.get_name();

        // add comma, if it is not the first parameter
        if ( i )
        {
            str += ", ";
        }

        if ( datatype::is_char_pointer( arg ) )
        {
            // most compilers add an additional 'int' parameter for each
            // char-pointer in the parameter list
            c_str_lengths += ", scorep_fortran_charlen_t " + arg_name + "_len";
            str           += arg_type + " ";
        }
        else if ( datatype::is_range_triplet( arg ) )
        {
            str += "MPI_Fint ";
        }
        else if ( datatype::is_special_int( arg ) )
        {
            if ( arg_type.find( "MPI_Aint" ) != string::npos &&
                 arg.has_special_tag( "noaint" ) )
            {
                str += "MPI_Fint* ";
            }
            else
            {
                str += arg_type;

                if ( datatype::is_pointer( arg ) )
                {
                    str += " ";
                }
                else
                {
                    str += "* ";
                }
            }
        }
        else if ( datatype::is_handler_function( arg ) ||
                  datatype::is_pointer_to_buffer( arg ) )
        {
            str += "void* ";
        }
        else
        {
            str += "MPI_Fint* ";
        }

        str += arg_name;
        if ( datatype::is_range_triplet( arg ) ||
             ( datatype::is_special_int( arg ) && datatype::is_pointer( arg ) ) )
        {
            str += arg.get_suffix();
        }
    }
    if ( func.get_param_count() != 0 )
    {
        str += ", ";
    }
    str += "MPI_Fint* ierr" + c_str_lengths + ")";

    return str;
}

string
SCOREP::Wrapgen::handler::mpi::proto_f2c_c2f
(
    const Func& func
)
{
    return proto_f2c_c2f( func, "" );
}

string
SCOREP::Wrapgen::handler::mpi::version
(
    const Func& func
)
{
    const MPIFunc mpifunc = dynamic_cast<const MPIFunc&>( func );
    // return MPI Version this call was introduced
    return mpifunc.get_version();
}

string
SCOREP::Wrapgen::handler::mpi::send_rule
(
    const Func& func
)
{
    const MPIFunc mpifunc = dynamic_cast<const MPIFunc&>( func );

    return mpifunc.get_sendcount_rule();
}

string
SCOREP::Wrapgen::handler::mpi::recv_rule
(
    const Func& func
)
{
    const MPIFunc mpifunc = dynamic_cast<const MPIFunc&>( func );

    return mpifunc.get_recvcount_rule();
}

string
SCOREP::Wrapgen::handler::mpi::rtype
(
    const Func& func
)
{
    return func.get_rtype();
}

string
SCOREP::Wrapgen::handler::mpi::xblock
(
    const Func& func
)
{
    return func.get_expr_block();
}

string
SCOREP::Wrapgen::handler::mpi::xblock_pre( const Func& func )
{
    return func.get_expr_block( "pre" );
}

string
SCOREP::Wrapgen::handler::mpi::xblock_post( const Func& func )
{
    return func.get_expr_block( "post" );
}

string
SCOREP::Wrapgen::handler::mpi::xblock_fortran( const Func& func )
{
    return func.get_expr_block( "fortran" );
}

string
SCOREP::Wrapgen::handler::mpi::xblock_f2c_c2f( const Func& func )
{
    return func.get_expr_block( "f2c_c2f" );
}

string
SCOREP::Wrapgen::handler::mpi::guard_start
(
    const Func& func
)
{
    string guard = "#if ";
    // write individual guard
    string tmp = func.get_name();
    toupper( tmp );

    const MPIFunc& mpifunc = dynamic_cast<const MPIFunc&>( func );
    string         version = mpifunc.get_version( '_' );

    // individual guard check by generated header
    guard += "HAVE(MPI_" + version + "_SYMBOL_P" + tmp + ")";

    if ( func.get_guard().length() > 0 )
    {
        vector<string> token;

        tokenize( func.get_guard(), ",", token );
        vector<string>::const_iterator it = token.begin();

        // write group guards
        while ( it != token.end() )
        {
            string guard_token = trim( *it );
            toupper( guard_token );

            guard += " && ! defined(SCOREP_MPI_NO_" + guard_token + ")";
            ++it;
        }

        // write guard to prevent wrapping if the function name is a define
        guard += " && ! defined( " + func.get_name() + " )";
    }

    return guard;
}

string
SCOREP::Wrapgen::handler::mpi::guard_end
(
    const Func& func
)
{
    return "#endif";
}

string
SCOREP::Wrapgen::handler::mpi::comm_new
(
    const Func& func
)
{
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg = func.get_param( i );
        if ( arg.has_special_tag( "commnew" ) )
        {
            return arg.get_name();
        }
    }
    std::cerr << "ERROR: Requesting a new communicator argument from function '"
              << func.get_name() << "' which does not have one."  << std::endl;
    exit( EXIT_FAILURE );
    return "";
}

string
SCOREP::Wrapgen::handler::mpi::comm_parent
(
    const Func& func
)
{
    int comm_new_arg_index    = -1;
    int comm_parent_arg_index = -1;
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg = func.get_param( i );
        if ( arg.has_special_tag( "commnew" ) )
        {
            comm_new_arg_index = i;
        }
        if ( arg.has_special_tag( "commparent" ) )
        {
            comm_parent_arg_index = i;
        }
    }
    if ( comm_new_arg_index < 0 )
    {
        std::cerr << "ERROR: Requesting a parent communicator argument from function '"
                  << func.get_name() << "' which does not have a new communicator."  << std::endl;
        exit( EXIT_FAILURE );
    }
    if ( comm_new_arg_index == comm_parent_arg_index )
    {
        std::cerr << "ERROR: The new and parent communicator argument from function '"
                  << func.get_name() << "' are the same arguments."  << std::endl;
        exit( EXIT_FAILURE );
    }
    if ( comm_parent_arg_index >= 0 )
    {
        return func.get_param( comm_parent_arg_index ).get_name();
    }

    // Use MPI_COMM_NULL, if no parent communicator is available
    return "MPI_COMM_NULL";
}

string
SCOREP::Wrapgen::handler::mpi::group_new
(
    const Func& func
)
{
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg = func.get_param( i );
        if ( arg.has_special_tag( "groupnew" ) )
        {
            return arg.get_name();
        }
    }
    std::cerr << "ERROR: Requesting a new group argument from function '"
              << func.get_name() << "' which does not have one."  << std::endl;
    exit( EXIT_FAILURE );
    return "";
}
