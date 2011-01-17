#ifndef MPI_PATTERN_H
#define MPI_PATTERN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mpi_util.h"

/*
 * Default Base Communication
 */
extern MPI_Datatype base_type;
extern int          base_cnt;

void
set_base_comm( MPI_Datatype type,
               int          cnt );
void
set_base_comm_a( char* basecomm_desc_str );
void
get_base_comm( MPI_Datatype* type,
               int*          cnt );


/*
 * Point-to-Point Communication Performance Properties
 */
void
late_sender( double   basework,
             double   extrawork,
             int      r,
             MPI_Comm c );
void
late_receiver( double   basework,
               double   extrawork,
               int      r,
               MPI_Comm c );


/*
 * Collective Communication Performance Properties
 */
void
imbalance_at_mpi_barrier( distr_func_t df,
                          distr_t*     dd,
                          int          r,
                          MPI_Comm     c );
void
imbalance_at_mpi_alltoall( distr_func_t df,
                           distr_t*     dd,
                           int          r,
                           MPI_Comm     c );
void
late_broadcast( double   basework,
                double   rootextrawork,
                int      root,
                int      r,
                MPI_Comm c );
void
late_scatter( double   basework,
              double   rootextrawork,
              int      root,
              int      r,
              MPI_Comm c );
void
late_scatterv( double   basework,
               double   rootextrawork,
               int      root,
               int      r,
               MPI_Comm c );
void
early_reduce( double   rootwork,
              double   baseextrawork,
              int      root,
              int      r,
              MPI_Comm c );
void
early_gather( double   rootwork,
              double   baseextrawork,
              int      root,
              int      r,
              MPI_Comm c );
void
early_gatherv( double   rootwork,
               double   baseextrawork,
               int      root,
               int      r,
               MPI_Comm c );
void
right_order( double   basework,
             int      r,
             MPI_Comm c );
void
wrong_order( double   basework,
             int      r,
             MPI_Comm c );

#ifdef __cplusplus
}
#endif
#endif  /*MPI_PATTERN_H*/
