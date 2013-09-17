/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_TRACING_DEFINITIONS_H
#define SCOREP_TRACING_DEFINITIONS_H


/**
 * @file
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
