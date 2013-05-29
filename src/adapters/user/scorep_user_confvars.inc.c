/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       src/adapters/user/scorep_user_confvars.inc.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

/**
   Contains the file name for the selective recording configuration file. This filename
   is filled out by the configration system. In particular, it is set by the environment
   variable 'SCOREP_SELECTIVE_CONFIG_FILE'.
 */
static char* scorep_selective_file_name;

/**
   Array of configuration variables for the selective recording.
 */
SCOREP_ConfigVariable scorep_selective_configs[] = {
    {
        "config_file",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_selective_file_name,
        NULL,
        "",
        "A file name which configures selective recording",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
