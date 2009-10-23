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

/**
 * Register a set of configure variables to the measurement system.
 *
 * @param variables         array of type SILC_ConfigVariable which will be
 *                          registered to the measurement system
 * @param numberOfVariables number of variables in the @a variables array

 * Example:
 * @code
 *      SILC_Bool unify;
 *      SILC_ConfigVariable unify_vars[] = {
 *          {
 *              NULL, // is in global namespace
 *              "unify",
 *              SILC_CONFIG_TYPE_BOOL,
 *              &unify,
 *              NULL,
 *              "true", // default value as string
 *              "Unify trace files after the measurement",
 *              "long help\nwith line breaks\n\nwill be nicely aligned."
 *          }
 *      };
 *      :
 *      SILC_ConfigRegister( unify_vars, 1 );
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
    SILC_ConfigVariable* variables,
    uint32_t             numberOfVariables
);

/**
 * @ToDo how can the online measurement system change configs?
 * This interface will be private to the measurement system.
 */

/*
 * @}
 */

#endif /* SILC_CONFIG_H */
