/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <config.h>

#include "SCOREP_Pomp_Fortran.h"
#include "SCOREP_Pomp_Common.h"

#include <stdlib.h>
#include <string.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */

void FSUB(POMP2_Assign_handle)( POMP2_Region_handle_fortran* regionHandle,
                                char*                        ctc_string,
                                int                          ctc_string_len )
{
    char *str;
    str = (char*) malloc((ctc_string_len+1)*sizeof(char));
    strncpy(str,ctc_string,ctc_string_len);
    str[ctc_string_len] = '\0';
    POMP2_Assign_handle(SCOREP_POMP_F2C_REGION(regionHandle),str);
    free(str);
}
