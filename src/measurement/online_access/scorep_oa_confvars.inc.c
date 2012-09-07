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
 * @file       src/measurement/online_access/scorep_oa_confvars.inc.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


uint64_t silc_oa_registry_port  = 50100;
bool     scorep_oa_is_requested = false;
char*    silc_oa_registry_host  = NULL;
char*    silc_oa_app_name       = NULL;
uint64_t silc_oa_port           = 50010;


/**
   Configuration variable registration structures for the online access system.
 */
static SCOREP_ConfigVariable scorep_oa_configs[] =
{
    {
        "enable",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_oa_is_requested,
        NULL,
        "false",
        "Enable online access interface",
        ""
    },
    {
        "reg_port",
        SCOREP_CONFIG_TYPE_NUMBER,
        &silc_oa_registry_port,
        NULL,
        "50100",
        "Online access registry service port",
        ""
    },
    {
        "reg_host",
        SCOREP_CONFIG_TYPE_STRING,
        &silc_oa_registry_host,
        NULL,
        "localhost",
        "Online access registry service hostname",
        ""
    },
    {
        "base_port",
        SCOREP_CONFIG_TYPE_NUMBER,
        &silc_oa_port,
        NULL,
        "50010",
        "Base port for online access server",
        ""
    },
    {
        "appl_name",
        SCOREP_CONFIG_TYPE_STRING,
        &silc_oa_app_name,
        NULL,
        "appl",
        "Application name to be registered",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
