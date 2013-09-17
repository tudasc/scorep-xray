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

#ifndef SCOREP_PROFILE_MPIEVENTS_H
#define SCOREP_PROFILE_MPIEVENTS_H

/**
 * @file
 *
 * @brief   Declaration of MPI profiling events.
 *
 */

#include <SCOREP_Types.h>
#include <SCOREP_Location.h>
#include <SCOREP_DefinitionHandles.h>

/**
 * Initializes MPI Event specific data. Is called from MPI Initialize
 */
void
SCOREP_Profile_InitializeMpi( void );

/**
 * Process an MPI send event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param destinationRank    Destination rank of the message.
 * @param communicatorHandle Handle of the used communicator.
 * @param tag                The message tag.
 * @param bytesSent          Number of sent bytes.
 */
void
SCOREP_Profile_MpiSend( SCOREP_Location*                 location,
                        SCOREP_MpiRank                   destinationRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent );

/**
 * Process an MPI receive event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param sourceRank         Source rank of the message.
 * @param communicatorHandle Handle of the used communicator.
 * @param tag                The message tag.
 * @param bytesReceived      Number of received bytes.
 */
void
SCOREP_Profile_MpiRecv( SCOREP_Location*                 location,
                        SCOREP_MpiRank                   sourceRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesReceived );

/**
 * Process a collective  MPI comunication event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param rootRank           Root rank of the communication.
 * @param communicatorHandle Handle of the used communicator.
 * @param tag                The message tag.
 * @param bytesSent          Number of sent bytes.
 * @param bytesReceived      Number of received bytes.
 */
void
SCOREP_Profile_CollectiveEnd( SCOREP_Location*                 location,
                              SCOREP_InterimCommunicatorHandle communicatorHandle,
                              SCOREP_MpiRank                   rootRank,
                              SCOREP_MpiCollectiveType         collectiveType,
                              uint64_t                         bytesSent,
                              uint64_t                         bytesReceived );



#endif /* SCOREP_PROFILE_MPIEVENTS_H */
