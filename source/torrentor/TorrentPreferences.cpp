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
//	File Name:		TorrentPreferences.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <libtransmission/transmission.h>

#include "Torrentor.h"
#include "TorrentPreferences.h"

TorrentPreferences::TorrentPreferences() 
	:	fSession(NULL)
{
	tr_bencInitDict(&fHandle, 2);
	
	//
	//
	//
	tr_bencDictAddBool(&fHandle, TR_PREFS_KEY_AUTO_ADD_TORRENT_ENABLED, false);
 	tr_bencDictAddStr(&fHandle, TR_PREFS_KEY_AUTO_ADD_TORRENT_FOLDER, TR_DEFAULT_AUTO_ADD_TORRENT_FOLDER);
	
	tr_sessionLoadSettings(&fHandle, tr_getDefaultConfigDir("Torrentor"), "Torrentor");
}

TorrentPreferences::~TorrentPreferences()
{
	tr_bencFree(&fHandle);
}


void TorrentPreferences::Save()
{
	tr_sessionGetSettings(fSession, &fHandle);
	tr_sessionSaveSettings(fSession, tr_getDefaultConfigDir("Torrentor"), &fHandle);
}

#pragma mark - Get config value

bool TorrentPreferences::StartWhenAddedEnabled()
{
	return !tr_sessionGetPaused(fSession);	
}

bool TorrentPreferences::AutoAddTorrentEnabled()
{
	bool result = false;
	
	if (tr_bencDictFindBool(&fHandle, TR_PREFS_KEY_AUTO_ADD_TORRENT_ENABLED, &result))
		return result;
	
	return false;
}

BString	TorrentPreferences::AutoAddTorrentPath()
{
	const char* result = B_EMPTY_STRING;
	
	tr_bencDictFindStr(&fHandle, TR_PREFS_KEY_AUTO_ADD_TORRENT_FOLDER, &result);
	
	return result;
}


bool TorrentPreferences::IncompleteFileNamingEnabled()
{
	return tr_sessionIsIncompleteFileNamingEnabled(fSession);
}

BString TorrentPreferences::DownloadFolder()
{
	return tr_sessionGetDownloadDir(fSession);	
}

bool TorrentPreferences::IncompleteFolderEnabled()
{
	return tr_sessionIsIncompleteDirEnabled(fSession);
}

BString TorrentPreferences::IncompleteFolder()
{
	return tr_sessionGetIncompleteDir(fSession);	
}

#pragma mark - Set config value

void TorrentPreferences::StartWhenAddedToggle()
{
	tr_sessionSetPaused(fSession, StartWhenAddedEnabled());
}

void TorrentPreferences::IncompleteFileNamingToggle()
{
	tr_sessionSetIncompleteFileNamingEnabled(fSession, !IncompleteFileNamingEnabled());
}

void TorrentPreferences::IncompleteFolderToggle()
{
	tr_sessionSetIncompleteDirEnabled(fSession, !IncompleteFolderEnabled());
}


void TorrentPreferences::SetDownloadFolder(BString path)
{
	tr_sessionSetDownloadDir(fSession, path.String());
}

void TorrentPreferences::SetIncompleteFolder(BString path)
{
	tr_sessionSetIncompleteDir(fSession, path.String());
}

