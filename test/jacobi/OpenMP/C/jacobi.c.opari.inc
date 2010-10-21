#include <pomp2_lib.h>

extern void* FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate( FORTRAN_MANGLED(pomp_tpd) )
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;

void POMP2_Init_regions_1287563449107105_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "62*regionType=parallel*sscl=jacobi.c:60:60*escl=jacobi.c:89:89**" );
    POMP2_Assign_handle( &pomp2_region_2, "57*regionType=for*sscl=jacobi.c:62:62*escl=jacobi.c:69:69**" );
    POMP2_Assign_handle( &pomp2_region_3, "57*regionType=for*sscl=jacobi.c:73:73*escl=jacobi.c:88:88**" );
}
