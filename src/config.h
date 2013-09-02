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

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file       config.h
 *
 * @brief      Provide a single config.h that hides the frontend/backend
 *             issues from the developer
 *
 */

#if defined CROSS_BUILD
    #if defined FRONTEND_BUILD
        #include <config-frontend.h>
        #include <config-backend-for-frontend.h>
        #define HAVE_BACKEND( H ) ( defined( HAVE_BACKEND_ ## H ) && HAVE_BACKEND_ ## H )
    #elif defined BACKEND_BUILD_NOMPI
        #include <config-backend.h>
        #define HAVE_BACKEND( H ) ( defined( HAVE_ ## H ) && HAVE_ ## H )
    #elif defined BACKEND_BUILD_MPI
        #include <config-backend-mpi.h>
        #define HAVE_BACKEND( H ) ( defined( HAVE_ ## H ) && HAVE_ ## H )
    #else
        #error "You can not use config.h without defining either FRONTEND_BUILD, BACKEND_BUILD_NOMPI or BACKEND_BUILD_MPI."
    #endif

#elif defined NOCROSS_BUILD
    #if defined BACKEND_BUILD_NOMPI
        #include <config-backend.h>
    #elif defined BACKEND_BUILD_MPI
        #include <config-backend-mpi.h>
    #else
        #error "You can not use config.h without defining either BACKEND_BUILD_NOMPI or BACKEND_BUILD_MPI."
    #endif

    #define HAVE_BACKEND( H ) ( defined( HAVE_ ## H ) && HAVE_ ## H )

#elif defined SCORE_BUILD
    #include <config-score.h>

#else
    #error "You can not use config.h without defining either CROSS_BUILD or NOCROSS_BUILD."
#endif

#include <config-common.h>

#include <config-custom.h>

#endif /* CONFIG_H */
