#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
extern "C" 
{
void POMP2_Init_regions_1311686542363312_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "190*regionType=parallelfor*sscl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/main.cpp:130:130*escl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/main.cpp:146:146**" );
}
}
