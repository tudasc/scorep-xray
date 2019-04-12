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
 * Copyright (c) 2009-2013, 2015-2017,
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

#ifndef SCOREP_PLATFORM_H
#define SCOREP_PLATFORM_H

/**
 * @file
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Types.h>
#include <UTILS_Error.h>

UTILS_BEGIN_C_DECLS

/**
 * Contains one element of the system tree path. Used to specify a path in the
 * system tree
 */
typedef struct SCOREP_Platform_SystemTreeProperty
{
    struct SCOREP_Platform_SystemTreeProperty* next;
    char*                                      property_name;
    char*                                      property_value;
} SCOREP_Platform_SystemTreeProperty;


/**
 * Contains one element of the system tree path. Used to specify a path in the
 * system tree
 */
typedef struct SCOREP_Platform_SystemTreePathElement
{
    struct SCOREP_Platform_SystemTreePathElement* next;
    SCOREP_SystemTreeDomain                       domains;
    char*                                         node_class;
    char*                                         node_name;
    SCOREP_Platform_SystemTreeProperty*           properties;
    SCOREP_Platform_SystemTreeProperty**          properties_tail;
} SCOREP_Platform_SystemTreePathElement;


struct SCOREP_Location;

/**
 * Returns the path of this process in the system tree, starting at the root.
 *
 * The path can be traversed with the @a SCOREP_PLATFORM_SYSTEM_TREE_FORALL()
 * macro.
 *
 * The path should be freed with @a SCOREP_Platform_FreePath().
 *
 * @param root A pointer to a @a SCOREP_Platform_SystemTreePathElement* element
 *             which represents the already existing root node of the system tree
 *             machine domain and the start of the path for this process in the
 *             system tree.
 * @param machineName  The machine name used for the root node.
 * @param platformName The platform name used for the root node.
 *
 * @return SCOREP_SUCCESS on success else an appropriate error code.
 */
extern SCOREP_ErrorCode
SCOREP_Platform_GetPathInSystemTree( SCOREP_Platform_SystemTreePathElement** root,
                                     const char*                             machineName,
                                     const char*                             platformName );


/**
 * Deletes the path returned from @a SCOREP_Platform_GetPathInSystemTree().
 *
 * @param path Pointer to the returned path from @a SCOREP_Platform_GetPathInSystemTree
 *             that should be freed.
 */
extern void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreePathElement* path );


/**
 * Returns a unique node identifier.
 */
extern int32_t
SCOREP_Platform_GetNodeId( void );

/**
 * Used to iterate over a system tree path starting at the root.
 */
#define SCOREP_PLATFORM_SYSTEM_TREE_FORALL( _path, _node ) \
    for ( _node = _path; _node; _node = _node->next )

/**
 * Used to iterate over all properties of an system tree node.
 */
#define SCOREP_PLATFORM_SYSTEM_TREE_PROPERTY_FORALL( _node, _property ) \
    for ( _property = _node->properties; _property; _property = _property->next )


struct SCOREP_MountInfo;
typedef struct SCOREP_MountInfo SCOREP_MountInfo;

/**
 * Initializes mount information service and reads the mount table of the system.
 */
extern SCOREP_ErrorCode
SCOREP_Platform_MountInfoInitialize( void );

/**
 * Finalizes mount information service and destroys internal structures.
 */
extern void
SCOREP_Platform_MountInfoFinalize( void );

/**
 * Returns an appropriate mount entry for a given file/path.
 *
 * @param filename String that contains an absolute path.
 */
extern SCOREP_MountInfo*
SCOREP_Platform_GetMountInfo( const char* filename );

/**
 * Returns @a SCOREP_SystemTreeNodeHandle that represents the scope of a given mount entry.
 *
 * @param mount_entry Mount entry that was returned by @a SCOREP_Platform_GetMountInfo.
 */
extern SCOREP_SystemTreeNodeHandle
SCOREP_Platform_GetTreeNodeHandle( SCOREP_MountInfo* mount_entry );

/**
 * Adds mount information as @a IoFileProperty to a given @a SCOREP_IoFileHandle.
 *
 * @param io_file_handle Specifies the @a IoFileHandle where the propertier will be added.
 * @param mount_entry Mount entry that was returned by @a SCOREP_Platform_GetMountInfo.
 */
extern void
SCOREP_Platform_AddMountInfoProperties( SCOREP_IoFileHandle io_file_handle,
                                        SCOREP_MountInfo*   mount_entry );

/**
 * Returns the number of topology dimensions or 0 if undefined.
 */
uint32_t
SCOREP_Platform_GetHardwareTopologyNumberOfDimensions( void );

/**
 * Provides the hardware topology data about dimensions and its name.
 *
 * @param name              Name of the topology. Refers to static const
 *                          name strings, no free needed.
 * @param nDims             Number of dimensions, length of the arrays.
 * @param procsPerDim       Dimension sizes for all dimensions.
 * @param periodicityPerDim Periodicity information for all dimensions.
 * @param dimNames          Dimension names for all dimensions.
 *
 * @return          SCOREP_SUCCESS if coords have been provided,
 *                  SCOREP_ERROR_INVALID otherwise.
 */
SCOREP_ErrorCode
SCOREP_Platform_GetHardwareTopologyInformation( char const** name,
                                                int          nDims,
                                                int*         procsPerDim,
                                                int*         periodicityPerDim,
                                                char*        dimNames[] );

/**
 * Provides the coordinates for the respective hardware topology
 *
 * @param nCoords   Length of the coordinate array.
 * @param coords    The to be filled array of coordinates.
 * @param location  Location data to extract thread information if needed,
 *                  e.g., K Computer.
 *
 * @return          SCOREP_SUCCESS if coords have been provided,
 *                  SCOREP_ERROR_INVALID otherwise.
 */
SCOREP_ErrorCode
SCOREP_Platform_GetCartCoords( int                     nCoords,
                               int*                    coords,
                               struct SCOREP_Location* location  );

/**
 * Provides a check to allow the prevention of the platform topology
 * creation in certain cases, independent of user decisions.
 *
 * @return  true, if a topology can be generated. False, if the
 *          current mode doesn't support it. Default is true for
 *          a platform without additional restrictions.
 */
bool
SCOREP_Platform_GenerateTopology( void );

UTILS_END_C_DECLS

#endif /* SCOREP_PLATFORM_H */
