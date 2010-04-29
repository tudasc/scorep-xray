#ifndef SILC_DEFINITIONHANDLES_H
#define SILC_DEFINITIONHANDLES_H

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
 * @file       SILC_DefinitionHandles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


typedef struct SILC_String_Definition         SILC_String_Definition;
typedef struct SILC_String_Definition_Movable SILC_String_Definition_Movable;
typedef SILC_String_Definition_Movable*       SILC_StringHandle;


typedef struct SILC_Location_Definition         SILC_Location_Definition;
typedef struct SILC_Location_Definition_Movable SILC_Location_Definition_Movable;
typedef SILC_Location_Definition_Movable*       SILC_LocationHandle;


typedef struct SILC_MPICommunicator_Definition         SILC_MPICommunicator_Definition;
typedef struct SILC_MPICommunicator_Definition_Movable SILC_MPICommunicator_Definition_Movable;
typedef SILC_MPICommunicator_Definition_Movable*       SILC_MPICommunicatorHandle;


typedef struct SILC_MPIWindow_Definition         SILC_MPIWindow_Definition;
typedef struct SILC_MPIWindow_Definition_Movable SILC_MPIWindow_Definition_Movable;
typedef SILC_MPIWindow_Definition_Movable*       SILC_MPIWindowHandle;


typedef struct SILC_MPICartesianTopology_Definition         SILC_MPICartesianTopology_Definition;
typedef struct SILC_MPICartesianTopology_Definition_Movable SILC_MPICartesianTopology_Definition_Movable;
typedef SILC_MPICartesianTopology_Definition_Movable*       SILC_MPICartesianTopologyHandle;


typedef struct SILC_MPICartesianCoords_Definition         SILC_MPICartesianCoords_Definition;
typedef struct SILC_MPICartesianCoords_Definition_Movable SILC_MPICartesianCoords_Definition_Movable;
typedef SILC_MPICartesianCoords_Definition_Movable*       SILC_MPICartesianCoordsHandle;


typedef struct SILC_IOFileGroup_Definition         SILC_IOFileGroup_Definition;
typedef struct SILC_IOFileGroup_Definition_Movable SILC_IOFileGroup_Definition_Movable;
typedef SILC_IOFileGroup_Definition_Movable*       SILC_IOFileGroupHandle;


typedef struct SILC_IOFile_Definition         SILC_IOFile_Definition;
typedef struct SILC_IOFile_Definition_Movable SILC_IOFile_Definition_Movable;
typedef SILC_IOFile_Definition_Movable*       SILC_IOFileHandle;


typedef struct SILC_MarkerGroup_Definition         SILC_MarkerGroup_Definition;
typedef struct SILC_MarkerGroup_Definition_Movable SILC_MarkerGroup_Definition_Movable;
typedef SILC_MarkerGroup_Definition_Movable*       SILC_MarkerGroupHandle;


typedef struct SILC_Marker_Definition         SILC_Marker_Definition;
typedef struct SILC_Marker_Definition_Movable SILC_Marker_Definition_Movable;
typedef SILC_Marker_Definition_Movable*       SILC_MarkerHandle;


typedef struct SILC_Parameter_Definition         SILC_Parameter_Definition;
typedef struct SILC_Parameter_Definition_Movable SILC_Parameter_Definition_Movable;
typedef SILC_Parameter_Definition_Movable*       SILC_ParameterHandle;


typedef struct SILC_Callpath_Definition         SILC_Callpath_Definition;
typedef struct SILC_Callpath_Definition_Movable SILC_Callpath_Definition_Movable;
typedef SILC_Callpath_Definition_Movable*       SILC_CallpathHandle;


#endif /* SILC_DEFINITIONHANDLES_H */
