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

#ifndef SILC_WRAPGEN_H_
#define SILC_WRAPGEN_H_

/**
 * @file       SILC_Wrapgen.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Wrapper generator for automatic creation of MPI wrappers.
 */

#include <string>
#include <map>

#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_MpiFunc.h"
using namespace SILC::Wrapgen;

/**
   @defgroup Wrapgen_module Wrapper generator
   This module contains a tool for generation of wrapper libraries from templates.
   It was especially developed to wrap the MPI library.

   It reads a list of available function and thei signature together with other meta
   informations from an prototype file in an XML like syntax. For the functions
   contained in the prototype file wrappers are generated either on
   @li a function template base, or
   @li a file template base.

   If wrappers are generated on a function template base, a functions wrapper file with
   suffix '.w' must be specified. All selected functions are wrapped using this template
   for the function. The wapper can contain some variables which allow to insert e.g.
   name, signature, or other function specifics. By default all functions in the
   prototype file are selected. Functions can be excluded from the selection by using
   the '-r' option when invoking the tool.

   If wrappers are generated on a file template base, a file template with suffix
   '.tmpl' must be specified. It contains a skeleton of a source file, which can contain
   several '#pragma wrapgen' statements which can specify a selection of functions which
   are processed using a particular function template file. The generated code for the
   function is inserted at the location of the pragma statement. To specify the functions
   three types of statments exists:
   @li #pragma wrapgen single function-name wrapper-file: Selects exactly one function
       specified by the funcion-name.
   @li #pragma wrapgen multiple regex expression wrapper-file: Allows the specification
       of a regular expression and processes all function which are described by the
       regular expression according to the wrapper-file.
   @li #pragma wrapgen multiple restrict expression wrapper-file: Starts with all
       functions and excludes the function specified by the expression. The expression
       has the same syntax like the -r option of the command line.

   @{
 */

namespace SILC
{
namespace Wrapgen
{
/**
 * @struct option
 * @brief  Holds all global options of the wrapper generator
 */
typedef struct
{
    bool        suppressbanner;  ///< suppress auto-generation banner
    std::string restriction;     ///< restriction rule for wrapper generation
    std::string prototypes;      ///< filename of prototype definitions
} option;


void
init
    ();
void
process_cmd_line
(
    int&    argc,
    char**& argv
);
void
read_prototypes
(
    const char* filename,
    std::map<std::string, MPIFunc>& funcs
);
void
read_xml_prototypes
(
    const char* filename,
    std::map<std::string, MPIFunc>& funcs
);
void
write_xml_prototypes
(
    const char* filename,
    std::map<std::string, MPIFunc>& funcs
);
void
handle_textfile
(
    const char* filename
);
int
handle_file_template
(
    const char* filename
);
int
handle_wrapper_template
(
    const char* filename
);
bool
is_selected
(
    const Func&        func,
    const std::string& restriction
);
bool
generateOutput
(
    MPIFunc&    func,
    const char* wrappername,
    std::string rstring = ""
);
}   // namespace Wrapgen
}   // namespace SILC

/** @} */

#endif /* SILC_WRAPGEN_H_ */
