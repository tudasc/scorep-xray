/**
 * @file scorep_xry_plugin.hpp
 * @brief Score-P LLVM XRAY instrumentation plugin
 * Sources are instrumented with basic XRAY functionality by scorep at compile time.
 * XRAYPlugin namespace contains entry point to the management of the XRay plugin to invoke at runtime
 * Other functionality implemented as free functions in {@link src/adapters/compiler/xray-plugin/scorep_xray_plugin.cpp}
 */

#ifndef SCOREP_XRAY_PLUGIN_H
#define SCOREP_XRAY_PLUGIN_H

#define XRAY_INSTRUMENT_NEVER __attribute__((xray_never_instrument))
#define XRAY_INSTRUMENT_ALWAYS __attribute__((xray_always_instrument))

#include "config.h"

// This file is both C and C++ conformant, as it will be imported by a CC and a CPP compiler depending on the
// compilation step

UTILS_BEGIN_C_DECLS
#include <SCOREP_ErrorCodes.h>

/**
 * Initializes XRay runtime instrumentation and sets everything up.
 * On success, the application is fully instrumented and patched according to user specs, therefore ready to run
 * @return SCOREP_SUCCESS on success, error code otherwise
 */
SCOREP_ErrorCode initXRayPlugin() XRAY_INSTRUMENT_NEVER;

/**
 * Finalize XRay plugin and free associated memory.
 * Note: This does not call finalize on other Score-P internal structures
 */
void finalizeXRayPlugin() XRAY_INSTRUMENT_NEVER;

UTILS_END_C_DECLS

#endif








