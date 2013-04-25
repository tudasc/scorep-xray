#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
POMP2_Init_reg_pg6lu98tlh0n_3();
extern void
POMP2_Init_reg_mg6lu98tlexp_1();

void
POMP2_Init_regions()
{
    POMP2_Init_reg_pg6lu98tlh0n_3();
    POMP2_Init_reg_mg6lu98tlexp_1();
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
