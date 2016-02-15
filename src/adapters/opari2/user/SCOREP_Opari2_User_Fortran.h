/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_POMP2_USER_FORTRAN_H
#define SCOREP_POMP2_USER_FORTRAN_H

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Provides defines for function name decoration for POMP2 Fortran wrappers.
 */

#include <opari2/pomp2_user_lib.h>

#include "SCOREP_Fortran_Wrapper.h"

/* Fortran types */
typedef uint64_t POMP2_Region_handle_fortran;

#define SCOREP_POMP_F2C_REGION( handle ) ( ( POMP2_USER_Region_handle* )handle )

/*
 * Fortan subroutine external name setup
 */

#define POMP2_Finalize_U         POMP2_FINALIZE
#define POMP2_Init_U             POMP2_INIT
#define POMP2_Off_U              POMP2_OFF
#define POMP2_On_U               POMP2_ON
#define POMP2_Begin_U            POMP2_BEGIN
#define POMP2_End_U              POMP2_END
#define POMP2_USER_Assign_handle_U    POMP2_USER_ASSIGN_HANDLE

#define POMP2_Finalize_L         pomp2_finalize
#define POMP2_Init_L             pomp2_init
#define POMP2_Off_L              pomp2_off
#define POMP2_On_L               pomp2_on
#define POMP2_Begin_L            pomp2_begin
#define POMP2_End_L              pomp2_end
#define POMP2_USER_Assign_handle_L    pomp2_user_assign_handle

#endif /* SCOREP_POMP2_USER_FORTRAN_H */
