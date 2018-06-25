/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2017,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_UTILS_HPP
#define SCOREP_INSTRUMENTER_UTILS_HPP

/**
 * @file
 *
 * @brief      Declares helper functions for the instrumenter
 */

#include <string>

/**
    Checks whether a file is a source file.
    @param filename A file name.
    @returns true if the file extension indicates a C/C++ or Fortran source
             file.
 */
bool
is_source_file( const std::string& filename );

/**
    Checks whether a file is a header file.
    @param filename A file name.
    @returns true if the file extension indicates a C/C++ or Fortran header
             file.
 */
bool
is_header_file( const std::string& filename );


/**
    Checks whether a file is an assembler file.
    @param filename A file name.
    @returns true if the file extension indicates a assembler source
             file.
 */
bool
is_assembler_file( const std::string& filename );

/**
    Checks whether a file is a Fortran source file.
    @param filename A file name.
    @returns true if the file extension indicates Fortran source file.
 */
bool
is_fortran_file( const std::string& filename );

/**
    Checks whether a file is a C source file.
    @param filename A file name.
    @returns true if the file extension indicates C source file.
 */
bool
is_c_file( const std::string& filename );

/**
    Checks whether a file is a C++ source file.
    @param filename A file name.
    @returns true if the file extension indicates C++ source file.
 */
bool
is_cpp_file( const std::string& filename );

/**
    Checks whether a file is a Cuda source file.
    @param filename A file name.
    @returns true if the file extension indicates Cuda source file.
 */
bool
is_cuda_file( const std::string& filename );

/**
    Checks whether a file is an object file.
    @param filename A file name.
    @returns true if the file extension indicates an object file.
 */
bool
is_object_file( const std::string& filename );

/**
    Checks whether a file is a library.
    @param filename A file name.
    @param allowDynamic accept dynamic libraries too (.so*)
    @param allowStatic accept static libraries too (.a)
    @returns true if the file starts with "lib" and has a known library extension.
 */
bool
is_library( const std::string& filename,
            bool               allowDynamic = true,
            bool               allowStatic  = true );

/**
    Extracts the library name from a library file (i.e., to be used in '-l' flags).
    @precond is_library(@p filename) == true
    @param filename A file name.
    @returns The library name.
 */
std::string
get_library_name( const std::string& filename );

/**
    Check if the given value is the specified library.
    @param libraryName The expected library.
    @param value The value to check against the library name.
    @returns True if check is successful.
 */
bool
check_lib_name( const std::string& libraryName,
                const std::string& value );

/**
 * Creates a string for intermediate files to avoid name clashes.
 */
std::string
create_random_string( void );


#endif // SCOREP_INSTRUMENTER_UTILS_HPP
