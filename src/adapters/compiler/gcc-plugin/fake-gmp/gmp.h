/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2024,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_FAKE_GMP_H
#define SCOREP_FAKE_GMP_H

struct __fake_mpz_struct
{
    int empty;
};

typedef struct __fake_mpz_struct  mpz_t[ 1 ];
typedef struct __fake_mpz_struct* mpz_ptr;

void mpz_init( mpz_ptr );

void mpz_clear( mpz_ptr );

#endif /* SCOREP_FAKE_GMP_H */
