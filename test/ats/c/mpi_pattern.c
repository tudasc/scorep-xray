#include <config.h>
#include <stdlib.h>
#include "mpi_pattern.h"

MPI_Datatype base_type = MPI_INT;
int          base_cnt  = 1;

void
set_base_comm( MPI_Datatype type,
               int          cnt )
{
    base_type = type;
    base_cnt  = cnt;
}

void
set_base_comm_a( char* basecomm_desc_str )
{
    switch ( basecomm_desc_str[ 0 ] )
    {
        case 'b':
            base_type = MPI_BYTE;
            break;
        case 'c':
            base_type = MPI_CHAR;
            break;
        case 'd':
            base_type = MPI_DOUBLE;
            break;
        case 'f':
            base_type = MPI_FLOAT;
            break;
        case 'i':
            base_type = MPI_INT;
            break;
        case 'r':
            base_type = MPI_FLOAT;
            break;
        default:
            base_type = MPI_DOUBLE;
            break;
    }
    base_cnt = atoi( basecomm_desc_str + 2 );
}

void
get_base_comm( MPI_Datatype* type,
               int*          cnt )
{
    *type = base_type;
    *cnt  = base_cnt;
}

void
late_sender( double   basework,
             double   extrawork,
             int      r,
             MPI_Comm c )
{
    val2_distr_t dd;
    int          i;
    mpi_buf_t*   buf = alloc_mpi_buf( base_type, base_cnt );

    dd.low  = basework + extrawork;
    dd.high = basework;

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_cyclic2, &dd, default_sf, c );
        mpi_commpattern_sendrecv( buf, DIR_UP, 0, 0, c );
    }
    free_mpi_buf( buf );
}

void
late_receiver( double   basework,
               double   extrawork,
               int      r,
               MPI_Comm c )
{
    val2_distr_t dd;
    int          i;
    mpi_buf_t*   buf = alloc_mpi_buf( base_type, base_cnt );

    dd.low  = basework;
    dd.high = basework + extrawork;

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_cyclic2, &dd, default_sf, c );
        mpi_commpattern_sendrecv( buf, DIR_UP, 0, 0, c );
    }
    free_mpi_buf( buf );
}

void
imbalance_at_mpi_barrier( distr_func_t df,
                          distr_t*     dd,
                          int          r,
                          MPI_Comm     c )
{
    int i;

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df, dd, default_sf, c );
        MPI_Barrier( c );
    }
}

void
imbalance_at_mpi_alltoall( distr_func_t df,
                           distr_t*     dd,
                           int          r,
                           MPI_Comm     c )
{
    mpi_buf_t* sbuf, * rbuf;
    int        i, sz;

    MPI_Comm_size( c, &sz );
    sbuf = alloc_mpi_buf( base_type, base_cnt * sz );
    rbuf = alloc_mpi_buf( base_type, base_cnt * sz );

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df, dd, default_sf, c );
        MPI_Alltoall( sbuf->buf, sbuf->cnt / sz, sbuf->type,
                      rbuf->buf, rbuf->cnt / sz, rbuf->type, c );
    }
    free_mpi_buf( sbuf );
    free_mpi_buf( rbuf );
}

void
late_broadcast( double   basework,
                double   rootextrawork,
                int      root,
                int      r,
                MPI_Comm c )
{
    mpi_buf_t*     buf = alloc_mpi_buf( base_type, base_cnt );
    val2_n_distr_t dd;
    int            i, sz;

    MPI_Comm_size( c, &sz );
    dd.low  = basework;
    dd.high = basework + rootextrawork;
    dd.n    = root % sz;

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_peak, &dd, default_sf, c );
        MPI_Bcast( buf->buf, buf->cnt, buf->type, dd.n, c );
    }
    free_mpi_buf( buf );
}

void
late_scatter( double   basework,
              double   rootextrawork,
              int      root,
              int      r,
              MPI_Comm c )
{
    mpi_buf_t*     sbuf = 0, * rbuf = 0;
    val2_n_distr_t dd;
    int            i, me, sz;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );
    dd.low  = basework;
    dd.high = basework + rootextrawork;
    dd.n    = root % sz;
    sbuf    = alloc_mpi_buf( base_type, me == dd.n ? base_cnt * sz : 0 );
    rbuf    = alloc_mpi_buf( base_type, base_cnt );

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_peak, &dd, default_sf, c );
        MPI_Scatter( sbuf->buf, sbuf->cnt / sz, sbuf->type,
                     rbuf->buf, rbuf->cnt, rbuf->type, dd.n, c );
    }
    free_mpi_buf( sbuf );
    free_mpi_buf( rbuf );
}

void
late_scatterv( double   basework,
               double   rootextrawork,
               int      root,
               int      r,
               MPI_Comm c )
{
    val1_distr_t   de;
    val2_n_distr_t dd;
    mpi_vbuf_t*    vbuf = 0;
    int            i, sz;

    MPI_Comm_size( c, &sz );
    dd.low  = basework;
    dd.high = basework + rootextrawork;
    dd.n    = root % sz;
    de.val  = base_cnt;
    vbuf    = alloc_mpi_vbuf( base_type, df_same, &de, dd.n, c );

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_peak, &dd, default_sf, c );
        MPI_Scatterv( vbuf->rootbuf, vbuf->rootcnt, vbuf->rootdispl, vbuf->type,
                      vbuf->buf, vbuf->cnt, vbuf->type, dd.n, c );
    }
    free_mpi_vbuf( vbuf );
}

void
early_reduce( double   rootwork,
              double   baseextrawork,
              int      root,
              int      r,
              MPI_Comm c )
{
    mpi_buf_t*     sbuf = 0, * rbuf = 0;
    int            i, sz, me;
    val2_n_distr_t dd;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );
    dd.low  = rootwork + baseextrawork;
    dd.high = rootwork;
    dd.n    = root % sz;
    sbuf    = alloc_mpi_buf( base_type, base_cnt );
    rbuf    = alloc_mpi_buf( base_type, me == dd.n ? base_cnt : 0 );

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_peak, &dd, default_sf, c );
        MPI_Reduce( sbuf->buf, rbuf->buf, sbuf->cnt, sbuf->type, MPI_MIN, dd.n, c );
    }

    free_mpi_buf( sbuf );
    free_mpi_buf( rbuf );
}

void
early_gather( double   rootwork,
              double   baseextrawork,
              int      root,
              int      r,
              MPI_Comm c )
{
    mpi_buf_t*     sbuf = 0, * rbuf = 0;
    int            i, sz, me;
    val2_n_distr_t dd;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );
    dd.low  = rootwork + baseextrawork;
    dd.high = rootwork;
    dd.n    = root % sz;
    sbuf    = alloc_mpi_buf( base_type, base_cnt );
    rbuf    = alloc_mpi_buf( base_type, me == dd.n ? base_cnt * sz : 0 );

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_peak, &dd, default_sf, c );
        MPI_Gather( sbuf->buf, sbuf->cnt, sbuf->type,
                    rbuf->buf, rbuf->cnt / sz, rbuf->type, dd.n, c );
    }

    free_mpi_buf( sbuf );
    free_mpi_buf( rbuf );
}

void
early_gatherv( double   rootwork,
               double   baseextrawork,
               int      root,
               int      r,
               MPI_Comm c )
{
    val1_distr_t   de;
    val2_n_distr_t dd;
    mpi_vbuf_t*    vbuf = 0;
    int            i, sz;

    MPI_Comm_size( c, &sz );
    dd.low  = rootwork + baseextrawork;
    dd.high = rootwork;
    dd.n    = root % sz;
    de.val  = base_cnt;
    vbuf    = alloc_mpi_vbuf( base_type, df_same, &de, dd.n, c );

    for ( i = 0; i < r; ++i )
    {
        par_do_mpi_work( df_peak, &dd, default_sf, c );
        MPI_Gatherv( vbuf->buf, vbuf->cnt, vbuf->type,
                     vbuf->rootbuf, vbuf->rootcnt, vbuf->rootdispl, vbuf->type,
                     dd.n, c );
    }
    free_mpi_vbuf( vbuf );
}

void
right_order( double   basework,
             int      r,
             MPI_Comm c )
{
    val1_distr_t dd;
    int          i, j;
    mpi_buf_t*   buf = alloc_mpi_buf( base_type, base_cnt );

    int          me, sz;
    int          sf = 1;
    MPI_Status   stat;

    dd.val = basework;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );

    if ( sz > 1 )
    {
        for ( i = 0; i < r; ++i )
        {
            if ( me == 0 )
            {
                /* initiate ring */
                MPI_Send( buf->buf, buf->cnt, buf->type, me + 1, P_SR_TAG, c );

                /* now receive from "workers" in right order */
                for ( j = 1; j < sz; j++ )
                {
                    MPI_Recv( buf->buf, buf->cnt, buf->type, j, P_SR_TAG, c, &stat );
                }
            }
            else
            {
                /* wait for previous process */
                MPI_Recv( buf->buf, buf->cnt, buf->type, me - 1, P_SR_TAG, c, &stat );

                /* do my work */
                do_work( df_same( me, sz, sf, &dd ) );

                /* and pass it on */
                if ( me != sz - 1 )
                {
                    MPI_Send( buf->buf, buf->cnt, buf->type, ( me + 1 ) % sz, P_SR_TAG, c );
                }

                /* and send it to master */
                MPI_Send( buf->buf, buf->cnt, buf->type, 0, P_SR_TAG, c );
            }
        }
    }

    free_mpi_buf( buf );
}

void
wrong_order( double   basework,
             int      r,
             MPI_Comm c )
{
    val1_distr_t dd;
    int          i, j;
    mpi_buf_t*   buf = alloc_mpi_buf( base_type, base_cnt );

    int          me, sz;
    int          sf = 1;
    MPI_Status   stat;

    dd.val = basework;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );

    if ( sz > 1 )
    {
        for ( i = 0; i < r; ++i )
        {
            if ( me == 0 )
            {
                /* initiate ring */
                MPI_Send( buf->buf, buf->cnt, buf->type, me + 1, P_SR_TAG, c );

                /* now receive from "workers" in WRONG order */
                for ( j = 1; j < sz; j++ )
                {
                    MPI_Recv( buf->buf, buf->cnt, buf->type, sz - j, P_SR_TAG, c, &stat );
                }
            }
            else
            {
                /* wait for previous process */
                MPI_Recv( buf->buf, buf->cnt, buf->type, me - 1, P_SR_TAG, c, &stat );

                /* do my work */
                do_work( df_same( me, sz, sf, &dd ) );

                /* and pass it on */
                if ( me != sz - 1 )
                {
                    MPI_Send( buf->buf, buf->cnt, buf->type, ( me + 1 ) % sz, P_SR_TAG, c );
                }

                /* and send it to master */
                MPI_Send( buf->buf, buf->cnt, buf->type, 0, P_SR_TAG, c );
            }
        }
    }

    free_mpi_buf( buf );
}
