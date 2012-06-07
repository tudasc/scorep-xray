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

#ifndef SCOREP_IO_H
#define SCOREP_IO_H

#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdio.h>

#if defined( PACKAGE_SCOREP )
#include <SCOREP_Error_Codes.h>
#elif defined( PACKAGE_OTF2 )
#include <otf2/OTF2_Error_Codes.h>
#else
#error Unsupported package.
#endif

/**
   Checks whether a file exists.
   @param filename  The name of the file which is checked.
   @returns true if the file exists, else false.
 */
EXTERN bool
MANGLE_NAME( DoesFileExist ) ( const char* filename );

/**
   Gives the path to the executeable name (argv[0]) without the executable
   name and trailing slash. Checks whether the given name contains path
   information. If not, it searches the directories in the PATH environment
   variable.
   @param exe    The executable name as given to argv[0].
   @returns The path to the executable without the executable name itselfs and
            trailing slash. If no path was found, NULL is retured.
 */
EXTERN char*
MANGLE_NAME( GetExecutablePath ) ( const char* exe );

/**
 * Reads a line until a newline from @a file. The line is stored in the buffer
 * pointed to by @a buffer. If the buffer is not large enough, the buffer is reallocated
 * to contain the whole line. The current size of the buffer is stored in @a buffer_size.
 * @param buffer      Pointer to a storage location, where the pointer to an allocated
 *                    memory block is stored, to which the read line written. If the
 *                    memory block is too small it will be enlarged. The buffer must be
 *                    freed by the calling function. The pointer to the buffer must not
 *                    be NULL. However, *buffer may be NULL.
 * @param buffer_size Points to a memory location where the size of the memory block
 *                    pointed to by @a **buffer is stored. If the memory is resized,
 *                    the buffer_size is updated.
 * @param file        A file handle of an already open file, from which a line is read.
 * @returns MANGLE_NAME( SUCCESS ), if a line was read successfully. If the end of the file
 *          was reached, MANGLE_NAME( ERROR_END_OF_BUFFER ) is returned. If an error occured,
 *          an error code is returned. Possible error codes are:
 *          MANGLE_NAME( ERROR_MEM_ALLOC_FAILED ) and MANGLE_NAME( ERROR_FILE_INTERACTION ).
 */
EXTERN MANGLE_NAME( Error_Code )
MANGLE_NAME( IO_GetLine ) ( char**  buffer,
                            size_t * buffer_size,
                            FILE *   file );

/**
 * Returns true if @a path is a relative or absolute path.
 */
EXTERN bool
MANGLE_NAME( IO_HasPath ) ( const char* path );

/**
 * Cuts of the path prefix from a filename.
 * @param path a filename which might have a path prefix.
 * @returns a pointer to the position in @a path where the filename starts.
 */
EXTERN const char*
MANGLE_NAME( IO_GetWithoutPath ) ( const char* path );

/**
 * Simplifies the @a path. If @path contains ../ subpathes they are merged with
 * preceding pathes. Thus the final representation of the path is minimized. Furthermore,
 * /./ sequences are cut out.
 * If the original path contained at least one slash, this functions will prepend
 * a ./ if the simplified path has no nore slashes. This maintains the information
 * whether the sting was a simple file name or given as a path.
 * @param path the path that is simplified. The simplified path is stored in the same
 *        location. It must not be a pointer to a constant in the program segment.
 *        Passing a pointer to a string literal results in a segmentation fault.
 */
EXTERN void
MANGLE_NAME( IO_SimplifyPath ) ( char* path );

/**
 * Joins an arbitrary number of path elements into one path.
 *
 * If a path element is an absolute path, any previous path elements are
 * discarded. If an path element is empty (ie. "") it is ignored.
 *
 * @param nPaths The number of variable argument entries following the
 *               @a nPaths argument.
 * @param ...    Path elements to be joined. All of type <code>const char*</code>.
 *
 * @return Joined path, allocated with @a malloc.
 */
EXTERN char*
MANGLE_NAME( IO_JoinPath ) ( int nPaths,
                             ... );

/**
 * Wrapper function for gethostname which is not declared or implemented on all systems
 * @param name     Pointer to a memory location of length @a namelen where the name
 *                 is stored. The result is NULL-terminated, except when the name was
 *                 too long for the memory location.
 * @param namelen  Length of the memory segment reserved for @a name.
 * @returns zero if the operation was successful. Else a non-zero value is returned.
 */
EXTERN int
MANGLE_NAME( IO_GetHostname ) ( char*  name,
                                size_t namelen );

/**
 * Wrapper function for getcwd which is not declared or implemented on all systems.
 * Uses the same syntax as the POSIX getcwd.
 * @param buf  Points to the buffer to copy the current working directory to,
 *             of NULL if getcwd() should allocate the buffer.
 * @param size Is the size, in bytes, of the array of characters that buf points to.
 */
EXTERN char*
MANGLE_NAME( IO_GetCwd ) ( char*  buf,
                           size_t size );

#endif /* SCOREP_IO_H */
