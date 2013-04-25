#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
POMP2_Init_reg_ub6lb2ctln0oj_3();
extern void
POMP2_Init_reg_rb6lb2ctl52rj_1();

void
POMP2_Init_regions()
{
    POMP2_Init_reg_ub6lb2ctln0oj_3();
    POMP2_Init_reg_rb6lb2ctl52rj_1();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

const char*
POMP2_Get_opari2_version()
{
    return "1.0.7";
}

#ifdef __cplusplus
}
#endif
