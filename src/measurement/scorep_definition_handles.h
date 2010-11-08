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

#ifndef SCOREP_INTERNAL_DEFINITION_HANDLES_H
#define SCOREP_INTERNAL_DEFINITION_HANDLES_H



/**
 * @file       scorep_definition_handles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */



typedef struct scorep_any_definition scorep_any_definition;
typedef uint64_t                     scorep_any_handle;


struct scorep_any_definition
{
    scorep_any_handle next;
    scorep_any_handle unified;
    scorep_any_handle hash_next;
    uint32_t          hash_value;
    uint32_t          sequence_number;
};


#endif /* SCOREP_INTERNAL_DEFINITION_HANDLES_H */
