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
#include <Entry.h>
#include <String.h>
#include "Torrentor.h"
#include "TorrentObject.h"

#define UTF8_INFINITE_CHARACTER	"\xE2\x88\x9E"

int _TorrentRemoveFileHandler(const char* filename)
{
	BEntry entry(filename);
	entry.Remove();
	return 0;
}


TorrentObject::TorrentObject()
	:	fTorrentHandle(NULL),
		fStatistics(NULL),
		fInfo(NULL)
{
}

bool TorrentObject::LoadFromHandle(tr_torrent* TorrentHandle)
{
	fTorrentHandle = TorrentHandle;
	fInfo 		= tr_torrentInfo(fTorrentHandle);
	fStatistics = tr_torrentStat(fTorrentHandle);

	return true;
}

bool TorrentObject::LoadFromPath(const tr_session* Session, const char* TorrentPath)
{
	tr_ctor* ctor = tr_ctorNew(Session);
	
	tr_ctorSetPaused(ctor, TR_FORCE, TRUE);
	
	if( tr_ctorSetMetainfoFromFile(ctor, TorrentPath) != TR_PARSE_OK )
		return false;
		
	//
	//
	fTorrentHandle = tr_torrentNew(ctor, NULL);
	
	tr_ctorFree(ctor);
	
	
	if( fTorrentHandle == NULL )
		return false;
		
	
	fInfo 		= tr_torrentInfo(fTorrentHandle);
	fStatistics = tr_torrentStat(fTorrentHandle);


	return true;
}

bool TorrentObject::LoadFromMagnet(const tr_session* Session, const char* MagnetURL)
{
	tr_ctor* ctor = tr_ctorNew(Session);
	
	tr_ctorSetPaused(ctor, TR_FORCE, TRUE);
	
	if( tr_ctorSetMetainfoFromMagnetLink(ctor, MagnetURL) != TR_PARSE_OK )
		return false;
		
	//
	//
	fTorrentHandle = tr_torrentNew(ctor, NULL);
	tr_ctorFree(ctor);
	
	
	if( fTorrentHandle == NULL )
		return false;
		
	
	fInfo 		= tr_torrentInfo(fTorrentHandle);
	fStatistics = tr_torrentStat(fTorrentHandle);


	return true;	
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

bool TorrentObject::IsMagnet() const
{
	return !tr_torrentHasMetadata( fTorrentHandle );
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

void TorrentObject::Remove(bool DeleteFiles) const
{
	tr_torrentRemove(fTorrentHandle, DeleteFiles, _TorrentRemoveFileHandler);
}


//
//
//
BString TorrentObject::Name() const
{
	if( Info()->name == NULL )
		return fInfo->hashString;
		
	return Info()->name;
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
