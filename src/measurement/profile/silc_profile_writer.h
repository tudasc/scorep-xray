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

#ifndef SILC_PROFILE_WRITER_H
#define SILC_PROFILE_WRITER_H

/**
 * @file silc_profile_writer.h
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 * Declares profile writer functions.
 */

/**
   Writes the profile in TAU snapshot format to disk. For each rank a separate
   file is created. The files are named <basename>.<rank>.0.0.0
 */
extern void
silc_profile_write_tau_snapshot();


#endif /* SILC_PROFILE_WRITER_H */
