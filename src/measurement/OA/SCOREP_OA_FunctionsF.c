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
 * @file SCOREP_OA_FunctionsF.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access Phase functions for Fortran
 */

#include <config.h>

#include <SCOREP_Fortran_Wrapper.h>
#include "SCOREP_OA_Init.h"

#include "scorep_oa_phase.h"

#define SCOREP_F_OA_PhaseBegin_U SCOREP_F_OA_PHASEBEGIN
#define SCOREP_F_OA_PhaseEnd_U SCOREP_F_OA_PHASEEND
#define SCOREP_F_Begin_U SCOREP_F_BEGIN
#define SCOREP_F_Init_U SCOREP_F_INIT
#define SCOREP_F_RegionEnd_U SCOREP_F_REGIONEND
#define SCOREP_F_RegionEnter_U SCOREP_F_REGIONENTER

#define SCOREP_F_Begin_L scorep_f_begin
#define SCOREP_F_Init_L scorep_f_init
#define SCOREP_F_RegionEnd_L scorep_f_regionend
#define SCOREP_F_RegionEnter_L scorep_f_regionenter
#define SCOREP_F_OA_PhaseBegin_L scorep_f_oa_phasebegin
#define SCOREP_F_OA_PhaseEnd_L scorep_f_oa_phaseend

extern void
FSUB( SCOREP_F_Init )( SCOREP_Fortran_RegionHandle* handle,
                       char*                        name_f,
                       int32_t*                     type,
                       char*                        fileName_f,
                       int32_t*                     lineNo,
                       int                          nameLen,
                       int                          fileNameLen );
extern void
FSUB( SCOREP_F_RegionEnd )( SCOREP_Fortran_RegionHandle* handle );
extern void
FSUB( SCOREP_F_RegionEnter )( SCOREP_Fortran_RegionHandle* handle );

void
FSUB( SCOREP_F_OA_PhaseBegin )(  SCOREP_Fortran_RegionHandle* handle,
                                 char*                        name_f,
                                 int32_t*                     type,
                                 char*                        fileName_f,
                                 int32_t*                     lineNo,
                                 int                          nameLen,
                                 int                          fileNameLen )
{
    if ( !SCOREP_OA_Init() )
    {
        return;
    }

    FSUB( SCOREP_F_Init )( handle, name_f, type, fileName_f,
                           lineNo, nameLen, fileNameLen );
    scorep_oa_phase_enter( SCOREP_F2C_REGION( *handle ) );
    FSUB( SCOREP_F_RegionEnter )(  handle );
}

void
FSUB( SCOREP_F_OA_PhaseEnd )( SCOREP_Fortran_RegionHandle* handle )
{
    if ( !SCOREP_OA_Initialized() )
    {
        return;
    }
    FSUB( SCOREP_F_RegionEnd )( handle );
    scorep_oa_phase_exit( SCOREP_F2C_REGION( *handle ) );
}
