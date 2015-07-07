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

#ifndef SCOREP_PROFILE_WRITER_H
#define SCOREP_PROFILE_WRITER_H

/**
 * @file
 *
 *
 * Declares profile writer functions. This header provides a list of top level
 * profile writers. Thus, we avoid to include a special file per format. Only
 * those formats are declared, which are available on the platform.
 */

/**
   Writes the profile in TAU snapshot format to disk. For each rank a separate
   file is created. The files are named snapshot.<rank>.0.0
 */
extern void
scorep_profile_write_tau_snapshot( void );

/**
   Writes the profile in Cube 4 format to disk. For each metric a separate
   file is created.
   @param write_tuples True if it should write the whole tuple consisting of
                       sum, min, max, number of samples, sum of squares to Cube.
                       False if it should write only the sum.
 */
extern void
scorep_profile_write_cube4( bool write_tuples );

#endif /* SCOREP_PROFILE_WRITER_H */
