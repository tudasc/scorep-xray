#include <pomp2_lib.h>

extern void* pomp_tpd_;
#pragma omp threadprivate(pomp_tpd_)
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;
extern "C" 
{
void POMP2_Init_regions_1287563513947239_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "66*regionType=parallel*sscl=jacobi.cpp:57:57*escl=jacobi.cpp:87:87**" );
    POMP2_Assign_handle( &pomp2_region_2, "61*regionType=for*sscl=jacobi.cpp:59:59*escl=jacobi.cpp:66:66**" );
    POMP2_Assign_handle( &pomp2_region_3, "61*regionType=for*sscl=jacobi.cpp:71:71*escl=jacobi.cpp:86:86**" );
}
}
