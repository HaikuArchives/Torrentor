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
//	File Name:		Application.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	Main application class.
//------------------------------------------------------------------------------
#include <Notification.h>
#include <StringList.h>

#include "Torrentor.h"
#include "TorrentorMessages.h"
#include "MainWindow.h"
#include "AddMagnetWindow.h"
#include "AddTorrentWindow.h"
#include "Application.h"





#include <stdio.h>


// tr_torrentSetCompletenessCallback
// tr_torrentSetMetadataCallback
// tr_torrentSetRatioLimitHitCallback
// tr_torrentSetIdleLimitHitCallback

void TorrentCompletenessHandler(tr_torrent* torrent, tr_completeness completeness, bool wasRunning, void* data)
{
	static const char* _completeness[] = {
		"TR_LEECH",
		"TR_SEED",
		"TR_PARTIAL_SEED",	
	};
	
	printf("TorrentCompletenessHandler: %s, wasRunning -> %s\n",
		_completeness[completeness], wasRunning ? "true" : "false");
	//
	// 
	//
	if( completeness == TR_LEECH || !wasRunning )
		return;
	
	BNotification* notification = new BNotification(B_INFORMATION_NOTIFICATION);
	
	
	notification->SetGroup("Torrentor!");
	notification->SetTitle("Download complete");
	notification->SetContent( tr_torrentInfo( torrent )->name );
	notification->Send();
}



TorrentorApp::TorrentorApp()
	: BApplication("application/x-vnd.Torrentor")
{	
	const char * configDir;
	
	tr_formatter_mem_init(1024, "KiB", "MiB", "GiB", "TiB");
	tr_formatter_size_init(1024, "KiB", "MiB", "GiB", "TiB");
    tr_formatter_speed_init(1024, "KiB/s", "MiB/s", "GiB/s", "TiB/s");
 
 	tr_bencInitDict(&fTorrentPreferences, 0 );
 	tr_sessionGetDefaultSettings(&fTorrentPreferences);
 	
 	
 	configDir = tr_getDefaultConfigDir("Torrentor");
 	
 

	fTorrentSession = tr_sessionInit("torrentor", configDir, FALSE, &fTorrentPreferences);
	
	char * str = tr_bencToStr( &fTorrentPreferences, TR_FMT_JSON, NULL );

	
	//printf("configDir -> %s\n", configDir);


	// set up a rectangle and instantiate a new window
	fMainWindow = new MainWindow(BRect(0, 0, 800, 600));
			
	// make window visible
	fMainWindow->Show();
	
	
	fprintf( stderr, "%s", str );
	tr_free( str );

	
	LoadTorrentList();
}

TorrentorApp::~TorrentorApp()
{
	//tr_sessionSaveSettings(fTorrentSession, tr_getDefaultConfigDir("Torrentor"), &fTorrentPreferences);
	//tr_bencFree(&fTorrentPreferences );
	tr_sessionClose(fTorrentSession);
}

void TorrentorApp::MessageReceived(BMessage* message)
{
	switch( message->what ) 
	{
	case MSG_OPEN_MAGNET_REQUEST:
		OpenMagnetLinkWindow();
		break;
	case MSG_OPEN_MAGNET_LINK:
		LoadTorrentFromMagnet(message);
		break;
	case MSG_OPEN_TORRENT_RESULT:
		OpenTorrentResult(message);
		break;
	default:
		BApplication::MessageReceived(message);
		break;
	}

}

void TorrentorApp::ReadyToRun()
{
	SetPulseRate(500000);
}

void TorrentorApp::RefsReceived(BMessage* message)
{
	int32 index = 0;
	entry_ref ref;
	BStringList TorrentPathList;

	//
	//
	//
	while( message->FindRef("refs", index++, &ref) == B_OK )
	{
		BEntry FileEntry(&ref);
		BPath  FilePath;
		
		FileEntry.GetPath(&FilePath);
		
		TorrentPathList.Add(FilePath.Path());
	}
	LoadTorrentFromFiles(TorrentPathList);

}

void TorrentorApp::Pulse()
{
	if( fMainWindow != NULL )
		fMainWindow->Update();
}


//
//
//
void TorrentorApp::CreateSession()
{
	//fTorrentSession = tr_sessionInit("torrentor", configDir, true, &settings);
	//fTorrentSession = tr_sessionInit("torrentor", configDir, FALSE, &settings);

}

//
//
//
const tr_session* TorrentorApp::Session() const
{
	return fTorrentSession;
}

void TorrentorApp::LoadTorrentList()
{
	int torrentCount = 0;
	tr_torrent** torrentList;
	tr_ctor* ctor = tr_ctorNew(fTorrentSession);
	

	torrentList = tr_sessionLoadTorrents(fTorrentSession, ctor, &torrentCount);
	
	//
	//
	//
	for( int i = 0; i < torrentCount; i++ )
	{
		TorrentObject* torrentObject = new TorrentObject();
		
		//
		//
		//
		if( !torrentObject->LoadFromHandle(torrentList[i]) )
		{
			tr_torrentStop(torrentList[i]);
			delete torrentObject;
			continue;
		}
		
		tr_torrentSetCompletenessCallback(torrentList[i], TorrentCompletenessHandler, NULL);
		
		//
		//
		//
		fTorrentList.AddItem(torrentObject);
		
		//
		// If there's a MainWindow created, add torrent.
		//
		if( fMainWindow != NULL )
			fMainWindow->AddTorrent(torrentObject);
	}
	
	tr_free( torrentList );
    tr_ctorFree( ctor );
}

//
//
//
void TorrentorApp::LoadTorrentFromFiles(const BStringList& TorrentPathList) 
{
	int32 stringCount = TorrentPathList.CountStrings();
	

	//
	for( int32 i = 0; i < stringCount; i++ ) 
	{	
		TorrentObject* torrentObject = new TorrentObject();
		
		//
		//
		//
		if( !torrentObject->LoadFromPath(Session(), TorrentPathList.StringAt(i)) )
		{
			delete torrentObject;
			continue;
		}		
		
		AddTorrentWindow* addTorrentWindow = new AddTorrentWindow(torrentObject);
		addTorrentWindow->Show();
	}
}

void TorrentorApp::OpenTorrentResult(BMessage* message)
{
	bool addTorrent = false;
	bool startTorrent = false;
	TorrentObject* torrentObject = NULL;
	
	message->FindBool("add", &addTorrent);
	message->FindBool("start", &startTorrent);
	
	//
	// We always need the torrent object.
	//
	if( message->FindPointer("torrent", reinterpret_cast<void**>(&torrentObject)) != B_OK )
		return;


	//
	//
	//
	if( addTorrent == false ) 
	{
		torrentObject->Remove(true);
		delete torrentObject;
		
		return;
	}
	
	fTorrentList.AddItem(torrentObject);
		
	if( startTorrent )
		torrentObject->StartTransfer();
		
	//
	// If there's a MainWindow created, add torrent.
	//
	if( fMainWindow != NULL )
		fMainWindow->AddTorrent(torrentObject);
}

void TorrentorApp::LoadTorrentFromMagnet(BMessage* message)
{
	BString MagnetUrl;
	
	
	if( message->FindString("URL", &MagnetUrl) != B_OK )
		return;

	
	TorrentObject* torrentObject = new TorrentObject();
		
	if( !torrentObject->LoadFromMagnet(Session(), MagnetUrl) )
	{
		delete torrentObject;
		return;
	}
		
	//
	//
	//
	fTorrentList.AddItem(torrentObject);
		
	//
	// If there's a MainWindow created, add torrent.
	//
	if( fMainWindow != NULL )
		fMainWindow->AddTorrent(torrentObject);	
}

//
//
//
void TorrentorApp::OpenMagnetLinkWindow()
{
	OpenMagnetWindow* MagnetWindow = new OpenMagnetWindow();
	
	MagnetWindow->Show();
}
