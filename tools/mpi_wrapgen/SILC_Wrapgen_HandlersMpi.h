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
/**
 * @file SILC_Wrapgen_Handlers_mpi.h
 *
 * @ brief Output handlers for template variables in MPI wrappers
 */

#ifndef SILC_WRAPGEN_HANDLERS_MPI_H_
#define SILC_WRAPGEN_HANDLERS_MPI_H_

#include <string>
#include <map>
#include <vector>
#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_Handlers.h"

namespace SILC {
namespace Wrapgen {
namespace handler {
namespace mpi {
/**
 * @name Administrative function calls
 * @{
 */
void
_initialize
    ();

/**
 * @}
 * @name Function call handlers
 * @{
 */
std::string
call_pmpi
(
    const SILC::Wrapgen::Func& func
);
std::string
call_f2c_c2f
(
    const SILC::Wrapgen::Func& func
);
std::string
call_fortran
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 * @name Cleanup handlers
 * @{
 */
std::string
cleanup
(
    const SILC::Wrapgen::Func& func
);
std::string
cleanup_f2c_c2f
(
    const SILC::Wrapgen::Func& func
);
std::string
cleanup_fortran
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 * @name Additional-variable-declaration handlers
 * @{
 */
std::string
decl
(
    const SILC::Wrapgen::Func& func
);
std::string
decl_f2c_c2f
(
    const SILC::Wrapgen::Func& func
);
std::string
decl_fortran
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 * @name Additional-variable-initialization handlers
 * @{
 */
std::string
init
(
    const SILC::Wrapgen::Func& func
);
std::string
init_f2c_c2f
(
    const SILC::Wrapgen::Func& func
);
std::string
init_fortran
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 * @name Prototype handlers
 * @{
 */
std::string
proto_c
(
    const SILC::Wrapgen::Func& func
);
std::string
proto_fortran
(
    const SILC::Wrapgen::Func& func
);
std::string
proto_f2c_c2f
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 * @name Miscelaneous handlers
 * @{
 */
std::string
group
(
    const SILC::Wrapgen::Func& func
);
std::string
id
(
    const SILC::Wrapgen::Func& func
);
std::string
name
(
    const SILC::Wrapgen::Func& func
);
std::string
guard_start
(
    const SILC::Wrapgen::Func& func
);
std::string
guard_end
(
    const SILC::Wrapgen::Func& func
);
std::string
rtype
(
    const SILC::Wrapgen::Func& func
);
std::string
xblock
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 * @name MPI related functions
 * @{
 */

std::string
version
(
    const SILC::Wrapgen::Func& func
);
std::string
send_rule
(
    const SILC::Wrapgen::Func& func
);
std::string
recv_rule
(
    const SILC::Wrapgen::Func& func
);
std::string
multiplicity
(
    const SILC::Wrapgen::Func& func
);

/**
 * @}
 */
}           // namespace mpi
}           // namespace handler
}           // namespace wrapgen
}           // namespace SILC
#endif
