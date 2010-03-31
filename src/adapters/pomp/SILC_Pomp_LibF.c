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
void FSUB(POMP_Atomic_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Atomic_enter(*regionHandle);
}

void FSUB(POMP_Atomic_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Atomic_exit(*regionHandle);
}

void FSUB(POMP_Barrier_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Barrier_enter(*regionHandle);
}

void FSUB(POMP_Barrier_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Barrier_exit(*regionHandle);
}

void FSUB(POMP_Flush_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Flush_enter(*regionHandle);
}

void FSUB(POMP_Flush_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Flush_exit(*regionHandle);
}

void FSUB(POMP_Critical_begin)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Critical_begin(*regionHandle);
}

void FSUB(POMP_Critical_end)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Critical_end(*regionHandle);
}

void FSUB(POMP_Critical_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Critical_enter(*regionHandle);
}

void FSUB(POMP_Critical_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Critical_exit(*regionHandle);
}

void FSUB(POMP_Do_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_For_enter(*regionHandle);
}

void FSUB(POMP_Do_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_For_exit(*regionHandle);
}

void FSUB(POMP_Master_begin)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Master_begin(*regionHandle);
}

void FSUB(POMP_Master_end)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Master_end(*regionHandle);
}

void FSUB(POMP_Parallel_begin)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Parallel_begin(*regionHandle);
}

void FSUB(POMP_Parallel_end)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Parallel_end(*regionHandle);
}

void FSUB(POMP_Parallel_fork)(POMP_Region_handle* regionHandle, int *num_threads)
{
    if ( silc_pomp_is_tracing_on ) POMP_Parallel_fork(*regionHandle, *num_threads);
}

void FSUB(POMP_Parallel_join)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Parallel_join(*regionHandle);
}

void FSUB(POMP_Section_begin)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Section_begin(*regionHandle);
}

void FSUB(POMP_Section_end)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Section_end(*regionHandle);
}

void FSUB(POMP_Sections_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Sections_enter(*regionHandle);
}

void FSUB(POMP_Sections_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Sections_exit(*regionHandle);
}

void FSUB(POMP_Single_begin)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Single_begin(*regionHandle);
}

void FSUB(POMP_Single_end)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Single_end(*regionHandle);
}

void FSUB(POMP_Single_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Single_enter(*regionHandle);
}

void FSUB(POMP_Single_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Single_exit(*regionHandle);
}

void FSUB(POMP_Workshare_enter)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Workshare_enter(*regionHandle);
}

void FSUB(POMP_Workshare_exit)(POMP_Region_handle* regionHandle)
{
    if ( silc_pomp_is_tracing_on ) POMP_Workshare_exit(*regionHandle);
}
