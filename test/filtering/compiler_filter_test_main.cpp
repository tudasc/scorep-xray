/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#include <iostream>

using std::cout;
using std::endl;

namespace Kokkos
{
namespace Tools
{
void
hidden();
}

namespace Profiling
{
void
hidden();
}

void
visible();
}

extern "C"
{
void
pomp_hidden();

void
Pomp_hidden();

void
POMP_hidden();

void
visible_pomp();
};

int
main( int argc, char** argv )
{
    Kokkos::Tools::hidden();
    Kokkos::Profiling::hidden();
    Kokkos::visible();
    pomp_hidden();
    Pomp_hidden();
    POMP_hidden();
    visible_pomp();
    cout << endl;
    return 0;
}
