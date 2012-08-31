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


#ifndef SCOREP_INTERNAL_TRACING_H
#define SCOREP_INTERNAL_TRACING_H


/**
 * @file       src/measurement/tracing/scorep_tracing_interal.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 */


#include "scorep_rewind_stack_management.h"


struct SCOREP_TracingData
{
    OTF2_EvtWriter*      otf_writer;
    scorep_rewind_stack* rewind_stack;
};


OTF2_EvtWriter*
SCOREP_Tracing_GetEventWriter( void );


void
SCOREP_Tracing_LockArchive( void );


void
SCOREP_Tracing_UnlockArchive( void );


/**
 * Config variables to configure the SION substrate.
 *
 * Only used in the paradigm dependent tracing files.
 */
extern bool     scorep_tracing_use_sion;
extern uint64_t scorep_tracing_nlocations_per_sion_file;

#endif /* SCOREP_INTERNAL_TRACING_H */
