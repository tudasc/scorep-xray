/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#include <config.h>
#include "scorep_oa_connection.h"

void
scorep_oa_mri_set_mpiprofiling( int value )
{
    scorep_oa_connection_send_string( connection, "this is serial version of Score-P, no MPI available\n" );
}
