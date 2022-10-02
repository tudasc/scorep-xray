#ifndef VENDOR_HASH_JENKINS_H
#define VENDOR_HASH_JENKINS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)


#ifndef AFS_PACKAGE_name
# error "AFS_PACKAGE_name not defined."
#endif


#define _HASH_PREFIX_SYM2(a, b) a ## b
#define _HASH_PREFIX_SYM(a, b)  _HASH_PREFIX_SYM2(a, b)
#define _HASH_NAME(sym)         _HASH_PREFIX_SYM(AFS_PACKAGE_name, _ ## sym)


#define jenkins_hashword    _HASH_NAME(jenkins_hashword)
#define jenkins_hashword2   _HASH_NAME(jenkins_hashword2)
#define jenkins_hashlittle  _HASH_NAME(jenkins_hashlittle)
#define jenkins_hashlittle2 _HASH_NAME(jenkins_hashlittle2)
#define jenkins_hashbig     _HASH_NAME(jenkins_hashbig)
#define jenkins_hash        _HASH_NAME(jenkins_hash)


/*
--------------------------------------------------------------------
 This works on all machines.  To be useful, it requires
 -- that the key be an array of uint32_t's, and
 -- that the length be the number of uint32_t's in the key

 The function hashword() is identical to hashlittle() on little-endian
 machines, and identical to hashbig() on big-endian machines,
 except that the length has to be measured in uint32_ts rather than in
 bytes.  hashlittle() is more complicated than hashword() only because
 hashlittle() has to dance around fitting the key bytes into registers.
--------------------------------------------------------------------
*/
uint32_t jenkins_hashword(
const uint32_t *k,          /* the key, an array of uint32_t values */
size_t          length,     /* the length of the key, in uint32_ts */
uint32_t        initval);   /* the previous hash, or an arbitrary value */


/*
--------------------------------------------------------------------
hashword2() -- same as hashword(), but take two seeds and return two
32-bit values.  pc and pb must both be nonnull, and *pc and *pb must
both be initialized with seeds.  If you pass in (*pb)==0, the output
(*pc) will be the same as the return value from hashword().
--------------------------------------------------------------------
*/
void jenkins_hashword2 (
const uint32_t *k,      /* the key, an array of uint32_t values */
size_t          length, /* the length of the key, in uint32_ts */
uint32_t       *pc,     /* IN: seed OUT: primary hash value */
uint32_t       *pb);    /* IN: more seed OUT: secondary hash value */


#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
-------------------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.

This is reversible, so any information in (a,b,c) before mix() is
still in (a,b,c) after mix().

If four pairs of (a,b,c) inputs are run through mix(), or through
mix() in reverse, there are at least 32 bits of the output that
are sometimes the same for one pair and different for another pair.
This was tested for:
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or
  all zero plus a counter that starts at zero.

Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
satisfy this are
    4  6  8 16 19  4
    9 15  3 18 27 15
   14  9  3  7 17  3
Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
for "differ" defined as + with a one-bit base and a two-bit delta.  I
used http://burtleburtle.net/bob/hash/avalanche.html to choose
the operations, constants, and arrangements of the variables.

This does not achieve avalanche.  There are input bits of (a,b,c)
that fail to affect some output bits of (a,b,c), especially of a.  The
most thoroughly mixed value is c, but it doesn't really even achieve
avalanche in c.

This allows some parallelism.  Read-after-writes are good at doubling
the number of bits affected, so the goal of mixing pulls in the opposite
direction as the goal of parallelism.  I did what I could.  Rotates
seem to cost as much as shifts on every machine I could lay my hands
on, and rotates are much kinder to the top and bottom bits, so I used
rotates.
-------------------------------------------------------------------------------
*/
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

/*
-------------------------------------------------------------------------------
final -- final mixing of 3 32-bit values (a,b,c) into c

Pairs of (a,b,c) values differing in only a few bits will usually
produce values of c that look totally different.  This was tested for
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or
  all zero plus a counter that starts at zero.

These constants passed:
 14 11 25 16 4 14 24
 12 14 25 16 4 14 24
and these came close:
  4  8 15 26 3 22 24
 10  8 15 26 3 22 24
 11  8 15 26 3 22 24
-------------------------------------------------------------------------------
*/
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

/*
-------------------------------------------------------------------------------
hashlittle() -- hash a variable-length key into a 32-bit value
  k       : the key (the unaligned variable-length array of bytes)
  length  : the length of the key, counting by bytes
  initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Two keys differing by one or two bits will have
totally different hash values.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (uint8_t **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);

By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
-------------------------------------------------------------------------------
*/
static inline uint32_t jenkins_hashlittle( const void *key, size_t length, uint32_t initval)
{
  uint32_t a,b,c;                                          /* internal state */

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

  const uint8_t *k = (const uint8_t *)key;

  /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
  while (length > 12)
  {
    a += k[0];
    a += ((uint32_t)k[1])<<8;
    a += ((uint32_t)k[2])<<16;
    a += ((uint32_t)k[3])<<24;
    b += k[4];
    b += ((uint32_t)k[5])<<8;
    b += ((uint32_t)k[6])<<16;
    b += ((uint32_t)k[7])<<24;
    c += k[8];
    c += ((uint32_t)k[9])<<8;
    c += ((uint32_t)k[10])<<16;
    c += ((uint32_t)k[11])<<24;
    mix(a,b,c);
    length -= 12;
    k += 12;
  }

  /*-------------------------------- last block: affect all 32 bits of (c) */
  switch(length)                   /* all the case statements fall through */
  {
  case 12: c+=((uint32_t)k[11])<<24;
  case 11: c+=((uint32_t)k[10])<<16;
  case 10: c+=((uint32_t)k[9])<<8;
  case 9 : c+=k[8];
  case 8 : b+=((uint32_t)k[7])<<24;
  case 7 : b+=((uint32_t)k[6])<<16;
  case 6 : b+=((uint32_t)k[5])<<8;
  case 5 : b+=k[4];
  case 4 : a+=((uint32_t)k[3])<<24;
  case 3 : a+=((uint32_t)k[2])<<16;
  case 2 : a+=((uint32_t)k[1])<<8;
  case 1 : a+=k[0];
           break;
  case 0 : return c;
  }

  final(a,b,c);
  return c;
}


/*
 * hashlittle2: return 2 32-bit hash values
 *
 * This is identical to hashlittle(), except it returns two 32-bit hash
 * values instead of just one.  This is good enough for hash table
 * lookup with 2^^64 buckets, or if you want a second hash if you're not
 * happy with the first, or if you want a probably-unique 64-bit ID for
 * the key.  *pc is better mixed than *pb, so use *pc first.  If you want
 * a 64-bit value do something like "*pc + (((uint64_t)*pb)<<32)".
 */
void jenkins_hashlittle2(
  const void *key,      /* the key to hash */
  size_t      length,   /* length of the key */
  uint32_t   *pc,       /* IN: primary initval, OUT: primary hash */
  uint32_t   *pb);      /* IN: secondary initval, OUT: secondary hash */


/*
 * hashbig():
 * This is the same as hashword() on big-endian machines.  It is different
 * from hashlittle() on all machines.  hashbig() takes advantage of
 * big-endian byte ordering.
 */
static inline uint32_t jenkins_hashbig( const void *key, size_t length, uint32_t initval)
{
  uint32_t a,b,c;

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

  const uint8_t *k = (const uint8_t *)key;

  /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
  while (length > 12)
  {
    a += ((uint32_t)k[0])<<24;
    a += ((uint32_t)k[1])<<16;
    a += ((uint32_t)k[2])<<8;
    a += ((uint32_t)k[3]);
    b += ((uint32_t)k[4])<<24;
    b += ((uint32_t)k[5])<<16;
    b += ((uint32_t)k[6])<<8;
    b += ((uint32_t)k[7]);
    c += ((uint32_t)k[8])<<24;
    c += ((uint32_t)k[9])<<16;
    c += ((uint32_t)k[10])<<8;
    c += ((uint32_t)k[11]);
    mix(a,b,c);
    length -= 12;
    k += 12;
  }

  /*-------------------------------- last block: affect all 32 bits of (c) */
  switch(length)                   /* all the case statements fall through */
  {
  case 12: c+=k[11];
  case 11: c+=((uint32_t)k[10])<<8;
  case 10: c+=((uint32_t)k[9])<<16;
  case 9 : c+=((uint32_t)k[8])<<24;
  case 8 : b+=k[7];
  case 7 : b+=((uint32_t)k[6])<<8;
  case 6 : b+=((uint32_t)k[5])<<16;
  case 5 : b+=((uint32_t)k[4])<<24;
  case 4 : a+=k[3];
  case 3 : a+=((uint32_t)k[2])<<8;
  case 2 : a+=((uint32_t)k[1])<<16;
  case 1 : a+=((uint32_t)k[0])<<24;
           break;
  case 0 : return c;
  }

  final(a,b,c);
  return c;
}


/*
 * hash():
 * compiletime multiplexer for littel- and big-endian.
 * Note that the hash value is NOT endias agnostic.
 */
static inline uint32_t jenkins_hash( const void *key, size_t length, uint32_t initval)
{
#ifdef WORDS_BIGENDIAN
    return jenkins_hashbig(key, length, initval);
#else
    return jenkins_hashlittle(key, length, initval);
#endif
}


#ifdef __cplusplus
}
#endif

#endif /* VENDOR_HASH_JENKINS_H */
