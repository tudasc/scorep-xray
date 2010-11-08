/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_USER_VARIABLES_H
#define SCOREP_USER_VARIABLES_H

/** @file       SCOREP_User_Variables.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA
    @ingroup    SCOREP_User_External

    @brief Automatically, declares variables needed in the application's source files.

    This file contains variable declarations. This declarations are automatically
    included in any file that includes the User adapter interface @ref SCOREP_User.h.
    Thus, these variables are declared in every file that uses the user interface.
    Becuase static variables are only visible inside the source file where there are
    declared, it is possible to have source file specific variables. This is
    used to store and distinguish the source file related date from the SCOREP measurement
    system and access them efficiently.
 */

#include <stddef.h>

#include "SCOREP_PublicTypes.h"

/** @ingroup SCOREP_User_External
    @{
 */

/* **************************************************************************************
 * static variables
 * *************************************************************************************/

/**
    Stores the handle of the instrumented source file.
    It is automatically included in every instrumented file and thus declared as a static
    variable in every file. Thus, its scope is distinguished for each file.
    In most cases, only one source file name is used inside one compilation
    entity. Only if code is included several source file names may appear.
    Thus, in most cases, the string comparison can be avoided when storing the pointer
    to the last used file name, if the compiler uses always the same pointer.
 */
static SCOREP_SourceFileHandle SCOREP_User_LastFileHandle = SCOREP_INVALID_SOURCE_FILE;

/**
    Stores the file name of the instrumented source file.
    It is automatically included in every instrumented file and thus declared as a static
    variable in every file. Thus, its scope is distinguished for each file.
    In most cases, only one source file name is used inside one compilation
    entity. Only if code is included several source file names may appear.
    Thus, in most cases, the string comparison can be avoided when storing the pointer
    to the last used file name, if the compiler uses always the same pointer.
 */
static const char* SCOREP_User_LastFileName = 0;

/** @} */

#endif /* SCOREP_USER_VARIABLES_H */
