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
 * @ingroup    POMP
 *
 * @brief Implementation of the POMP fortran user adapter functions and initialization.
 */

#include <config.h>
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
void FSUB(POMP_Atomic_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Atomic_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Atomic_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Atomic_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Barrier_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Barrier_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Barrier_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Barrier_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Flush_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP_Flush_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Flush_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP_Flush_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Critical_begin)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP_Critical_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Critical_end)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP_Critical_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Critical_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Critical_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Critical_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Critical_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Do_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_For_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Do_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_For_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Master_begin)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Master_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Master_end)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Master_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Parallel_begin)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Parallel_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Parallel_end)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Parallel_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Parallel_fork)(POMP_Region_handle_fortran* regionHandle, int *num_threads)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Parallel_fork(SILC_POMP_F2C_REGION(*regionHandle), *num_threads);
}

void FSUB(POMP_Parallel_join)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Parallel_join(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Section_begin)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Section_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Section_end)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Section_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Sections_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Sections_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Sections_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Sections_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Single_begin)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Single_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Single_end)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Single_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Single_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Single_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Single_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Single_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Workshare_enter)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Workshare_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP_Workshare_exit)(POMP_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP_Workshare_exit(SILC_POMP_F2C_REGION(*regionHandle));
}
