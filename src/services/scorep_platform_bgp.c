/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file        scorep_platform_bgp.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 * Implementation to obtain the system tree information from a BlueGene/P
 * system.
 */

#include <config.h>
#include <SCOREP_Platform.h>
#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>

SCOREP_Platform_SystemTreeNode*
SCOREP_Platform_GetSystemTree( size_t* number_of_entries )
{
    SCOREP_Platform_SystemTreeNode* path;
    *number_of_entries = 0;

    _BGP_Personality_t                mybgp;
    _BGP_UniversalComponentIdentifier uci;
    Kernel_GetPersonality( &mybgp, sizeof( _BGP_Personality_t ) );
    uci.UCI = mybgp.Kernel_Config.UniversalComponentIdentifier;

    if ( ( uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard ) ||
         ( uci.IOCard.Component == _BGP_UCI_Component_IOCard ) )
    {
        unsigned rack_row = uci.ComputeCard.RackRow;
        unsigned rack_col = uci.ComputeCard.RackColumn;
        unsigned midplane = uci.ComputeCard.Midplane;
        unsigned nodecard = uci.ComputeCard.NodeCard;

        /* Allocate memory for path */
        path = ( SCOREP_Platform_SystemTreeNode* )
               malloc( 5 * sizeof( SCOREP_Platform_SystemTreeNode ) );
        if ( path == NULL )
        {
            return NULL;
        }

        /* Allocate memory for name strings */
        for ( int i = 1; i < 5; i++ )
        {
            path[ i ].name = ( char* )malloc( 32 * sizeof( char ) );
            if ( path[ i ].name == NULL )
            {
                SCOREP_Platform_FreePath( path, i - 1 );
                return 0;
            }
        }

        path[ 0 ].class = "machine";
        path[ 0 ].name  = "BlueGene/P";
        path[ 1 ].class = "rack row";
        sprintf( path[ 1 ].name, "%u", rack_row );
        path[ 2 ].class = "rack column";
        sprintf( path[ 2 ].name, "%u", rack_col );
        path[ 3 ].class = "midplane";
        sprintf( path[ 3 ].name, "%u", midplane );
        path[ 4 ].class = "nodecard";
        sprintf( path[ 4 ].name, "%u", nodecard );
        *number_of_entries = 5;
        return path;
    }
    else
    {
        /* Allocate memory for path */
        path = ( SCOREP_Platform_SystemTreeNode* )
               malloc( sizeof( SCOREP_Platform_SystemTreeNode ) );
        if ( path == NULL )
        {
            return NULL;
        }
        path[ 0 ].class    = "machine";
        path[ 0 ].name     = "BlueGene/P";
        *number_of_entries = 1;
        return path;
    }
}

void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreeNode* path,
                          size_t                          number_of_entries )
{
    for ( int i = 1; i < number_of_entries; i++ )
    {
        free( path[ i ].name );
    }
    free( path );
}
