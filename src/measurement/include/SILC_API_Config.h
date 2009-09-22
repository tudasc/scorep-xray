#ifndef SILC_API_CONFIG_H
#define SILC_API_CONFIG_H

/**
 * Register a set of configure variables to the measurement system.
 *
 * @param variables         array of type SILC_API_ConfigVariable which will be
 *                          registered to the measurement system
 * @param numberOfVariables number of variables in the @a variables array
 *
 * Example:
 *      SILC_Bool unify;
 *      SILC_API_ConfigVariable unify_vars[] = {
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
 *      SILC_API_ConfigRegister( unify_vars, 1 );
 *
 * @note the @a variables array will not be referenced from the measurement
 *       system after the call. But most of the members of the variables need
 *       to be valid after the call.
 *       These are:
 *        * @a SILC_API_ConfigVariable::nameSpace
 *          (questionable)
 *        * @a SILC_API_ConfigVariable::name
 *          (questionable)
 *        * @a SILC_API_ConfigVariable::variableReference
 *          (obvious)
 *        * @a SILC_API_ConfigVariable::variableContext
 *          (obvious)
 *        * @a SILC_API_ConfigVariable::defaultValue
 *          (for resetting to the default value)
 *        * @a SILC_API_ConfigVariable::shortHelp
 *          (questionable)
 *        * @a SILC_API_ConfigVariable::longHelp
 *          (questionable)
 *
 * @return Successful registration or failure
 */
SILC_ErroCode SILC_API_ConfigRegister( SILC_API_ConfigVariable* variables,
                                       uint32_t
                                                                numberOfVariables );

/**
 * @ToDo how can the online measurement system change configs?
 */

#endif /* SILC_API_CONFIG_H */
