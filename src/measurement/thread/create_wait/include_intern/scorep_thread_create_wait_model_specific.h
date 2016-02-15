/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_THREAD_CREATE_WAIT_MODEL_SPECIFIC_H
#define SCOREP_THREAD_CREATE_WAIT_MODEL_SPECIFIC_H

/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdbool.h>


struct SCOREP_Location;


/**
 *
 *
 * @param paradigm
 * @param modelData
 * @param location
 */
void
scorep_thread_create_wait_on_create( SCOREP_ParadigmType     paradigm,
                                     void*                   modelData,
                                     struct SCOREP_Location* location );


/**
 *
 *
 * @param paradigm
 * @param modelData
 * @param location
 */
void
scorep_thread_create_wait_on_wait( SCOREP_ParadigmType     paradigm,
                                   void*                   modelData,
                                   struct SCOREP_Location* location );


/**
 *
 *
 * @param      paradigm
 * @param      parentTpd
 * @param      sequenceCount
 * @param      locationReuseKey See locationReuseKey in SCOREP_ThreadCreateWait_Begin.
 * @param[out] currentTpd
 * @param[out] locationIsCreated
 */
void
scorep_thread_create_wait_on_begin( SCOREP_ParadigmType                 paradigm,
                                    struct scorep_thread_private_data*  parentTpd,
                                    uint32_t                            sequenceCount,
                                    size_t                              locationReuseKey,
                                    struct scorep_thread_private_data** currentTpd,
                                    bool*                               locationIsCreated );


/**
 *
 *
 * @param paradigm
 * @param parentTpd
 * @param currentTpd
 * @param sequenceCount
 */
void
scorep_thread_create_wait_on_end( SCOREP_ParadigmType                paradigm,
                                  struct scorep_thread_private_data* parentTpd,
                                  struct scorep_thread_private_data* currentTpd,
                                  uint32_t                           sequenceCount );


#endif /* SCOREP_THREAD_CREATE_WAIT_MODEL_SPECIFIC_H */
