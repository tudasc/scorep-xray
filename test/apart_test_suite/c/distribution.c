#include <config.h>
#include "distribution.h"
#include <stdio.h>
#include <string.h>

double default_sf = 1.0;

double
df_same( int      me,
         int      sz,
         double   scale,
         distr_t* dd )
{
    val1_distr_t* d = ( val1_distr_t* )dd;
    return d->val * scale;
}

double
df_cyclic2( int      me,
            int      sz,
            double   scale,
            distr_t* dd )
{
    val2_distr_t* d = ( val2_distr_t* )dd;
    if ( ( me % 2 ) == 0 )
    {
        return d->low * scale;
    }
    else
    {
        return d->high * scale;
    }
}

double
df_block2( int      me,
           int      sz,
           double   scale,
           distr_t* dd )
{
    val2_distr_t* d = ( val2_distr_t* )dd;
    if ( me < ( sz / 2 ) )
    {
        return d->low * scale;
    }
    else
    {
        return d->high * scale;
    }
}

double
df_linear( int      me,
           int      sz,
           double   scale,
           distr_t* dd )
{
    val2_distr_t* d    = ( val2_distr_t* )dd;
    double        step = ( d->high - d->low ) / ( sz - 1 );
    return ( d->low + me * step ) * scale;
}

double
df_peak( int      me,
         int      sz,
         double   scale,
         distr_t* dd )
{
    val2_n_distr_t* d = ( val2_n_distr_t* )dd;
    if ( me == d->n )
    {
        return d->high * scale;
    }
    else
    {
        return d->low * scale;
    }
}

double
df_cyclic3( int      me,
            int      sz,
            double   scale,
            distr_t* dd )
{
    val3_distr_t* d = ( val3_distr_t* )dd;
    switch ( me % 3 )
    {
        case 0:
            return d->low * scale;
        case 1:
            return d->med * scale;
        case 2:
            return d->high * scale;
        default:
            return 0.0;
    }
}

double
df_block3( int      me,
           int      sz,
           double   scale,
           distr_t* dd )
{
    val3_distr_t* d = ( val3_distr_t* )dd;
    if ( me < ( sz / 3 ) )
    {
        return d->low * scale;
    }
    else if ( me < ( 2 * sz / 3 ) )
    {
        return d->med * scale;
    }
    else
    {
        return d->high * scale;
    }
}

distr_func_t
atodf( char* distr_desc_str )
{
    switch ( distr_desc_str[ 0 ] )
    {
        case 's':
            return df_same;
        case 'c':
            switch ( distr_desc_str[ 1 ] )
            {
                case '2':
                    return df_cyclic2;
                case '3':
                    return df_cyclic3;
                default:
                    return 0;
            }
        case 'b':
            switch ( distr_desc_str[ 1 ] )
            {
                case '2':
                    return df_block2;
                case '3':
                    return df_block3;
                default:
                    return 0;
            }
        case 'l':
            return df_linear;
        case 'p':
            return df_peak;
        default:
            return 0;
    }
}

distr_t*
atodd( char* distr_desc_str )
{
    static val1_distr_t   d1;
    static val2_distr_t   d2;
    static val3_distr_t   d3;
    static val2_n_distr_t d2n;

    switch ( distr_desc_str[ 0 ] )
    {
        case 's':
        {
            sscanf( distr_desc_str, "%*c%*c:%lf", &d1.val );
            return &d1;
        }
        case 'c':
            switch ( distr_desc_str[ 1 ] )
            {
                case '2':
                {
                    sscanf( distr_desc_str, "%*c%*c:%lf:%lf", &d2.low, &d2.high );
                    return &d2;
                }
                case '3':
                {
                    sscanf( distr_desc_str, "%*c%*c:%lf:%lf:%lf", &d3.low, &d3.med, &d3.high );
                    return &d3;
                }
                default:
                    return 0;
            }
        case 'b':
            switch ( distr_desc_str[ 1 ] )
            {
                case '2':
                {
                    sscanf( distr_desc_str, "%*c%*c:%lf:%lf", &d2.low, &d2.high );
                    return &d2;
                }
                case '3':
                {
                    sscanf( distr_desc_str, "%*c%*c:%lf:%lf:%lf", &d3.low, &d3.med, &d3.high );
                    return &d3;
                }
                default:
                    return 0;
            }
        case 'l':
        {
            sscanf( distr_desc_str, "%*c%*c:%lf:%lf", &d2.low, &d2.high );
            return &d2;
        }
        case 'p':
        {
            sscanf( distr_desc_str, "%*c%*c:%lf:%lf:%d", &d2n.low, &d2n.high, &d2n.n );
            return &d2n;
        }
        default:
            return 0;
    }
}
