/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * Implementation for node type specific data handling.
 *
 *
 *
 */

#include <config.h>
#include <stddef.h>

#include <SCOREP_Memory.h>

#include "scorep_profile_node.h"
#include "scorep_profile_definition.h"
#include "scorep_profile_converter.h"


/* ***************************************************************************************
   Defines
*****************************************************************************************/

/**
   @def SCOREP_PROFILE_REGION2DATA( handle )
   Casts a region handle to node specific data type.
   @param handle The region handle which is casted to node specific data.
 */
#define SCOREP_PROFILE_HANDLE2INT( handle ) ( handle )

/**
   @def SCOREP_PROFILE_DATA2REGION( data )
   Casts a node specific data item to a region handle.
   @param data The node specific data that is casted to a region handle.
 */
#define SCOREP_PROFILE_INT2HANDLE( data ) ( data )


/* ***************************************************************************************
   Prototypes of internal comparison and copy functions for type dependent data.
*****************************************************************************************/

static void
copy_flat( scorep_profile_type_data_t* destination,
           scorep_profile_type_data_t  source );

static uint64_t
hash_by_handle( scorep_profile_type_data_t data );

static uint64_t
hash_by_value( scorep_profile_type_data_t data );

static uint64_t
hash_by_both_entries( scorep_profile_type_data_t data );

static bool
less_than_by_handle( scorep_profile_type_data_t data1,
                     scorep_profile_type_data_t data2 );

static bool
less_than_by_value( scorep_profile_type_data_t data1,
                    scorep_profile_type_data_t data2 );

static bool
less_than_by_both_entries( scorep_profile_type_data_t data1,
                           scorep_profile_type_data_t data2 );

static bool
compare_only_handle( scorep_profile_type_data_t data1,
                     scorep_profile_type_data_t data2 );

static bool
compare_only_value( scorep_profile_type_data_t data1,
                    scorep_profile_type_data_t data2 );

static bool
compare_both_entries( scorep_profile_type_data_t data1,
                      scorep_profile_type_data_t data2 );


/* ***************************************************************************************
   Type dependent data handling types and variables
*****************************************************************************************/

/**
   Type for entries in the functions table list @a scorep_profile_type_data_funcs, which
   contains functions for handling type dependent data. Currently, it contains a
   pointer to a comparison function and a pointer to a copy function.
 */
typedef struct
{
    uint64_t ( * hash_func )( scorep_profile_type_data_t );

    bool ( * less_than_func )( scorep_profile_type_data_t,
                               scorep_profile_type_data_t );
    bool ( * comp_func )( scorep_profile_type_data_t,
                          scorep_profile_type_data_t );
    void ( * copy_func )( scorep_profile_type_data_t*,
                          scorep_profile_type_data_t );
} scorep_profile_type_data_func_t;

/* *INDENT-OFF* */

/**
  Functions table list for handling type dependent data. All entries must occur in the
  same order like in @a scorep_profile_node_type.
 */
scorep_profile_type_data_func_t scorep_profile_type_data_funcs[] = {
  { &hash_by_handle,       &less_than_by_handle,       &compare_only_handle,  &copy_flat }, /* Regular region */
  { &hash_by_both_entries, &less_than_by_both_entries, &compare_both_entries, &copy_flat }, /* Parameter string */
  { &hash_by_both_entries, &less_than_by_both_entries, &compare_both_entries, &copy_flat }, /* Parameter integer */
  { &hash_by_value,        &less_than_by_value,        &compare_only_value,   &copy_flat }, /* Thread root */
  { &hash_by_handle,       &less_than_by_handle,       &compare_only_handle,  &copy_flat }, /* Thread start */
  { &hash_by_value,        &less_than_by_value,        &compare_only_value,   &copy_flat }, /* Collapse */
  { &hash_by_handle,       &less_than_by_handle,       &compare_only_handle,  &copy_flat }, /* Task root */
};

/* *INDENT-ON* */

/* ***************************************************************************************
   Implementation of comparison ond copy functions for type dependent data.
*****************************************************************************************/

static void
copy_flat( scorep_profile_type_data_t* destination,
           scorep_profile_type_data_t  source )
{
    destination->handle = source.handle;
    destination->value  = source.value;
}

static bool
compare_only_handle( scorep_profile_type_data_t data1,
                     scorep_profile_type_data_t data2 )
{
    return data1.handle == data2.handle;
}

static bool
compare_only_value( scorep_profile_type_data_t data1,
                    scorep_profile_type_data_t data2 )
{
    return data1.value == data2.value;
}

static bool
compare_both_entries( scorep_profile_type_data_t data1,
                      scorep_profile_type_data_t data2 )
{
    return ( data1.handle == data2.handle ) &&
           ( data1.value  == data2.value );
}

static uint64_t
hash_by_handle( scorep_profile_type_data_t data )
{
    return data.handle;
}

static uint64_t
hash_by_value( scorep_profile_type_data_t data )
{
    return data.value;
}

static uint64_t
hash_by_both_entries( scorep_profile_type_data_t data )
{
    uint64_t val = data.handle;
    val  = ( val >> 1 ) | ( val << 31 );
    val += data.value;

    return val;
}

static bool
less_than_by_handle( scorep_profile_type_data_t data1,
                     scorep_profile_type_data_t data2 )
{
    return data1.handle < data2.handle;
}

static bool
less_than_by_value( scorep_profile_type_data_t data1,
                    scorep_profile_type_data_t data2 )
{
    return data1.value < data2.value;
}

static bool
less_than_by_both_entries( scorep_profile_type_data_t data1,
                           scorep_profile_type_data_t data2 )
{
    if ( data1.handle < data2.handle )
    {
        return true;
    }
    if ( data1.handle > data2.handle )
    {
        return false;
    }
    return data1.value < data2.value;
}

/* ***************************************************************************************
   Type dependent functions
*****************************************************************************************/

uint64_t
scorep_profile_hash_for_type_data( scorep_profile_type_data_t data,
                                   scorep_profile_node_type   type )
{
    return ( *scorep_profile_type_data_funcs[ type ].hash_func )( data );
}

/* Gives an ordering for two data sets */
bool
scorep_profile_less_than_for_type_data( scorep_profile_type_data_t data1,
                                        scorep_profile_type_data_t data2,
                                        scorep_profile_node_type   type )
{
    return ( *scorep_profile_type_data_funcs[ type ].less_than_func )( data1, data2 );
}

/* Compares two data sets */
bool
scorep_profile_compare_type_data( scorep_profile_type_data_t data1,
                                  scorep_profile_type_data_t data2,
                                  scorep_profile_node_type   type )
{
    return ( *scorep_profile_type_data_funcs[ type ].comp_func )( data1, data2 );
}

/* Copies a data set */
void
scorep_profile_copy_type_data( scorep_profile_type_data_t* destination,
                               scorep_profile_type_data_t  source,
                               scorep_profile_node_type    type )
{
    ( *scorep_profile_type_data_funcs[ type ].copy_func )( destination, source );
}

/* ***************************************************************************************
   Getter / Setter functions
*****************************************************************************************/

/**
   Stores the region handle in the type specific data of a regular region node.
 */
void
scorep_profile_type_set_region_handle( scorep_profile_type_data_t* data,
                                       SCOREP_RegionHandle         handle )
{
    data->handle = SCOREP_PROFILE_HANDLE2INT( handle );
}

/**
   Retrieves the region handle from the type specific data of a regular region node.
 */
SCOREP_RegionHandle
scorep_profile_type_get_region_handle( scorep_profile_type_data_t data )
{
    return SCOREP_PROFILE_INT2HANDLE( data.handle );
}

/**
   Stores the parameter handle in the type specific data of a parameter node.
 */
void
scorep_profile_type_set_parameter_handle( scorep_profile_type_data_t* data,
                                          SCOREP_ParameterHandle      handle )
{
    data->handle = SCOREP_PROFILE_HANDLE2INT( handle );
}

/**
   Retrieves the parameter handle from the type specific data of a parameter node.
 */
SCOREP_ParameterHandle
scorep_profile_type_get_parameter_handle( scorep_profile_type_data_t data )
{
    return SCOREP_PROFILE_INT2HANDLE( data.handle );
}

/**
   Stores the string handle in the type specific data of a parameter string node.
 */
void
scorep_profile_type_set_string_handle( scorep_profile_type_data_t* data,
                                       SCOREP_StringHandle         handle )
{
    data->value = SCOREP_PROFILE_HANDLE2INT( handle );
}

/**
   Retrieves the string handle from the type specific data of a parameter string node.
 */
SCOREP_StringHandle
scorep_profile_type_get_string_handle( scorep_profile_type_data_t data )
{
    return SCOREP_PROFILE_INT2HANDLE( data.value );
}

/**
   Stores the fork node where the thread was created in the type specific data of
   a thread start node.
 */
void
scorep_profile_type_set_fork_node( scorep_profile_type_data_t* data,
                                   scorep_profile_node*        node )
{
    data->handle = SCOREP_PROFILE_POINTER2INT( node );
}

/**
   Retrieves the fork node where the thread was created from the type specific data of
   a thread start node.
 */
scorep_profile_node*
scorep_profile_type_get_fork_node( scorep_profile_type_data_t data )
{
    return ( scorep_profile_node* )SCOREP_PROFILE_INT2POINTER( data.handle );
}

/**
   Stores the profile location data in the type specific data of a thread root node.
 */
void
scorep_profile_type_set_location_data( scorep_profile_type_data_t*  data,
                                       SCOREP_Profile_LocationData* location )
{
    data->handle = SCOREP_PROFILE_POINTER2INT( location );
}

/**
   Retrieves the profile location data from the type specific data of a thread root node.
 */
SCOREP_Profile_LocationData*
scorep_profile_type_get_location_data( scorep_profile_type_data_t data )
{
    return ( SCOREP_Profile_LocationData* )SCOREP_PROFILE_INT2POINTER( data.handle );
}

/**
   Retrives the depth from the type specific data of a collapse node.
 */
uint64_t
scorep_profile_type_get_depth( scorep_profile_type_data_t data )
{
    return data.value;
}

/**
   Stores the depth in the type specific data of a collapse node.
 */
void
scorep_profile_type_set_depth( scorep_profile_type_data_t* data,
                               uint64_t                    depth )
{
    data->value = depth;
}

/**
   Retrives the integer value from the type specific data of a parameter integer  node.
 */
uint64_t
scorep_profile_type_get_int_value( scorep_profile_type_data_t data )
{
    return data.value;
}

/**
   Stores the integer value in the type specific data of a parameter integer node.
 */
void
scorep_profile_type_set_int_value( scorep_profile_type_data_t* data,
                                   uint64_t                    value )
{
    data->value = value;
}

/**
   Retrives the pointer value from the type specific data.
 */
void*
scorep_profile_type_get_ptr_value( scorep_profile_type_data_t data )
{
    return SCOREP_PROFILE_INT2POINTER( data.value );
}

/**
   Stores the pointer value in the type specific data.
 */
void
scorep_profile_type_set_ptr_value( scorep_profile_type_data_t* data,
                                   void*                       value )
{
    data->value = SCOREP_PROFILE_POINTER2INT( value );
}
