/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_FILTER_INIT_H
#define SCOREP_FILTER_INIT_H

/**
 * @file       SCOREP_Filter_Init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Implements the filter initialization interface for the filtering system.
 */

/**
   Initializes the filtering system and parses the configuration file.
 */
void
SCOREP_Filter_Initialize();

/**
   Registers the config variables for filtering.
 */
SCOREP_Error_Code
SCOREP_Filter_Register();

/**
   Finalizes the filtering system and frees all memory.
 */
void
SCOREP_Filter_Finalize();

#endif
