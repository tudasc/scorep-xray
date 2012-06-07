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
 * @status          alpha
 * @file            SCOREP_Hashtab.c
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @ingroup         SCOREP_Hashtab_module
 *
 * @brief           Extensions to the hashtable.
 *
 * The usage of the STL-like hash table requires frequent implementation of
 * a few patterns, e.g. for destruction of a hash table including all its
 * content, or adding a item which must be copied. The extentions contained in
 * this file provide a generic solution.
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>

#include <SCOREP_Error.h>
#include <SCOREP_Debug.h>

#include <SCOREP_Hashtab.h>


void
SCOREP_Hashtab_FreeAll( SCOREP_Hashtab*               instance,
                        SCOREP_Hashtab_DeleteFunction deleteKey,
                        SCOREP_Hashtab_DeleteFunction deleteValue )
{
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    /* Validate arguments */
    SCOREP_ASSERT( instance && deleteKey && deleteValue );

    /* Execute function for each entry */
    iter  = SCOREP_Hashtab_IteratorCreate( instance );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        deleteKey( entry->key );
        deleteValue( entry->value );
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    SCOREP_Hashtab_Free( instance );
}

/* ****************************************************************************
 * Delete Functions
 *****************************************************************************/

void
SCOREP_Hashtab_DeleteFree( void* item )
{
    free( item );
}

void
SCOREP_Hashtab_DeleteNone( void* item )
{
}

void
SCOREP_Hashtab_DeletePointer( void* item )
{
    free( ( void** )item );
}
