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

#ifndef SILC_WRAPGEN_HANDLERS_H_
#define SILC_WRAPGEN_HANDLERS_H_

/**
 * @file       SILC_Wrapgen_Handlers.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Output handlers for template variables
 */

#include <string>
#include <map>
#include <vector>
#include <stack>
#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_Funcparam.h"
using namespace SILC::Wrapgen;

namespace SILC
{
namespace Wrapgen
{
namespace handler
{
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
