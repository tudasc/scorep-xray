#include <pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
extern "C" 
{
void POMP2_Init_regions_128756369693901_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "66*regionType=parallel*sscl=jacobi.cpp:67:67*escl=jacobi.cpp:88:88**" );
    POMP2_Assign_handle( &pomp2_region_2, "61*regionType=for*sscl=jacobi.cpp:72:72*escl=jacobi.cpp:87:87**" );
    POMP2_Assign_handle( &pomp2_region_3, "73*regionType=parallelfor*sscl=jacobi.cpp:154:154*escl=jacobi.cpp:161:161**" );
}
}
