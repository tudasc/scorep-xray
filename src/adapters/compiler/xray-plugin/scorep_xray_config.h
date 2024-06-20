//
// Created by paul on 6/20/24.
//

#ifndef _BUILD_SCOREP_XRAY_CONFIG_H
#define _BUILD_SCOREP_XRAY_CONFIG_H

namespace XRayPlugin{

        struct Config {
            bool useDefaultInstrumentFilter{true};
            bool deleteInstrumentFilterAfterCompile{true};
            int instructionThreshold{1};
        };

} // XRayPlugin

#endif //_BUILD_SCOREP_XRAY_CONFIG_H
