/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @ file      user_test.cxx
 *
 * @brief Tests C++ user instrumentation and instrumentation of static constructors
 *        and destructors.
 */

#include <config.h>
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
        SCOREP_USER_REGION("MyClass Constructor", 0);
        printf("In constructor of static class instance\n");
#ifdef _OPENMP
        omp_init_lock( &m_lock );
#endif
    }

    virtual~ MyClass()
    {
        SCOREP_USER_REGION("MyClass Destructor", 0);
        printf("In destructor of static class instance\n");
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

int main()
{
    SCOREP_USER_REGION("main", 0);
    printf("In main\n");
    return 0;
}
