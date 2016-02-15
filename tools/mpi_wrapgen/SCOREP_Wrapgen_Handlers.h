/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
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

#ifndef SCOREP_WRAPGEN_HANDLERS_H_
#define SCOREP_WRAPGEN_HANDLERS_H_

/**
 * @file       SCOREP_Wrapgen_Handlers.h
 * @ingroup    Wrapgen_module
 *
 * @brief Output handlers for template variables
 */

#include <string>
#include <map>
#include <vector>
#include <stack>
#include "SCOREP_Wrapgen_Func.h"
#include "SCOREP_Wrapgen_Funcparam.h"
using namespace SCOREP::Wrapgen;

namespace SCOREP
{
namespace Wrapgen
{
namespace handler
{
/** handler callback type for Func objects */
typedef std::string ( * func_handler_t )( const Func& );

/** Callback map for func handlers */
typedef std::map<std::string, func_handler_t> func_handlers_t;

/** handler callback type for Func with arg objects */
typedef std::string ( * funcarg_handler_t )( const Func&,
                                             const std::string& );

/** Callback map for func with arg handlers */
typedef std::map<std::string, funcarg_handler_t> funcarg_handlers_t;

/** Key-value type */
typedef std::map<std::string, std::string> info_t;

extern func_handlers_t func_handlers;

extern funcarg_handlers_t funcarg_handlers;

std::string
dispatch
(
    const std::string&           key,
    const SCOREP::Wrapgen::Func& func
);
std::string
apply_modifiers
(
    const std::string&  instr,
    std::stack<string>& modifier_stack
);
}       // namespace handler
}       // namespace Wrapgen
}       // namespace SCOREP

#endif
