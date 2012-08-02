/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: session.h 11800 2011-01-31 23:35:10Z jordan $
 */

#ifndef __TRANSMISSION__
 #error only libtransmission should #include this header.
#endif

#ifndef TR_INTERNAL_H
#define TR_INTERNAL_H 1

#define TR_NAME "Transmission"

#ifndef UNUSED
 #ifdef __GNUC__
  #define UNUSED __attribute__ ( ( unused ) )
 #else
  #define UNUSED
 #endif
#endif

#include "bencode.h"
#include "bitfield.h"
#include "utils.h"

typedef enum { TR_NET_OK, TR_NET_ERROR, TR_NET_WAIT } tr_tristate_t;

uint8_t*       tr_peerIdNew( void );

const uint8_t* tr_getPeerId( void );

struct event_base;
struct tr_address;
struct tr_announcer;
struct tr_bandwidth;
struct tr_bindsockets;
struct tr_cache;
struct tr_fdInfo;

typedef void ( tr_web_config_func )( tr_session * session, void * curl_pointer, const char * url );

struct tr_turtle_info
{
    /* TR_UP and TR_DOWN speed limits */
    int speedLimit_Bps[2];

    /* is turtle mode on right now? */
    tr_bool isEnabled;

    /* does turtle mode turn itself on and off at given times? */
    tr_bool isClockEnabled;

    /* when clock mode is on, minutes after midnight to turn on turtle mode */
    int beginMinute;

    /* when clock mode is on, minutes after midnight to turn off turtle mode */
    int endMinute;

    /* only use clock mode on these days of the week */
    tr_sched_day days;

    /* called when isEnabled changes */
    tr_altSpeedFunc * callback;

    /* the callback's user_data argument */
    void * callbackUserData;

    /* the callback's changedByUser argument.
     * indicates whether the change came from the user or from the clock. */
    tr_bool changedByUser;

    /* bitfield of all the minutes in a week.
     * Each bit's value indicates whether the scheduler wants turtle
     * limits on or off at that given minute in the week. */
    tr_bitfield minutes;
};

/** @brief handle to an active libtransmission session */
struct tr_session
{
    tr_bool                      isPortRandom;
    tr_bool                      isPexEnabled;
    tr_bool                      isDHTEnabled;
    tr_bool                      isLPDEnabled;
    tr_bool                      isBlocklistEnabled;
    tr_bool                      isPrefetchEnabled;
    tr_bool                      isTorrentDoneScriptEnabled;
    tr_bool                      isClosed;
    tr_bool                      useLazyBitfield;
    tr_bool                      isIncompleteFileNamingEnabled;
    tr_bool                      isRatioLimited;
    tr_bool                      isIdleLimited;
    tr_bool                      isIncompleteDirEnabled;
    tr_bool                      pauseAddedTorrent;
    tr_bool                      deleteSourceTorrent;

    tr_benc                      removedTorrents;

    int                          umask;

    int                          speedLimit_Bps[2];
    tr_bool                      speedLimitEnabled[2];

    struct tr_turtle_info        turtle;

    struct tr_fdInfo           * fdInfo;

    int                          magicNumber;

    tr_encryption_mode           encryptionMode;

    tr_preallocation_mode        preallocationMode;

    struct event_base          * event_base;
    struct tr_event_handle     * events;

    uint16_t                     peerLimitPerTorrent;

    int                          uploadSlotsPerTorrent;

    /* The UDP sockets used for the DHT and uTP. */
    tr_port                      udp_port;
    int                          udp_socket;
    int                          udp6_socket;
    unsigned char *              udp6_bound;
    struct event                 *udp_event;
    struct event                 *udp6_event;

    /* The open port on the local machine for incoming peer requests */
    tr_port                      private_peer_port;

    /**
     * The open port on the public device for incoming peer requests.
     * This is usually the same as private_peer_port but can differ
     * if the public device is a router and it decides to use a different
     * port than the one requested by Transmission.
     */
    tr_port                      public_peer_port;

    tr_port                      randomPortLow;
    tr_port                      randomPortHigh;

    int                          peerSocketTOS;
    char *                       peer_congestion_algorithm;

    int                          torrentCount;
    tr_torrent *                 torrentList;

    char *                       torrentDoneScript;

    char *                       tag;
    char *                       configDir;
    char *                       downloadDir;
    char *                       resumeDir;
    char *                       torrentDir;
    char *                       incompleteDir;

    char *                       blocklist_url;

    struct tr_list *             blocklists;
    struct tr_peerMgr *          peerMgr;
    struct tr_shared *           shared;

    struct tr_cache *            cache;

    struct tr_lock *             lock;

    struct tr_web *              web;

    struct tr_rpc_server *       rpcServer;
    tr_rpc_func                  rpc_func;
    void *                       rpc_func_user_data;

    struct tr_stats_handle     * sessionStats;

    struct tr_announcer        * announcer;

    tr_benc                    * metainfoLookup;

    struct event               * nowTimer;
    struct event               * saveTimer;

    /* monitors the "global pool" speeds */
    struct tr_bandwidth        * bandwidth;

    double                       desiredRatio;

    uint16_t                     idleLimitMinutes;

    struct tr_bindinfo         * public_ipv4;
    struct tr_bindinfo         * public_ipv6;

    /* a page-aligned buffer for use by the libtransmission thread.
     * @see SESSION_BUFFER_SIZE */
    void * buffer;

    tr_bool bufferInUse;

    tr_web_config_func          * curl_easy_config_func;
};

static inline tr_port
tr_sessionGetPublicPeerPort( const tr_session * session )
{
    return session->public_peer_port;
}

tr_bool      tr_sessionAllowsDHT( const tr_session * session );

tr_bool      tr_sessionAllowsLPD( const tr_session * session );

const char * tr_sessionFindTorrentFile( const tr_session * session,
                                        const char *       hashString );

void         tr_sessionSetTorrentFile( tr_session * session,
                                       const char * hashString,
                                       const char * filename );

tr_bool      tr_sessionIsAddressBlocked( const tr_session        * session,
                                         const struct tr_address * addr );

void         tr_sessionLock( tr_session * );

void         tr_sessionUnlock( tr_session * );

tr_bool      tr_sessionIsLocked( const tr_session * );

const struct tr_address*  tr_sessionGetPublicAddress( const tr_session  * session,
                                                      int                 tr_af_type,
                                                      tr_bool           * is_default_value );


struct tr_bindsockets * tr_sessionGetBindSockets( tr_session * );

int tr_sessionCountTorrents( const tr_session * session );

enum
{
    SESSION_MAGIC_NUMBER = 3845,

    /* @see tr_session.buffer */
    SESSION_BUFFER_SIZE = (16*1024)
};

void* tr_sessionGetBuffer( tr_session * session );

void tr_sessionReleaseBuffer( tr_session * session );

static inline tr_bool tr_isSession( const tr_session * session )
{
    return ( session != NULL ) && ( session->magicNumber == SESSION_MAGIC_NUMBER );
}

static inline tr_bool tr_isPreallocationMode( tr_preallocation_mode m  )
{
    return ( m == TR_PREALLOCATE_NONE )
        || ( m == TR_PREALLOCATE_SPARSE )
        || ( m == TR_PREALLOCATE_FULL );
}

static inline tr_bool tr_isEncryptionMode( tr_encryption_mode m )
{
    return ( m == TR_CLEAR_PREFERRED )
        || ( m == TR_ENCRYPTION_PREFERRED )
        || ( m == TR_ENCRYPTION_REQUIRED );
}

static inline tr_bool tr_isPriority( tr_priority_t p )
{
    return ( p == TR_PRI_LOW )
        || ( p == TR_PRI_NORMAL )
        || ( p == TR_PRI_HIGH );
}

/***
****
***/

static inline unsigned int toSpeedBytes ( unsigned int KBps ) { return KBps * tr_speed_K; }
static inline double       toSpeedKBps  ( unsigned int Bps )  { return Bps / (double)tr_speed_K; }

static inline uint64_t toMemBytes ( unsigned int MB ) { uint64_t B = tr_mem_K * tr_mem_K; B *= MB; return B; }
static inline int      toMemMB    ( uint64_t B )      { return B / ( tr_mem_K * tr_mem_K ); }

/**
**/

int  tr_sessionGetSpeedLimit_Bps( const tr_session *, tr_direction );
int  tr_sessionGetAltSpeed_Bps  ( const tr_session *, tr_direction );
int  tr_sessionGetRawSpeed_Bps  ( const tr_session *, tr_direction );
int  tr_sessionGetPieceSpeed_Bps( const tr_session *, tr_direction );

void tr_sessionSetSpeedLimit_Bps( tr_session *, tr_direction, int Bps );
void tr_sessionSetAltSpeed_Bps  ( tr_session *, tr_direction, int Bps );

tr_bool  tr_sessionGetActiveSpeedLimit_Bps( const tr_session  * session,
                                            tr_direction        dir,
                                            int               * setme );


/**
 * Tries to use libevent's cached timeval so we can avoid excessive calls
 * to gettimeofday().
 *
 * This isn't for all uses, but should be reasonably accurate when called
 * near the beginning of a libevent callback.
 */
uint64_t tr_sessionGetTimeMsec( tr_session * session );


#endif
