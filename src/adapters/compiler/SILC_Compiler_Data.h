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


#ifndef SILC_COMPILER_DATA_H_
#define SILC_COMPILER_DATA_H_

#include "SILC_Types.h"
#include "SILC_Error.h"


/**
 * @brief Hash table to map function addresses to region identifier
 * identifier is called region handle
 *
 * @param id          hash key (address of function)
 * @param name        associated function name
 * @param fname       file name
 * @param lnobegin    line number of begin of function
 * @param lnoend      line number of end of function
 * @param reghandle   region identifier
 * @param HN          pointer to next element
 */
struct HashNode
{
    long              id;
    const char*       name;
    const char*       fname;
    SILC_LineNo       lnobegin;
    SILC_LineNo       lnoend;
    SILC_RegionHandle reghandle;
    struct HashNode*  next;
} HashNode;


#endif /* SILC_COMPILER_DATA_H_ */
