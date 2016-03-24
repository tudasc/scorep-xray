/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
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
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_UTILS_HPP
#define SCOREP_INSTRUMENTER_UTILS_HPP

/**
 * @file       scorep_instrumenter_utils.hpp
 *
 * @brief      Declares helper functions for the instrumenter
 */

#include <string>
#include <vector>

/**
    Retrieves the extension from a filename.
    @param filename  A filename
    @retuns the extension including the dot. If no extension is given
            (no dot appears in the filename) an empty string is returned.
 */
std::string
get_extension( const std::string& filename );

/**
   Removes the extension including the last dot from a filename.
   @param filename  A filename
   @retuns the filename without extension. If no extension is given
           (no dot appears in the filename) the whole filename is returned.
 */
std::string
remove_extension( const std::string& filename );

/**
    Checks whether a file is a source file.
    @param filename A file name.
    @returns true if the file extension indicates a C/C++ or Fortran source
             file.
 */
bool
is_source_file( const std::string& filename );

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
    @returns true if the file extension indicates an object file.
 */
bool
is_library( const std::string& filename );

/**
   Checks whether the library name is a library we want to interpose.
   @param library_name  The name of the library.
 */
bool
is_interposition_library( const std::string& library_name );

/**
   Checks whether the library name is an mpi library.
   @param library_name  The name of the library.
 */
bool
is_mpi_library( const std::string& library_name );

/**
   Checks whether the library name is an shmem library.
   @param library_name  The name of the library.
 */
bool
is_shmem_library( const std::string& library_name );

/**
   Checks whether the library name is an OpenCL library.
   @param library_name  The name of the library.
 */
bool
is_opencl_library( const std::string& library_name );

/**
   Checks whether the library name is a pthread library.
   @param library_name  The name of the library.
 */
bool
is_pthread_library( const std::string& library_name );

/**
   Simpifies a path.
   @param path The path that is simplified.
   @returns the simplified path.
 */
std::string
simplify_path( const std::string& path );

/**
   Removes everything before the last slash from @a full_path.
   @param a file with its full path.
   @returns The filename without its path.
 */
std::string
remove_path( const std::string& full_path );

/**
   Returns the path contained in @ filename
   @param filename a file name with a full path
 */
std::string
extract_path( const std::string& filename );

/**
    Trim  and replace multiple white-spaces in @ str by a single one.
    @param str              String to be processed.
    @return Returns string where all multiple white-spaces are replaced
            by a single one.
 */
std::string
remove_multiple_whitespaces( std::string str );

/**
    Replace all occurrences of @ pattern in string @ original by
    @ replacement.
    @param pattern          String that should be replaced.
    @param replacement      Replacement for @ pattern.
    @param original         Input string.
    @return Returns a string where all occurrences of @ pattern are
            replaced by @ replacement.
 */
std::string
replace_all( const std::string& pattern,
             const std::string& replacement,
             std::string        original );

/**
   Tests whether a given file exists.
   @param filename Name of the file, which existence is tested.
   @return true, if the file exists
 */
bool
exists_file( const std::string& filename );

/**
   Searches for a library in a list of path.
   @param library   The name of the library to search. If it starts with '-l', it is
                    replaced by 'lib'. A suffix is appended.
   @param path_list A list of possible library direcories.
   @return The full path of the library, including the library file name, if a matching
           library was found. Otherwise it return an empty string.
 */
std::string
find_library( std::string                     library,
              const std::vector<std::string>& path_list,
              bool                            allow_dynamic,
              bool                            allow_static );

/**
   Converts a string to lower case
   @param str  The string that shall be converted to lower case.
   @returns the string in lower case.
 */
std::string
scorep_tolower( std::string str );

/**
   Converts a string to upper case
   @param str  The string that shall be converted to upper case.
   @returns the string in upper case.
 */
std::string
scorep_toupper( std::string str );

/**
   Converts a vector of strings into a string. If the list is empty, an empty
   string is returned.
   @param list      The list of strings
   @param head      A string that is prepended at the front.
   @param tail      A string that is appended at the back.
   @param delimiter A string that is inserted between each strings from the list.
 */
std::string
scorep_vector_to_string( const std::vector<std::string>& list,
                         const std::string&              head,
                         const std::string&              tail,
                         const std::string&              delimiter );

/**
   Inserts a backsalsh before characters that a shell does not interpret as part of
   a file/path name by default.
   @param str  The string where we want to insert the backslashes.
   @return the modified string.
 */
std::string
backslash_special_chars( std::string str );

/**
   Removes the backslashes of backslashed characters.
   @param str The string which is modified.
   @returns the resulting string.
 */
std::string
undo_backslashing( std::string str );


/**
 *  Locates a substring in a string list.
 *  @param haystackList A list of strings joined by @a separator.
 *  @param needleString The string to search for.
 *  @param separator    The separator character which splits the lists into
 *                      individual tokens.
 */
std::string::size_type
find_string_in_list( const std::string& haystackList,
                     const std::string& needleString,
                     char               separator );

/**
 *  Removes all occurrences of a substring in a string list.
 *  @param haystackList A list of strings joined by @a separator.
 *  @param needleString The string to remove.
 *  @param separator    The separator character which splits the lists into
 *                      individual tokens.
 */
std::string
remove_string_from_list( const std::string& haystackList,
                         const std::string& needleString,
                         char               separator );

#endif // SCOREP_INSTRUMENTER_UTILS_HPP
