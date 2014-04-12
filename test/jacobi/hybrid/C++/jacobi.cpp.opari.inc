#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern "C"
{
extern int64_t FORTRAN_MANGLED(pomp_tpd);
}
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "247*regionType=parallel*sscl=jacobi.cpp:67:67*escl=jacobi.cpp:88:88**"
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "257*regionType=for*sscl=jacobi.cpp:72:72*escl=jacobi.cpp:87:87*hasReduction=1**"
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "254*regionType=parallelfor*sscl=jacobi.cpp:154:154*escl=jacobi.cpp:161:161**"
extern "C"
{
void POMP2_Init_reg_4h6ldx9tlnr9d_3()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
}
}
