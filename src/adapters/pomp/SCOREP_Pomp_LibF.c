/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
#include "pomp2_lib.h"
#include "SCOREP_Fortran_Wrapper.h"
#include "SCOREP_Pomp_Variables.h"
#include "SCOREP_Pomp_Fortran.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */
void FSUB(POMP2_Atomic_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Atomic_enter(&c_handle);
      }
}

void FSUB(POMP2_Atomic_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Atomic_exit(&c_handle);
      }
}

void FSUB(POMP2_Barrier_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Barrier_enter(&c_handle);
      }
}

void FSUB(POMP2_Barrier_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Barrier_exit(&c_handle);
      }
}

void FSUB(POMP2_Flush_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
      POMP2_Flush_enter(&c_handle);
      }
}

void FSUB(POMP2_Flush_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
      POMP2_Flush_exit(&c_handle);
      }
}

void FSUB(POMP2_Critical_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
      POMP2_Critical_begin(&c_handle);
      }}

void FSUB(POMP2_Critical_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
      POMP2_Critical_end(&c_handle);
      }}

void FSUB(POMP2_Critical_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Critical_enter(&c_handle);
      }}

void FSUB(POMP2_Critical_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Critical_exit(&c_handle);
      }
}

void FSUB(POMP2_Do_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_For_enter(&c_handle);
      }
}

void FSUB(POMP2_Do_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_For_exit(&c_handle);
      }
}

void FSUB(POMP2_Master_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Master_begin(&c_handle);
}}

void FSUB(POMP2_Master_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Master_end(&c_handle);
}}

void FSUB(POMP2_Parallel_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
          POMP2_Parallel_begin(&c_handle);
}}

void FSUB(POMP2_Parallel_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Parallel_end(&c_handle);
}}

void FSUB(POMP2_Parallel_fork)(POMP2_Region_handle_fortran* regionHandle, int *num_threads)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
          POMP2_Parallel_fork(&c_handle, *num_threads );
      }
}

void FSUB(POMP2_Parallel_join)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Parallel_join(&c_handle);
}}

void FSUB(POMP2_Section_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
         POMP2_Section_begin(&c_handle);
      }
}

void FSUB(POMP2_Section_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Section_end(&c_handle);
}}

void FSUB(POMP2_Sections_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Sections_enter(&c_handle);
}}

void FSUB(POMP2_Sections_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Sections_exit(&c_handle);
}}

void FSUB(POMP2_Single_begin)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Single_begin(&c_handle);
}}

void FSUB(POMP2_Single_end)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Single_end(&c_handle);
}}

void FSUB(POMP2_Single_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Single_enter(&c_handle);
}}

void FSUB(POMP2_Single_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Single_exit(&c_handle);
}}

void FSUB(POMP2_Workshare_enter)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Workshare_enter(&c_handle);
}}

void FSUB(POMP2_Workshare_exit)(POMP2_Region_handle_fortran* regionHandle)
{
    if ( scorep_pomp_is_tracing_on )
      {
	  POMP2_Region_handle c_handle = SCOREP_POMP_F2C_REGION( *regionHandle );
        POMP2_Workshare_exit(&c_handle);
}}
