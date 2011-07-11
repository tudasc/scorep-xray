/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       SCOREP_Tracing.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "SCOREP_Tracing.h"
#include <scorep_environment.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


/* For the SCOREP_Tracing_GetFileSubstrate() see paradigm/scorep_sion_*.c */


uint64_t
SCOREP_Tracing_GetChunkSize()
{
    return 1024 * 1024; // 1MB
}
