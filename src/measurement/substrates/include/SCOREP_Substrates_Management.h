/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
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

void
SCOREP_Substrates_WriteData( void );

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

size_t
SCOREP_Substrates_NumberOfRegisteredSubstrates( void );

#endif /* SCOREP_SUBSTRATES_MANAGEMENT_H */
