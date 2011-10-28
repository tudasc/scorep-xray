#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;
extern "C" 
{
void POMP2_Init_regions_1311686535614111_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "187*regionType=parallel*sscl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/jacobi.cpp:57:57*escl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/jacobi.cpp:87:87**" );
    POMP2_Assign_handle( &pomp2_region_2, "182*regionType=for*sscl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/jacobi.cpp:59:59*escl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/jacobi.cpp:66:66**" );
    POMP2_Assign_handle( &pomp2_region_3, "210*regionType=for*sscl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/jacobi.cpp:71:71*escl=/home/peterp/currentwork/silc/tasking/test/jacobi/OpenMP/C++/jacobi.cpp:86:86*hasReduction=1*hasOrdered=1**" );
}
}
