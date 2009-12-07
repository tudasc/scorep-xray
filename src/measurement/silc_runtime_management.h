/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SILC_INTERNAL_RUNTIME_MANAGEMENT_H
#define SILC_INTERNAL_RUNTIME_MANAGEMENT_H

#include <OTF2_EvtWriter.h>

extern OTF2_EvtWriter* local_event_writer;
extern OTF2_TimeStamp  local_timestamp_counter;

#if 0
/**
 * Get the location handle of the caller
 *
 * @note could be a function pointer?
 */
SILC_LocationHandle
silc_get_location( void );

#endif

#endif /* SILC_INTERNAL_RUNTIME_MANAGEMENT_H */
