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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
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

    /* Optional name for a region group */
    SCOREP_StringHandle group_name_handle;
};


/**
 * Associate a code region with a process unique file handle.
 *
 * @param regionName A meaningful name for the region, e.g. a function
 * name. The string will be copied.
 *
 * @param regionCanonicalName An canonical name for the region, e.g. a
 * mangled name. The string will be copied. If this parameter is
 * @code{NULL}, content of @a regionName will be used.
 *
 * @param fileHandle A previously defined SCOREP_SourceFileHandle or
 * SCOREP_INVALID_SOURCE_FILE.
 *
 * @param beginLine The file line number where the region starts or
 * SCOREP_INVALID_LINE_NO.
 *
 * @param endLine The file line number where the region ends or
 * SCOREP_INVALID_LINE_NO.
 *
 * @param paradigm The paradigm type of adapter (SCOREP_ParadigmType)
 * that is calling.
 *
 * @param regionType The type of the region. Until now, the @a regionType was
 * not used during the measurement but during analysis. This @e may change in
 * future with e.g. dynamic regions or parameter based profiling. In the first
 * run, we can implement at least dynamic regions in the adapter.
 *
 * @note The name of e.g. Java classes, previously provided as a string to the
 * region description, should now be encoded in the region name. The region
 * description field is replaced by the adapter type as that was it's primary
 * use.
 *
 * @note During unification, we compare @a regionName, @a fileHandle, @a
 * beginLine, @a endLine and @a adapter of regions from different
 * processes. If all values are equal, we consider the regions to be equal. We
 * don't check for uniqueness of this tuple in this function, i.e. during
 * measurement, this is the adapters responsibility, but we require that every
 * call defines a unique and distinguishable region.
 *
 * @todo Change SCOREP_RegionType from enum to bitstring? When using phases,
 * parametr-based profiling and dynamic regions, combinations of these
 * features may be useful. These combinations can be encoded in a
 * bitstring. The adapter group was queried for detailed input.
 *
 * @return A process unique region handle to be used in calls to
 * SCOREP_EnterRegion() and SCOREP_ExitRegion().
 *
 */
SCOREP_RegionHandle
SCOREP_Definitions_NewRegion( const char*             regionName,
                              const char*             regionCanonicalName,
                              SCOREP_SourceFileHandle fileHandle,
                              SCOREP_LineNo           beginLine,
                              SCOREP_LineNo           endLine,
                              SCOREP_ParadigmType     paradigm,
                              SCOREP_RegionType       regionType );


/**
 * Assigns a group to this region.
 *
 * Region group definitions are created automatically by the unification
 * based on this group name and the paradigm of the region.
 *
 * @param handle A region handle.
 * @param groupName The group name.
 */
void
SCOREP_RegionHandle_SetGroup( SCOREP_RegionHandle handle,
                              const char*         groupName );


uint32_t
SCOREP_RegionHandle_GetId( SCOREP_RegionHandle handle );


const char*
SCOREP_RegionHandle_GetName( SCOREP_RegionHandle handle );


const char*
SCOREP_RegionHandle_GetCanonicalName( SCOREP_RegionHandle handle );


const char*
SCOREP_RegionHandle_GetFileName( SCOREP_RegionHandle handle );


SCOREP_RegionType
SCOREP_RegionHandle_GetType( SCOREP_RegionHandle handle );


SCOREP_ParadigmType
SCOREP_RegionHandle_GetParadigmType( SCOREP_RegionHandle handle );


SCOREP_LineNo
SCOREP_RegionHandle_GetBeginLine( SCOREP_RegionHandle handle );


SCOREP_LineNo
SCOREP_RegionHandle_GetEndLine( SCOREP_RegionHandle handle );


void
scorep_definitions_unify_region( SCOREP_RegionDef*                    definition,
                                 struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_REGION_H */
