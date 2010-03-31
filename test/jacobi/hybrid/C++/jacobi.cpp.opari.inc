#include "pomp_lib.h"

extern "C"
{

#define POMP_DLIST_00001 shared(pomp_region_1,pomp_region_2)
static POMP_Region_handle pomp_region_1;
static POMP_Region_handle pomp_region_2;
#define POMP_DLIST_00003 shared(pomp_region_3)
static POMP_Region_handle pomp_region_3;

void POMP_Init_regions_1269963773215493_3()
{
    POMP_Assign_handle( &pomp_region_1, "66*regionType=parallel*sscl=jacobi.cpp:65:65*escl=jacobi.cpp:86:86**" );
    POMP_Assign_handle( &pomp_region_2, "61*regionType=for*sscl=jacobi.cpp:70:70*escl=jacobi.cpp:85:85**" );
    POMP_Assign_handle( &pomp_region_3, "73*regionType=parallelfor*sscl=jacobi.cpp:152:152*escl=jacobi.cpp:159:159**" );
}

}