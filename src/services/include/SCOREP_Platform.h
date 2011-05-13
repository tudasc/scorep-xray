#ifndef SCOREP_PLATFORM_H
#define SCOREP_PLATFORM_H

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
 * @file       SCOREP_Platform.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/**
   Contains one element of the system tree. Used in the returned array of
   SCOREP_Platform_GetSystemTree.
 */
typedef struct
{
    char* name;
    char* class;
} SCOREP_Platform_SystemTreeNode;

/**
   Returns an array with the path from the process (without the process itself) to the
   root in the system tree. The root is the last entry in the array. The function
   allocates the arrey, which must be freed by the application.
   @param number_of_entries Returns the number of entries in the returned array.
 */
extern SCOREP_Platform_SystemTreeNode*
SCOREP_Platform_GetSystemTree( size_t* number_of_entries );

/**
   Deletes the array returned by SCOREP_Platform_GetSystemTree. Just using free might
   nott free all strings. Generically freeing all strings might cause segmentation
   faults if not all strings are dynamically allocated.
   @param path Pointer to the returned path from SCOREP_Platform_GetSystemTree that
               should be freed.
   @param number_of_entries number of entries in the path.
 */
extern void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreeNode* path,
                          size_t                          number_of_entries );


#endif /* SCOREP_PLATFORM_H */
