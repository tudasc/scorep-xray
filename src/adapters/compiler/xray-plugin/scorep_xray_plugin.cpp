#include "scorep_xray_plugin.hpp"

int helloxray(){
    std::cout << "Xray instrumented!" << std::endl;
    return 0;
}

struct test{
    int a = helloxray();
} test;