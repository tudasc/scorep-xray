/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
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
 * @file       src/measurement/paradigm/scorep_sion_not_supported.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <otf2/otf2.h>


#include <tracing/SCOREP_Tracing.h>


void
scorep_tracing_register_sion_callbacks( OTF2_Archive* archive )
{
    // don't register anything if sion is not supported.
}


OTF2_FileSubstrate
scorep_tracing_get_file_substrate()
{
    return OTF2_SUBSTRATE_POSIX;
}
