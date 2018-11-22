/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_FILTER_H
#define SCOREP_FILTER_H

/**
 * @file
 *
 *
 * The interface to the filtering system.  The file that contains the filter
 * definitions can be set via the environment variable
 * SCOREP_FILTERING_FILE. If no file is specified or the given file is not
 * found, filtering is disabled.
 */

#include <SCOREP_ErrorCodes.h>

UTILS_BEGIN_C_DECLS

typedef struct SCOREP_Filter SCOREP_Filter;

/**
 * Creates a filter object.
 *
 * @return The new filter object.
 */
SCOREP_Filter*
SCOREP_Filter_New( void );


/**
 * Destroys a filter object.
 *
 * @param filter  The new filter object.
 */
void
SCOREP_Filter_Delete( SCOREP_Filter* filter );


/**
 * Loads filter rules from a file.
 *
 * @param filter        The filter rules.
 * @param fileName      The file name to load filter rules from.
 *
 * @return SCOREP_SUCCESS or error code.
 */
SCOREP_ErrorCode
SCOREP_Filter_ParseFile( SCOREP_Filter* filter,
                         const char*    fileName );


/**
 * Checks whether a file/function combination is filtered.
 *
 * @param filter        The filter rules.
 * @param fileName      The file name of the file that contain the considered function.
 * @param functionName  The function name as it will be displayed in profile and
 *                      trace definitions.
 * @param mangledName   A mangled name of the function is available that differs
 *                      from @a function_name. If no different mangled name is
 *                      available you may pass NULL here. In this case the function_name
 *                      will be used to compare against patterns that are prepended
 *                      by the MANGLED keyword. In particular, passing NULL or the
 *                      same string as for @a function_name leads to the same results.
 * @param[out] result   True, if the tested file/function should be excluded from measurement.
 *
 * @return SCOREP_SUCCESS or error code.
 */
SCOREP_ErrorCode
SCOREP_Filter_Match( const SCOREP_Filter* filter,
                     const char*          fileName,
                     const char*          functionName,
                     const char*          mangledName,
                     int*                 result );

/**
 * Checks whether a file is filtered.
 *
 * @param filter        The filter rules.
 * @param fileName      The file name of the file that is checked.
 * @param[out] result   True, if the tested file should be excluded from measurement.
 *
 * @return SCOREP_SUCCESS or error code.
 */
SCOREP_ErrorCode
SCOREP_Filter_MatchFile( const SCOREP_Filter* filter,
                         const char*          fileName,
                         int*                 result );

/**
 * Checks whether a function is filtered.
 *
 * @param filter        The filter rules.
 * @param functionName  The function name as it will be displayed in profile and
 *                      trace definitions.
 * @param mangledName   A mangled name of the function is available that differs
 *                      from @a function_name. If no different mangled name is
 *                      available you may pass NULL here. In this case the function_name
 *                      will be used to compare against patterns that are prepended
 *                      by the MANGLED keyword. In particular passing NULL or the
 *                      same string as for @a function_name leads to the same results.
 * @param[out] result   True, if the tested function should be excluded from measurement.
 *
 * @return SCOREP_SUCCESS or error code.
 */
SCOREP_ErrorCode
SCOREP_Filter_MatchFunction( const SCOREP_Filter* filter,
                             const char*          functionName,
                             const char*          mangledName,
                             int*                 result );

/**
 * Checks whether a function is explicitly included.
 *
 * Handles a special case, where the result can only be true, if this function
 * was matched by an explicit "INCLUDE" rule other than "INCLUDE *" or the implicit
 * "start with everything is included".
 *
 * @param filter        The filter rules.
 * @param functionName  The function name as it will be displayed in profile and
 *                      trace definitions.
 * @param mangledName   A mangled name of the function is available that differs
 *                      from @a function_name. If no different mangled name is
 *                      available you may pass NULL here. In this case the function_name
 *                      will be used to compare against patterns that are prepended
 *                      by the MANGLED keyword. In particular passing NULL or the
 *                      same string as for @a function_name leads to the same results.
 * @param[out] result   True, if the tested function was explicitly included.
 *
 * @return SCOREP_SUCCESS or error code.
 */
SCOREP_ErrorCode
SCOREP_Filter_IncludeFunction( const SCOREP_Filter* filter,
                               const char*          functionName,
                               const char*          mangledName,
                               int*                 result );

UTILS_END_C_DECLS

#endif /* SCOREP_FILTER_H */
