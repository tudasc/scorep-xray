/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef UTILS_NORMALIZE_FILE_H
#define UTILS_NORMALIZE_FILE_H


/**
 * @file
 *
 *
 */


static char*
normalize_file( const char* file )
{
    char* normalized_file = UTILS_IO_JoinPath( 2, AFS_PACKAGE_BUILDDIR, file );
    UTILS_IO_SimplifyPath( normalized_file );
    char srcdir[] = AFS_PACKAGE_SRCDIR;
    UTILS_IO_SimplifyPath( srcdir );
    size_t srcdir_len = strlen( srcdir );

    /* srcdir is guaranteed to have an trailing slash, so no need to test for it
       in the file */
    if ( strncmp( normalized_file, srcdir, srcdir_len ) == 0 )
    {
        char* to   = normalized_file;
        char* from = normalized_file + srcdir_len;
        while ( *from )
        {
            *to++ = *from++;
        }
        *to = *from;
    }

    return normalized_file;
}


#endif /* UTILS_NORMALIZE_FILE_H */
