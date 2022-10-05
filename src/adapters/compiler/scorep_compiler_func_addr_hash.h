/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_COMPILER_FUNC_ADDR_HASH_H
#define SCOREP_COMPILER_FUNC_ADDR_HASH_H


#include <stdint.h>

void
scorep_compiler_func_addr_hash_dlclose_cb( void*       soHandle,
                                           const char* soFileName,
                                           uintptr_t   soBaseAddr,
                                           uint16_t    soToken );


#endif /* SCOREP_COMPILER_FUNC_ADDR_HASH_H */
