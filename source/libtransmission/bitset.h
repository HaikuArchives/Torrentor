/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: bitset.h 11709 2011-01-19 13:48:47Z jordan $
 */

#ifndef __TRANSMISSION__
 #error only libtransmission should #include this header.
#endif

#ifndef TR_BITSET_H
#define TR_BITSET_H 1

#include "transmission.h"
#include "bitfield.h"

/** @brief like a tr_bitfield, but supports haveAll and haveNone */
typedef struct tr_bitset
{
    tr_bool haveAll;
    tr_bool haveNone;
    tr_bitfield bitfield;
}
tr_bitset;

static inline void
tr_bitsetConstructor( tr_bitset * b, size_t size )
{
    tr_bitfieldConstruct( &b->bitfield, size );
}

static inline void
tr_bitsetDestructor( tr_bitset * b )
{
    tr_bitfieldDestruct( &b->bitfield );
}

void tr_bitsetReserve( tr_bitset * b, size_t size );

static inline tr_bool
tr_bitsetHasFast( const tr_bitset * b, const size_t nth )
{
    if( b->haveAll ) return TRUE;
    if( b->haveNone ) return FALSE;
    if( nth >= b->bitfield.bitCount ) return FALSE;
    return tr_bitfieldHasFast( &b->bitfield, nth );
}

static inline tr_bool
tr_bitsetHas( const tr_bitset * b, const size_t nth )
{
    if( b->haveAll ) return TRUE;
    if( b->haveNone ) return FALSE;
    if( nth >= b->bitfield.bitCount ) return FALSE;
    return tr_bitfieldHas( &b->bitfield, nth );
}

static inline void
tr_bitsetOr( tr_bitfield * a, const tr_bitset * b )
{
    if( b->haveAll )
        tr_bitfieldAddRange( a, 0, a->bitCount );
    else if( !b->haveNone )
        tr_bitfieldOr( a, &b->bitfield );
}

/* set 'a' to all the flags that were in 'a' but not 'b' */
static inline void
tr_bitsetDifference( tr_bitfield * a, const tr_bitset * b )
{
    if( b->haveAll )
        tr_bitfieldClear( a );
    else if( !b->haveNone )
        tr_bitfieldDifference( a, &b->bitfield );
}

static inline double
tr_bitsetPercent( const tr_bitset * b )
{
    if( b->haveAll ) return 1.0;
    if( b->haveNone ) return 0.0;
    if( b->bitfield.bitCount == 0 ) return 0.0;
    return tr_bitfieldCountTrueBits( &b->bitfield ) / (double)b->bitfield.bitCount;
}

static inline void
tr_bitsetSetHaveAll( tr_bitset * b )
{
    b->haveAll = 1;
    b->haveNone = 0;
}

static inline void
tr_bitsetSetHaveNone( tr_bitset * b )
{
    b->haveAll = 0;
    b->haveNone = 1;
}

static inline int
tr_bitsetAdd( tr_bitset * b, size_t i )
{
    int ret = 0;
    if( !b->haveAll ) {
        b->haveNone = 0;
        tr_bitsetReserve( b, i+1 );
        ret = tr_bitfieldAdd( &b->bitfield, i );
    }
    return ret;
}

#endif
