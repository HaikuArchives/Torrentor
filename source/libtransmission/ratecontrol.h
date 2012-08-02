/******************************************************************************
 * $Id: ratecontrol.h 11709 2011-01-19 13:48:47Z jordan $
 *
 * Copyright (c) Transmission authors and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#ifndef __TRANSMISSION__
#error only libtransmission should #include this header.
#endif

#ifndef _TR_RATECONTROL_H_
#define _TR_RATECONTROL_H_

#include <string.h> /* memset() */

#include "transmission.h"

/* these are PRIVATE IMPLEMENTATION details that should not be touched.
 * it's included in the header for inlining and composition. */
enum
{
    TR_RC_HISTORY_MSEC = 2000,
    TR_RC_GRANULARITY_MSEC = 250,
    TR_RC_HISTORY_SIZE = ( TR_RC_HISTORY_MSEC / TR_RC_GRANULARITY_MSEC )
};

/* these are PRIVATE IMPLEMENTATION details that should not be touched.
 * it's included in the header for inlining and composition. */
struct tr_transfer
{
    uint64_t    date;
    uint64_t    size;
};

/* these are PRIVATE IMPLEMENTATION details that should not be touched.
 * it's included in the header for inlining and composition. */
typedef struct tr_ratecontrol
{
    int                   newest;
    struct tr_transfer    transfers[TR_RC_HISTORY_SIZE];
}
tr_ratecontrol;

/***
****
***/

static inline void tr_rcConstruct ( tr_ratecontrol * rc ) { memset( rc, 0, sizeof( tr_ratecontrol ) ); }

static inline void tr_rcDestruct  ( tr_ratecontrol * rc ) { memset( rc, 0xDEAD, sizeof( tr_ratecontrol ) ); }

void             tr_rcTransferred ( tr_ratecontrol         * ratecontrol,
                                    size_t                   byteCount );

int              tr_rcRate_Bps ( const tr_ratecontrol   * ratecontrol,
                                 uint64_t                 now );


#endif
