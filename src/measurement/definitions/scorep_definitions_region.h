/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_REGION_H
#define SCOREP_PRIVATE_DEFINITIONS_REGION_H


/**
 * @file
 *
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( Region )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Region );

    // Add SCOREP_Region stuff from here on.
    SCOREP_StringHandle name_handle;            // This field contains demangled region name
                                                // (if available, otherwise mangled region name)
    SCOREP_StringHandle canonical_name_handle;  // This field always contains mangled region name
    SCOREP_StringHandle description_handle;     // Currently not used
    SCOREP_RegionType   region_type;
    SCOREP_StringHandle file_name_handle;
    SCOREP_LineNo       begin_line;
    SCOREP_LineNo       end_line;
    SCOREP_ParadigmType paradigm_type;           // Used by Cube 4
};


void
scorep_definitions_unify_region( SCOREP_RegionDef*             definition,
                                 SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_REGION_H */
