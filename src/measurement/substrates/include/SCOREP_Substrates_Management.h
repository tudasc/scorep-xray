/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains the definitions of substrate management functions.
 *
 */

#ifndef SCOREP_SUBSTRATES_MANAGEMENT_H
#define SCOREP_SUBSTRATES_MANAGEMENT_H

#include <stdint.h>
#include <stddef.h>

struct SCOREP_Location;

void
SCOREP_Substrates_WriteData( void );



/* temporary, will be triggered by scorep_subsystems_activate_cpu_location once its there. */
void
SCOREP_Substrates_CallSubstratesOnActivation( struct SCOREP_Location* current,
                                              struct SCOREP_Location* parent,
                                              uint32_t                forkSequenceCount );

/* temporary, will be triggered by scorep_subsystems_activate_cpu_location once its there. */
void
SCOREP_Substrates_CallSubstratesOnDeactivation( struct SCOREP_Location* current,
                                                struct SCOREP_Location* parent );

/**
 * Switch substrate callbacks to enable recording.
 * Assumes that !SCOREP_Thread_InParallel() holds.
 */
void
SCOREP_Substrates_EnableRecording( void );

/**
 * Switch substrate callbacks to disable recording.
 * Assumes that !SCOREP_Thread_InParallel() holds.
 */
void
SCOREP_Substrates_DisableRecording( void );


void
SCOREP_Substrates_InitializeMpp( void );


size_t
SCOREP_Substrates_NumberOfRegisteredSubstrates( void );

#endif /* SCOREP_SUBSTRATES_MANAGEMENT_H */
