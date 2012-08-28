/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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


#ifndef UTILS_NORMALIZE_FILE_H
#define UTILS_NORMALIZE_FILE_H


/**
 * @file       exception/normalize_file.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


static char*
normalize_file( const char* srcdir,
                const char* builddir,
                const char* file )
{
    char* normalized_file = UTILS_IO_JoinPath( 2, builddir, file );
    UTILS_IO_SimplifyPath( normalized_file );
    char* srcdir_dup = UTILS_CStr_dup( srcdir );
    UTILS_IO_SimplifyPath( srcdir_dup );
    size_t srcdir_len = strlen( srcdir_dup );

    if ( strncmp( normalized_file, srcdir_dup, srcdir_len ) == 0
         && normalized_file[ srcdir_len ] == '/' )
    {
        char* to   = normalized_file;
        char* from = normalized_file + srcdir_len + 1;
        while ( *from )
        {
            *to++ = *from++;
        }
        *to = *from;
    }
    free( srcdir_dup );

    return normalized_file;
}


#endif /* UTILS_NORMALIZE_FILE_H */
