/**
 * @file scorep_xry_plugin.hpp
 * @brief Score-P LLVM XRAY instrumentation plugin
 * Sources are instrumented with basic XRAY functionality by scorep at compile time.
 * XRAYPlugin namespace contains entry point to the management of the XRay plugin to invoke at runtime
 * Other functionality implemented as free functions in {@link src/adapters/compiler/xray-plugin/scorep_xray_plugin.cpp}
 */

#ifndef SCOREP_XRAY_PLUGIN_HPP
#define SCOREP_XRAY_PLUGIN_HPP

#define XRAY_INSTRUMENT_NEVER __attribute__((xray_never_instrument))
#define XRAY_INSTRUMENT_ALWAYS __attribute__((xray_always_instrument))

namespace XRayPlugin {

    /**
    * Can hold metadata about a xray instrumented function, enriched with name info for Score-P
    */
    struct XRayIFuncMetadata {
        int32_t funcId{0};
        uint64_t funcAddress{0};
        std::string funcName;
        std::string funcNameDemangled;

        explicit XRayIFuncMetadata(int32_t funcId, uint64_t funcAddress) : funcId(funcId), funcAddress(funcAddress){}
        XRayIFuncMetadata(int32_t funcId, uint64_t funcAddress, std::string& funcName, std::string& funcNameDemangled)
                         : funcId(funcId), funcAddress(funcAddress), funcName(funcName), funcNameDemangled(funcNameDemangled){}
    };


    /**
     * Initializes XRay runtime instrumentation and sets up everything.
     * On success, the application is fully instrumented and patched according to user specs, therefore ready to run
     * @return 0 (SCOREP_SUCCESS) on success, error code otherwise
     */
    bool initXRay() XRAY_INSTRUMENT_NEVER;
}

#endif





