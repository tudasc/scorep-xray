#include "pomp_lib.h"

#define POMP_DLIST_00001 shared(pomp_region_1,pomp_region_2,pomp_region_3)
static POMP_Region_handle pomp_region_1;
static POMP_Region_handle pomp_region_2;
static POMP_Region_handle pomp_region_3;

void POMP_Init_regions_1269863383320327_3()
{
    POMP_Assign_handle( &pomp_region_1, "62*regionType=parallel*sscl=jacobi.c:58:58*escl=jacobi.c:87:87**" );
    POMP_Assign_handle( &pomp_region_2, "57*regionType=for*sscl=jacobi.c:60:60*escl=jacobi.c:67:67**" );
    POMP_Assign_handle( &pomp_region_3, "57*regionType=for*sscl=jacobi.c:71:71*escl=jacobi.c:86:86**" );
}
