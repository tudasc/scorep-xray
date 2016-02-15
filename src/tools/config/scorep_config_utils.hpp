/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_UTILS_HPP
#define SCOREP_CONFIG_UTILS_HPP

/**
 * @file scorep_config_utils.cpp
 *
 * Utility functions for the config tool.
 */

#include <string>
#include <deque>

/**
 *  Replace all occurrences of @ pattern in string @ original by
 *  @ replacement.
 *
 *  @param pattern          String that should be replaced.
 *  @param replacement      Replacement for @ pattern.
 *  @param original         Input string.
 *
 *  @return Returns a string where all occurrences of @ pattern are
 *          replaced by @ replacement.
 */
std::string
replace_all( const std::string& pattern,
             const std::string& replacement,
             std::string        original );


/**
 *  Trim  and replace multiple white-spaces in @ str by a single one.
 *
 *  @param str              String to be processed.
 *
 *  @return Returns string where all multiple white-spaces are replaced
 *          by a single one.
 */
std::string
remove_multiple_whitespaces( std::string str );

/**
 * Checks whether @a input contains @a item.
 * @param input A list of strings which is tested for @a item
 * @param item  A string that is searched in @a input.
 * @return true if @a input contains @a item.
 */
bool
has_item( const std::deque<std::string>& input,
          const std::string&             item );

/**
 * Removes dublicate entries from a deque container of strings. It keeps only the
 * last occurence of each entry. This ensures that the dependencies are maintained.
 * @param input  The list of strings which where double entries are removed.
 * @returns the list of strings where all but the last occurence are removed.
 */
std::deque<std::string>
remove_double_entries( const std::deque<std::string>& input );

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
 * Strips the @a head and leading @a delimiter from a @a input string.
 * Ignores leading whitespaces.
 * @param input     the string which is stripped.
 * @param head      the head which is removed from the @a input if present.
 * @param delimiter a delimiter which is removed from the @a input if present.
 */
std::string
strip_head( const std::string& input,
            const std::string& head_orig,
            const std::string& delimiter_orig );

/**
 * Converts a string that contains multiple tokens separated by a certain string
 * into a deque of strings.
 * @param input     A string which contains multiple tokens, separated by @a delimiter
 * @param delimiter A string which separates the different tokens in @a input.
 * @returns a list of tokens found in @input.
 */
std::deque<std::string>
string_to_deque( const std::string& input,
                 const std::string& delimiter );

#endif // SCOREP_CONFIG_UTILS_HPP
