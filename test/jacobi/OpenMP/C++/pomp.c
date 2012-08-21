#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
POMP2_Init_reg_1320065783274680_3();
extern void
POMP2_Init_reg_1320065788651638_1();

void
POMP2_Init_regions()
{
    POMP2_Init_reg_1320065783274680_3();
    POMP2_Init_reg_1320065788651638_1();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

const char*
POMP2_Get_opari2_version()
{
    return "0.9";
}

#ifdef __cplusplus
}
#endif
