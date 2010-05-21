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


#ifndef SILC_CONFIG_H
#define SILC_CONFIG_H


/**
 * @file        SILC_Config.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Runtime configuration subsystem.
 *
 */


/**
 * @defgroup SILC_Config SILC Configuration

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

 * @{
 */


#include <SILC_Types.h>
#include <SILC_Error.h>


/**
 * Register a set of configure variables to the measurement system.
 *
 * @param nameSpace the namespace in which these variables reside can be
 *                  @a NULL for global namespace
 * @param variables array of type SILC_ConfigVariable which will be registered
 *                  to the measurement system. Terminated by
 *                  @a SILC_CONFIG_TERMINATOR.

 * Example:
 * @code
 *      bool unify;
 *      SILC_ConfigVariable unify_vars[] = {
 *          {
 *              "unify",
 *              SILC_CONFIG_TYPE_BOOL,
 *              &unify,
 *              NULL,
 *              "true", // default value as string
 *              "Unify trace files after the measurement",
 *              "long help\nwith line breaks\n\nwill be nicely aligned."
 *          },
 *          SILC_CONFIG_TERMINATOR
 *      };
 *      :
 *      SILC_ConfigRegister( NULL, unify_vars ); // in global namespace
 * @endcode

 * @note the @a variables array will not be referenced from the measurement
 *       system after the call. But most of the members of the variables need
 *       to be valid after the call.
 *       These are:
 *        @li @a SILC_ConfigVariable::variableReference
 *            (reason: obvious)
 *        @li @a SILC_ConfigVariable::variableContext
 *            (reason: obvious)
 *        @li @a SILC_ConfigVariable::defaultValue
 *            (reason: for resetting to the default value)

 * @return Successful registration or failure
 */
SILC_Error_Code
SILC_ConfigRegister
(
    const char*          nameSpace,
    SILC_ConfigVariable* variables
);


/**
 * @brief Terminates an array of SILC_ConfigVariable.
 */
#define SILC_CONFIG_TERMINATOR { \
        NULL, \
        SILC_INVALID_CONFIG_TYPE, \
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


#endif /* SILC_CONFIG_H */
