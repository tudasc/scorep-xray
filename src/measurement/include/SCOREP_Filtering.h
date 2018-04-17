/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_FILTERING_H
#define SCOREP_FILTERING_H

/**
 * @file
 *
 *
 * Implements the filter interface.
 */

#include <stdbool.h>

bool
SCOREP_Filtering_Match( const char* fileName,
                        const char* functionName,
                        const char* mangledName );


bool
SCOREP_Filtering_MatchFile( const char* fileName );


bool
SCOREP_Filtering_MatchFunction( const char* functionName,
                                const char* mangledName );


#endif /* SCOREP_FILTERING_H */
