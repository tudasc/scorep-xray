/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief  The definition for inserting the GCC plugin instrumentation descriptors.
 *
 */

tree
scorep_plugin_region_description_build( const char* function_name,
                                        const char* assembler_name,
                                        const char* file_name,
                                        tree        handle_var );
