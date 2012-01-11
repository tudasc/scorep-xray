/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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


#ifndef SCOREP_TRACING_FILE_SUBSTRATE_H
#define SCOREP_TRACING_FILE_SUBSTRATE_H


/**
 * @file       src/trunk/src/measurement/tracing/scorep_tracing_file_substrate.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 */


void
scorep_tracing_register_sion_callbacks( OTF2_Archive* archive );


OTF2_FileSubstrate
scorep_tracing_get_file_substrate();


#endif /* SCOREP_TRACING_FILE_SUBSTRATE_H */
