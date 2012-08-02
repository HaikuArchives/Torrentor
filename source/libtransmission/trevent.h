/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: trevent.h 11709 2011-01-19 13:48:47Z jordan $
 */

#ifndef __TRANSMISSION__
#error only libtransmission should #include this header.
#endif

#ifndef TR_EVENT_H
#define TR_EVENT_H

#include <stddef.h> /* size_t */
#include <inttypes.h> /* uint64_t */

/**
**/

void      tr_eventInit( tr_session * );

void      tr_eventClose( tr_session * );

tr_bool   tr_amInEventThread( const tr_session * );

void      tr_runInEventThread( tr_session *, void func( void* ), void * user_data );

#endif
