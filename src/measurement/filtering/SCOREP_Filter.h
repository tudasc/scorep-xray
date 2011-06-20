/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_FILTER_H
#define SCOREP_FILTER_H

/**
 * @file       SCOREP_Filter.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * The interface to the filtering system.
 * The file that contains the filter defintions can be set via the envirionment variable
 * SCOREP_FILTERING_FILE. If no file is specified or the given file is not found,
 * filtering is disabled.
 */

/**
   Initializes the filterign system and parses the configuration file.
 */
void
SCOREP_Filter_Initialize();

/**
   Registers the config variables for filtering.
 */
SCOREP_Error_Code
SCOREP_Filter_Register();

#endif // SCOREP_FILTER_H
