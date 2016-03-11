/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief  A wrapper for gcc-plugin.h.
 *
 */

#ifndef SCOREP_PLUGIN_INTERFACE_H
#define SCOREP_PLUGIN_INTERFACE_H

/* This file should be included before config.h, otherwise gcc-plugin.h
 * will mess up some defines */

#include <gcc-plugin.h>
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME

#endif /* SCOREP_PLUGIN_INTERFACE_H */
