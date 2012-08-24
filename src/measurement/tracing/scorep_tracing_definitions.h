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

#ifndef SCOREP_TRACING_DEFINITIONS_H
#define SCOREP_TRACING_DEFINITIONS_H


/**
 * @status      alpha
 * @file        src/measurement/tracing/scorep_tracing_definitions.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Handles tracing specific definitions tasks.
 *
 *
 */

void
scorep_tracing_write_mappings( OTF2_DefWriter* localDefinitionWriter );

void
scorep_tracing_write_clock_offsets( OTF2_DefWriter* localDefinitionWriter );

void
scorep_tracing_write_local_definitions( OTF2_DefWriter* localDefinitionWriter );

void
scorep_tracing_write_global_definitions( OTF2_GlobalDefWriter* global_definition_writer );

void
scorep_tracing_set_properties( OTF2_Archive* scorep_otf2_archive );


#endif /* SCOREP_TRACING_DEFINITIONS_H */
