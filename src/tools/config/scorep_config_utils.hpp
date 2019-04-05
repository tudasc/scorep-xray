/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_UTILS_HPP
#define SCOREP_CONFIG_UTILS_HPP

/**
 * @file
 *
 * Utility functions for the config tool.
 */
#include <deque>
#include <string>

/**
 *  Produces the linker flag with the @-file argument for the given name.
 *
 *  @param prefix        The path and name of the wap file name.
 *  @param nvcc          Is the nvcc compiler wrapper in use?
 *
 *  @return Returns a string where all occurrences of @ pattern are
 *          replaced by @ replacement.
 */
std::string
get_ld_wrap_flag( const std::string& name,
                  bool               nvcc );

/**
 *  Produces the linker flag with the @-file argument for the given name.
 *
 *  @param name         The name of the wrapper.
 *  @param buildCheck   Was --build-check given?
 *  @param nvcc         Is the nvcc compiler wrapper in use?
 *
 *  @return Returns a string where all occurrences of @ pattern are
 *          replaced by @ replacement.
 */
std::string
get_ld_wrap_flag( const std::string& name,
                  bool               buildCheck,
                  bool               nvcc );

#endif /* SCOREP_CONFIG_UTILS_HPP */
