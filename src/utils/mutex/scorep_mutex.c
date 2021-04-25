/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#include <config.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Atomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <UTILS_Error.h>

#define STATIC_INLINE
#include "scorep_mutex.inc.c"
#undef STATIC_INLINE
