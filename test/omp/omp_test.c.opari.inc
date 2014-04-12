#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "219*regionType=parallel*sscl=omp_test.c:49:49*escl=omp_test.c:52:52**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_4k6lsdctlab37_1()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
}
