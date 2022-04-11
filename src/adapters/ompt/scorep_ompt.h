/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#ifndef SCOREP_OMPT_H
#define SCOREP_OMPT_H

#include <stdbool.h>
#include <omp-tools.h>


static inline size_t
scorep_ompt_get_subsystem_id( void )
{
    extern size_t scorep_ompt_subsystem_id;
    return scorep_ompt_subsystem_id;
}

/* We want to record events between ompt_subsystem_begin and ompt_substytem_end
   only, but cannot register and deregister callbacks there. Thus, pass events
   to the measurement core only if scorep_ompt_record_event() return true. */
static inline bool
scorep_ompt_record_event( void )
{
    extern bool scorep_ompt_record_events;
    return scorep_ompt_record_events;
}


static inline bool
scorep_ompt_finalizing_tool_in_progress( void )
{
    extern bool scorep_ompt_finalizing_tool;
    return scorep_ompt_finalizing_tool;
}


#endif /* SCOREP_OMPT_H */
