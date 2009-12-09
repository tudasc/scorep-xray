/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


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
extern size_t        silc_number_of_adapters;


#endif /* SILC_INTERNAL_ADAPTER_H */
