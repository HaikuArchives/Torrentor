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
#include "Torrentor.h"
#include "MainWindow.h"
#include "Application.h"





#include <stdio.h>

//tr_session* _TransmissionSession = NULL;

#define MEM_K 1024
#define MEM_B_STR   "B"
#define MEM_K_STR "KiB"
#define MEM_M_STR "MiB"
#define MEM_G_STR "GiB"
#define MEM_T_STR "TiB"

#define DISK_K 1024
#define DISK_B_STR   "B"
#define DISK_K_STR "KiB"
#define DISK_M_STR "MiB"
#define DISK_G_STR "GiB"
#define DISK_T_STR "TiB"

#define SPEED_K 1024
#define SPEED_B_STR   "B/s"
#define SPEED_K_STR "KiB/s"
#define SPEED_M_STR "MiB/s"
#define SPEED_G_STR "GiB/s"
#define SPEED_T_STR "TiB/s"


TorrentorApp::TorrentorApp()
	: BApplication("application/x-vnd.Torrentor")
{	
	const char * configDir;
	
	
	tr_formatter_mem_init( MEM_K, MEM_K_STR, MEM_M_STR, MEM_G_STR, MEM_T_STR );
    tr_formatter_size_init( DISK_K,DISK_K_STR, DISK_M_STR, DISK_G_STR, DISK_T_STR );
    tr_formatter_speed_init( SPEED_K, SPEED_K_STR, SPEED_M_STR, SPEED_G_STR, SPEED_T_STR );

	
 
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

void TorrentorApp::ReadyToRun()
{
	SetPulseRate(500000);
}

void TorrentorApp::RefsReceived(BMessage* message)
{
	int32 index = 0;
	entry_ref ref;

	//
	//
	//
	while( message->FindRef("refs", index++, &ref) == B_OK )
		LoadTorrentFromFile(&ref);

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
 	
	//if( paused )
	//tr_ctorSetPaused( ctor, TR_FORCE, TRUE );
	

	torrentList = tr_sessionLoadTorrents(fTorrentSession, ctor, &torrentCount);
	
	//
	//
	//
	for( int i = 0; i < torrentCount; i++ )
	{
		//
		//
		//
		TorrentObject* torrentObject = new TorrentObject(torrentList[i]);
		
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
void TorrentorApp::LoadTorrentFromFile(const entry_ref* FileRef) 
{
	size_t   FileLength;
	uint8_t* FileContent;
	BPath  	 FilePath;
	BEntry 	 FileEntry(FileRef);
	
	//
	//
	//
	FileEntry.GetPath(&FilePath);
	
	//
	// @TODO: create TorrentObject
	//
	//
	//
	//
	tr_ctor* ctor = tr_ctorNew(Session());
	
	//
	//
	//
	FileContent = tr_loadFile(FilePath.Path(), &FileLength);
	
	if( FileContent == NULL )
	{
		fprintf( stderr, "ERROR: Unrecognized torrent \"%s\".\n", FilePath.Path() );
        fprintf( stderr, " * If you're trying to create a torrent, use transmission-create.\n" );
        fprintf( stderr, " * If you're trying to see a torrent's info, use transmission-show.\n" );
        //tr_sessionClose( h );
		return;
	}

	tr_ctorSetMetainfo( ctor, FileContent, FileLength );
	
	tr_free( FileContent );
	
	int error = 0;
	tr_torrent* tor = tr_torrentNew( ctor, &error );
    tr_ctorFree( ctor );
    if( !tor )
    {
        fprintf( stderr, "Failed opening torrent file `%s'\n", FilePath.Path() );
        //tr_sessionClose( h );
        return;
    }

	//
	tr_ctorSetMetainfoFromFile(ctor, FilePath.Path());
	//tr_core_apply_defaults(ctor);
	//
	
	//
	//
	//
	//tr_ctorSetPaused(ctor, TR_FORCE, true);
	
	
	//tr_info inf;
    //int err = tr_torrentParse( ctor, &inf );
    
	//
	//
	//
	TorrentObject* torrentObject = new TorrentObject(tor);
		
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
