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
 * @file SILC_Wrapgen.h
 *
 * Wrapper generator for automatic creation of MPI wrappers
 */

#ifndef SILC_WRAPGEN_H_
#define SILC_WRAPGEN_H_

#include <string>
#include <map>

#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_MpiFunc.h"
using namespace SILC::Wrapgen;

namespace SILC {
namespace Wrapgen {
/**
 * @struct option
 * @brief Holds all global options of the wrapper generator
 */
typedef struct
{
    bool        suppressbanner;  ///< suppress auto-generation banner
    std::string restriction;     ///< restriction rule for wrapper generation
    std::string partial_predef;  ///< filename of partial predefined wrappers
    std::string complete_predef; ///< filename of complete predefined wrappers
    std::string prototypes;      ///< filename of prototype definitions
    std::string counts;          ///< filename of count specifications
    std::string guard;           ///< preprocessor guards for the spec block
    std::string banner;          ///< banner to print prior to spec block
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
read_predefined
(
    const char* filename,
    std::map<std::string, std::string>& predef
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
read_count_spec
(
    const char* filename,
    std::map<std::string, MPIFunc>& funcs
);
void
print_banner
(
    const std::string& str
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
int
handle_spec_file
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

#endif /* SILC_WRAPGEN_H_ */
