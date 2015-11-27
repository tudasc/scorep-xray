/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <config.h>

#include "scorep_timer_confvars.inc.c"

timer_type scorep_timer;

void
SCOREP_Timer_Register( void )
{
    SCOREP_ConfigRegister( "", scorep_timer_confvars );
}
