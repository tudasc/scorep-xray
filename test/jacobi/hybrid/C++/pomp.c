#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
POMP2_Init_reg_4h6ldx9tlnr9d_3();
extern void
POMP2_Init_reg_1h6ldx9tlsrcd_1();

void
POMP2_Init_regions()
{
    POMP2_Init_reg_4h6ldx9tlnr9d_3();
    POMP2_Init_reg_1h6ldx9tlsrcd_1();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

#ifdef __cplusplus
}
#endif
