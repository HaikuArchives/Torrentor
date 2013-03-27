//------------------------------------------------------------------------------
//	Copyright (c) 2012-2013, Guido Pola.
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

#ifndef countof
#define countof(x) (sizeof(x)/sizeof(x[0]))
#endif


//static const uint32 kYearInSeconds	= (12 
//static const uint32 kMonthInSeconds	= (30.43666 * 24 * 60 * 60);
static const uint32 kDayInSeconds 	= (24 * 60 * 60);
static const uint32 kHourInSeconds	= (60 * 60);
static const uint32 kMinuteInSeconds = 60;


//
//
//
void FormatRatioText(BString& Buffer, double Ratio, bool ClearBuffer)
{
	char FormatBuffer[128];
	
	//	
	if( ClearBuffer )
		Buffer = B_EMPTY_STRING;
		
	//
	// Format the ratio.
	//
	Buffer << tr_strratio(FormatBuffer, sizeof(FormatBuffer), Ratio, "Infinite");
}


void FormatSizeText(BString& Buffer, int64_t Bytes, bool ClearBuffer)
{
	char FormatBuffer[128];
	
	//	
	if( ClearBuffer )
		Buffer = B_EMPTY_STRING;
		
	//
	// Format the size.
	//
	Buffer << tr_formatter_mem_B(FormatBuffer, Bytes, sizeof(FormatBuffer));
}


void FormatPercentText(BString& Buffer, float Percent, bool ClearBuffer)
{
	char FormatBuffer[128];
	
	//	
	if( ClearBuffer )
		Buffer = B_EMPTY_STRING;
		
	//
	// Format the percent.
	//
	Buffer << tr_strpercent(FormatBuffer, Percent, sizeof(FormatBuffer)) << "%";
}


void FormatStatusText(BString& Buffer, TorrentObject* torrent)
{
	char FormatBuffer[1024];	
	
	const tr_stat * st = torrent->Statistics();
	
	//
	// If we're a magnet (so we don't have metadata), dont format.
	//
	if( torrent->IsMagnet() )
		return;
	
	
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

//
//
//
struct FormatTimeData_t
{
	char 	Name[32];
	uint32 	Division;
};
FormatTimeData_t FormatTimeData[] = 
{
	{"day", kDayInSeconds},
	{"hour", kHourInSeconds},
	{"minute", kMinuteInSeconds},
	{"second", 1}
};

void FormatTimeText(BString& Buffer, int seconds, bool ClearBuffer)
{
	int Remainder 	= seconds;
	bool FirstFormat= true;
	
	if( ClearBuffer )
		Buffer = B_EMPTY_STRING;
	
	//
	//
	//
	for( int i = 0; i < countof(FormatTimeData); i++ )
	{
		if( Remainder < 1 )
			return;
		
		int value = Remainder / FormatTimeData[i].Division;
		Remainder %= FormatTimeData[i].Division;
		
		if( value < 1 )
			continue;
		
		// Format the text.
		Buffer 	<< (FirstFormat ? "" : ", ") 	// if we aren't the first item, put a comma.
				<< value << " " 
				<< FormatTimeData[i].Name 	<< ( value > 1 ? "s" : "");
			
		//
		FirstFormat = false;
	}
}

void FormatProgressText(BString& Buffer, TorrentObject* torrent)
{
	char FormatBuffer[1024] = {0};
	
	const tr_stat* st 		 	 = torrent->Statistics();
    const bool     IsDownloading = st->leftUntilDone > 0;
    const uint64_t haveTotal 	 = st->haveUnchecked + st->haveValid;
    
    Buffer = B_EMPTY_STRING;
    
   
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
		//
		// If we're downloading the metadata
		//
		if( torrent->IsMagnet() )
		{
			Buffer << "Downloading metadata.";
			return;
		}
		
		const int EstimatedTimeLeft = st->eta;
		
		if( EstimatedTimeLeft < 0 )
		{
			Buffer << " - Remaining time unknown.";
		}	
		else
		{
			BString EstimatedTimeBuffer;
			FormatTimeText(EstimatedTimeBuffer, EstimatedTimeLeft);
			
			Buffer 	<< " - " 
					<< EstimatedTimeBuffer 
					<< " remaining.";
		}
	}
}


void FormatStateString(BString& Buffer, const TorrentObject* torrent)
{
	char FormatBuffer[1024] = {0};
	Buffer = B_EMPTY_STRING;
	
	switch( torrent->Statistics()->activity )
	{
	case TR_STATUS_STOPPED:
		Buffer = "Paused";
		break;
	case TR_STATUS_CHECK_WAIT:
		Buffer = "Waiting to check existing data" B_UTF8_ELLIPSIS;
		break;
	case TR_STATUS_CHECK:
		Buffer.SetToFormat("Checking existing data (%s)", 
			tr_strpercent(FormatBuffer, torrent->Statistics()->recheckProgress * 100.0, sizeof(FormatBuffer)));	
		break;
	case TR_STATUS_DOWNLOAD:
		Buffer = "Downloading";
		break;
	case TR_STATUS_SEED:
		Buffer = "Seeding";
		break;
	}
}

