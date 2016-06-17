#define shmem_team_split_L shmem_team_split
#define shmem_team_split_H SHMEM_TEAM_SPLIT

#define shmem_team_create_strided_L shmem_team_create_strided
#define shmem_team_create_strided_H SHMEM_TEAM_CREATE_STRIDED

#define shmem_team_free_L shmem_team_free
#define shmem_team_free_H SHMEM_TEAM_FREE

#define shmem_team_npes_L shmem_team_npes
#define shmem_team_npes_H SHMEM_TEAM_NPES

#define shmem_team_mype_L shmem_team_mype
#define shmem_team_mype_H SHMEM_TEAM_MYPE

#define shmem_team_translate_pe_L shmem_team_translate_pe
#define shmem_team_translate_pe_H SHMEM_TEAM_TRANSLATE_PE

#define shmem_team_barrier_L shmem_team_barrier
#define shmem_team_barrier_H SHMEM_TEAM_BARRIER

#define shmem_team_alltoall_L shmem_team_alltoall
#define shmem_team_alltoall_H SHMEM_TEAM_ALLTOALL

#define shmem_team_alltoallv_L shmem_team_alltoallv
#define shmem_team_alltoallv_H SHMEM_TEAM_ALLTOALLV

#define shmem_team_alltoallv_packed_L shmem_team_alltoallv_packed
#define shmem_team_alltoallv_packed_H SHMEM_TEAM_ALLTOALLV_PACKED


/* *INDENT-OFF* */

#define TEAM_SPLIT( FUNCNAME )                      \
    void                                            \
    FSUB( FUNCNAME ) ( shmem_team_t * team,         \
                       int          * color,        \
                       int          * key,          \
                       shmem_team_t * newTeam )     \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        FUNCNAME( *team,                            \
                  *color,                           \
                  *key,                             \
                  newTeam );                        \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_SPLIT )
TEAM_SPLIT( shmem_team_split )
#endif


/* *INDENT-OFF* */

#define TEAM_CREATE_STRIDED( FUNCNAME )             \
    void                                            \
    FSUB( FUNCNAME ) ( int          * peStart,      \
                       int          * peStride,     \
                       int          * peSize,       \
                       shmem_team_t * newTeam )     \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        FUNCNAME( *peStart,                         \
                  *peStride,                        \
                  *peSize,                          \
                  newTeam );                        \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_CREATE_STRIDED )
TEAM_CREATE_STRIDED( shmem_team_create_strided )
#endif


/* *INDENT-OFF* */

#define TEAM_FREE( FUNCNAME )                       \
    void                                            \
    FSUB( FUNCNAME ) ( shmem_team_t * team )        \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        FUNCNAME( team );                           \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_FREE )
TEAM_FREE( shmem_team_free )
#endif


/* *INDENT-OFF* */

#define TEAM_QUERY( FUNCNAME )                      \
    int                                             \
    FSUB( FUNCNAME ) ( shmem_team_t * team )        \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        int ret = FUNCNAME( *team );                \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
                                                    \
        return ret;                                 \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_NPES )
TEAM_QUERY( shmem_team_npes )
#endif

#if SHMEM_HAVE_DECL( SHMEM_TEAM_MYPE )
TEAM_QUERY( shmem_team_mype )
#endif


/* *INDENT-OFF* */

#define TEAM_TRANSLATE_PE( FUNCNAME )               \
    int                                             \
    FSUB( FUNCNAME ) ( shmem_team_t * team1,        \
                       int          * team1Pe,      \
                       shmem_team_t * team2 )       \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        int ret = FUNCNAME( *team1,                 \
                            *team1Pe,               \
                            *team2 );               \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
                                                    \
        return ret;                                 \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_TRANSLATE_PE )
TEAM_TRANSLATE_PE( shmem_team_translate_pe )
#endif


/* *INDENT-OFF* */

#define TEAM_BARRIER( FUNCNAME )                    \
    void                                            \
    FSUB( FUNCNAME ) ( shmem_team_t * team,         \
                       long         * pSync )       \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        FUNCNAME( *team,                            \
                  pSync );                          \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_BARRIER )
TEAM_BARRIER( shmem_team_barrier )
#endif


/* *INDENT-OFF* */

#define TEAM_ALLTOALL( FUNCNAME )               \
    void                                        \
    FSUB( FUNCNAME ) ( void         * target,   \
                       const void   * src,      \
                       size_t       * len,      \
                       shmem_team_t * team,     \
                       long         * pSync )   \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
                                                \
        FUNCNAME( target,                       \
                  src,                          \
                  *len,                         \
                  *team,                        \
                  pSync );                      \
                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALL )
TEAM_ALLTOALL( shmem_team_alltoall )
#endif


/* *INDENT-OFF* */

#define TEAM_ALLTOALLV( FUNCNAME )              \
    void                                        \
    FSUB( FUNCNAME ) ( void         * target,   \
                       size_t       * tOffsets, \
                       size_t       * tSizes,   \
                       const void   * src,      \
                       size_t       * sOffsets, \
                       size_t       * sSizes,   \
                       shmem_team_t * team,     \
                       long         * pSync )   \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
                                                \
        FUNCNAME( target,                       \
                  tOffsets,                     \
                  tSizes,                       \
                  src,                          \
                  sOffsets,                     \
                  sSizes,                       \
                  *team,                        \
                  pSync );                      \
                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALLV )
TEAM_ALLTOALLV( shmem_team_alltoallv )
#endif


/* *INDENT-OFF* */

#define TEAM_ALLTOALLV_PACKED( FUNCNAME )       \
    void                                        \
    FSUB( FUNCNAME ) ( void         * target,   \
                       size_t       * tLength,  \
                       size_t       * tSizes,   \
                       const void   * src,      \
                       size_t       * sOffsets, \
                       size_t       * sSizes,   \
                       shmem_team_t * team,     \
                       long         * pSync )   \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
                                                \
        FUNCNAME( target,                       \
                  *tLength,                     \
                  tSizes,                       \
                  src,                          \
                  sOffsets,                     \
                  sSizes,                       \
                  *team,                        \
                  pSync );                      \
                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALLV_PACKED )
TEAM_ALLTOALLV_PACKED( shmem_team_alltoallv_packed )
#endif
