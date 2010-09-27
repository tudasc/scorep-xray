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

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file       config.h
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @status     alpha
 *
 * @brief      Provide a single config.h that hides the frontend/backend
 *             issues from the developer
 *
 */

#if defined CROSS_BUILD
    #if defined FRONTEND_BUILD
        #include <config-frontend.h>
    #elif defined BACKEND_BUILD_NOMPI
        #include <config-backend.h>
    #elif defined BACKEND_BUILD_MPI
        #include <config-backend-mpi.h>
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
#else
    #error "You can not use config.h without defining either CROSS_BUILD or NOCROSS_BUILD."
#endif

#endif /* CONFIG_H */
