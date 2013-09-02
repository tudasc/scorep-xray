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

#ifndef SCOREP_COMPILER_SYMBOL_TABLE_H
#define SCOREP_COMPILER_SYMBOL_TABLE_H

/**
 * @file       scorep_compiler_symbol_table.c
 *
 * @brief Symbol table analysis functions declaration.
 * Contains functions that read the symbol table of a executable and add all functions
 * found to a hashtable.
 */

void
scorep_compiler_get_sym_tab( void );

#endif // SCOREP_COMPILER_SYMBOL_TABLE_H
