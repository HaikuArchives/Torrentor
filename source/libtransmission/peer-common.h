/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: peer-common.h 11709 2011-01-19 13:48:47Z jordan $
 */

#ifndef __TRANSMISSION__
 #error only libtransmission should #include this header.
#endif

#ifndef TR_PEER_H
#define TR_PEER_H

/**
 * @addtogroup peers Peers
 * @{
 */

/**
*** Fields common to webseed and bittorrent peers
**/

#include "transmission.h"

enum
{
    /** when we're making requests from another peer,
        batch them together to send enough requests to
        meet our bandwidth goals for the next N seconds */
    REQUEST_BUF_SECS = 10,

    /** this is the maximum size of a block request.
        most bittorrent clients will reject requests
        larger than this size. */
    MAX_BLOCK_SIZE = ( 1024 * 16 )
};

typedef enum
{
    TR_ADDREQ_OK = 0,
    TR_ADDREQ_FULL,
    TR_ADDREQ_DUPLICATE,
    TR_ADDREQ_MISSING,
    TR_ADDREQ_CLIENT_CHOKED
}
tr_addreq_t;

/**
***  Peer Publish / Subscribe
**/

typedef enum
{
    TR_PEER_CLIENT_GOT_BLOCK,
    TR_PEER_CLIENT_GOT_CHOKE,
    TR_PEER_CLIENT_GOT_DATA,
    TR_PEER_CLIENT_GOT_ALLOWED_FAST,
    TR_PEER_CLIENT_GOT_SUGGEST,
    TR_PEER_CLIENT_GOT_PORT,
    TR_PEER_CLIENT_GOT_REJ,
    TR_PEER_PEER_GOT_DATA,
    TR_PEER_PEER_PROGRESS,
    TR_PEER_ERROR
}
PeerEventType;

typedef struct
{
    PeerEventType    eventType;
    uint32_t         pieceIndex;   /* for GOT_BLOCK, CANCEL, ALLOWED, SUGGEST */
    uint32_t         offset;       /* for GOT_BLOCK */
    uint32_t         length;       /* for GOT_BLOCK + GOT_DATA */
    float            progress;     /* for PEER_PROGRESS */
    int              err;          /* errno for GOT_ERROR */
    tr_bool          wasPieceData; /* for GOT_DATA */
    tr_port          port;         /* for GOT_PORT */
}
tr_peer_event;

struct tr_peer;

typedef void tr_peer_callback( struct tr_peer       * peer,
                               const tr_peer_event  * event,
                               void                 * client_data );

#ifdef WIN32
 #define EMSGSIZE WSAEMSGSIZE
#endif

/** @} */

#endif
