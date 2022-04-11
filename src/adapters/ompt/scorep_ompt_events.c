/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>

#include "scorep_ompt.h"

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME OMPT
#include <UTILS_Debug.h>

#include "scorep_ompt_debug.inc.c"

#include "scorep_ompt_events_host.inc.c"
#include "scorep_ompt_events_device.inc.c"
