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
 * The interface to the filtering system.  The file that contains the filter
 * definitions can be set via the envirionment variable
 * SCOREP_FILTERING_FILE. If no file is specified or the given file is not
 * found, filtering is disabled.
 */

#include <SCOREP_ErrorCodes.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

UTILS_BEGIN_C_DECLS

SCOREP_ErrorCode
SCOREP_Filter_ParseFile( const char* file_name );

void
SCOREP_Filter_FreeRules( void );

void
SCOREP_Filter_Enable( void );

void
SCOREP_Filter_Disable( void );

bool
SCOREP_Filter_IsEnabled( void );

bool
SCOREP_Filter_Match( const char* file_name,
                     const char* function_name,
                     bool        use_fortran );

UTILS_END_C_DECLS

#endif // SCOREP_FILTER_H
