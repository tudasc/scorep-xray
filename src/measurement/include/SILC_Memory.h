#ifndef SILC_MEMORY_H
#define SILC_MEMORY_H

#include <stdlib.h>

/**
 * @file        SILC_Memory.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Provides a abstraction layer to the runtime memory management
 *        system.
 *
 */

/**
 * @defgroup SILC_Memory SILC Memory Management
 *
 * The measurement system will maybe switch to a pre-allocated memory management
 * system. To ease the transition into such a system, we provide a abstraction
 * layer to the memory management system with this interface.
 *
 * @note This interface will probably be moved to the utility library.
 *
 * @planned A memory pool interface is planned for the pre-allocated memory
 *          management system.
 */
/*@{*/

/**
 * Performs a memory allocation of size @a objectSize.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param objectSize The size of the object.
 *
 * @return never NULL
 */
void*
SILC_Malloc
(
    size_t objectSize
);

/**
 * Performs a memory allocation of size @a objectSize * @a numberOfObjects
 * and initialized the memory block with 0.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param numberOfObjects The number of objects to be allocated.
 * @param objectSize      The size of one object.
 *
 * @return never NULL
 */
void*
SILC_Calloc
(
    size_t numberOfObjects,
    size_t objectSize
);

/**
 * Performs a memory re-allocation of size @a newObjectSize.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param currentPointer A pointer to a previously allocated memory object or
 *                       NULL.
 * @param newObjectSize  The new size of the object.
 *
 * @return never NULL
 */
void*
SILC_Realloc
(
    void*  currentPointer,
    size_t newObjectSize
);

/**
 * Releases the memory pointed to by @a pointer.
 *
 * Errors are handled by the error handling system, which by default terminates
 * the measurement process.
 *
 * @param pointer The pointer.
 *
 * @return never NULL
 */
void
SILC_Free
(
    void* pointer
);

/*@}*/

#endif /* SILC_MEMORY_H */
