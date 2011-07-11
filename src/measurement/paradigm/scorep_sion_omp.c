/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       src/trunk/src/measurement/paradigm/scorep_sion_serial.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <SCOREP_Tracing.h>
#include <otf2/OTF2_Sion_Ext.h>
#include <scorep_environment.h>

// no support from sionconfig yet!

void
SCOREP_Tracing_RegisterSionCallbacks()
{
    if ( SCOREP_Env_UseSionSubstrate() )
    {
        // todo if sionconfig supports hybrid
        //OTF2_File_Sion_Register_Open_Callback( scorep_sion_open );
        //OTF2_File_Sion_Register_Close_Callback( scorep_sion_close );
    }
}
