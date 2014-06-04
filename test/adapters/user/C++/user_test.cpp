/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @ file      user_test.cxx
 *
 * @brief Tests C++ user instrumentation and instrumentation of static constructors
 *        and destructors.
 */

#include <stdio.h>
#include <scorep/SCOREP_User.h>

#ifdef _OPENMP
#include <omp.h>
#endif

class MyClass
{
public:
    MyClass()
    {
        SCOREP_USER_REGION( "MyClass Constructor", 0 );
        printf( "In constructor of static class instance\n" );
#ifdef _OPENMP
        omp_init_lock( &m_lock );
#endif
    }

    virtual
    ~MyClass()
    {
        SCOREP_USER_REGION( "MyClass Destructor", 0 );
        printf( "In destructor of static class instance\n" );
#ifdef _OPENMP
        omp_destroy_lock( &m_lock );
#endif
    }

private:
#ifdef _OPENMP
    omp_lock_t m_lock;
#endif
};


MyClass my_static_instance;

int
main()
{
    SCOREP_USER_REGION( "main", 0 );
    printf( "In main\n" );
    return 0;
}
