#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi_util.h"

void
par_do_mpi_work( distr_func_t df,
                 distr_t*     dd,
                 double       sf,
                 MPI_Comm     c )
{
    int me, sz;
    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );
    do_work( df( me, sz, sf, dd ) );
}

mpi_buf_t*
alloc_mpi_buf( MPI_Datatype type,
               int          cnt )
{
    mpi_buf_t* res = malloc( sizeof( mpi_buf_t ) );
    MPI_Aint   text;
    if ( !res )
    {
        fprintf( stderr, "+++ ERROR1: allocation of MPI buffer failed!!!\n" );
        MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    MPI_Type_extent( type, &text );
    if ( cnt > 0 )
    {
        res->buf = malloc( cnt * text );
        if ( !res->buf )
        {
            free( res );
            fprintf( stderr, "+++ ERROR2: allocation of MPI buffer failed!!!\n" );
            MPI_Abort( MPI_COMM_WORLD, 2 );
        }
    }
    else
    {
        res->buf = 0;
    }
    res->type = type;
    res->cnt  = cnt;
    return res;
}

void
free_mpi_buf( mpi_buf_t* buf )
{
    if ( buf )
    {
        if ( buf->buf )
        {
            free( buf->buf );
        }
        free( buf );
    }
}

static void*
ALLOC_BUF( mpi_vbuf_t* res,
           int         bytes,
           int         errno )
{
    void* b = 0;

    if ( bytes <= 0 )
    {
        return 0;
    }

    b = malloc( bytes );
    if ( !b )
    {
        free_mpi_vbuf( res );
        fprintf( stderr, "+++ ERROR%d: allocation of MPI buffer failed!!!\n", errno );
        MPI_Abort( MPI_COMM_WORLD, errno );
    }
    return b;
}

mpi_vbuf_t*
alloc_mpi_vbuf( MPI_Datatype type,
                distr_func_t df,
                distr_t*     dd,
                int          root,
                MPI_Comm     c )
{
    int         i, me, sz, tcnt;
    MPI_Aint    text;
    mpi_vbuf_t* res = malloc( sizeof( mpi_vbuf_t ) );
    if ( !res )
    {
        fprintf( stderr, "+++ ERROR3: allocation of MPI buffer failed!!!\n" );
        MPI_Abort( MPI_COMM_WORLD, 3 );
    }
    res->buf = res->rootbuf = res->rootcnt = res->rootdispl = 0;

    MPI_Type_extent( type, &text );
    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );

    if ( me == root )
    {
        res->isroot    = 1;
        res->rootcnt   = ALLOC_BUF( res, sz * sizeof( int ), 4 );
        res->rootdispl = ALLOC_BUF( res, sz * sizeof( int ), 5 );

        tcnt = 0;
        for ( i = 0; i < sz; ++i )
        {
            res->rootdispl[ i ] = tcnt;
            tcnt               += res->rootcnt[ i ] = df( i, sz, 1.0, dd );
        }
        res->rootbuf = ALLOC_BUF( res, tcnt * text, 6 );
    }
    else
    {
        res->isroot = 0;
    }
    res->cnt  = df( me, sz, 1.0, dd );
    res->type = type;
    res->buf  = ALLOC_BUF( res, res->cnt * text, 7 );
    return res;
}

void
free_mpi_vbuf( mpi_vbuf_t* buf )
{
    if ( buf )
    {
        if ( buf->isroot )
        {
            if ( buf->rootbuf )
            {
                free( buf->rootbuf );
            }
            if ( buf->rootcnt )
            {
                free( buf->rootcnt );
            }
            if ( buf->rootdispl )
            {
                free( buf->rootdispl );
            }
        }
        if ( buf->buf )
        {
            free( buf->buf );
        }
        free( buf );
    }
}

void
mpi_commpattern_sendrecv( mpi_buf_t* buf,
                          mpi_dir_t  dir,
                          int        use_isend,
                          int        use_irecv,
                          MPI_Comm   c )
{
    int         me, sz;
    MPI_Request req;
    MPI_Status  stat;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );

    /* if odd number of processors, get rid of last rank */
    if ( sz % 2 == 1 )
    {
        if ( me == sz - 1 )
        {
            return;
        }
        else
        {
            --sz;
        }
    }

    if ( me % 2 == 0 )
    {
        /* sender */
        if ( use_isend )
        {
            MPI_Isend( buf->buf, buf->cnt, buf->type, ( me + dir + sz ) % sz, P_SR_TAG,
                       c, &req );
            MPI_Wait( &req, &stat );
        }
        else
        {
            MPI_Ssend( buf->buf, buf->cnt, buf->type, ( me + dir + sz ) % sz, P_SR_TAG, c );
        }
    }
    else
    {
        /* receiver */
        if ( use_irecv )
        {
            MPI_Irecv( buf->buf, buf->cnt, buf->type, ( me - dir + sz ) % sz, P_SR_TAG,
                       c, &req );
            MPI_Wait( &req, &stat );
        }
        else
        {
            MPI_Recv( buf->buf, buf->cnt, buf->type, ( me - dir + sz ) % sz, P_SR_TAG,
                      c, &stat );
        }
    }
}

void
mpi_commpattern_shift( mpi_buf_t* sbuf,
                       mpi_buf_t* rbuf,
                       mpi_dir_t  dir,
                       int        use_isend,
                       int        use_irecv,
                       MPI_Comm   c )
{
    int         me, sz;
    MPI_Request sreq, rreq;
    MPI_Status  stat;

    MPI_Comm_rank( c, &me );
    MPI_Comm_size( c, &sz );

    if ( use_isend )
    {
        /* use separate send and recv */
        MPI_Isend( sbuf->buf, sbuf->cnt, sbuf->type, ( me + dir + sz ) % sz, P_SFT_TAG,
                   c, &sreq );
        if ( use_irecv )
        {
            MPI_Irecv( rbuf->buf, rbuf->cnt, rbuf->type, ( me - dir + sz ) % sz, P_SFT_TAG,
                       c, &rreq );
            MPI_Wait( &rreq, &stat );
        }
        else
        {
            MPI_Recv( rbuf->buf, rbuf->cnt, rbuf->type, ( me - dir + sz ) % sz, P_SFT_TAG,
                      c, &stat );
        }
        MPI_Wait( &sreq, &stat );
    }
    else
    {
        /* use sendrecv */
        MPI_Sendrecv( sbuf->buf, sbuf->cnt, sbuf->type, ( me + dir + sz ) % sz, P_SFT_TAG,
                      rbuf->buf, rbuf->cnt, rbuf->type, ( me - dir + sz ) % sz, P_SFT_TAG,
                      c, &stat );
    }
}
