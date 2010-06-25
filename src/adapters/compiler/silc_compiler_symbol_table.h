/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SILC_COMPILER_SYMBOL_TABLE_H
#define SILC_COMPILER_SYMBOL_TABLE_H

/**
 * @file       silc_compiler_symbol_table.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Symbol table analysis functions declaration.
 * Contains functions that read the symbol table of a executable and add all functions
 * found to a hashtable.
 */

void
silc_compiler_get_sym_tab( void );

#endif // SILC_COMPILER_SYMBOL_TABLE_H
