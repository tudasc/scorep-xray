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
 * @ file      SILC_Compiler_Init.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief Compiler adapter interface support to the measurement system
 */

#include <stdio.h>

#include "SILC_Compiler_Init.h"
#include "SILC_Types.h"
#include "SILC_Error.h"
#include <SILC_Utils.h>

extern SILC_Error_Code
silc_compiler_init_adapter();

extern void
silc_compiler_finalize();

SILC_Error_Code
silc_compiler_register()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " register compiler adapter!" );
    return SILC_SUCCESS;
}

SILC_Error_Code
silc_compiler_init_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter init loacation!" );
    return SILC_SUCCESS;
}

void
silc_compiler_final_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter final loacation!" );
}

void
silc_compiler_deregister()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter deregister!n" );
}


/**
 * Initialize the adapter structure for compilers
 */
const SILC_Adapter SILC_Compiler_Adapter =
{
    SILC_ADAPTER_COMPILER,
    "COMPILER",
    &silc_compiler_register,
    &silc_compiler_init_adapter,
    &silc_compiler_init_location,
    &silc_compiler_final_location,
    &silc_compiler_finalize,
    &silc_compiler_deregister
};
