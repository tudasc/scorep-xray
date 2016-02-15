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

#ifndef SCOREP_WRAPGEN_HANDLERS_MPI_H_
#define SCOREP_WRAPGEN_HANDLERS_MPI_H_

/**
 * @file SCOREP_Wrapgen_Handlers_mpi.h
 *
 * @ brief Output handlers for template variables in MPI wrappers.
 *
 */

#include <string>
#include <map>
#include <vector>
#include "SCOREP_Wrapgen_Func.h"
#include "SCOREP_Wrapgen_Handlers.h"

namespace SCOREP
{
namespace Wrapgen
{
namespace handler
{
namespace mpi
{
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

/** Function handler to create a PMPI call.
 */
std::string
call_pmpi
(
    const SCOREP::Wrapgen::Func& func
);

/** Function handler to create a post communication hook call.
 */
std::string
call_posthook
(
    const SCOREP::Wrapgen::Func& func
);

/** Function handler to create a pre communication hook call.
 */
std::string
call_prehook
(
    const SCOREP::Wrapgen::Func& func
);

/** Function handler to declare variables needed by hooks.
 */
std::string
declare_hooks
(
    const SCOREP::Wrapgen::Func& func
);


std::string
attribute
(
    const SCOREP::Wrapgen::Func& func,
    const std::string&           attribute
);

std::string
call_f2c_c2f
(
    const SCOREP::Wrapgen::Func& func
);

/** Creates a call of the corresponding C function from a Fortran wrapper.
 */
std::string
call_fortran
(
    const SCOREP::Wrapgen::Func& func
);

/**
 * @}
 * @name Cleanup handlers
 * @{
 */
/** Insert the cleanup block from the prototypes
 */
std::string
cleanup
(
    const SCOREP::Wrapgen::Func& func
);

std::string
cleanup_f2c_c2f
(
    const SCOREP::Wrapgen::Func& func
);

/** Frees allocated memory for created c_strings.
    For every char pointer in the parameter list, a "free(c_name)" is inserted.
 */
std::string
cleanup_fortran
(
    const SCOREP::Wrapgen::Func& func
);

/**
 * @}
 * @name Additional-variable-declaration handlers
 * @{
 */
/** Inserts a declaration block from the prototypes
 */
std::string
decl
(
    const SCOREP::Wrapgen::Func& func
);

std::string
decl_f2c_c2f
(
    const SCOREP::Wrapgen::Func& func
);

/** Declares c-strings for Fortran wrappers.
    For every char pointer in the parameter list, a char pointer with prepended 'c_'
    is declared.
 */
std::string
decl_fortran
(
    const SCOREP::Wrapgen::Func& func
);

/**
 * @}
 * @name Additional-variable-initialization handlers
 * @{
 */
/** Inserts the init block from the prototypes.
 */
std::string
init
(
    const SCOREP::Wrapgen::Func& func
);

std::string
init_f2c_c2f
(
    const SCOREP::Wrapgen::Func& func
);

/** Converts Fortran strings to C-Strings.
    For every char pointer in the parameter list it assumes that a char pointer with
    a prepended 'c_', and an integer with the name of the char pointer but a appended
    '_len' exists which contains the length of the Fortran string. It allocates memory
    for the c-String, copies the content of the Fortran string and terminates the
    C-string with a '\0'.
 */
std::string
init_fortran
(
    const SCOREP::Wrapgen::Func& func
);

/**
 * @}
 * @name Prototype handlers
 * @{
 */
/** Creates a prototype of the function for C-wrappers.
 */
std::string
proto_c
(
    const SCOREP::Wrapgen::Func& func
);

/** Creates a prototype of the function for Fortran-wrappers.
 */
std::string
proto_fortran
(
    const SCOREP::Wrapgen::Func& func
);

std::string
proto_f2c_c2f
(
    const SCOREP::Wrapgen::Func& func
);

/**
 * @}
 * @name Miscelaneous handlers
 * @{
 */
/** Returns the group of the function.
 */
std::string
group
(
    const SCOREP::Wrapgen::Func& func
);
std::string
id
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the name of the function
 */
std::string
name
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the opening statement of the guard.
    The guard prevents the building of wrappers for functions for which no corresponding
    PMPI function exists or which belong to a group which should not be build.
    it inserts an '#if HAVE(DECL_PMPI_name)' and appends ' && ! defined SCOREP_MPI_NO_GUARD'
    for every entry listed in the prototypes in guard.
 */
std::string
guard_start
(
    const SCOREP::Wrapgen::Func& func
);

/** Closes the guard.
    Inserts '#endif'.
 */
std::string
guard_end
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the opening statement of the hooks guard.
    The guard prevents the building of MPI Porfiling Hooks.
    It inserts #if ! defined(SCOREP_MPI_NO_HOOKS).
 */
std::string
guard_hooks
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the if statement to check whether hooks are enabled.
    It inserts if(SCOREP_IS_MPI_HOOKS_ON).
 */
std::string
check_hooks
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the return type of the function
 */
std::string
rtype
(
    const SCOREP::Wrapgen::Func& func
);

/** Inserts the expr block from the prototypes.
 */
std::string
xblock
(
    const SCOREP::Wrapgen::Func& func
);

std::string
xblock_fortran( const SCOREP::Wrapgen::Func& func );

std::string
xblock_f2c_c2f( const SCOREP::Wrapgen::Func& func );

/**
 * @}
 * @name MPI related functions
 * @{
 */
/** Returns the MPI version, which introduced this function
 */
std::string
version
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the expression from the prototypes, how to calculate the number of
    sent bytes.
 */
std::string
send_rule
(
    const SCOREP::Wrapgen::Func& func
);

/** Returns the expression from the prototypes, how to calculate the number of
    received bytes.
 */
std::string
recv_rule
(
    const SCOREP::Wrapgen::Func& func
);

std::string
comm_new
(
    const SCOREP::Wrapgen::Func& func
);

std::string
comm_parent
(
    const SCOREP::Wrapgen::Func& func
);

/**
 * @}
 */
}           // namespace mpi
}           // namespace handler
}           // namespace wrapgen
}           // namespace SCOREP
#endif
