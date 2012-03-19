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


#include <SCOREP_Location.h>
#include <SCOREP_Subsystem.h>


size_t
scorep_subsystems_get_number( void );

void
scorep_subsystems_register( void );

void
scorep_subsystems_deregister( void );

void
scorep_subsystems_initialize( void );

void
scorep_subsystems_finalize( void );

void
scorep_subsystems_initialize_location( SCOREP_Location* locationData );

void
scorep_subsystems_finalize_location( SCOREP_Location* locationData );


#endif /* SCOREP_INTERNAL_SUBSYSTEM_H */
