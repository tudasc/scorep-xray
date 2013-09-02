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

#ifndef SCOREP_PROFILE_DEBUG_H
#define SCOREP_PROFILE_DEBUG_H

/**
 * @file        scorep_profile_node.h
 *
 * @brief Provides functionality for extended debug information on errors in the
 *        profile.
 *
 */

#include <SCOREP_Profile.h>

void
scorep_profile_on_error( SCOREP_Profile_LocationData* location );

#endif // SCOREP_PROFILE_DEBUG_H
