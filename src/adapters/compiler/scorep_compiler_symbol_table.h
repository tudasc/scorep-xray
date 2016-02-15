/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015,
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

#ifndef SCOREP_COMPILER_SYMBOL_TABLE_H
#define SCOREP_COMPILER_SYMBOL_TABLE_H

#include <sys/types.h>

typedef void
( * scorep_compiler_process_symbol_cb )( long         addr,
                                         const char*  funcname,
                                         const char*  filename,
                                         unsigned int lno );

/**
 * @brief Symbol table analysis functions declaration.
 * Contains functions that read the symbol table of a executable and add all functions
 * found to a hashtable.
 */
void
scorep_compiler_load_symbols( void );

/**
 *
 * @brief Symbol table extraction method specific implementation.
 */
void
scorep_compiler_process_symbol_table( const char*                       executable,
                                      scorep_compiler_process_symbol_cb processSymbol );


/**
 * @param nmFilename Filename of file that contains nm -l output.
 * @param processSymbol Callback for processing individual symbols.
 */
void
scorep_compiler_parse_nm_file( const char*                       nmFilename,
                               scorep_compiler_process_symbol_cb processSymbol );

#endif /* SCOREP_COMPILER_SYMBOL_TABLE_H */
