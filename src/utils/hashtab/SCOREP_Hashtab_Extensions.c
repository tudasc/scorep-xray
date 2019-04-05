/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
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
#include <SCOREP_Hashtab.h>

#include <UTILS_Error.h>

#include <stdlib.h>


void
SCOREP_Hashtab_FreeAll( SCOREP_Hashtab*               instance,
                        SCOREP_Hashtab_DeleteFunction deleteKey,
                        SCOREP_Hashtab_DeleteFunction deleteValue )
{
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    /* Validate arguments */
    UTILS_ASSERT( instance && deleteKey && deleteValue );

    /* Execute function for each entry */
    iter  = SCOREP_Hashtab_IteratorCreate( instance );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        if ( deleteKey )
        {
            deleteKey( entry->key );
        }

        if ( deleteValue )
        {
            deleteValue( entry->value.ptr );
        }

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
