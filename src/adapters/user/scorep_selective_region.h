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

/**
 *  @file       scorep_selective_region.h
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the declaration of types and functions for selective tracing.
 *  If selective tracing is enabled, the measurement system records those only parts
 *  of the whole execution, that are executed within a user specifed region. Only
 *  regions defined in the user adapter are selectable.
 *  For specifying the traced regions, the user must provide a configuration file, where
 *  every line starts with the name of the traced regions, optionally followed by
 *  a list of instance intervals which are recorded. Example:
 *  <code> my_region 1, 23:100, 110:120, 130 </code>
 *  This line would configure to trace form a region named 'my_region' the instances
 *  1, 23 to 100, 110 to 120, and 130.
 *  The configration file must be specified in the environment variable
 *  <code>SCOREP_SELECTIVE_CONFIG_FILE</code>.
 */

#include <stdbool.h>
#include <scorep/SCOREP_User_Types.h>
#include <scorep_utility/SCOREP_Vector.h>

/* **************************************************************************************
   Type definitions
****************************************************************************************/
/**
   Type for an instance interval of a selected region.
 */
typedef struct
{
    uint64_t first;
    uint64_t last;
} scorep_selected_interval;

/**
   Type for storing data of one selected region. Contained intervals are sorted after
   their first instance.
 */
typedef struct
{
    char*          region_name;
    SCOREP_Vector* intervals;
} scorep_selected_region;

/**
   Type for storing data for one user region.
 */
typedef struct SCOREP_User_Region
{
    SCOREP_RegionHandle      handle;
    scorep_selected_region*  selection;
    size_t                   current_interval_index;
    scorep_selected_interval current_interval;
    uint64_t                 enters;
    uint64_t                 exits;
    bool                     has_enabled;
} SCOREP_User_Region;

/**
   Creates an new struct of type SCOREP_User_Region and initializes it.
 */
SCOREP_User_Region*
scorep_user_create_region( const char* name );

/**
   Checks whether recording is switched on.
   @returns true if the enter event will be recorded.
 */
bool
scorep_selective_check_enter( SCOREP_User_RegionHandle region );

/**
   Checks whether recording is switched off.
   @returns true if the exit event will be recorded.
 */
bool
scorep_selective_check_exit( SCOREP_User_RegionHandle region );

/**
   Looks in the list of selected region whether a given region was selected.
   @pointer returns a pointer to an struct which contains the details about the selected
            region.
 */
scorep_selected_region*
scorep_selective_get_region( const char* name );
