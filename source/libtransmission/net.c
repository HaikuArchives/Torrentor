/******************************************************************************
 *
 * $Id: net.c 11709 2011-01-19 13:48:47Z jordan $
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>

#ifdef WIN32
 #define _WIN32_WINNT   0x0501
 #include <ws2tcpip.h>
#else
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/tcp.h>
 #include <arpa/inet.h> /* inet_addr */
 #include <netdb.h>
 #include <fcntl.h>
#endif
#include <unistd.h>

#include <event2/util.h>

#include "transmission.h"
#include "fdlimit.h"
#include "natpmp.h"
#include "net.h"
#include "peer-io.h"
#include "platform.h"
#include "session.h"
#include "utils.h"

#ifndef IN_MULTICAST
#define IN_MULTICAST( a ) ( ( ( a ) & 0xf0000000 ) == 0xe0000000 )
#endif

const tr_address tr_in6addr_any = { TR_AF_INET6, { IN6ADDR_ANY_INIT } };
const tr_address tr_inaddr_any = { TR_AF_INET, { { { { INADDR_ANY, 0x00, 0x00, 0x00 } } } } };

#ifdef WIN32
const char *
inet_ntop( int af, const void * src, char * dst, socklen_t cnt )
{
    if (af == AF_INET)
    {
        struct sockaddr_in in;
        memset( &in, 0, sizeof( in ) );
        in.sin_family = AF_INET;
        memcpy( &in.sin_addr, src, sizeof( struct in_addr ) );
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in),
                    dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }
    else if (af == AF_INET6)
    {
        struct sockaddr_in6 in;
        memset( &in, 0, sizeof( in ) );
        in.sin6_family = AF_INET6;
        memcpy( &in.sin6_addr, src, sizeof( struct in_addr6 ) );
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6),
                    dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }
    return NULL;
}

int
inet_pton( int af, const char * src, void * dst )
{
    struct addrinfo hints, *res, *ressave;
    struct sockaddr_in * s4;
    struct sockaddr_in6 * s6;

    memset( &hints, 0, sizeof( struct addrinfo ));
    hints.ai_family = af;
    hints.ai_flags = AI_NUMERICHOST;

    if( getaddrinfo( src, NULL, &hints, &res ) ) {
        if( WSAGetLastError() == WSAHOST_NOT_FOUND )
            return 0;
        else {
            errno = EAFNOSUPPORT;
            return -1;
        }
    }

    ressave = res;
    while( res ) {
        switch (res->ai_family) {
            case AF_INET:
                s4 = (struct sockaddr_in *) res->ai_addr;
                memcpy( dst, &s4->sin_addr, sizeof( struct in_addr ) );
                break;
            case AF_INET6:
                s6 = (struct sockaddr_in6 *) res->ai_addr;
                memcpy( dst, &s6->sin6_addr, sizeof( struct in6_addr ) );
                break;
            default: /* AF_UNSPEC, AF_NETBIOS */
                break;
        }
        res = res->ai_next;
    }

    freeaddrinfo(ressave);
    return 1;
}
#endif

void
tr_netInit( void )
{
    static int initialized = FALSE;

    if( !initialized )
    {
#ifdef WIN32
        WSADATA wsaData;
        WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
#endif
        initialized = TRUE;
    }
}

char *
tr_net_strerror( char * buf, size_t buflen, int err )
{
    *buf = '\0';
#ifdef WIN32
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, buf, buflen, NULL );
#else
    tr_strlcpy( buf, tr_strerror( err ), buflen );
#endif
    return buf;
}

const char *
tr_ntop( const tr_address * src, char * dst, int size )
{
    assert( tr_isAddress( src ) );

    if( src->type == TR_AF_INET )
        return inet_ntop( AF_INET, &src->addr, dst, size );
    else
        return inet_ntop( AF_INET6, &src->addr, dst, size );
}

/*
 * Non-threadsafe version of tr_ntop, which uses a static memory area for a buffer.
 * This function is suitable to be called from libTransmission's networking code,
 * which is single-threaded.
 */
const char *
tr_ntop_non_ts( const tr_address * src )
{
    static char buf[INET6_ADDRSTRLEN];
    return tr_ntop( src, buf, sizeof( buf ) );
}

tr_address *
tr_pton( const char * src, tr_address * dst )
{
    int retval = inet_pton( AF_INET, src, &dst->addr );
    assert( dst );
    if( retval < 0 )
        return NULL;
    else if( retval == 0 )
        retval = inet_pton( AF_INET6, src, &dst->addr );
    else
    {
        dst->type = TR_AF_INET;
        return dst;
    }

    if( retval < 1 )
        return NULL;
    dst->type = TR_AF_INET6;
    return dst;
}

/*
 * Compare two tr_address structures.
 * Returns:
 * <0 if a < b
 * >0 if a > b
 * 0  if a == b
 */
int
tr_compareAddresses( const tr_address * a, const tr_address * b)
{
    static const int sizes[2] = { sizeof(struct in_addr), sizeof(struct in6_addr) };

    /* IPv6 addresses are always "greater than" IPv4 */
    if( a->type != b->type )
        return a->type == TR_AF_INET ? 1 : -1;

    return memcmp( &a->addr, &b->addr, sizes[a->type] );
}

/***********************************************************************
 * TCP sockets
 **********************************************************************/

int
tr_netSetTOS( int s, int tos )
{
#ifdef IP_TOS
    return setsockopt( s, IPPROTO_IP, IP_TOS, (char*)&tos, sizeof( tos ) );
#else
    return 0;
#endif
}

int
tr_netSetCongestionControl( int s UNUSED, const char *algorithm UNUSED )
{
#ifdef TCP_CONGESTION
    return setsockopt( s, IPPROTO_TCP, TCP_CONGESTION,
                       algorithm, strlen(algorithm) + 1 );
#else
    errno = ENOSYS;
    return -1;
#endif
}

static socklen_t
setup_sockaddr( const tr_address        * addr,
                tr_port                   port,
                struct sockaddr_storage * sockaddr)
{
    assert( tr_isAddress( addr ) );

    if( addr->type == TR_AF_INET )
    {
        struct sockaddr_in  sock4;
        memset( &sock4, 0, sizeof( sock4 ) );
        sock4.sin_family      = AF_INET;
        sock4.sin_addr.s_addr = addr->addr.addr4.s_addr;
        sock4.sin_port        = port;
        memcpy( sockaddr, &sock4, sizeof( sock4 ) );
        return sizeof( struct sockaddr_in );
    }
    else
    {
        struct sockaddr_in6 sock6;
        memset( &sock6, 0, sizeof( sock6 ) );
        sock6.sin6_family   = AF_INET6;
        sock6.sin6_port     = port;
        sock6.sin6_flowinfo = 0;
        sock6.sin6_addr     = addr->addr.addr6;
        memcpy( sockaddr, &sock6, sizeof( sock6 ) );
        return sizeof( struct sockaddr_in6 );
    }
}

int
tr_netOpenPeerSocket( tr_session        * session,
                      const tr_address  * addr,
                      tr_port             port,
                      tr_bool             clientIsSeed )
{
    static const int domains[NUM_TR_AF_INET_TYPES] = { AF_INET, AF_INET6 };
    int                     s;
    struct sockaddr_storage sock;
    socklen_t               addrlen;
    const tr_address      * source_addr;
    socklen_t               sourcelen;
    struct sockaddr_storage source_sock;

    assert( tr_isAddress( addr ) );

    if( !tr_isValidPeerAddress( addr, port ) )
        return -EINVAL;

    s = tr_fdSocketCreate( session, domains[addr->type], SOCK_STREAM );
    if( s < 0 )
        return -1;

    /* seeds don't need much of a read buffer... */
    if( clientIsSeed ) {
        int n = 8192;
        if( setsockopt( s, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n) ) )
            tr_inf( "Unable to set SO_RCVBUF on socket %d: %s", s, tr_strerror( sockerrno ) );
    }

    if( evutil_make_socket_nonblocking( s ) < 0 ) {
        tr_netClose( session, s );
        return -1;
    }

    addrlen = setup_sockaddr( addr, port, &sock );

    /* set source address */
    source_addr = tr_sessionGetPublicAddress( session, addr->type, NULL );
    assert( source_addr );
    sourcelen = setup_sockaddr( source_addr, 0, &source_sock );
    if( bind( s, ( struct sockaddr * ) &source_sock, sourcelen ) )
    {
        tr_err( _( "Couldn't set source address %s on %d: %s" ),
                tr_ntop_non_ts( source_addr ), s, tr_strerror( errno ) );
        return -errno;
    }

    if( ( connect( s, (struct sockaddr *) &sock,
                  addrlen ) < 0 )
#ifdef WIN32
      && ( sockerrno != WSAEWOULDBLOCK )
#endif
      && ( sockerrno != EINPROGRESS ) )
    {
        int tmperrno;
        tmperrno = sockerrno;
        if( ( tmperrno != ENETUNREACH && tmperrno != EHOSTUNREACH )
                || addr->type == TR_AF_INET )
            tr_err( _( "Couldn't connect socket %d to %s, port %d (errno %d - %s)" ),
                    s, tr_ntop_non_ts( addr ), (int)ntohs( port ), tmperrno,
                    tr_strerror( tmperrno ) );
        tr_netClose( session, s );
        s = -tmperrno;
    }

    tr_deepLog( __FILE__, __LINE__, NULL, "New OUTGOING connection %d (%s)",
               s, tr_peerIoAddrStr( addr, port ) );

    return s;
}

static int
tr_netBindTCPImpl( const tr_address * addr, tr_port port, tr_bool suppressMsgs, int * errOut )
{
    static const int domains[NUM_TR_AF_INET_TYPES] = { AF_INET, AF_INET6 };
    struct sockaddr_storage sock;
    int fd;
    int addrlen;
    int optval;

    assert( tr_isAddress( addr ) );

    fd = socket( domains[addr->type], SOCK_STREAM, 0 );
    if( fd < 0 ) {
        *errOut = sockerrno;
        return -1;
    }

    if( evutil_make_socket_nonblocking( fd ) < 0 ) {
        *errOut = sockerrno;
        tr_netCloseSocket( fd );
        return -1;
    }

    optval = 1;
    setsockopt( fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval) );
    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval) );

#ifdef IPV6_V6ONLY
    if( addr->type == TR_AF_INET6 )
        if( setsockopt( fd, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof( optval ) ) == -1 )
            if( sockerrno != ENOPROTOOPT ) { /* if the kernel doesn't support it, ignore it */
                *errOut = sockerrno;
                return -1;
            }
#endif

    addrlen = setup_sockaddr( addr, htons( port ), &sock );
    if( bind( fd, (struct sockaddr *) &sock, addrlen ) ) {
        const int err = sockerrno;
        if( !suppressMsgs )
        {
            const char * fmt;
            const char * hint;

            if( err == EADDRINUSE )
                hint = _( "Is another copy of Transmission already running?" );
            else
                hint = NULL;

            if( hint == NULL )
                fmt = _( "Couldn't bind port %d on %s: %s" );
            else
                fmt = _( "Couldn't bind port %d on %s: %s (%s)" );

            tr_err( fmt, port, tr_ntop_non_ts( addr ), tr_strerror( err ), hint );
        }
        tr_netCloseSocket( fd );
        *errOut = err;
        return -1;
    }

    if( !suppressMsgs )
        tr_dbg( "Bound socket %d to port %d on %s", fd, port, tr_ntop_non_ts( addr ) );

    if( listen( fd, 128 ) == -1 ) {
        *errOut = sockerrno;
        tr_netCloseSocket( fd );
        return -1;
    }

    return fd;
}

int
tr_netBindTCP( const tr_address * addr, tr_port port, tr_bool suppressMsgs )
{
    int unused;
    return tr_netBindTCPImpl( addr, port, suppressMsgs, &unused );
}

tr_bool
tr_net_hasIPv6( tr_port port )
{
    static tr_bool result = FALSE;
    static tr_bool alreadyDone = FALSE;

    if( !alreadyDone )
    {
        int err;
        int fd = tr_netBindTCPImpl( &tr_in6addr_any, port, TRUE, &err );
        if( fd >= 0 || err != EAFNOSUPPORT ) /* we support ipv6 */
            result = TRUE;
        if( fd >= 0 )
            tr_netCloseSocket( fd );
        alreadyDone = TRUE;
    }

    return result;
}

int
tr_netAccept( tr_session  * session,
              int           b,
              tr_address  * addr,
              tr_port     * port )
{
    int fd = tr_fdSocketAccept( session, b, addr, port );

    if( fd>=0 && evutil_make_socket_nonblocking(fd)<0 ) {
        tr_netClose( session, fd );
        fd = -1;
    }

    return fd;
}

void
tr_netCloseSocket( int fd )
{
    EVUTIL_CLOSESOCKET( fd );
}

void
tr_netClose( tr_session * session, int s )
{
    tr_fdSocketClose( session, s );
}

/*
   get_source_address() and global_unicast_address() were written by
   Juliusz Chroboczek, and are covered under the same license as dht.c.
   Please feel free to copy them into your software if it can help
   unbreaking the double-stack Internet. */

/* Get the source address used for a given destination address. Since
   there is no official interface to get this information, we create
   a connected UDP socket (connected UDP... hmm...) and check its source
   address. */
static int
get_source_address( const struct sockaddr  * dst,
                    socklen_t                dst_len,
                    struct sockaddr        * src,
                    socklen_t              * src_len )
{
    int s, rc, save;

    s = socket(dst->sa_family, SOCK_DGRAM, 0);
    if(s < 0)
        goto fail;

    /* Since it's a UDP socket, this doesn't actually send any packets. */
    rc = connect(s, dst, dst_len);
    if(rc < 0)
        goto fail;

    rc = getsockname(s, src, src_len);
    if(rc < 0)
        goto fail;

    EVUTIL_CLOSESOCKET( s );

    return rc;

 fail:
    save = errno;
    EVUTIL_CLOSESOCKET( s );
    errno = save;
    return -1;
}

/* We all hate NATs. */
static int
global_unicast_address(struct sockaddr *sa)
{
    if(sa->sa_family == AF_INET) {
        const unsigned char *a =
            (unsigned char*)&((struct sockaddr_in*)sa)->sin_addr;
        if(a[0] == 0 || a[0] == 127 || a[0] >= 224 ||
           a[0] == 10 || (a[0] == 172 && a[1] >= 16 && a[1] <= 31) ||
           (a[0] == 192 && a[1] == 168))
            return 0;
        return 1;
    } else if(sa->sa_family == AF_INET6) {
        const unsigned char *a =
            (unsigned char*)&((struct sockaddr_in6*)sa)->sin6_addr;
        /* 2000::/3 */
        return (a[0] & 0xE0) == 0x20;
    } else {
        errno = EAFNOSUPPORT;
        return -1;
    }
}

static int
tr_globalAddress( int af, void *addr, int *addr_len )
{
    struct sockaddr_storage ss;
    socklen_t sslen = sizeof(ss);
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
    struct sockaddr *sa;
    socklen_t salen;
    int rc;

    switch(af) {
    case AF_INET:
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        inet_pton(AF_INET, "91.121.74.28", &sin.sin_addr);
        sin.sin_port = htons(6969);
        sa = (struct sockaddr*)&sin;
        salen = sizeof(sin);
        break;
    case AF_INET6:
        memset(&sin6, 0, sizeof(sin6));
        sin6.sin6_family = AF_INET6;
        /* In order for address selection to work right, this should be
           a native IPv6 address, not Teredo or 6to4. */
        inet_pton(AF_INET6, "2001:1890:1112:1::20", &sin6.sin6_addr);
        sin6.sin6_port = htons(6969);
        sa = (struct sockaddr*)&sin6;
        salen = sizeof(sin6);
        break;
    default:
        return -1;
    }

    rc = get_source_address( sa, salen, (struct sockaddr*)&ss, &sslen );

    if( rc < 0 )
        return -1;

    if( !global_unicast_address( (struct sockaddr*)&ss) )
        return -1;

    switch(af) {
    case AF_INET:
        if(*addr_len < 4)
            return -1;
        memcpy(addr, &((struct sockaddr_in*)&ss)->sin_addr, 4);
        *addr_len = 4;
        return 1;
    case AF_INET6:
        if(*addr_len < 16)
            return -1;
        memcpy(addr, &((struct sockaddr_in6*)&ss)->sin6_addr, 16);
        *addr_len = 16;
        return 1;
    default:
        return -1;
    }
}

/* Return our global IPv6 address, with caching. */

const unsigned char *
tr_globalIPv6( void )
{
    static unsigned char ipv6[16];
    static time_t last_time = 0;
    static int have_ipv6 = 0;
    const time_t now = tr_time( );

    /* Re-check every half hour */
    if( last_time < now - 1800 )
    {
        int addrlen = 16;
        const int rc = tr_globalAddress( AF_INET6, ipv6, &addrlen );
        have_ipv6 = ( rc >= 0 ) && ( addrlen == 16 );
        last_time = now;
    }

    return have_ipv6 ? ipv6 : NULL;
}

/***
****
****
***/

static tr_bool
isIPv4MappedAddress( const tr_address * addr )
{
    return ( addr->type == TR_AF_INET6 ) && IN6_IS_ADDR_V4MAPPED( &addr->addr.addr6 );
}

static tr_bool
isIPv6LinkLocalAddress( const tr_address * addr )
{
    return ( ( addr->type == TR_AF_INET6 )
                  && IN6_IS_ADDR_LINKLOCAL( &addr->addr.addr6 ) );
}

/* isMartianAddr was written by Juliusz Chroboczek,
   and is covered under the same license as third-party/dht/dht.c. */
static tr_bool
isMartianAddr( const struct tr_address * a )
{
    static const unsigned char zeroes[16] =
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    assert( tr_isAddress( a ) );

    switch( a->type )
    {
        case TR_AF_INET: {
            const unsigned char * address = (const unsigned char*)&a->addr.addr4;
            return (address[0] == 0) ||
                   (address[0] == 127) ||
                   ((address[0] & 0xE0) == 0xE0);
            break;
        }

        case TR_AF_INET6: {
            const unsigned char * address = (const unsigned char*)&a->addr.addr6;
            return (address[0] == 0xFF) ||
                   (memcmp(address, zeroes, 15) == 0 &&
                    (address[15] == 0 || address[15] == 1)) ||
                   /* Addresses outside of 2000::/3 are currently reserved,
                      but might be allocated at some future time. Since
                      there are a lot of buggy peers pushing around such
                      addresses over PEX, we reject them until the end of
                      the 13th Baktun. */
                   (tr_time() < 1356130800 && (address[0] & 0xE0) != 0x20);
            break;
        }

        default:
            return TRUE;
    }
}

tr_bool
tr_isValidPeerAddress( const tr_address * addr, tr_port port )
{
    return ( port != 0 )
        && ( tr_isAddress( addr ) )
        && ( !isIPv6LinkLocalAddress( addr ) )
        && ( !isIPv4MappedAddress( addr ) )
        && ( !isMartianAddr( addr ) );
}
