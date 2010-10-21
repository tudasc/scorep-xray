#include <pomp2_lib.h>

extern void* FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3,pomp2_region_4,pomp2_region_5,pomp2_region_6,pomp2_region_7)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;
static POMP2_Region_handle pomp2_region_4 = NULL;
static POMP2_Region_handle pomp2_region_5 = NULL;
static POMP2_Region_handle pomp2_region_6 = NULL;
static POMP2_Region_handle pomp2_region_7 = NULL;

void POMP2_Init_regions_1286533991217016_7()
{
    POMP2_Assign_handle( &pomp2_region_1, "68*regionType=parallel*sscl=pomp_test.c:25:25*escl=pomp_test.c:60:60**" );
    POMP2_Assign_handle( &pomp2_region_2, "63*regionType=for*sscl=pomp_test.c:28:28*escl=pomp_test.c:32:32**" );
    POMP2_Assign_handle( &pomp2_region_3, "66*regionType=master*sscl=pomp_test.c:34:34*escl=pomp_test.c:37:37**" );
    POMP2_Assign_handle( &pomp2_region_4, "65*regionType=barrier*sscl=pomp_test.c:39:39*escl=pomp_test.c:0:0**" );
    POMP2_Assign_handle( &pomp2_region_5, "87*regionType=critical*sscl=pomp_test.c:41:41*escl=pomp_test.c:44:44*criticalName=test1**" );
    POMP2_Assign_handle( &pomp2_region_6, "66*regionType=atomic*sscl=pomp_test.c:46:46*escl=pomp_test.c:47:47**" );
    POMP2_Assign_handle( &pomp2_region_7, "82*regionType=sections*sscl=pomp_test.c:49:49*escl=pomp_test.c:59:59*numSections=2**" );
}
