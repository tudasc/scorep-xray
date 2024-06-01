
#include "scorep_xray_plugin.hpp"

int helloxray(){
#ifdef XRAY_INSTRUMENTED
    std::cout << "Xray instrumented!" << std::endl;
#else
    std::cout << "Not xray instrumented!" << std::endl;
#endif
    return 0;
}