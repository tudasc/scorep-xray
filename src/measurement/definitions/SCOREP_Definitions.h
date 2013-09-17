/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
#define SCOREP_INTERNAL_DEFINITIONS_H


/**
 * @file
 *
 *
 */

#include <SCOREP_DefinitionHandles.h>
#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>

/* forward decl */
struct SCOREP_DefinitionManager;
typedef struct SCOREP_DefinitionManager SCOREP_DefinitionManager;

#include "scorep_definitions_private.h"

#include "scorep_definitions_string.h"
#include "scorep_definitions_source_file.h"
#include "scorep_definitions_location_group.h"
#include "scorep_definitions_location.h"
#include "scorep_definitions_system_tree_node.h"
#include "scorep_definitions_system_tree_node_property.h"
#include "scorep_definitions_region.h"
#include "scorep_definitions_communicator.h"
#include "scorep_definitions_group.h"
#include "scorep_definitions_metric.h"
#include "scorep_definitions_sampling_set.h"
#include "scorep_definitions_sampling_set_recorder.h"
#include "scorep_definitions_parameter.h"
#include "scorep_definitions_callpath.h"
#include "scorep_definitions_clock_offset.h"
#include "scorep_definitions_property.h"
#include "scorep_definitions_rma_window.h"
#include "scorep_definitions_topology.h"
#include "scorep_definitions_io_file_group.h"
#include "scorep_definitions_io_file.h"
#include "scorep_definitions_marker_group.h"
#include "scorep_definitions_marker.h"

/* super object for all definitions */
SCOREP_DEFINE_DEFINITION_TYPE( Any )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Any );
};


static inline uint32_t
scorep_handle_to_id( SCOREP_AnyHandle handle )
{
    if ( handle == SCOREP_MOVABLE_NULL )
    {
        return UINT32_MAX;
    }

    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Any );
}


// the list of definitions for what we generate mappings to global
// ids in the unifier
#define SCOREP_LIST_OF_DEFS_WITH_MAPPINGS \
    DEF_WITH_MAPPING( String, string ) \
    DEF_WITH_MAPPING( Region, region ) \
    DEF_WITH_MAPPING( Group, group ) \
    DEF_WITH_MAPPING( Communicator, communicator ) \
    DEF_WITH_MAPPING( RmaWindow, rma_window ) \
    DEF_WITH_MAPPING( Metric, metric ) \
    DEF_WITH_MAPPING( SamplingSet, sampling_set ) \
    DEF_WITH_MAPPING( Parameter, parameter ) \
    DEF_WITH_MAPPING( Callpath, callpath )


/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
struct SCOREP_DefinitionManager
{
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( string );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( system_tree_node );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( system_tree_node_property );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( location_group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( location );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( source_file );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( region );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( interim_communicator );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( communicator );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( interim_rma_window );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( rma_window );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( mpi_cartesian_topology );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( mpi_cartesian_coords );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( metric );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( sampling_set );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( sampling_set_recorder );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( io_file_group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( io_file );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( marker_group );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( marker );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( parameter );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( callpath );
    SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( property );

    /** The pager manager where all definition objects resides */
    SCOREP_Allocator_PageManager* page_manager;
};

/* *INDENT-ON* */

extern SCOREP_DefinitionManager scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;

void
SCOREP_Definitions_Initialize( void );


void
SCOREP_Definitions_Finalize( void );


void
SCOREP_Definitions_Lock( void );

void
SCOREP_Definitions_Unlock( void );

void
SCOREP_Definitions_InitializeDefinitionManager(
    SCOREP_DefinitionManager**    manager,
    SCOREP_Allocator_PageManager* pageManager,
    bool                          allocHashTables );


void
SCOREP_Definitions_Write( void );


/**
 * Returns the number of unified callpath definitions.
 */
uint32_t
SCOREP_Definitions_GetNumberOfUnifiedCallpathDefinitions( void );


#endif /* SCOREP_INTERNAL_DEFINITIONS_H */
