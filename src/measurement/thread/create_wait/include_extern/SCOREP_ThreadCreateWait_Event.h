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

#ifndef SCOREP_THREADCREATEWAIT_EVENT_H
#define SCOREP_THREADCREATEWAIT_EVENT_H

/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdlib.h>
#include <stdint.h>


struct scorep_thread_private_data;
struct SCOREP_Location;


/**
 *
 *
 * @param paradigm
 * @param parent
 * @param sequenceCount
 */
void
SCOREP_ThreadCreateWait_Create( SCOREP_ParadigmType                 paradigm,
                                struct scorep_thread_private_data** parent,
                                uint32_t*                           sequenceCount );


/**
 *
 *
 * @param paradigm
 * @param sequenceCount
 */
void
SCOREP_ThreadCreateWait_Wait( SCOREP_ParadigmType paradigm,
                              uint32_t            sequenceCount );


/**
 *
 *
 * @param paradigm
 * @param parent
 * @param sequenceCount
 * @param locationReuseKey If this value is not 0, than the threading system
 *                         tries to reuse an unused location for this thread.
 *                         But will only reuse a location which was created
 *                         with the same locationReuseKey value.
 * @param location The newly created location object.
 */
void
SCOREP_ThreadCreateWait_Begin( SCOREP_ParadigmType                paradigm,
                               struct scorep_thread_private_data* parent,
                               uint32_t                           sequenceCount,
                               size_t                             locationReuseKey,
                               struct SCOREP_Location**           location );


/**
 *
 *
 * @param paradigm
 * @param parentTpd
 * @param sequenceCount
 */
void
SCOREP_ThreadCreateWait_End( SCOREP_ParadigmType                paradigm,
                             struct scorep_thread_private_data* parentTpd,
                             uint32_t                           sequenceCount );


#endif /* SCOREP_THREADCREATEWAIT_EVENT_H */
