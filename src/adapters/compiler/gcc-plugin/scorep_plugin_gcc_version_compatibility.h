/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
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
 * @brief  Definitions for garanting compatibility of the plugin to all versions of GCC.
 *
 */

/* bool add_referenced_var( tree ) */
#if GCC_VERSION >= 4008
#define add_referenced_var( t ) do { } while ( 0 )
#endif
