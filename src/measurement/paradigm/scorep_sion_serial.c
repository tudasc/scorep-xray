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
 * @file       scorep_sion_serial.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <scorep_sion.h>
//#include <otf2/OTF2_File_Sion_Ext.h>


void
SCOREP_RegisterSionCallbacks()
{
    // serial is implemented in otf2, so nothing to do here, not even call
    // the register functions
    //OTF2_File_Sion_Register_Open_Callback( 0 );
    //OTF2_File_Sion_Register_Close_Callback( 0 );
}
