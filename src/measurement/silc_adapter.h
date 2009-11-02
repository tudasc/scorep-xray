#ifndef SILC_INTERNAL_ADAPTER_H
#define SILC_INTERNAL_ADAPTER_H

/**
 * @file        silc_adatper.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief       Exports the adapters array for the measurement system.
 */

#include <SILC_Adapter.h>

/** @brief a NULL terminated list of linked in adapters. */
extern SILC_Adapter* silc_adapters[];

#endif /* SILC_INTERNAL_ADAPTER_H */
