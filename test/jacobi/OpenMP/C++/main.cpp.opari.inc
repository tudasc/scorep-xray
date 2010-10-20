#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
extern "C" 
{
void POMP2_Init_regions_1287559834726324_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "69*regionType=parallelfor*sscl=main.cpp:129:129*escl=main.cpp:145:145**" );
}
}
