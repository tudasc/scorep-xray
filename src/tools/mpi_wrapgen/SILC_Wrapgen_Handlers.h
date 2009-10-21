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
 * @file SILC_Wrapgen_Handlers.h
 *
 * @ brief Output handlers for template variables
 */

#ifndef SILC_WRAPGEN_HANDLERS_H_
#define SILC_WRAPGEN_HANDLERS_H_

#include <string>
#include <map>
#include <vector>
#include <stack>
#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_Funcparam.h"
using namespace SILC::Wrapgen;

namespace SILC {
namespace Wrapgen {
namespace handler {
/** handler callback type for Func objects */
typedef std::string ( *func_handler_t )( const Func& );

/** Callback map for func handlers */
typedef std::map<std::string, func_handler_t> func_handlers_t;

/** Key-value type */
typedef std::map<std::string, std::string> info_t;

extern func_handlers_t func_handlers;

std::string
dispatch
(
    const std::string&         key,
    const SILC::Wrapgen::Func& func
);
std::string
apply_modifiers
(
    const std::string&  instr,
    std::stack<string>& modifier_stack
);
}       // namespace handler
}       // namespace Wrapgen
}       // namespace SILC

#endif
