#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "227*regionType=task*sscl=oa_omp_test.c:33:33*escl=oa_omp_test.c:36:36**"
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "231*regionType=taskwait*sscl=oa_omp_test.c:38:38*escl=oa_omp_test.c:38:38**"
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "231*regionType=parallel*sscl=oa_omp_test.c:51:51*escl=oa_omp_test.c:54:54**"
#define POMP2_DLIST_00004 shared(pomp2_region_4,pomp2_region_5)
static POMP2_Region_handle pomp2_region_4 = NULL;
#define pomp2_ctc_4 "231*regionType=parallel*sscl=oa_omp_test.c:61:61*escl=oa_omp_test.c:74:74**"
static POMP2_Region_handle pomp2_region_5 = NULL;
#define pomp2_ctc_5 "229*regionType=single*sscl=oa_omp_test.c:71:71*escl=oa_omp_test.c:72:72**"
#define POMP2_DLIST_00006 shared(pomp2_region_6,pomp2_region_7,pomp2_region_8,pomp2_region_9,pomp2_region_10,pomp2_region_11,pomp2_region_12)
static POMP2_Region_handle pomp2_region_6 = NULL;
#define pomp2_ctc_6 "233*regionType=parallel*sscl=oa_omp_test.c:77:77*escl=oa_omp_test.c:112:112**"
static POMP2_Region_handle pomp2_region_7 = NULL;
#define pomp2_ctc_7 "226*regionType=for*sscl=oa_omp_test.c:80:80*escl=oa_omp_test.c:84:84**"
static POMP2_Region_handle pomp2_region_8 = NULL;
#define pomp2_ctc_8 "229*regionType=master*sscl=oa_omp_test.c:86:86*escl=oa_omp_test.c:89:89**"
static POMP2_Region_handle pomp2_region_9 = NULL;
#define pomp2_ctc_9 "230*regionType=barrier*sscl=oa_omp_test.c:91:91*escl=oa_omp_test.c:91:91**"
static POMP2_Region_handle pomp2_region_10 = NULL;
#define pomp2_ctc_10 "250*regionType=critical*sscl=oa_omp_test.c:93:93*escl=oa_omp_test.c:96:96*criticalName=test1**"
static POMP2_Region_handle pomp2_region_11 = NULL;
#define pomp2_ctc_11 "229*regionType=atomic*sscl=oa_omp_test.c:98:98*escl=oa_omp_test.c:99:99**"
static POMP2_Region_handle pomp2_region_12 = NULL;
#define pomp2_ctc_12 "249*regionType=sections*sscl=oa_omp_test.c:101:101*escl=oa_omp_test.c:111:111*numSections=2**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_1l6l1fetlr3qk_12()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
    POMP2_Assign_handle( &pomp2_region_4, pomp2_ctc_4 );
    POMP2_Assign_handle( &pomp2_region_5, pomp2_ctc_5 );
    POMP2_Assign_handle( &pomp2_region_6, pomp2_ctc_6 );
    POMP2_Assign_handle( &pomp2_region_7, pomp2_ctc_7 );
    POMP2_Assign_handle( &pomp2_region_8, pomp2_ctc_8 );
    POMP2_Assign_handle( &pomp2_region_9, pomp2_ctc_9 );
    POMP2_Assign_handle( &pomp2_region_10, pomp2_ctc_10 );
    POMP2_Assign_handle( &pomp2_region_11, pomp2_ctc_11 );
    POMP2_Assign_handle( &pomp2_region_12, pomp2_ctc_12 );
}
