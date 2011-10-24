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


#ifndef SCOREP_INTERNAL_SUBSYSTEM_H
#define SCOREP_INTERNAL_SUBSYSTEM_H


/**
 * @file        scorep_subsystem.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief       Exports the subsystems array for the measurement system.
 */


#include <SCOREP_Subsystem.h>

/** @brief a NULL terminated list of linked in subsystems. */
extern const SCOREP_Subsystem* scorep_subsystems[];
extern const size_t            scorep_number_of_subsystems;

#endif /* SCOREP_INTERNAL_SUBSYSTEM_H */
