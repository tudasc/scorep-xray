#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern "C"
{
extern int64_t FORTRAN_MANGLED(pomp_tpd);
}
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "247*regionType=parallel*sscl=jacobi.cpp:57:57*escl=jacobi.cpp:87:87**"
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "242*regionType=for*sscl=jacobi.cpp:59:59*escl=jacobi.cpp:66:66**"
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "257*regionType=for*sscl=jacobi.cpp:71:71*escl=jacobi.cpp:86:86*hasReduction=1**"
extern "C"
{
void POMP2_Init_reg_ti6lq4btl1t1l_3()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
}
}
