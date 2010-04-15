#ifndef SILC_DEFINITION_HANDLES_H
#define SILC_DEFINITION_HANDLES_H

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


/**
 * @file       silc_definition_handles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

typedef struct silc_any_definition         silc_any_definition;
typedef struct silc_any_definition_movable silc_any_definition_movable;
typedef silc_any_definition_movable*       silc_any_handle;

struct silc_any_definition
{
    void*    next;
    uint64_t id;
};



#endif /* SILC_DEFINITION_HANDLES_H */
