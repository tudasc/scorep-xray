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
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Implementation of the POMP fortran user adapter functions and initialization.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pomp_lib.h"
#include "SILC_Fortran_Wrapper.h"
#include "SILC_Pomp_Variables.h"
#include "SILC_Pomp_Fortran.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */

void FSUB(POMP_Assign_handle)(POMP_Region_handle* regionHandle,
                              char* ctc_string,
                              int ctc_string_len)
{
    char *str;
    str = (char*) malloc((ctc_string_len+1)*sizeof(char));
    strncpy(str,ctc_string,ctc_string_len);
    str[ctc_string_len] = '\0';
    POMP_Assign_handle(regionHandle,str);
    free(str);
}

void FSUB(POMP_Finalize)()
{
  POMP_Finalize();
}

void FSUB(POMP_Init)()
{
  POMP_Init();
}

void FSUB(POMP_Off)()
{
  silc_pomp_is_tracing_on = 0;
}

void FSUB(POMP_On)()
{
  silc_pomp_is_tracing_on = 1;
}

void FSUB(POMP_Begin)(int* regionHandle)
{
  if ( silc_pomp_is_tracing_on ) POMP_Begin(regionHandle);
}

void FSUB(POMP_End)(int* regionHandle)
{
  if ( silc_pomp_is_tracing_on ) POMP_End(regionHandle);
}
