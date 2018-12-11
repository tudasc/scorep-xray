/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2018,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Fortran wrapper for region initialization
 */


#include <config.h>

#include "SCOREP_Opari2_User_Fortran.h"

#include <stdlib.h>
#include <string.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */

/*
 * Fortran wrappers calling the C versions
 */

void
FSUB( POMP2_USER_Assign_handle )( POMP2_Region_handle_fortran* regionHandle,
                                  char*                        ctc_string,
                                  scorep_fortran_charlen_t     ctc_string_len )
{
    char* str;
    str = ( char* )malloc( ( ctc_string_len + 1 ) * sizeof( char ) );
    strncpy( str, ctc_string, ctc_string_len );
    str[ ctc_string_len ] = '\0';
    POMP2_USER_Assign_handle( SCOREP_POMP_F2C_REGION( regionHandle ), str );
    free( str );
}
