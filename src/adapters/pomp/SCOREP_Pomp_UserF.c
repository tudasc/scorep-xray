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
 * @file       SCOREP_Pomp_UserF.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 fortran user adapter functions and initialization.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scorep/pomp2_lib.h"
#include "SCOREP_Fortran_Wrapper.h"
#include "SCOREP_Pomp_Variables.h"
#include "SCOREP_Pomp_Fortran.h"
#include "SCOREP_Pomp_Variables.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */

void FSUB(POMP2_Assign_handle)(POMP2_Region_handle_fortran* regionHandle,
                              char* ctc_string,
                              int ctc_string_len)
{
    char *str;
    str = (char*) malloc((ctc_string_len+1)*sizeof(char));
    strncpy(str,ctc_string,ctc_string_len);
    str[ctc_string_len] = '\0';
    POMP2_Assign_handle(SCOREP_POMP_F2C_REGION(regionHandle),str);
    free(str);
}

void FSUB(POMP2_Finalize)()
{
  POMP2_Finalize();
}

void FSUB(POMP2_Init)()
{
  POMP2_Init();
}

void FSUB(POMP2_Off)()
{
  scorep_pomp_is_tracing_on = 0;
}

void FSUB(POMP2_On)()
{
  scorep_pomp_is_tracing_on = 1;
}

void FSUB(POMP2_Begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Begin(SCOREP_POMP_F2C_REGION( regionHandle ));
    }
}

void FSUB(POMP2_End)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_End(SCOREP_POMP_F2C_REGION( regionHandle ));
    }
}
