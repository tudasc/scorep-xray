/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2018,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains the header of the low-level substrate plugins
 *
 */


/**
 * traverse through the plugins and return their management functions
 * You should call it the following way:
 * SCOREP_Substrates_Callback * current_plugin_functions=NULL;
 * int     current_plugin = 0;
 * while ( ( current_plugin = SCOREP_Substrate_Plugins_GetSubstrateMgmtCallbacks( current_plugin, &callbacks ) ) != 0 )
 * {
 *     // .. do sth
 *     free( callbacks );
 * }
 * @param current_plugin: the current plugin to get the functions from. Start calling this function with 0 and continue with the returned
 * @param returned_callbacks: (out) a pointer to the allocated function array to store the callbacks. This should be freed.
 * @return the next plugin index or 0 if there is no plugin at index current plugin (i.e., current_plugin >= SCOREP_Substrate_Plugins_GetNumberRegisteredPlugins())
 */
uint32_t
SCOREP_Substrate_Plugins_GetSubstrateMgmtCallbacks( uint32_t                     current_plugin,
                                                    SCOREP_Substrates_Callback** returned_callbacks );



/**
 * traverse through the plugins and return their event functions
 * You should call it the following way:
 * SCOREP_Substrates_Callback * current_plugin_functions=NULL;
 * int current_plugin = 0;
 * uint32_t current_array_length = 0;
 * while ((current_plugin = SCOREP_Substrate_Plugins_GetSubstrateCallbacks(mode,current_plugin, &current_plugin_functions, &current_array_length)) != 0)
 * {
 *   // process the current_plugin_functions, remember the array has only a length of current_array_length
 *   ...
 *   // do not free anything that is returned
 * }
 * @param mode: The current SCOREP_Substrates_Mode (enabled or disabled)
 * @param current_plugin: the current plugin to get the functions from. Start calling this function with 0 and continue with the returned
 * @param returned_callbacks: (out) a pointer to the function array to store the callbacks. Do not free it!
 * @param current_array_length: (out) the length of the returned functions
 * @return the next plugin index or 0 if there is no plugin at index current_plugin (i.e., current_plugin >= SCOREP_Substrate_Plugins_GetNumberRegisteredPlugins())
 */
uint32_t
SCOREP_Substrate_Plugins_GetSubstrateCallbacks( SCOREP_Substrates_Mode       mode,
                                                uint32_t                     current_plugin,
                                                SCOREP_Substrates_Callback** returned_callbacks,
                                                uint32_t*                    current_array_length );

/**
 * This will allocate per plugin data structures so that plugins have a thread
 * local storeage too.
 */
void
SCOREP_Substrate_Plugins_InitLocationData( struct SCOREP_Location* location );

/**
 * This substrate is initialized early, (i.e., after reading the environment variables)
 * The substrate reads the respective environment variables and initializes all registered plugins.
 */
void
SCOREP_Substrate_Plugins_EarlyInit( void );

/**
 * Register the environment variables for substrate plugins.
 */
void
SCOREP_Substrate_Plugins_Register( void );


/**
 * Get number of registered plugins
 */
uint32_t
SCOREP_Substrate_Plugins_GetNumberRegisteredPlugins( void );
