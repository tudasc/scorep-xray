#include "pomp_lib.h"

extern "C"
{

#define POMP_DLIST_00001 shared(pomp_region_1)
static POMP_Region_handle pomp_region_1;

void POMP_Init_regions_1269949782624507_1()
{
    POMP_Assign_handle( &pomp_region_1, "69*regionType=parallelfor*sscl=main.cpp:111:111*escl=main.cpp:127:127**" );
}

}