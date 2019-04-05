/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_IO_HANDLE_PROPERTY_H
#define SCOREP_PRIVATE_DEFINITIONS_IO_HANDLE_PROPERTY_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( IoFileProperty )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IoFileProperty );

    SCOREP_IoFileHandle io_file_handle;
    SCOREP_StringHandle property_name_handle;
    SCOREP_StringHandle property_value_handle;

    /* chaint of properties for the referenced system tree node */
    SCOREP_IoFilePropertyHandle properties_next;
};


void
SCOREP_IoFileHandle_AddProperty( SCOREP_IoFileHandle ioFileHandle,
                                 const char*         propertyName,
                                 const char*         propertyValue );


void
scorep_definitions_unify_io_file_property( SCOREP_IoFilePropertyDef*            definition,
                                           struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_IO_HANDLE_PROPERTY_H */
