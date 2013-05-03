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
 * @file SCOREP_OA_DummyF.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains dummy implementation of Online Access Phase functions
 */

#include <config.h>
#include <stdio.h>

#include <SCOREP_Fortran_Wrapper.h>
#include "scorep/SCOREP_User_Functions.h"
#include "SCOREP_Types.h"
#define SCOREP_F_OA_PhaseBegin_U SCOREP_F_OA_PHASEBEGIN
#define SCOREP_F_OA_PhaseEnd_U SCOREP_F_OA_PHASEEND

#define SCOREP_F_OA_PhaseBegin_L scorep_f_oa_phasebegin
#define SCOREP_F_OA_PhaseEnd_L scorep_f_oa_phaseend

void
FSUB( SCOREP_F_OA_PhaseBegin )(  SCOREP_Fortran_RegionHandle* handle,
                                 char*                        name_f,
                                 int32_t*                     type,
                                 char*                        fileName_f,
                                 int32_t*                     lineNo,
                                 int                          nameLen,
                                 int                          fileNameLen )
{
    printf( "Dummy %s\n", __func__ );
}

void
FSUB( SCOREP_F_OA_PhaseEnd )( SCOREP_Fortran_RegionHandle* handle )
{
    printf( "Dummy %s\n", __func__ );
}
