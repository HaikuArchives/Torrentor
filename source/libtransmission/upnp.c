/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: upnp.c 11709 2011-01-19 13:48:47Z jordan $
 */

#include <assert.h>
#include <errno.h>

#include <miniupnp/miniupnpc.h>
#include <miniupnp/upnpcommands.h>

#include "transmission.h"
#include "port-forwarding.h"
#include "session.h"
#include "upnp.h"
#include "utils.h"

static const char *
getKey( void ) { return _( "Port Forwarding (UPnP)" ); }

typedef enum
{
    TR_UPNP_IDLE,
    TR_UPNP_ERR,
    TR_UPNP_DISCOVER,
    TR_UPNP_MAP,
    TR_UPNP_UNMAP
}
tr_upnp_state;

struct tr_upnp
{
    tr_bool            hasDiscovered;
    struct UPNPUrls    urls;
    struct IGDdatas    data;
    int                port;
    char               lanaddr[16];
    unsigned int       isMapped;
    tr_upnp_state      state;
};

/**
***
**/

tr_upnp*
tr_upnpInit( void )
{
    tr_upnp * ret = tr_new0( tr_upnp, 1 );

    ret->state = TR_UPNP_DISCOVER;
    ret->port = -1;
    return ret;
}

void
tr_upnpClose( tr_upnp * handle )
{
    assert( !handle->isMapped );
    assert( ( handle->state == TR_UPNP_IDLE )
          || ( handle->state == TR_UPNP_ERR )
          || ( handle->state == TR_UPNP_DISCOVER ) );

    if( handle->hasDiscovered )
        FreeUPNPUrls( &handle->urls );
    tr_free( handle );
}

/**
***
**/

enum
{
  UPNP_IGD_NONE = 0,
  UPNP_IGD_VALID_CONNECTED = 1,
  UPNP_IGD_VALID_NOT_CONNECTED = 2,
  UPNP_IGD_INVALID = 3
};

int
tr_upnpPulse( tr_upnp * handle,
              int       port,
              int       isEnabled,
              int       doPortCheck )
{
    int ret;

    if( isEnabled && ( handle->state == TR_UPNP_DISCOVER ) )
    {
        struct UPNPDev * devlist;
        errno = 0;
        devlist = upnpDiscover( 2000, NULL, NULL, 0 );
        if( devlist == NULL )
        {
            tr_ndbg(
                 getKey( ), "upnpDiscover failed (errno %d - %s)", errno,
                tr_strerror( errno ) );
        }
        errno = 0;
        if( UPNP_GetValidIGD( devlist, &handle->urls, &handle->data,
                             handle->lanaddr, sizeof( handle->lanaddr ) ) == UPNP_IGD_VALID_CONNECTED )
        {
            tr_ninf( getKey( ), _(
                         "Found Internet Gateway Device \"%s\"" ),
                     handle->urls.controlURL );
            tr_ninf( getKey( ), _(
                         "Local Address is \"%s\"" ), handle->lanaddr );
            handle->state = TR_UPNP_IDLE;
            handle->hasDiscovered = 1;
        }
        else
        {
            handle->state = TR_UPNP_ERR;
            tr_ndbg(
                 getKey( ), "UPNP_GetValidIGD failed (errno %d - %s)",
                errno,
                tr_strerror( errno ) );
            tr_ndbg(
                getKey( ),
                "If your router supports UPnP, please make sure UPnP is enabled!" );
        }
        freeUPNPDevlist( devlist );
    }

    if( handle->state == TR_UPNP_IDLE )
    {
        if( handle->isMapped && ( !isEnabled || ( handle->port != port ) ) )
            handle->state = TR_UPNP_UNMAP;
    }

    if( isEnabled && handle->isMapped && doPortCheck )
    {
        char portStr[8];
        char intPort[8];
        char intClient[16];

        tr_snprintf( portStr, sizeof( portStr ), "%d", handle->port );
        if( UPNP_GetSpecificPortMappingEntry( handle->urls.controlURL, handle->data.first.servicetype, 
            portStr, "TCP", intClient, intPort ) != UPNPCOMMAND_SUCCESS  ||
            UPNP_GetSpecificPortMappingEntry( handle->urls.controlURL, handle->data.first.servicetype, 
            portStr, "UDP", intClient, intPort ) != UPNPCOMMAND_SUCCESS )
        {
            tr_ninf( getKey( ), _( "Port %d isn't forwarded" ), handle->port );
            handle->isMapped = FALSE;
        }
    }

    if( handle->state == TR_UPNP_UNMAP )
    {
        char portStr[16];
        tr_snprintf( portStr, sizeof( portStr ), "%d", handle->port );
        UPNP_DeletePortMapping( handle->urls.controlURL,
                                handle->data.first.servicetype,
                                portStr, "TCP", NULL );
        UPNP_DeletePortMapping( handle->urls.controlURL,
                                handle->data.first.servicetype,
                                portStr, "UDP", NULL );
        tr_ninf( getKey( ),
                 _(
                     "Stopping port forwarding through \"%s\", service \"%s\"" ),
                 handle->urls.controlURL, handle->data.first.servicetype );
        handle->isMapped = 0;
        handle->state = TR_UPNP_IDLE;
        handle->port = -1;
    }

    if( handle->state == TR_UPNP_IDLE )
    {
        if( isEnabled && !handle->isMapped )
            handle->state = TR_UPNP_MAP;
    }

    if( handle->state == TR_UPNP_MAP )
    {
        int  err_tcp = -1;
        int  err_udp = -1;
        errno = 0;

        if( !handle->urls.controlURL || !handle->data.first.servicetype )
            handle->isMapped = 0;
        else
        {
            char portStr[16];
            char desc[64];
            const int prev_errno = errno;
            tr_snprintf( portStr, sizeof( portStr ), "%d", port );
            tr_snprintf( desc, sizeof( desc ), "%s at %d", TR_NAME, port );

            errno = 0;
            err_tcp = UPNP_AddPortMapping( handle->urls.controlURL,
                                       handle->data.first.servicetype,
                                       portStr, portStr, handle->lanaddr,
                                       desc, "TCP", NULL );
            if( err_tcp )
                tr_ndbg( getKey( ), "TCP Port forwarding failed with error %d (errno %d - %s)",
                         err_tcp, errno, tr_strerror( errno ) );

            errno = 0;
            err_udp = UPNP_AddPortMapping( handle->urls.controlURL,
                                       handle->data.first.servicetype,
                                       portStr, portStr, handle->lanaddr,
                                       desc, "UDP", NULL );
            if( err_udp )
                tr_ndbg( getKey( ), "UDP Port forwarding failed with error %d (errno %d - %s)",
                         err_udp, errno, tr_strerror( errno ) );

            errno = prev_errno;
            handle->isMapped = !err_tcp | !err_udp;
        }
        tr_ninf( getKey( ),
                 _( "Port forwarding through \"%s\", service \"%s\". (local address: %s:%d)" ),
                 handle->urls.controlURL, handle->data.first.servicetype,
                 handle->lanaddr, port );
        if( handle->isMapped )
        {
            tr_ninf( getKey( ), "%s", _( "Port forwarding successful!" ) );
            handle->port = port;
            handle->state = TR_UPNP_IDLE;
        }
        else
        {
            tr_ndbg( getKey( ), "If your router supports UPnP, please make sure UPnP is enabled!" );
            handle->port = -1;
            handle->state = TR_UPNP_ERR;
        }
    }

    switch( handle->state )
    {
        case TR_UPNP_DISCOVER:
            ret = TR_PORT_UNMAPPED; break;

        case TR_UPNP_MAP:
            ret = TR_PORT_MAPPING; break;

        case TR_UPNP_UNMAP:
            ret = TR_PORT_UNMAPPING; break;

        case TR_UPNP_IDLE:
            ret = handle->isMapped ? TR_PORT_MAPPED
                  : TR_PORT_UNMAPPED; break;

        default:
            ret = TR_PORT_ERROR; break;
    }

    return ret;
}

