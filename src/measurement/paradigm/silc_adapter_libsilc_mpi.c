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


#include <stdlib.h>
#include <stdio.h>


/**
 * @file        silc_adatper_libsilc_mpi.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief       Holds the list of adapters linked into the measurement system.
 */


#include <silc_adapter.h>

#include <SILC_Compiler_Init.h>
#include <SILC_User_Init.h>
#include <SILC_Mpi_Init.h>


/**
 * List of adapters.
 */
SILC_Adapter* silc_adapters[] = {
    &SILC_Compiler_Adapter,
    &SILC_Mpi_Adapter,
    &SILC_User_Adapter,
};


size_t silc_number_of_adapters = sizeof( silc_adapters ) /
                                 sizeof( silc_adapters[ 0 ] );
