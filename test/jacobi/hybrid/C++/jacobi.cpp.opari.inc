#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
extern "C" 
{
void POMP2_Init_regions_1311763901619512_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "301*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/jacobi/hybrid/C++/jacobi.cpp:67:67*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/jacobi/hybrid/C++/jacobi.cpp:88:88**" );
    POMP2_Assign_handle( &pomp2_region_2, "324*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/jacobi/hybrid/C++/jacobi.cpp:72:72*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/jacobi/hybrid/C++/jacobi.cpp:87:87*hasReduction=1*hasOrdered=1**" );
    POMP2_Assign_handle( &pomp2_region_3, "308*regionType=parallelfor*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/jacobi/hybrid/C++/jacobi.cpp:154:154*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/jacobi/hybrid/C++/jacobi.cpp:161:161**" );
}
}
