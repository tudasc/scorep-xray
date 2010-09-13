/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Pomp_UserF.c
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
#include "pomp2_lib.h"
#include "SILC_Fortran_Wrapper.h"
#include "SILC_Pomp_Variables.h"
#include "SILC_Pomp_Fortran.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */

void FSUB(POMP2_Assign_handle)(POMP2_Region_handle_fortran* regionHandle,
                              char* ctc_string,
                              int ctc_string_len)
{
    char *str;
    POMP2_Region_handle c_handle;
    str = (char*) malloc((ctc_string_len+1)*sizeof(char));
    strncpy(str,ctc_string,ctc_string_len);
    str[ctc_string_len] = '\0';
    POMP2_Assign_handle(&c_handle,str);
    *regionHandle = SILC_POMP_C2F_REGION( c_handle );
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
  silc_pomp_is_tracing_on = 0;
}

void FSUB(POMP2_On)()
{
  silc_pomp_is_tracing_on = 1;
}

void FSUB(POMP2_Begin)(POMP2_Region_handle_fortran* regionHandle)
{
  if ( silc_pomp_is_tracing_on ) POMP2_Begin(SILC_POMP_F2C_REGION(regionHandle));
}

void FSUB(POMP2_End)(POMP2_Region_handle_fortran* regionHandle)
{
  if ( silc_pomp_is_tracing_on ) POMP2_End(SILC_POMP_F2C_REGION(regionHandle));
}
