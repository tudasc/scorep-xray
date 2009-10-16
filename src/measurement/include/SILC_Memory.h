#ifndef SILC_MEMORY_H
#define SILC_MEMORY_H

/**
 * @file        SILC_Memory.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Provides a abstraction layer to the runtime memory management
 *        system.
 *
 * The measurement system will maybe switch to a pre-allocated memory management
 * system. To ease the transition into such a system, we provide a abstraction
 * layer to the memory management system with this interface.
 *
 * @note This interface will probably be moved to the utility library.
 */

#include <stdlib.h>

/**
 * Performs a memory allocation of size \a object_size.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param object_size The size of the object.
 *
 * @return never NULL
 */
void* SILC_Malloc( size_t object_size );

/**
 * Performs a memory allocation of size \a object_size * \a number_of_objects
 * and initialized the memory block with 0.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param number_of_objects The number of objects to be allocated.
 * @param object_size       The size of one object.
 *
 * @return never NULL
 */
void* SILC_Calloc( size_t number_of_objects,
                   size_t object_size );

/**
 * Performs a memory re-allocation of size \a new_object_size.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param old_pointer A pointer to a previously allocated memory object or NULL.
 * @param new_object_size       The new size of the object.
 *
 * @return never NULL
 */
void* SILC_Realloc( void*  old_pointer,
                    size_t new_object_size )

/**
 * Releases the memory pointed to by \a pointer.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param pointer The pointer.
 *
 * @return never NULL
 */
void SILC_Free( void* pointer )

#endif /* SILC_MEMORY_H */
