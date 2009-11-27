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

/**
 * @file       SILC_Wrapgen_HandlersMpi.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Output handlers for template variables in MPI wrappers.
 */

#include <string>
using std::string;
#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_Funcparam.h"
#include "SILC_Wrapgen_MpiFunc.h"
#include "SILC_Wrapgen_Handlers.h"
#include "SILC_Wrapgen_HandlersMpi.h"
#include "SILC_Wrapgen_HandlersMpiDatatypes.h"
#include "SILC_Wrapgen_Util.h"

using namespace SILC::Wrapgen;
using namespace SILC::Wrapgen::handler;
using namespace SILC::Wrapgen::handler::mpi;

/* static Variables */

/** @internal
 * Map to enable MPI-Type to MPI-F2C-call conversion. It is needed
 * as the type MPI_Datatype is handled by MPI_Type_* function calls
 */
static handler::info_t f2c_types;

/** @internal
 * Map to enable the 'class' template variable
 */
static handler::info_t comm_multiplicity;


/**
 * Initialize and assign handlers for MPI family.
 * These include:
 */
void
SILC::Wrapgen::handler::mpi::_initialize
    ()
{
    /** - Collective class macros */
    comm_multiplicity[ "MPI_Allgather" ]      = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Allgatherv" ]     = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Allreduce" ]      = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Alltoall" ]       = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Alltoallv" ]      = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Barrier" ]        = "_COLL_BARRIER";
    comm_multiplicity[ "MPI_Bcast" ]          = "_COLL_ONE2ALL";
    comm_multiplicity[ "MPI_Gather" ]         = "_COLL_ALL2ONE";
    comm_multiplicity[ "MPI_Gatherv" ]        = "_COLL_ALL2ONE";
    comm_multiplicity[ "MPI_Reduce" ]         = "_COLL_ALL2ONE";
    comm_multiplicity[ "MPI_Reduce_scatter" ] = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Scan" ]           = "_COLL_OTHER";
    comm_multiplicity[ "MPI_Scatter" ]        = "_COLL_ONE2ALL";
    comm_multiplicity[ "MPI_Scatterv" ]       = "_COLL_ONE2ALL";
    comm_multiplicity[ "MPI_Win_create" ]     = "_COLL_BARRIER";
    comm_multiplicity[ "MPI_Win_fence" ]      = "_COLL_BARRIER";
    comm_multiplicity[ "MPI_Win_free" ]       = "_COLL_BARRIER";
    comm_multiplicity[ "MPI_Alltoallw" ]      = "_COLL_ALL2ALL";
    comm_multiplicity[ "MPI_Exscan" ]         = "_COLL_OTHER";

    /** - Func-object template handlers */
    func_handlers[ "call:f2c_c2f" ]     = handler::mpi::call_f2c_c2f;
    func_handlers[ "call:fortran" ]     = handler::mpi::call_fortran;
    func_handlers[ "call:pmpi" ]        = handler::mpi::call_pmpi;
    func_handlers[ "cleanup" ]          = handler::mpi::cleanup;
    func_handlers[ "cleanup:f2c_c2f" ]  = handler::mpi::cleanup_f2c_c2f;
    func_handlers[ "cleanup:fortran" ]  = handler::mpi::cleanup_fortran;
    func_handlers[ "decl" ]             = handler::mpi::decl;
    func_handlers[ "decl:f2c_c2f" ]     = handler::mpi::decl_f2c_c2f;
    func_handlers[ "decl:fortran" ]     = handler::mpi::decl_fortran;
    func_handlers[ "group" ]            = handler::mpi::group;
    func_handlers[ "id" ]               = handler::mpi::id;
    func_handlers[ "init" ]             = handler::mpi::init;
    func_handlers[ "init:f2c_c2f" ]     = handler::mpi::init_f2c_c2f;
    func_handlers[ "init:fortran" ]     = handler::mpi::init_fortran;
    func_handlers[ "mpi:multiplicity" ] = handler::mpi::multiplicity;
    func_handlers[ "mpi:sendcount" ]    = handler::mpi::send_rule;
    func_handlers[ "mpi:recvcount" ]    = handler::mpi::recv_rule;
    func_handlers[ "mpi:version" ]      = handler::mpi::version;
    func_handlers[ "name" ]             = handler::mpi::name;
    func_handlers[ "proto:c" ]          = handler::mpi::proto_c;
    func_handlers[ "proto:fortran" ]    = handler::mpi::proto_fortran;
    func_handlers[ "proto:f2c_c2f" ]    = handler::mpi::proto_f2c_c2f;
    func_handlers[ "rtype" ]            = handler::mpi::rtype;
    func_handlers[ "xblock" ]           = handler::mpi::xblock;
    func_handlers[ "guard:start" ]      = handler::mpi::guard_start;
    func_handlers[ "guard:end" ]        = handler::mpi::guard_end;

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
}

string
SILC::Wrapgen::handler::mpi::call_f2c_c2f
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
                if ( datatype::is_pointer( arg ) )
                {
                    str += "&";
                }

                str     += "c_" + arg_name;
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
                // TODO
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
                str += "*";
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
SILC::Wrapgen::handler::mpi::call_fortran
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
            // dereference argument
            str += '*';
        }
        if ( datatype::is_char_pointer( arg ) )
        {
            // character arrays from Fortran are not null-terminated
            // thus we have to use our own internal character array,
            // initialized in a preceeding declaration and init block
            str += "c_" + arg_name;
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
SILC::Wrapgen::handler::mpi::call_pmpi
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
SILC::Wrapgen::handler::mpi::cleanup
(
    const Func& func
)
{
    return func.get_cleanup_block();
}

string
SILC::Wrapgen::handler::mpi::cleanup_fortran
(
    const Func& func
)
{
    string str = handler::mpi::cleanup( func );

    // Fortran wrappers need additional cleanup
    // depending on call parameters
    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam& arg = func.get_param( i );

        if ( datatype::is_char_pointer( arg ) )
        {
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            str += "free(c_" + arg.get_name() + ");";
        }
    }

    return str;
}

string
SILC::Wrapgen::handler::mpi::cleanup_f2c_c2f
(
    const Func& func
)
{
    string str = handler::mpi::cleanup_fortran( func );

    for ( size_t i = 0; i < func.get_param_count(); ++i )
    {
        const Funcparam&       arg          = func.get_param( i );
        string                 arg_type     = arg.get_type();
        string                 arg_name     = arg.get_name();
        string                 striped_type = arg_type.substr( 0, arg_type.find_first_of( "*" ) );
        info_t::const_iterator it           = f2c_types.find( striped_type );

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
                str += "MPI_Status_c2f(&c_" + arg.get_name() + ", " + arg.get_name() + ");";
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
SILC::Wrapgen::handler::mpi::decl
(
    const Func& func
)
{
    return func.get_decl_block();
}

string
SILC::Wrapgen::handler::mpi::decl_f2c_c2f
(
    const Func& func
)
{
    string str = decl_fortran( func );

    return str;
}

string
SILC::Wrapgen::handler::mpi::decl_fortran
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
        }
    }

    return str;
}

string
SILC::Wrapgen::handler::mpi::group
(
    const Func& func
)
{
    return func.get_group();
}

string
SILC::Wrapgen::handler::mpi::id
(
    const Func& func
)
{
    return func.get_id();
}

string
SILC::Wrapgen::handler::mpi::init
(
    const Func& func
)
{
    return func.get_init_block();
}

string
SILC::Wrapgen::handler::mpi::init_f2c_c2f
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
                str += striped_type + " c_" + arg.get_name() + ";";

                if ( datatype::is_input_output_param( arg ) ||
                     datatype::is_input_param( arg ) )
                {
                    str += "\n    ";
                    str += it->second + "_f2c(" + arg.get_name() + ", &c_" + arg.get_name() + ");";
                }
            }
            else if ( datatype::needs_output_handling( arg ) )
            {
                if ( str != "" )
                {
                    str += "\n    ";
                }
                str += striped_type + " c_" + arg.get_name();

                if ( datatype::is_input_output_param( arg ) )
                {
                    // initialize with f2c'ed input value
                    str += " = " + it->second + "_f2c(*" + arg.get_name() + ")";
                }
                str += ";";
            }
        }

        if ( datatype::is_char_pointer( arg ) )
        {
            // start new line if we already handled other parameters
            if ( str.length() > 0 )
            {
                str += "\n    ";
            }
            // char* need allocation and strncpy
            str += "c_" + arg_name + " = (char*) malloc((" + arg_name + "_len + 1) * sizeof(char));\n  " +
                   "if (!c_" + arg_name + ") exit(EXIT_FAILURE);\n  " +
                   "strncpy(c_" + arg_name + ", " + arg_name + ", " + arg_name + "_len);\n  " +
                   "c_" + arg_name + "[" + arg_name + "_len] = '\\0';\n";
        }
    }

    return str;
}

string
SILC::Wrapgen::handler::mpi::init_fortran
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
            // char* need allocation and strncpy
            str += "c_" + arg_name + " = (char*) malloc((" + arg_name + "_len + 1) * sizeof(char));\n  " +
                   "if (!c_" + arg_name + ") exit(EXIT_FAILURE);\n  " +
                   "strncpy(c_" + arg_name + ", " + arg_name + ", " + arg_name + "_len);\n  " +
                   "c_" + arg_name + "[" + arg_name + "_len] = '\\0';\n";
        }
    }

    return str;
}

string
SILC::Wrapgen::handler::mpi::name
(
    const Func& func
)
{
    return func.get_name();
}

string
SILC::Wrapgen::handler::mpi::proto_c
(
    const Func& func
)
{
    string str = func.get_rtype() + " " + func.get_name() + "(";

    for ( size_t i = 0; i < func.get_param_count();  ++i )
    {
        // add comma, if it is not the first parameter
        if ( i )
        {
            str += ", ";
        }

        const Funcparam& arg = func.get_param( i );
        str += arg.get_type() + " " + arg.get_name() + arg.get_suffix();
    }
    str += ")";
    return str;
}

string
SILC::Wrapgen::handler::mpi::proto_fortran
(
    const Func& func
)
{
    string str = "void FSUB(" + func.get_name() + ")(";
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
            c_str_lengths += ", int " + arg_name + "_len";
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
            if ( datatype::is_pointer( arg ) )
            {
                str += arg_type + " ";
            }
            else
            {
                str += arg_type + "* ";
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
SILC::Wrapgen::handler::mpi::proto_f2c_c2f
(
    const Func& func
)
{
    string str = "void FSUB(" + func.get_name() + ")(";
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
            c_str_lengths += ", int " + arg_name + "_len";
            str           += arg_type + " ";
        }
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
    str += "int* ierr" + c_str_lengths + ")";

    return str;
}

string
SILC::Wrapgen::handler::mpi::version
(
    const Func& func
)
{
    const MPIFunc mpifunc = dynamic_cast<const MPIFunc&>( func );
    // return MPI Version this call was introduced
    return int2string( mpifunc.get_version() );
}

string
SILC::Wrapgen::handler::mpi::send_rule
(
    const Func & func
)
{
    const MPIFunc mpifunc = dynamic_cast<const MPIFunc&>( func );

    return mpifunc.get_sendcount_rule();
}

string
SILC::Wrapgen::handler::mpi::recv_rule
(
    const Func& func
)
{
    const MPIFunc mpifunc = dynamic_cast<const MPIFunc&>( func );

    return mpifunc.get_recvcount_rule();
}

string
SILC::Wrapgen::handler::mpi::rtype
(
    const Func& func
)
{
    return func.get_rtype();
}

string
SILC::Wrapgen::handler::mpi::multiplicity
(
    const Func& func
)
{
    info_t::const_iterator it = comm_multiplicity.find( func.get_name() );

    if ( it != comm_multiplicity.end() )
    {
        return it->second;
    }
    else
    {
        return "";
    }
}

string
SILC::Wrapgen::handler::mpi::xblock
(
    const Func& func
)
{
    return func.get_expr_block();
}

string
SILC::Wrapgen::handler::mpi::guard_start
(
    const Func& func
)
{
    string guard = "#if ";
    // write individual guard
    string tmp = func.get_name();
    toupper( tmp );

    // individual guard check by generated header
    guard += "HAVE(DECL_" + tmp + ")";

    // individual guard for manual deactivation
    // guard += " && !defined (SILC_MPI_NO_" + tmp + ")";

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

            guard += " && ! defined(SILC_MPI_NO_" + guard_token + ")";
            ++it;
        }
    }

    return guard;
}

string
SILC::Wrapgen::handler::mpi::guard_end
(
    const Func& func
)
{
    return "#endif";
}
