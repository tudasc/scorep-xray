/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Uses libbfd to iterate over the symbol table of an executable.
 */

#include <config.h>

#include "scorep_compiler_symbol_table.h"

#include <stdlib.h>
#include <string.h>

#include <bfd.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_Types.h>

/* ***************************************************************************************
   BFD based symbol table analysis
*****************************************************************************************/

/**
 * Get symbol table using BFD. Stores all functions obtained from the symbol table
 * in a hashtable. The key of the hashtable is the function pointer. This must be done
 * during initialization of the GNU compiler adapter, because enter and exit events
 * provide only a file pointer.
 * It also collects information about source file and line number.
 */
void
scorep_compiler_process_symbol_table( const char*                       executable,
                                      scorep_compiler_process_symbol_cb processSymbol )
{
    bfd*      bfd_image = 0;
    int       nr_all_syms;
    int       i;
    size_t    size;
    asymbol** canonic_symbols;

    /* initialize BFD */
    bfd_init();

    UTILS_DEBUG( "Read symbol table using BFD" );

    bfd_image = bfd_openr( executable, 0 );
    if ( !bfd_image )
    {
        UTILS_ERROR( SCOREP_ERROR_ENOENT, "BFD image not present at path: %s", executable );
        return;
    }

    /* check image format   */
    if ( !bfd_check_format( bfd_image, bfd_object ) )
    {
        UTILS_ERROR( SCOREP_ERROR_EIO, "BFD: bfd_check_format(): failed" );
        return;
    }

    /* return if file has no symbols at all */
    if ( !( bfd_get_file_flags( bfd_image ) & HAS_SYMS ) )
    {
        UTILS_ERROR( SCOREP_ERROR_FILE_INTERACTION,
                     "BFD: bfd_get_file_flags(): failed" );
        return;
    }

    /* get the upper bound number of symbols */
    size = bfd_get_symtab_upper_bound( bfd_image );

    /* HAS_SYMS can be set even with no symbols in the file! */
    if ( size < 1 )
    {
        UTILS_ERROR( SCOREP_ERROR_INVALID_SIZE_GIVEN,
                     "BFD: bfd_get_symtab_upper_bound(): < 1" );
        return;
    }

    /* read canonicalized symbols  */
    canonic_symbols = ( asymbol** )malloc( size );

    nr_all_syms = bfd_canonicalize_symtab( bfd_image, canonic_symbols );
    if ( nr_all_syms < 1 )
    {
        UTILS_ERROR( SCOREP_ERROR_INVALID_SIZE_GIVEN,
                     "BFD: bfd_canonicalize_symtab(): < 1" );
    }
    for ( i = 0; i < nr_all_syms; ++i )
    {
        long         addr;
        const char*  filename = NULL;
        const char*  funcname;
        unsigned int lno = SCOREP_INVALID_LINE_NO;

        if ( !canonic_symbols[ i ] )
        {
            UTILS_WARN_ONCE( "Failed to retrive symbol information from BFD." );
            continue;
        }

        /* Process only symbols of type function */
        if ( !( canonic_symbols[ i ]->flags & BSF_FUNCTION ) )
        {
            continue;
        }

        /* ignore system functions */
        if ( strncmp( canonic_symbols[ i ]->name, "bfd_", 4 ) == 0 ||
             strncmp( canonic_symbols[ i ]->name, "_bfd_", 5 ) == 0 ||
             strstr( canonic_symbols[ i ]->name, "@@" ) != NULL )
        {
            continue;
        }

        /* get filename and linenumber from debug info */
        /* needs -g */
        filename = NULL;
        lno      = SCOREP_INVALID_LINE_NO;

        /* calculate function address */
        addr = canonic_symbols[ i ]->section->vma + canonic_symbols[ i ]->value;

        /* get the source info for every function in case of gnu by default */
        /* calls BFD_SEND */
        bfd_find_nearest_line( bfd_image,
                               bfd_get_section( canonic_symbols[ i ] ),
                               canonic_symbols,
                               canonic_symbols[ i ]->value,
                               &filename,
                               &funcname,
                               &lno );
        funcname = canonic_symbols[ i ]->name;

        processSymbol( addr, funcname, filename, lno );
    }
    free( canonic_symbols );
    bfd_close( bfd_image );

    return;
}
