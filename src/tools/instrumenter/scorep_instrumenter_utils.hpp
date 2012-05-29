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

#ifndef SCOREP_INSTRUMENTER_UTILS_HPP
#define SCOREP_INSTRUMENTER_UTILS_HPP

/**
 * @file       scorep_instrumenter_utils.hpp
 * @status     alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief      Declares helper functions for the instrumenter
 */

#include <string>

/**
    Retrieves the extension from a filename.
    @param filename  A filename
    @retuns the extension including the dot. If no extension is given
            (no dot appears in the filename) an empty string is returned.
 */
std::string
get_extension( std::string filename );

/**
   Removes the extension including the last dot from a filename.
   @param filename  A filename
   @retuns the filename without extension. If no extension is given
           (no dot appears in the filename) the whole filename is returned.
 */
std::string
remove_extension( std::string filename );

/** Checks whether a file is a source file.
    @param filename A file name.
    @returns true if the file extension indicates a C/C++ or Fortran source
             file.
 */
bool
is_source_file( std::string filename );

/** Checks whether a file is a Fortran source file.
    @param filename A file name.
    @returns true if the file extension indicates Fortran source file.
 */
bool
is_fortran_file( std::string filename );

/** Checks whether a file is a C source file.
    @param filename A file name.
    @returns true if the file extension indicates C source file.
 */
bool
is_c_file( std::string filename );

/** Checks whether a file is a C++ source file.
    @param filename A file name.
    @returns true if the file extension indicates C++ source file.
 */
bool
is_cpp_file( std::string filename );

/** Checks whether a file is a CUDA source file.
    @param filename A file name.
    @returns true if the file extension indicates CUDA source file.
 */
bool
is_cuda_file( std::string filename );

/** Checks whether a file is an object file.
    @param filename A file name.
    @returns true if the file extension indicates an object file.
 */
bool
is_object_file( std::string filename );

/** Checks whether a file is a library.
    @param filename A file name.
    @returns true if the file extension indicates an object file.
 */
bool
is_library( std::string filename );

/**
   Removes everything before the first slash from @a full_path.
   @param a file with its full path.
   @returns The filename without its path.
 */
std::string
remove_path( std::string full_path );

/**
   Returns the path contained in @ filename
   @param filename a file name with a full path
 */
std::string
extract_path( std::string filename );

/** Trim  and replace multiple white-spaces in @ str by a single one.
    @param str              String to be processed.
    @return Returns string where all multiple white-spaces are replaced
            by a single one.
 */
std::string
remove_multiple_whitespaces( std::string str );

/** Replace all occurrences of @ pattern in string @ original by
    @ replacement.
    @param pattern          String that should be replaced.
    @param replacement      Replacement for @ pattern.
    @param original         Input string.
    @return Returns a string where all occurrences of @ pattern are
            replaced by @ replacement.
 */
std::string
replace_all( std::string &pattern,
             std::string &replacement,
             std::string  original );




#endif // SCOREP_INSTRUMENTER_UTILS_HPP
