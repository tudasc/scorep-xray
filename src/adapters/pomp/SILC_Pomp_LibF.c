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
void FSUB(POMP2_Atomic_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Atomic_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Atomic_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Atomic_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Barrier_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Barrier_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Barrier_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Barrier_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Flush_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP2_Flush_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Flush_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP2_Flush_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Critical_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP2_Critical_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Critical_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
      POMP2_Critical_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Critical_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Critical_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Critical_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Critical_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Do_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_For_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Do_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_For_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Master_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Master_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Master_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Master_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Parallel_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Parallel_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Parallel_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Parallel_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Parallel_fork)(POMP2_Region_handle_fortran* regionHandle, int *num_threads)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Parallel_fork(SILC_POMP_F2C_REGION(*regionHandle), *num_threads);
}

void FSUB(POMP2_Parallel_join)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Parallel_join(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Section_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Section_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Section_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Section_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Sections_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Sections_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Sections_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Sections_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Single_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Single_begin(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Single_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Single_end(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Single_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Single_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Single_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Single_exit(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Workshare_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Workshare_enter(SILC_POMP_F2C_REGION(*regionHandle));
}

void FSUB(POMP2_Workshare_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( silc_pomp_is_tracing_on )
        POMP2_Workshare_exit(SILC_POMP_F2C_REGION(*regionHandle));
}
