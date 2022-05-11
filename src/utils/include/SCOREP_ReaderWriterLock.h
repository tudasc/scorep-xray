/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_READERWRITERLOCK_H
#define SCOREP_READERWRITERLOCK_H

/**
 * @file
 *
 * @brief Reader-writer lock implementation based on atomic operations
 *        and spin-loops.
 */

#include <stdint.h>

#include <UTILS_Atomic.h>
#include <UTILS_Error.h>
#include <UTILS_Mutex.h>


/*
 * A writer-preferring RW lock implementation based on
 * https://eli.thegreenplace.net/2019/implementing-reader-writer-locks/,
 * see section 'A more efficient writer-preferring RW lock'.
 *
 * In the absence of a writer, reading is fast as the overhead of
 * SCOREP_RWLock_ReaderLock() and SCOREP_RWLock_ReaderUnlock() is just
 * incrementing and decrementing an integer atomically.
 *
 * In case of a writer and one or more readers, spinning CAS loops are
 * used to signal a waiting writer once the last reader in flight has
 * finished, and to signal waiting readers once the writer is
 * done. CAS spin loops might not scale well with the number of
 * readers.
 *
 * Users of this RW lock need to define following shared variables:
 *
 * int16_t     pending; // initially 0
 * int16_t     departing; // initially 0
 * int16_t     release_n_readers; // initially 0
 * int16_t     release_writer; // initially 0
 * UTILS_Mutex writer_mutex; // initially unlocked
 *
 * We don't provide a struct to increase flexibility on the user side
 * on how to layout the RW lock data structure, e.g.,
 * w.r.t. alignment.
 *
 * These variables are then passed in by pointer to the RW lock
 * functions. The intended use is as follows:
 *
 * Reader:
 * SCOREP_RWLock_ReaderLock( &pending, &release_n_readers );
 * // reader critical section
 * SCOREP_RWLock_ReaderUnlock( &pending, &departing, &release_writer );
 *
 * Writer:
 * SCOREP_RWLock_WriterLock( &writer_mutex, &pending, &departing, &release_writer );
 * // writer critical section
 * SCOREP_RWLock_WriterUnlock( &writer_mutex, &pending, &release_n_readers );
 */


/* Some value large enough to let a writer make 'pending' negative via
   'pending -= RWLOCK_MAX_READERS', thus indicating to readers that a
   write operation is in progress. */
#define RWLOCK_MAX_READERS ( INT16_C( 1 ) << 14 )


static inline void
SCOREP_RWLock_ReaderLock( int16_t* rwlockPending,
                          int16_t* rwlockReleaseNReaders )
{
    /* increment number of active readers */
    int16_t pending = UTILS_Atomic_AddFetch_int16( rwlockPending, 1,
                                                   UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    if ( pending < 0 )
    {
        /* writer operation in progress, wait for writer signalling to
           release readers */
        int16_t release_n_readers;
        do
        {
            UTILS_CPU_RELAX;
            release_n_readers = UTILS_Atomic_LoadN_int16( rwlockReleaseNReaders,
                                                          UTILS_ATOMIC_RELAXED );
        }
        while ( release_n_readers == 0 ||
                !UTILS_Atomic_CompareExchangeN_int16( rwlockReleaseNReaders,
                                                      &release_n_readers,
                                                      /* release one at a time: */
                                                      release_n_readers - 1,
                                                      true /* weak */,
                                                      UTILS_ATOMIC_ACQUIRE_RELEASE,
                                                      UTILS_ATOMIC_RELAXED ) );
    }
}


static inline void
SCOREP_RWLock_ReaderUnlock( int16_t* rwlockPending,
                            int16_t* rwlockDeparting,
                            int16_t* rwlockReleaseWriter )
{
    /* decrement number of active readers */
    int16_t pending = UTILS_Atomic_AddFetch_int16( rwlockPending, -1,
                                                   UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    if ( pending < 0 )
    {
        /* write operation in progress, writer waiting in SCOREP_RWLock_WriterLock() */
        int16_t departing = UTILS_Atomic_AddFetch_int16( rwlockDeparting, -1,
                                                         UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        if ( departing == 0 )
        {
            /* last departing reader releases waiting writer */
            int16_t swapped = UTILS_Atomic_ExchangeN_int16( rwlockReleaseWriter, 1,
                                                            UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
            UTILS_BUG_ON( swapped != 0 );
        }
    }
}


static inline void
SCOREP_RWLock_WriterLock( UTILS_Mutex* rwlockWriterMutex,
                          int16_t*     rwlockPending,
                          int16_t*     rwlockDeparting,
                          int16_t*     rwlockReleaseWriter )
{
    UTILS_MutexLock( rwlockWriterMutex );
    /* get number of active readers but make rwlockPending < 0, thus
       readers know that a writer is active */
    int16_t pending = UTILS_Atomic_AddFetch_int16( rwlockPending, -RWLOCK_MAX_READERS,
                                                   UTILS_ATOMIC_SEQUENTIAL_CONSISTENT )
                      + RWLOCK_MAX_READERS;
    if ( pending != 0 )
    {
        /* reading operation(s) in progress (not the ones waiting in
           SCOREP_RWLock_ReaderLock()) */
        int16_t departing = UTILS_Atomic_AddFetch_int16( rwlockDeparting, pending,
                                                         UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        if ( departing != 0 )
        {
            /* wait for last departing reader signaling to release writer */
            int16_t release_writer;
            do
            {
                UTILS_CPU_RELAX;
                release_writer = UTILS_Atomic_LoadN_int16( rwlockReleaseWriter,
                                                           UTILS_ATOMIC_RELAXED );
            }
            while ( release_writer == 0 ||
                    !UTILS_Atomic_CompareExchangeN_int16( rwlockReleaseWriter,
                                                          &release_writer,
                                                          release_writer - 1,
                                                          true /* weak */,
                                                          UTILS_ATOMIC_ACQUIRE_RELEASE,
                                                          UTILS_ATOMIC_RELAXED ) );
        }
    }
}


static inline void
SCOREP_RWLock_WriterUnlock( UTILS_Mutex* rwlockWriterMutex,
                            int16_t*     rwlockPending,
                            int16_t*     rwlockReleaseNReaders )
{
    /* 'restore' rwlockPending by making it > 0 again */
    uint16_t pending = UTILS_Atomic_AddFetch_int16( rwlockPending, RWLOCK_MAX_READERS,
                                                    UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    if ( pending > 0 )
    {
        /* release readers waiting in SCOREP_RWLock_ReaderLock() */
        int16_t swapped = UTILS_Atomic_ExchangeN_int16( rwlockReleaseNReaders, pending,
                                                        UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        UTILS_BUG_ON( swapped != 0 );
    }
    UTILS_MutexUnlock( rwlockWriterMutex );
}


#undef RWLOCK_MAX_READERS

#endif /* SCOREP_READERWRITERLOCK_H */
