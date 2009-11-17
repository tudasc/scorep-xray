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


#ifndef SILC_COMPILER_H_
#define SILC_COMPILER_H_


/**
 * Compiler interface support
 */


#incluce <stdio.h>


/**
 * Compiler adapter finalizer
 */
extern void
( *silc_comp_finalize )
(
    void
);


#endif /* SILC_COMPILER_H_ */
