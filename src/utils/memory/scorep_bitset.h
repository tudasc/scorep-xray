/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_BITSET_H
#define SCOREP_INTERNAL_BITSET_H



/**
 * @file       src/memory/scorep_bitset.h
 *
 *
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>


#define BITSET_WORDSIZE    64
#define __BITSET_TYPE( x ) uint##x##_t
#define _BITSET_TYPE( x )  __BITSET_TYPE( x )
#define bitset_word_t      _BITSET_TYPE( BITSET_WORDSIZE )

#define DO_DUMP 0

#if DO_DUMP
static void
bitset_dump( const char* name,
             void*       bitset,
             uint32_t    number_of_members )
{
    char           buf[ number_of_members + 1 ];
    bitset_word_t* words = bitset;
    bitset_word_t  bit   = 1;

    for ( uint32_t i = 0; i < number_of_members; i++, bit <<= 1 )
    {
        if ( i && i % BITSET_WORDSIZE == 0 )
        {
            words++;
            bit = 1;
        }
        buf[ i ] = ( *words & ( bit ) ) ? '1' : '0';
    }
    buf[ number_of_members ] = 0;
    printf( "%s: %s\n", name, buf );
}
#define dump_bitset( n, b, s ) bitset_dump( n, b, s )
#else
#define dump_bitset( n, b, s ) do { } while ( 0 )
#endif

/**
 * Returns the number of bytes needed to hold @a number_of_members members
 *
 * @param number_of_members
 *
 * @return The size needed
 */
static inline size_t
bitset_size( uint32_t number_of_members )
{
    return ( number_of_members / BITSET_WORDSIZE
             + !!( number_of_members % BITSET_WORDSIZE ) )
           * ( BITSET_WORDSIZE / 8 );
}


/**
 * Sets all bits to 1 which are greater or equal number_of_members.
 *
 * @param bitset A memory location
 * @param number_of_members
 */
static inline void
bitset_mark_invalid( void* bitset, uint32_t number_of_members )
{
    uint32_t last_bit = number_of_members % BITSET_WORDSIZE;

    /* nothing to do, when there are no invalid bits */
    if ( last_bit == 0 )
    {
        return;
    }

    bitset_word_t* words     = bitset;
    bitset_word_t* last_word = &words[ number_of_members / BITSET_WORDSIZE ];
    bitset_word_t  mask      = 1;
    mask     <<= last_bit;
    *last_word = -mask;
}

/**
 *  Sets a bit.
 */
static inline void
bitset_set( void* bitset, uint32_t number_of_members, uint32_t pos )
{
    assert( bitset );
    assert( pos < number_of_members );

    bitset_word_t* words = bitset;
    bitset_word_t* word  = &words[ pos / BITSET_WORDSIZE ];
    bitset_word_t  bit   = 1;
    bit  <<= pos % BITSET_WORDSIZE;
    *word |= bit;
}


/**
 *  Clears a bit.
 */
static inline void
bitset_clear( void* bitset, uint32_t number_of_members, uint32_t pos )
{
    assert( bitset );
    assert( pos < number_of_members );

    bitset_word_t* words = bitset;
    bitset_word_t* word  = &words[ pos / BITSET_WORDSIZE ];
    bitset_word_t  bit   = 1;
    bit  <<= pos % BITSET_WORDSIZE;
    *word &= ~bit;
}


/**
 *  Sets a range of bits.
 */
static void
bitset_set_range( void*    bitset,
                  uint32_t number_of_members,
                  uint32_t offset,
                  uint32_t length )
{
    assert( bitset );
    assert( offset < number_of_members );
    assert( length > 0 );
    assert( length <= number_of_members );
    assert( offset + length <= number_of_members );

    bitset_word_t* words         = bitset;
    bitset_word_t  one           = 1;
    bitset_word_t  zero          = 0;
    uint32_t       first_word    =  offset / BITSET_WORDSIZE;
    uint32_t       first_bit_pos =  offset % BITSET_WORDSIZE;
    uint32_t       last_word     =  ( offset + length ) / BITSET_WORDSIZE;
    uint32_t       last_bit_pos  =  ( offset + length ) % BITSET_WORDSIZE;
    uint32_t       i             = first_word;

    dump_bitset( "  pre", bitset, number_of_members );

    /* set bits in the first partial word */
    if ( first_bit_pos )
    {
        bitset_word_t mask = -( one << first_bit_pos );
        if ( first_word == last_word && last_bit_pos )
        {
            mask &= ( one << last_bit_pos ) - 1;
        }
        assert( ( words[ i ] & mask ) == 0 );
        words[ i ] |= mask;
        i++;
        dump_bitset( " mask", &mask, BITSET_WORDSIZE );
        dump_bitset( "first", bitset, number_of_members );
    }

    /* fill all full words */
    for (; i < last_word; i++ )
    {
        bitset_word_t mask = ~zero;
        assert( ( words[ i ] & mask ) == 0 );
        words[ i ] = mask;
        dump_bitset( " full", bitset, number_of_members );
    }

    /* set bits in the last partial word */
    if ( last_bit_pos && !( first_word == last_word && first_bit_pos ) )
    {
        bitset_word_t mask = ( one << last_bit_pos ) - 1;
        assert( ( words[ last_word ] & mask ) == 0 );
        words[ last_word ] |= mask;
        dump_bitset( " mask", &mask, BITSET_WORDSIZE );
        dump_bitset( " last", bitset, number_of_members );
    }
}


/**
 *  Clears a range of bits.
 */
static void
bitset_clear_range( void*    bitset,
                    uint32_t number_of_members,
                    uint32_t offset,
                    uint32_t length )
{
    assert( bitset );
    assert( offset < number_of_members );
    assert( length > 0 );
    assert( length <= number_of_members );
    assert( offset + length < number_of_members );

    bitset_word_t* words         = bitset;
    bitset_word_t  one           = 1;
    bitset_word_t  zero          = 0;
    uint32_t       first_word    =  offset / BITSET_WORDSIZE;
    uint32_t       first_bit_pos =  offset % BITSET_WORDSIZE;
    uint32_t       last_word     =  ( offset + length ) / BITSET_WORDSIZE;
    uint32_t       last_bit_pos  =  ( offset + length ) % BITSET_WORDSIZE;
    uint32_t       i             = first_word;

    dump_bitset( "  pre", bitset, number_of_members );

    /* set bits in the first partial word */
    if ( first_bit_pos )
    {
        bitset_word_t mask = -( one << first_bit_pos );
        if ( first_word == last_word && last_bit_pos )
        {
            mask &= ( one << last_bit_pos ) - 1;
        }
        assert( ( words[ i ] & mask ) == mask );
        words[ i ] &= ~mask;
        i++;
        dump_bitset( "first", bitset, number_of_members );
    }

    /* fill all full words */
    for (; i < last_word; i++ )
    {
        assert( ( words[ i ] & zero ) == zero );
        words[ i ] = zero;
        dump_bitset( " full", bitset, number_of_members );
    }

    /* set bits in the last partial word */
    if ( last_bit_pos && !( first_word == last_word && first_bit_pos ) )
    {
        bitset_word_t mask = ( one << last_bit_pos ) - 1;
        assert( ( words[ last_word ] & mask ) == mask );
        words[ last_word ] &= ~mask;
        dump_bitset( " last", bitset, number_of_members );
    }
}


static inline uint32_t
_find_first_zero( bitset_word_t word )
{
    uint32_t pos = 0;
    word = ( ~word ^ ( ~word - 1 ) ) >> 1;
    while ( word )
    {
        word >>= 1;
        pos++;
    }

    return pos;
}


static inline uint32_t
_find_first_set( bitset_word_t word )
{
    return _find_first_zero( ~word );
}


/**
 * Finds a free bit in the bitset which is greate or equal to @a offset
 * and set it.
 *
 * Returns @a number_of_members if the search failed.
 *
 */
static uint32_t
bitset_next_free( void*    bitset,
                  uint32_t number_of_members,
                  uint32_t offset )
{
    assert( bitset );

    uint32_t       i             = offset / BITSET_WORDSIZE;
    uint32_t       first_bit_pos = offset % BITSET_WORDSIZE;
    uint32_t       last_bit      = number_of_members % BITSET_WORDSIZE;
    uint32_t       n_words       = ( ( number_of_members / BITSET_WORDSIZE ) + !!last_bit );
    bitset_word_t* words         = bitset;
    uint32_t       pos;
    bitset_word_t  one = 1;

    if ( offset >= number_of_members )
    {
        return number_of_members;
    }

    if ( first_bit_pos )
    {
        /* We need to ignore the bits prior to first_bit_pos in the ith word */
        bitset_word_t word = words[ i ] | ( ( one << first_bit_pos ) - 1 );
        if ( ~word )
        {
            pos = _find_first_zero( word );
            goto out;
        }

        /* no free bits in this word, start searching in next word */
        i++;
    }

    /* find a word which does have unset bits */
    for (; i < n_words; i++ )
    {
        if ( ~words[ i ] )
        {
            break;
        }
    }
    if ( i == n_words )
    {
        /* all words full */
        return number_of_members;
    }

    pos = _find_first_zero( words[ i ] );

out:
    /* convert to global position (from position in the ith word) */
    return i * BITSET_WORDSIZE + pos;
}


/**
 * Finds a free bit in the bitset and set it.
 * Returns @a number_of_members if the search failed.
 *
 */
static inline uint32_t
bitset_find_and_set( void* bitset, uint32_t number_of_members )
{
    uint32_t bit = bitset_next_free( bitset, number_of_members, 0 );

    if ( bit < number_of_members )
    {
        bitset_set( bitset, number_of_members, bit );
    }

    return bit;
}


/**
 * Finds the next used bit in the bitset which is greate or equal to @a offset.
 *
 * Returns @a number_of_members if the search failed.
 */
static uint32_t
bitset_next_used( void*    bitset,
                  uint32_t number_of_members,
                  uint32_t offset )
{
    assert( bitset );

    uint32_t       i             = offset / BITSET_WORDSIZE;
    uint32_t       first_bit_pos = offset % BITSET_WORDSIZE;
    uint32_t       last_bit      = number_of_members % BITSET_WORDSIZE;
    uint32_t       n_words       = ( ( number_of_members / BITSET_WORDSIZE ) + !!last_bit );
    bitset_word_t* words         = bitset;
    uint32_t       pos;
    bitset_word_t  one = 1;

    if ( first_bit_pos )
    {
        /* We need to ignore the bits prior to first_bit_pos in the ith word */
        bitset_word_t word = words[ i ] & ~( ( one << first_bit_pos ) - 1 );
        if ( word )
        {
            /*
             * There are bits set after first_bit_pos,
             * we will stop the calculation in this word
             */
            pos = _find_first_set( word );
            goto out;
        }

        /* no used bits in this word, start searching in next word */
        i++;
    }

    /* find a word which does have set bits */
    for (; i < n_words; i++ )
    {
        if ( words[ i ] )
        {
            break;
        }
    }
    if ( i == n_words )
    {
        /* all words empty */
        return number_of_members;
    }

    pos = _find_first_set( words[ i ] );

out:
    /* convert to global position (from position in the ith word) */
    return i * BITSET_WORDSIZE + pos;
}

/**
 * Finds @a range_length consecutive free bits in the bitset and set them.
 * Returns @a number_of_members if the search failed otherwise the position
 * of the first bit in the range.
 *
 */
static uint32_t
bitset_find_and_set_range( void*    bitset,
                           uint32_t number_of_members,
                           uint32_t range_length )
{
    assert( bitset );

    uint32_t pos, length;
    for ( pos = bitset_next_free( bitset, number_of_members, 0 );
          pos < number_of_members;
          pos = bitset_next_free( bitset, number_of_members, pos + length ) )
    {
        uint32_t next_used
               = bitset_next_used( bitset, number_of_members, pos );
        length = next_used - pos;
        if ( length >= range_length )
        {
            bitset_set_range( bitset,
                              number_of_members,
                              pos,
                              range_length );
            return pos;
        }
    }

    return number_of_members;
}

#undef __BITSET_TYPE
#undef _BITSET_TYPE
#undef bitset_word_t


#endif /* SCOREP_INTERNAL_BITMAP_H */
