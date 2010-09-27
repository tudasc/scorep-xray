/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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


#ifndef SCOREP_INTERNAL_ADAPTER_H
#define SCOREP_INTERNAL_ADAPTER_H


/**
 * @file        scorep_adatper.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief       Exports the adapters array for the measurement system.
 */


#include <SCOREP_Adapter.h>
#include <stddef.h>

/** @brief a NULL terminated list of linked in adapters. */
extern const SCOREP_Adapter* scorep_adapters[];
extern const size_t          scorep_number_of_adapters;

#endif /* SCOREP_INTERNAL_ADAPTER_H */
