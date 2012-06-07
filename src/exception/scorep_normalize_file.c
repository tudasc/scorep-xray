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
 * @file       src/exception/scorep_normalize_file.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>


#include <SCOREP_CStr.h>
#include <SCOREP_IO.h>


char*
scorep_normalize_file( const char* srcdir,
                       const char* builddir,
                       const char* file )
{
    char* normized_file = MANGLE_NAME( IO_JoinPath ) ( 2, builddir, file );
    MANGLE_NAME( IO_SimplifyPath ) ( normized_file );
    char* srcdir_dup = MANGLE_NAME( CStr_dup ) ( srcdir );
    MANGLE_NAME( IO_SimplifyPath ) ( srcdir_dup );
    size_t srcdir_len = strlen( srcdir_dup );

    if ( strncmp( normized_file, srcdir_dup, srcdir_len ) == 0
         && normized_file[ srcdir_len ] == '/' )
    {
        char* to   = normized_file;
        char* from = normized_file + srcdir_len + 1;
        while ( *from )
        {
            *to++ = *from++;
        }
        *to = *from;
    }
    free( srcdir_dup );

    return normized_file;
}
