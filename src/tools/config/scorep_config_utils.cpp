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

/**
 * @file
 *
 * Utility functions for the config tool.
 */

#include <config.h>
#include <algorithm>
#include "scorep_config_utils.hpp"

#include <scorep_config_tool_backend.h>

using namespace std;

string
get_ld_wrap_flag( const string& prefix,
                  bool          nvcc )
{
    string wrap_flag_name = prefix + ( nvcc ? ".nvcc" : "" ) + ".wrap";

    return " -Wl,@" + wrap_flag_name;
}

string
get_ld_wrap_flag( const string& name,
                  bool          buildCheck,
                  bool          nvcc )
{
    extern string path_to_binary;
    string        wrap_flag_dir = buildCheck
                                  ? ( path_to_binary + "../share/" )
                                  : ( SCOREP_DATADIR "/" );

    return get_ld_wrap_flag( wrap_flag_dir + name, nvcc );
}
