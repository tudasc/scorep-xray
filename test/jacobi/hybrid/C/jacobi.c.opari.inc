#include <pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;

void POMP2_Init_regions_1287563645158730_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "62*regionType=parallel*sscl=jacobi.c:66:66*escl=jacobi.c:85:85**" );
    POMP2_Assign_handle( &pomp2_region_2, "57*regionType=for*sscl=jacobi.c:69:69*escl=jacobi.c:84:84**" );
    POMP2_Assign_handle( &pomp2_region_3, "69*regionType=parallelfor*sscl=jacobi.c:155:155*escl=jacobi.c:162:162**" );
}
