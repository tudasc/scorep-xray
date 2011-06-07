#ifndef MPI_UTIL_H
#define MPI_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif


#include <mpi.h>
#include "distribution.h"
#include "work.h"

/*
 * par_do_mpi_work:  do work according to distribution across communicator c
 */
void
par_do_mpi_work( distr_func_t df,
                 distr_t*     dd,
                 double       sf,
                 MPI_Comm     c );

/*
 * MPI Buffer Management
 *
 * alloc_mpi_buf:    allocate simple communication buffer
 * free_mpi_buf:     free simple communication buffer
 *
 * alloc_mpi_vbuf:   allocate vector communication buffer according to
 *                   distribution across communicator c
 * free_mpi_vbuf:    free vector communication buffer
 */
typedef struct
{
    void*        buf;
    MPI_Datatype type;
    int          cnt;
} mpi_buf_t;

mpi_buf_t*
alloc_mpi_buf( MPI_Datatype type,
               int          cnt );
void
free_mpi_buf( mpi_buf_t* buf );

typedef struct
{
    void*        buf;
    void*        rootbuf;
    MPI_Datatype type;
    int          cnt;
    int*         rootcnt;
    int*         rootdispl;
    int          isroot;
} mpi_vbuf_t;

mpi_vbuf_t*
alloc_mpi_vbuf( MPI_Datatype type,
                distr_func_t df,
                distr_t*     dd,
                int          root,
                MPI_Comm     c );
void
free_mpi_vbuf( mpi_vbuf_t* buf );


/*
 * Simple Point-to-Point Communication Patterns
 *
 * mpi_commpattern_sendrecv:  sender and receiver alternate
 * mpi_commpattern_shift:     all ranks send to neighbor
 */

#define P_SR_TAG  42
#define P_SFT_TAG 43

typedef enum MPI_DIR
{
    DIR_UP = 1, DIR_DOWN = -1
} mpi_dir_t;

void
mpi_commpattern_sendrecv( mpi_buf_t* buf,
                          mpi_dir_t  dir,
                          int        use_isend,
                          int        use_irecv,
                          MPI_Comm   c );

void
mpi_commpattern_shift( mpi_buf_t* sbuf,
                       mpi_buf_t* rbuf,
                       mpi_dir_t  dir,
                       int        use_isend,
                       int        use_irecv,
                       MPI_Comm   c );

#ifdef __cplusplus
}
#endif
#endif  /*MPI_UTIL_H*/
