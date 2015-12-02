/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013, 2015
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 *  @file
 *
 *  This file contains the implementation of class SCOREP_User_RegionClass.
 *  Putting this into the header file. leads to incorrect instrumentation.
 *  See ticket #875.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>

SCOREP_User_RegionClass::SCOREP_User_RegionClass(
    SCOREP_User_RegionHandle*    regionHandle,
    const char*                  regionName,
    const SCOREP_User_RegionType regionType,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFileHandle,
    const char*                  fileName,
    const uint32_t               lineNo )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_User_RegionBegin( regionHandle,
                                 lastFileName,
                                 lastFileHandle,
                                 regionName,
                                 regionType,
                                 fileName,
                                 lineNo );

        region_handle = *regionHandle;
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

SCOREP_User_RegionClass::~SCOREP_User_RegionClass()
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_User_RegionEnd( region_handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
