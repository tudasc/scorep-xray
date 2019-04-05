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
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_TOOLS_UTILS_HPP
#define SCOREP_TOOLS_UTILS_HPP

/**
 * @file       scorep_tools_utils.hpp
 *
 * @brief      Declares helper functions for the tools
 */

#include <string>
#include <vector>
#include <deque>
#include <sstream>

/**
    Retrieves the extension from a filename.
    @param filename  A filename
    @return the extension including the dot. If no extension is given
            (no dot appears in the filename) an empty string is returned.
 */
std::string
get_extension( const std::string& filename );

/**
   Removes the extension including the last dot from a filename.
   @param filename  A filename
   @return the filename without extension. If no extension is given
           (no dot appears in the filename) the whole filename is returned.
 */
std::string
remove_extension( const std::string& filename );

/**
   Simplifies a path.
   @param path The path that is simplified.
   @return the simplified path.
 */
std::string
simplify_path( const std::string& path );

/**
   Make a path absolute and simplifies it.
   @param path The path.
   @return the canonical path.
 */
std::string
canonicalize_path( const std::string& path );

/**
   Join two path elements and take care of absolute components.
   @param path0, path1 two paths.
   @return The joined path.
 */
std::string
join_path( const std::string& path0,
           const std::string& path1 );

/**
   Removes everything before the last slash from @a full_path.
   @param a file with its full path.
   @return The filename without its path.
 */
std::string
remove_path( const std::string& fullPath );

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
   @param pathList A list of possible library directories.
   @param allowStatic, Only honor static libraries when searching library.
   @param allowDynamic Only honor dynamic libraries when searching library.
   @param onlyDashLArgs Do nothing more, if @p library starts with '-l'.
   @return The full path of the library, including the library file name, if a matching
           library was found. Otherwise it return an empty string.
 */
std::string
find_library( const std::string&              library,
              const std::vector<std::string>& pathList,
              bool                            allowStatic,
              bool                            allowDynamic,
              bool                            onlyDashLArgs = false );

/**
   Converts a string to lower case
   @param str  The string that shall be converted to lower case.
   @return the string in lower case.
 */
std::string
scorep_tolower( std::string str );

/**
   Converts a string to upper case
   @param str  The string that shall be converted to upper case.
   @return the string in upper case.
 */
std::string
scorep_toupper( std::string str );

template<typename IteratorType>
std::string
join_to_string( IteratorType       iterator,
                IteratorType       end,
                const std::string& head,
                const std::string& tail,
                const std::string& delimiter )
{
    if ( iterator == end )
    {
        return "";
    }
    std::stringstream result;
    std::string       sep = head;
    while ( iterator != end )
    {
        result << sep << *iterator++;
        sep = delimiter;
    }
    result << tail;
    return result.str();
}

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
   Inserts a backslash before characters that a shell does not interpret as part of
   a file/path name by default.
   @param str  The string where we want to insert the backslashes.
   @return the modified string.
 */
std::string
backslash_special_chars( std::string str );

/**
   Removes the backslashes of backslashed characters.
   @param str The string which is modified.
   @return the resulting string.
 */
std::string
undo_backslashing( std::string str );


/**
   Creates a string which is usable as a string litaral in C/C++ code.
   Including the surrounding double quotes.
   @param str The string.
   @return The resulting string.
 */
std::string
make_string_literal( std::string str );


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

/**
 * Converts a string that contains multiple tokens separated by a certain string
 * into a vector of strings.
 * @param input     A string which contains multiple tokens, separated by @a delimiter
 * @param delimiter A string which separates the different tokens in @a input.
 * @return a list of tokens found in @input.
 */
std::vector<std::string>
split_string( const std::string& input,
              const std::string& delimiter );

/**
 * Converts deque of strings into a string where all entries are separated, by
 * the @a delimiter. The resulting string is prefixed with @a head and a @tail
 * is appended.
 * @param input      A list of strings that are concatenated.
 * @param head       A prefix for the resulting string.
 * @param delimiter  A string that is inserted between every two entries in @a input.
 * @param tail       A string that is appended after the last entry of @a input.
 */
std::string
deque_to_string( const std::deque<std::string>& input,
                 const std::string&             head,
                 const std::string&             delimiter,
                 const std::string&             tail );

/**
 * Converts a string that contains multiple tokens separated by a certain string
 * into a deque of strings.
 * @param input     A string which contains multiple tokens, separated by @a delimiter
 * @param delimiter A string which separates the different tokens in @a input.
 * @return a list of tokens found in @input.
 */
std::deque<std::string>
string_to_deque( const std::string& input,
                 const std::string& delimiter );

/**
 * Removes dublicate entries from a deque container of strings. It keeps only the
 * last occurence of each entry. This ensures that the dependencies are maintained.
 * @param input  The list of strings which where double entries are removed.
 * @return the list of strings where all but the last occurence are removed.
 */
std::deque<std::string>
remove_double_entries( const std::deque<std::string>& input );

std::string
wrap_lines( const std::string& message,
            size_t             indent,
            size_t             firstIndent,
            size_t             width = 80 );

#endif // SCOREP_TOOLS_UTILS_HPP
