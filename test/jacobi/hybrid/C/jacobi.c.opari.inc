#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "239*regionType=parallel*sscl=jacobi.c:65:65*escl=jacobi.c:84:84**"
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "249*regionType=for*sscl=jacobi.c:68:68*escl=jacobi.c:83:83*hasReduction=1**"
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "246*regionType=parallelfor*sscl=jacobi.c:154:154*escl=jacobi.c:161:161**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_pg6lu98tlh0n_3()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
}
