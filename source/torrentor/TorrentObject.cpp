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
//	File Name:		TorrentObject.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <String.h>
#include "Torrentor.h"
#include "TorrentObject.h"

#define UTF8_INFINITE_CHARACTER	"\xE2\x88\x9E"

/*
void RatioToString(float ratio)
{
	if(	ratio == TR_RATIO_NA )
		return "No Ratio";
		
	if( ratio == TR_RATIO_INF )
		return UTF8_INFINITE_CHARACTER;
		
	if( ratio < 10.0f )
		return format("%.2f", tr_truncd(ratio, 2));
		
	if( ratio < 100.0f )
		return format("%.1f", tr_truncd(ratio, 1));
		
		
	return format("%.0f", tr_truncd(ratio, 0));
}*/


TorrentObject::TorrentObject(tr_torrent* TorrentHandle)
	:	fTorrentHandle(TorrentHandle)
{
	fInfo 		= tr_torrentInfo(fTorrentHandle);
	fStatistics = tr_torrentStat(fTorrentHandle);
}

TorrentObject::~TorrentObject()
{
//	if( fFileStat )
//		tr_torrentFilesFree(fFileStat, [self fileCount]);

}

void TorrentObject::Update()
{
	fStatistics = tr_torrentStat(fTorrentHandle);
}

void TorrentObject::StopTransfer()
{
	if( !IsActive() )
		return;
		
	//
	//
	//
	tr_torrentStop(fTorrentHandle);
	Update();
}
	
//
//
//
void TorrentObject::StartTransfer()
{
	if( IsActive() )
		return;
	
	tr_torrentStart(fTorrentHandle);
	Update();
}



//
//
//
BString TorrentObject::Name() const
{
	if( fInfo->name == NULL )
		return fInfo->hashString;
		
	return fInfo->name;
}

BString TorrentObject::DownloadFolder() const
{
	return tr_torrentGetDownloadDir(fTorrentHandle);
}

bool TorrentObject::IsAnyErrorOrWarning() const
{
	return Statistics()->error != TR_STAT_OK;
}


BString TorrentObject::ErrorMessage() const
{
	if( !IsAnyErrorOrWarning() )
		return B_EMPTY_STRING;
		
	//
	//
	//
	return Statistics()->errorString;	
}

int TorrentObject::SecondsDownloading() const
{
    return Statistics()->secondsDownloading;
}

int TorrentObject::SecondsSeeding() const
{
    return Statistics()->secondsSeeding;
}
