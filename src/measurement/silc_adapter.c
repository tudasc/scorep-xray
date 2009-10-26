#include <stdlib.h>
#include <stdio.h>

/**
 * @file        silc_adatper.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief       Holds the list of adapters linked into the measurement system.
 */

#include <silc_adapter.h>

/**
 * List of adapters.
 */
SILC_Adapter* silc_adapters[] = {
    NULL /**< Needs to be NULL-terminated */
};
