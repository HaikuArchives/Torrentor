/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: metainfo.h 11709 2011-01-19 13:48:47Z jordan $
 */

#ifndef __TRANSMISSION__
 #error only libtransmission should #include this header.
#endif

#ifndef TR_METAINFO_H
#define TR_METAINFO_H 1

#include "transmission.h"

struct tr_benc;

tr_bool  tr_metainfoParse( const tr_session     * session,
                           const struct tr_benc * benc,
                           tr_info              * setmeInfo,
                           tr_bool              * setmeHasInfoDict,
                           int                  * setmeInfoDictLength );

void tr_metainfoRemoveSaved( const tr_session * session,
                             const tr_info    * info );

void tr_metainfoMigrate( tr_session * session,
                         tr_info    * inf );

char* tr_metainfoGetBasename( const tr_info * );


#endif
