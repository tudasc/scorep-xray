/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
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
 * @brief  A GCC 4.9.0 compatible version of the tree-flow header.
 *
 */

/* Test for GCC >= 4.9.0 */
#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 4009

#include "stringpool.h"
#include "basic-block.h"
#include "tree-ssa-alias.h"

#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 7000
#include "tree-vrp.h"
#endif

#include "tree-ssanames.h"
#include "internal-fn.h"
#include "gimple-expr.h"
#include "gimple.h"
#include "gimple-low.h"
#include "gimple-iterator.h"
#include "stor-layout.h"
#include "varasm.h"

#define FOR_EACH_BB( BB ) FOR_EACH_BB_FN( BB, cfun )
#define ENTRY_BLOCK_PTR ENTRY_BLOCK_PTR_FOR_FN( cfun )

#else /* #if (GCC >= 4.9.0) */

#include "tree-flow.h"

#endif /* #if (GCC >= 4.9.0) */
