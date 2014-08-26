/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>
#include "scorep_oa_connection.h"

void
scorep_oa_mri_set_mpiprofiling( int value )
{
    scorep_oa_connection_send_string( scorep_oa_connection, "this is a serial version of Score-P, no MPI available\n" );
}
