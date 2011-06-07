#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#ifdef __cplusplus
extern "C" {
#endif


#include <math.h>

extern double default_sf;

/*
 * TYPE distr_t:  generic type for distribution descriptors
 */
typedef void distr_t;

/*
 * TYPE distr_func_t:  type of distribution functions
 *
 *   IN  me  id (0..sz-1) inside group
 *   IN  sz  size of group (e.g. OpenMP team, MPI commnicator)
 *   IN  sf  scale factor
 *   IN  dd  distribution descriptor
 */
typedef double ( *distr_func_t )( int me, int sz, double sf, distr_t* dd );

/*
 * Generation Functions
 *
 * atodf:  distr_desc_str -> distribution function
 * atodd:  distr_desc_str -> distribution descriptor
 *
 */

distr_func_t
atodf( char* distr_desc_str );
distr_t*
atodd( char* distr_desc_str );

/*
 * Predefined distribution descriptors
 *
 * TYPE val1_distr_t:    1 distribution parameter (val)
 * TYPE val2_distr_t:    2 distribution parameters (low, high)
 * TYPE val2_n_distr_t:  3 distribution parameters (low, high, n)
 * TYPE val3_distr_t:    3 distribution parameters (low, med, high)
 */

typedef struct
{
    double val;
} val1_distr_t;

typedef struct
{
    double low;
    double high;
} val2_distr_t;

typedef struct
{
    double low;
    double high;
    int    n;
} val2_n_distr_t;

typedef struct
{
    double low;
    double high;
    double med;
} val3_distr_t;

/*
 * SAME Distribution: everyone gets the same value
 * uses:  val1_distr_t
 * desc:  ss
 */
double
df_same( int      me,
         int      sz,
         double   scale,
         distr_t* dd );

/*
 * CYCLIC2 Distribution: alternate between low and high
 * uses:  val2_distr_t
 * desc:  c2
 */
double
df_cyclic2( int      me,
            int      sz,
            double   scale,
            distr_t* dd );

/*
 * BLOCK2 Distribution: two blocks of low and high respectively
 * uses:  val2_distr_t
 * desc:  b2
 */
double
df_block2( int      me,
           int      sz,
           double   scale,
           distr_t* dd );

/*
 * LINEAR Distribution: linear extrapolation between low and high
 * uses:  val2_distr_t
 * desc:  lr
 */
double
df_linear( int      me,
           int      sz,
           double   scale,
           distr_t* dd );

/*
 * PEAK Distribution: task n -> high, all other low
 * uses:  val2_n_distr_t
 * desc:  pk
 */
double
df_peak( int      me,
         int      sz,
         double   scale,
         distr_t* dd );

/*
 * CYCLIC3 Distribution: alternate between low, med, and high
 * uses:  val3_distr_t
 * desc:  c3
 */
double
df_cyclic3( int      me,
            int      sz,
            double   scale,
            distr_t* dd );

/*
 * BLOCK3 Distribution: three blocks of low, med, and high respectively
 * uses:  val3_distr_t
 * desc:  b3
 */
double
df_block3( int      me,
           int      sz,
           double   scale,
           distr_t* dd );


#ifdef __cplusplus
}
#endif
#endif  /*DISTRIBUTION_H*/
