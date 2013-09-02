/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SCOREP_WRAPGEN_HELP_H_
#define SCOREP_WRAPGEN_HELP_H_

/**
 * @file       SCOREP_Wrapgen_Help.h
 * @ingroup    Wrapgen_module
 *
 * @brief Usage output for the wrapper generator.
 */

#include <iostream>
using std::cout;
using std::endl;

namespace SCOREP
{
namespace Wrapgen
{
void
help
(
    string name
)
{
    cout << "'" << name << "' generates SCOREP wrappers for MPI functions\n" << endl;

    cout << "USAGE :" << name << " -p <prototypes.xml> [OPTIONS] <templatefile> [<template2> ... ]\n" << endl;
    cout << "If more than one template file are specified, the output of all templates" << endl;
    cout << "is appended.\n" << endl;
    cout << "TEMPLATE FILE TYPES:" << endl;
    cout << ".txt   are interpreted as text files and the content is copied." << endl;
    cout << ".w     are interpreted as function template. All selected functions" << endl;
    cout << "       from the prototypes are processed using the given wrapper template." << endl;
    cout << ".tmpl  A file template which contains basic code and function template" << endl;
    cout << "       configurations via #pragma wrapgen directives. When using this format," << endl;
    cout << "       the -r option takes no effect for this file.\n" << endl;
    cout << "OPTIONS:" << endl;
    cout << "-b   disables the banner output before generating new code\n" << endl;
    cout << "-r   (restrict option) can specify the output depending on the specifier" << endl;
    cout << "     g   specifies the group : '" << name << " -rgp2p' (for Peer to Peer)" << endl;
    cout << "     i   specifies the ID of a function: '" << name << " -ri92' (Function with ID 92)" << endl;
    cout << "     n   specifies a string in the functionname: '" << name << " -rncast'" << endl;
    cout << "         (all function including the string 'cast')" << endl;
    cout << "     t   specifies the returntype of the MPI function: '" << name << " -rtint'" << endl;
    cout << "         (all functions return an int)" << endl;
    cout << "     v   specifies the version: '" << name << " -rv1' (for MPI Version 1)\n" << endl;
    cout << "     A combination of -r options is possible using a '+' between the options." << endl;
    cout << "     Example: " << name << " -p proto.xml -rntype+v1+gp2p test.w" << endl;
    cout << "     Process all functions in proto.xml which contain the string 'type'," << endl;
    cout << "     and belong to MPI Version 1, and belong to group" << endl;
    cout << "     'peer-to peer communication'.\n" << endl;
    cout << "-h   Prints this help and exit.\n"  << endl;
    exit( 0 );
}
}   // namespace Wrapgen
}   // namespace SCOREP


#endif /* SCOREP_WRAPGEN_HELP_H_ */
