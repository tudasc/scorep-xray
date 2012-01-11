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
 * @file       src/trunk/src/measurement/tracing/scorep_tracing_file_substrate.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <SCOREP_Tracing.h>
#include <scorep_environment.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


OTF2_FileSubstrate
scorep_tracing_get_file_substrate()
{
    OTF2_FileSubstrate substrate = OTF2_SUBSTRATE_POSIX;
    if ( SCOREP_Env_UseSionSubstrate() )
    {
        substrate = OTF2_SUBSTRATE_SION;
    }
    return substrate;
}
