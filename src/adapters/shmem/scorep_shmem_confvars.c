/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

#include <config.h>

#include <SCOREP_Subsystem.h>


/* The initialization struct for the SHMEM adapter */
const SCOREP_Subsystem SCOREP_Subsystem_ShmemAdapter =
{
    .subsystem_name = "SHMEM (config variables only)",
};
