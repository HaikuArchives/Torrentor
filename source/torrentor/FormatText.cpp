//------------------------------------------------------------------------------
//	Copyright (c) 2012, Guido Pola.
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the "Software"),
//	to deal in the Software without restriction, including without limitation
//	the rights to use, copy, modify, merge, publish, distribute, sublicense,
//	and/or sell copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//	DEALINGS IN THE SOFTWARE.
//
//	File Name:		FormatText.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <libtransmission/transmission.h>
#include <libtransmission/utils.h>
#include <InterfaceDefs.h>
#include <String.h>
#include <StringList.h>

#include "TorrentObject.h"
#include "FormatText.h"

#define B_UTF8_ARROW_UP_CHARACTER "\xE2\x86\x91"
#define B_UTF8_ARROW_DOWN_CHARACTER "\xE2\x86\x93"


static const int32 kDayInSeconds 	= (24 * 60 * 60);
static const int32 kHourInSeconds	= (60 * 60);
static const int32 kMinuteInSeconds = 60;


//
//
//
void FormatRatioText(BString& Buffer, float Ratio)
{
	if( static_cast<int>(Ratio) == TR_RATIO_NA )
		Buffer = "None";
	else if( static_cast<int>(Ratio) == TR_RATIO_INF )
		Buffer = "Inf";
	else if( Ratio < 10.0f )
        Buffer.SetToFormat("%.2f", Ratio);
    else if( Ratio < 100.0f )
        Buffer.SetToFormat("%.1f", Ratio);
    else
        Buffer.SetToFormat("%.0f", Ratio);
}


void FormatStatusText(BString& Buffer, TorrentObject* torrent)
{
	char FormatBuffer[1024];	
	
	const tr_stat * st = torrent->Statistics();
	
	
	switch( st->activity )
	{
	case TR_STATUS_STOPPED:
		Buffer.SetTo("Paused");
		break;
	case TR_STATUS_CHECK_WAIT:
		Buffer.SetTo("Waiting to check existing data"B_UTF8_ELLIPSIS);
		break;
	case TR_STATUS_CHECK:
		Buffer.SetToFormat("Checking existing data (%s%%)",
			tr_strpercent(FormatBuffer, st->recheckProgress * 100.0, sizeof(FormatBuffer)));
		break;
	case TR_STATUS_DOWNLOAD:
	{
		BString DownloadText = tr_formatter_speed_KBps(FormatBuffer, st->pieceDownloadSpeed_KBps, sizeof(FormatBuffer));
		BString UploadText = tr_formatter_speed_KBps(FormatBuffer, st->pieceUploadSpeed_KBps, sizeof(FormatBuffer));
		Buffer.SetToFormat("Downloading from %d of %d connected peers - " B_UTF8_ARROW_DOWN_CHARACTER " %s, " 
							B_UTF8_ARROW_UP_CHARACTER " %s", 
							st->webseedsSendingToUs + st->peersSendingToUs,
							st->webseedsSendingToUs + st->peersConnected,
							DownloadText.String(), UploadText.String());
		}
		break;
	case TR_STATUS_SEED:
	{
		BString UploadText = tr_formatter_speed_KBps(FormatBuffer, st->pieceUploadSpeed_KBps, sizeof(FormatBuffer));
		Buffer.SetToFormat("Seeding to %d of %d connected peer(s) - " B_UTF8_ARROW_UP_CHARACTER " %s", 
							st->peersGettingFromUs, st->peersConnected, UploadText.String());
	}break;
	default:
		Buffer.SetTo(B_EMPTY_STRING);
		break;
	}
}


void FormatTimeText(BString& Buffer, int seconds)
{
	BString FormatBuffer;
	int remaining = seconds;
	
	const int Days 	= seconds / kDayInSeconds;
	const int Hours = (seconds % kDayInSeconds) / kHourInSeconds;
	const int Minutes = (seconds % kMinuteInSeconds) / 60;
	const int Seconds = (seconds % kHourInSeconds) % 60;
	
	if( Days > 0 )
	{
		if( Hours > 0 )
		{
			Buffer.SetToFormat("%d days, %d hours", Days, Hours);
		}
		else
		{
			Buffer.SetToFormat("%d days", Days);
		}
	}
	else if( Hours > 0 )
	{
		if( Minutes > 0 )
		{
			Buffer.SetToFormat("%d hours, %d minutes", Hours, Minutes);
		}
		else
		{
			Buffer.SetToFormat("%d hours", Hours);
		}
	}
	else if( Minutes > 0 )
	{
		if( Minutes > 0 )
		{
			Buffer.SetToFormat("%d minutes, %d seconds", Minutes, Seconds);
		}
		else
		{
			Buffer.SetToFormat("%d minutes", Minutes);
		}
	}
	else
	{
		Buffer.SetToFormat("%d seconds", Seconds);
	}
}

void FormatProgressText(BString& Buffer, TorrentObject* torrent)
{
	char FormatBuffer[1024] = {0};
	
	const tr_stat* st 		 	 = torrent->Statistics();
    const bool     IsDownloading = st->leftUntilDone > 0;
    const uint64_t haveTotal 	 = st->haveUnchecked + st->haveValid;
    
    
   
	if( IsDownloading )
    {
    	//
    	//
    	//
    	BString PartialSize	 = tr_formatter_mem_B(FormatBuffer, haveTotal, sizeof(FormatBuffer));
    	BString CompleteSize = tr_formatter_mem_B(FormatBuffer, st->sizeWhenDone, sizeof(FormatBuffer));
    	BString PercentBuffer= tr_strpercent(FormatBuffer, st->percentDone * 100.0, sizeof(FormatBuffer));
    	
    	Buffer.SetToFormat("%s of %s (%s%%)", PartialSize.String(), CompleteSize.String(), PercentBuffer.String());
    }
    
    //
    // @FIXME: Clean this code.
    //
    if( st->activity == TR_STATUS_DOWNLOAD )
	{
		const int EstimatedTimeLeft = st->eta;
		
		if( EstimatedTimeLeft < 0 )
		{
			Buffer << " - Remaining time unknown.";
		}	
		else
		{
			BString EstimatedTimeBuffer;
			
			FormatTimeText(EstimatedTimeBuffer, EstimatedTimeLeft);
			
			Buffer << " - " << EstimatedTimeBuffer << " remaining.";
		}
	}
}

