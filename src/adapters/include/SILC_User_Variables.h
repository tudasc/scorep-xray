#ifndef SILC_USER_VARIABLES_H
#define SILC_USER_VARIABLES_H

/** @file SILC_User_Variables
    @author Daniel Lorenz
    @ingroup SILC_User_External
    @brief Automatically, declares variables needed in the application's source files.

    This file contains variable declarations. This declarations are automatically
    included in any file that includes the User adapter interface @ref SILC_User.h.
    Thus, these variables are declared in every file that uses the user interface.
    Becuase static variables are only visible inside the source file where there are
    declared, it is possible to have source file specific variables. This is
    used to store and distinguish the source file related date from the SILC measurement
    system and access them efficiently.
 */

/** @ingroup SILC_User_External
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
static SILC_SourceFileHandle SILC_User_LastFileHandle = SILC_INVALID_SOURCE_FILE;

/**
    Stores the file name of the instrumented source file.
    It is automatically included in every instrumented file and thus declared as a static
    variable in every file. Thus, its scope is distinguished for each file.
    In most cases, only one source file name is used inside one compilation
    entity. Only if code is included several source file names may appear.
    Thus, in most cases, the string comparison can be avoided when storing the pointer
    to the last used file name, if the compiler uses always the same pointer.
 */
static const char* SILC_User_LastFileName = 0;

/** @} */

#endif /* SILC_USER_VARIABLES_H */
