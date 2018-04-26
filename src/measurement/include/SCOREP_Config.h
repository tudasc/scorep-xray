/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015, 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_H
#define SCOREP_CONFIG_H


/**
 * @file
 *
 * @brief Runtime configuration subsystem.
 *
 */


/**
 * @defgroup SCOREP_Config SCOREP Configuration

 * To centralize the reading and parsing of the configuration the adapters
   need to announce their configuration options to the measurement system.

 * The configuration will, beside environment variables, also be read from
   configuration files. All these sources will have a defined priority. With
   the environment variables as the highest one (this may change, if we can
   pass parameters from the tool wrapper to the measurement system, but
   the tool could also pass the parameters per environment variables to the
   measurement system).

 * With this centralizing the measurement system has also the ability to write
   the configuration of the measurement into the experiment directory.

 * The tools can also provide a way to show all known configuration options
   with their current and default value and a appropriate help description to
   the user.

 * There is a limitation of 32 characters, per namespace, per variable name.

 * Namespaces and variables can consists only of alpha numeric characters
   and variables can also have "_" - underscore ( but not at the beginning
   or end).

 * @{
 */


#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>


#include <SCOREP_ErrorCodes.h>


#include <SCOREP_Types.h>


UTILS_BEGIN_C_DECLS


SCOREP_ErrorCode
SCOREP_ConfigInit( void );


void
SCOREP_ConfigFini( void );


/**
 * Register a set of configure variables to the measurement system.
 *
 * @param nameSpace The namespace in which these variables reside in.
 *                  (Can be the empty string, for the global namespace.)
 * @param variables Array of type SCOREP_ConfigVariable which will be registered
 *                  to the measurement system. Terminated by
 *                  @a SCOREP_CONFIG_TERMINATOR.

 * Example:
 * @code
 *      bool unify;
 *      SCOREP_ConfigVariable unify_vars[] = {
 *          {
 *              "unify",
 *              SCOREP_CONFIG_TYPE_BOOL,
 *              &unify,
 *              NULL,
 *              "true", // default value as string
 *              "Unify trace files after the measurement",
 *              "long help\nwith line breaks\n\nwill be nicely aligned."
 *          },
 *          SCOREP_CONFIG_TERMINATOR
 *      };
 *      :
 *      SCOREP_ConfigRegister( "", unify_vars ); // in global namespace
 * @endcode

 * @note the @a variables array will not be referenced from the measurement
 *       system after the call. But most of the members of the variables need
 *       to be valid after the call.
 *       These are:
 *        @li @a SCOREP_ConfigVariable::variableReference
 *            (reason: obvious)
 *        @li @a SCOREP_ConfigVariable::variableContext
 *            (reason: obvious)
 *        @li @a SCOREP_ConfigVariable::defaultValue
 *            (reason: for resetting to the default value)

 * @return Successful registration or failure
 */
SCOREP_ErrorCode
SCOREP_ConfigRegister( const char*                  nameSpace,
                       const SCOREP_ConfigVariable* variables );


/**
 *  Same as @a SCOREP_ConfigRegister() but is effective only if either
 *  @a isAvailable is set, or a prior call to
 *  @a SCOREP_ConfigForceConditionalRegister() was made.
 *
 */
SCOREP_ErrorCode
SCOREP_ConfigRegisterCond( const char*                  nameSpaceName,
                           const SCOREP_ConfigVariable* variables,
                           bool                         isAvailable );


/**
 *  Force registrations made with @a SCOREP_ConfigRegisterCond.
 */
void
SCOREP_ConfigForceConditionalRegister( void );


/**
 * Evaluate all corresponding environment variables and assign the values to
 * the config variables. This function can be called multiple but already
 * evaluated config variables will be skipped and not evaluated once more.
 *
 * @return Success or error code.
 */
SCOREP_ErrorCode
SCOREP_ConfigApplyEnv( void );


/**
 * Assign a value to an existing config variable.
 *
 * @param nameSpaceName  The name space where the variable lives in.
 * @param variableName   The name of the variable.
 * @param variableValue  The new value for the variable.
 *
 * @return Success or one of the following error codes:
 *         @li @a SCOREP_ERROR_INDEX_OUT_OF_BOUNDS The given variable does not exist.
 *         @li @a SCOREP_ERROR_PARSE_INVALID_VALUE The value could not be parsed.
 */
SCOREP_ErrorCode
SCOREP_ConfigSetValue( const char* nameSpaceName,
                       const char* variableName,
                       const char* variableValue );

/**
 * Returns the config variable data if it exists.
 *
 * @param nameSpaceName  The name space where the variable lives in.
 * @param variableName   The name of the variable.
 *
 * @return SCOREP_ConfigVariable if variable was found, otherwise NULL.
 */
SCOREP_ConfigVariable*
SCOREP_ConfigGetData( const char* nameSpaceName,
                      const char* variableName );


/**
 * Dumps the current value of all config variables to the given file, one per line.
 *
 * The output is suitable for shell consumption.
 *
 * @param dumpFile An already opened I/O stream for writing.
 *
 * @return SCOREP_SUCCESS.
 */
SCOREP_ErrorCode
SCOREP_ConfigDump( FILE* dumpFile );

/**
 * Dumps the current value of all explicitly set config variables to the given file,
 * one per line.
 *
 * The output is suitable for shell consumption.
 *
 * @param dumpFile An already opened I/O stream for writing.
 *
 * @return SCOREP_SUCCESS.
 */
SCOREP_ErrorCode
SCOREP_ConfigDumpChangedVars( FILE* dumpFile );

/**
 * Copy the file referenced in the Score-P variable, if necessary
 *
 * @param nameSpaceName     The name space for the variable.
 * @param variableName      The name of the variable.
 * @param relativeSourceDir directory used as starting point for relative paths,
 *                          e.g., the current working dir.
 * @param targetDir target directory for the copy operation.
 *
 * @return true on success false otherwise.
 */
bool
SCOREP_ConfigCopyFile( const char* nameSpaceName,
                       const char* variableName,
                       const char* relativeSourceDir,
                       const char* targetDir );

void
SCOREP_ConfigManifestSectionHeader( FILE*       out,
                                    const char* section );

void
SCOREP_ConfigManifestSectionEntry( FILE*       out,
                                   const char* fileName,
                                   const char* descriptionFormatString,
                                   ... );


/**
 * Displays all config variables with there short description, type and default value.
 *
 * If @a full is set than the long description is also displayed.
 *
 * @param full Displays the long help description too.
 * @param html Use an html description list for the output.
 */
void
SCOREP_ConfigHelp( bool  full,
                   bool  html,
                   FILE* out );


/**
 * @brief Terminates an array of SCOREP_ConfigVariable.
 */
#define SCOREP_CONFIG_TERMINATOR { \
        NULL, \
        SCOREP_INVALID_CONFIG_TYPE, \
        NULL, \
        NULL, \
        NULL, \
        NULL, \
        NULL  \
}

/**
 * @ToDo how can the online measurement system change configs?
 * This interface will be private to the measurement system.
 */


/*
 * @}
 */


UTILS_END_C_DECLS


#endif /* SCOREP_CONFIG_H */
