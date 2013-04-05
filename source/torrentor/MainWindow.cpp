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
//	File Name:		MainWindow.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <Alert.h>
#include <Application.h>
#include <Autolock.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <ListItem.h>
#include <ListView.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Notification.h>
#include <Roster.h>
#include <ScrollView.h>
#include <SpaceLayoutItem.h>
#include <StatusBar.h>
#include <StringView.h>


#include <assert.h>

#include "Torrentor.h"
#include "TorrentorMessages.h"
#include "Application.h"
#include "MainWindow.h"
#include "ToolBar.h"
#include "TorrentRefFilter.h"
#include "IconView.h"
#include "DownloadView.h"
#include "DownloadItem.h"
#include "InfoWindow.h"
#include "PreferencesWindow.h"



#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"


enum 
{
	MENU_FILE_OPEN_TORRENT		= 0x400000,
	MENU_FILE_OPEN_TORRENT_URL,
	MENU_EDIT_PREFERENCES,
	MENU_TORRENT_INSPECT,
	MENU_TORRENT_OPEN_DOWNLOAD,
	MENU_TORRENT_REMOVE,
};






MainWindow::MainWindow(BRect frame) 
	: BWindow(frame, "Torrentor!", B_DOCUMENT_WINDOW, 
			  B_AUTO_UPDATE_SIZE_LIMITS |
			  B_ASYNCHRONOUS_CONTROLS | 
			  B_NOT_ZOOMABLE )
{
	SetPulseRate(1000000);
	
	//
	//
	//
	SetLayout(new BGroupLayout(B_VERTICAL, 0.0));
	
	CreateMenuBar();
	
	
	fDownloadView 	= new DownloadView;
	
	
//	BStringView* fStatusText = new BStringView("status", "Laralala");
//	fStatusText->SetAlignment(B_ALIGN_LEFT);
//	fStatusText->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
///	fStatusText->SetExplicitMinSize(BSize(150, 12));

	
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 0.0)
		.Add(fMenuBar)
		.Add(fDownloadView->ScrollView())
/*
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, spacing)
			.AddGlue()
			.Add(fRemoveMissingButton)
			.Add(fRemoveFinishedButton)
			.SetInsets(12, 5, 12, 5)
		)*/
	);


	//
	//
	//
	fOpenPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, true, NULL, 
								new TorrentRefFilter);
								
	//
	//
	//
	fPreferencesWindow = new PreferencesWindow();




	//
	CenterOnScreen();
}

MainWindow::~MainWindow()
{
	delete fOpenPanel;
}

void MainWindow::InitWindow()
{
}


void MainWindow::CreateMenuBar()
{
	//
	//
	//
	fMenuBar = new BMenuBar(BRect(0, 0, 0, 0), "MainMenuBar");
	
	fMenuFile = new BMenu(B_TRANSLATE("File"));
	
	//
	//
	//
	//menu->AddItem(new BMenuItem("New Torrent", NULL));
	fMenuFile->AddItem(new BMenuItem(B_TRANSLATE("Open Torrent"), new BMessage(MENU_FILE_OPEN_TORRENT), 'O'));
	fMenuFile->AddItem(new BMenuItem(B_TRANSLATE("Open From Magnet"), new BMessage(MENU_FILE_OPEN_TORRENT_URL), 'M'));
	fMenuFile->AddSeparatorItem();
	fMenuFile->AddItem(new BMenuItem(B_TRANSLATE("Start All Torrents"), NULL));
	fMenuFile->AddItem(new BMenuItem(B_TRANSLATE("Pause All Torrent"), NULL));
	fMenuFile->AddSeparatorItem();
	fMenuFile->AddItem(new BMenuItem(B_TRANSLATE("Quit"), new BMessage(B_QUIT_REQUESTED), 'Q'));
	
	//
	//
	//
	fMenuBar->AddItem(fMenuFile);
	
	//
	//
	//
	fMenuEdit = new BMenu("Edit");
	fMenuEdit->AddItem(new BMenuItem("Select All", NULL));
	fMenuEdit->AddItem(new BMenuItem("Deselect All", NULL));
	fMenuEdit->AddSeparatorItem();
	fMenuEdit->AddItem(new BMenuItem("Preferences", new BMessage(MENU_EDIT_PREFERENCES), 'P'));
	
	fMenuBar->AddItem(fMenuEdit);
	
	
	//
	//
	//
	fMenuTorrent = new BMenu("Torrent");
	fMenuTorrent->SetEnabled(false);
	fMenuTorrent->AddItem(new BMenuItem("Inspect", new BMessage(MENU_TORRENT_INSPECT), 'I'));
	fMenuTorrent->AddItem(new BMenuItem("Open download folder", new BMessage(MENU_TORRENT_OPEN_DOWNLOAD)));
	fMenuTorrent->AddSeparatorItem();
	fMenuTorrent->AddItem(new BMenuItem("Remove", new BMessage(MENU_TORRENT_REMOVE)));
	//menu->AddItem(new BMenuItem("Select All", NULL));
	
	fMenuBar->AddItem(fMenuTorrent);
	
	
	//
	//
	//
	fMenuView = new BMenu("View");
	
	
	fMenuBar->AddItem(fMenuView);
	
	
	//
	//
	//
	fMenuHelp = new BMenu("Help");
	
	
	fMenuBar->AddItem(fMenuHelp);
}






void MainWindow::MessageReceived(BMessage* message)
{
	switch( message->what )
	{
	case MENU_FILE_OPEN_TORRENT:
		fOpenPanel->Show();
		break;
	case MENU_FILE_OPEN_TORRENT_URL:
		be_app->PostMessage(MSG_OPEN_MAGNET_REQUEST);
		break;
	case MENU_EDIT_PREFERENCES:
		OpenPreferencesWindow();
		break;
	case MENU_TORRENT_INSPECT:
		OnTorrentInspect();
		break;
	case MENU_TORRENT_OPEN_DOWNLOAD:
		OpenTorrentDownloadFolder();
		break;
	case MENU_TORRENT_REMOVE:
		OnMenuTorrentRemove();
		break;
	case MSG_DOWNLOAD_VIEW_SELECTION_CHANGED:
		fMenuTorrent->SetEnabled(fDownloadView->ItemSelected() != NULL);
		fMenuBar->Invalidate();
		break;	
	
	default:
		BWindow::MessageReceived(message);
		break;
	}
}

bool MainWindow::QuitRequested()
{
	bool downloading = !static_cast<TorrentorApp*>(be_app)->TorrentList().IsEmpty();
	
	//
	//
	//
	//for (int32 i = fTorrentList.CountItems(); i > 0; i--) {
	//	const TorrentObject* torrent = fTorrentList.ItemAt(i);
	//	
	//	active |= torrent->IsActive();
	//}
	
	//
	if (downloading) {
		BAlert* confirmAlert = new BAlert("", B_TRANSLATE("Quit Torrentor!?"),
			B_TRANSLATE("Cancel"), B_TRANSLATE("Quit"), NULL, B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);

		confirmAlert->SetShortcut(0, B_ESCAPE);
	
		if (confirmAlert->Go() == 0)
			return false;
	}
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void MainWindow::OnTorrentInspect()
{
	
	if( const DownloadItem* item = fDownloadView->ItemSelected() )
	{
		//
		// Get the torrent object.
		//	
		const TorrentObject* torrent = item->GetTorrentObject();
	
		//
		// @TODO: check if inspect window is already created.
		//
		InfoWindow* window = new InfoWindow(const_cast<TorrentObject*>(torrent));
	
		window->Show();
	}
}


void MainWindow::OpenPreferencesWindow()
{
//	BAutolock _Lock(fPreferencesWindow);
	
	
	if (fPreferencesWindow->IsHidden())
		fPreferencesWindow->Show();
	else
		fPreferencesWindow->Activate();
}


void MainWindow::OpenTorrentDownloadFolder()
{
	
	entry_ref ref;
	
	
	if( const DownloadItem* item = fDownloadView->ItemSelected() )
	{
		//
		// Get the torrent object.
		//	
		const TorrentObject* torrent = item->GetTorrentObject();

	
		//
		// Build the download path.
		//
		BString DownloadPath = torrent->DownloadFolder();
		
		//
		//
		//
		if( torrent->IsFolder() )
		{
			BString FolderName = torrent->Info()->files[ 0 ].name;
			
			FolderName.Truncate(FolderName.FindFirst('/'));
			
			DownloadPath << "/" << FolderName;
		}
	
		status_t status = get_ref_for_path(DownloadPath.String(), &ref);
		if (status == B_OK)
			status = be_roster->Launch(&ref);
	}

/*const char* kTrackerSignature = "application/x-vnd.Be-TRAK";

	if( const DownloadItem* item = fDownloadView->ItemSelected() )
	{
		const TorrentObject* torrent = item->GetTorrentObject();

			
		entry_ref ref;
		status_t status = get_ref_for_path(torrent->GetDownloadDir(), &ref);
		

		
		if( status == B_OK )
		{
			BMessage message(B_REFS_RECEIVED);
			message.AddRef("refs", &ref);

			BMessenger messenger(kTrackerSignature);
			messenger.SendMessage(&message);
	
		}
	}
*/
}


void MainWindow::OnMenuTorrentRemove()
{
	//
	//
	//
	if( const DownloadItem* selectedItem = fDownloadView->ItemSelected() )
	{
		//
		// Ask if the user want to delete local data or the torrent only.
		//
		BAlert* alert = new BAlert("Remove torrent", "Remove the selected torrent?",
    						"Cancel", "Remove torrent only", "Remove with data",
    						B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
    						
    	int32 buttonIndex 	= alert->Go();
    	bool removeData 	= buttonIndex == 2;
		
		//
		// Return if the user press cancel.
		//
		if( buttonIndex == 0 )
			return;
			
		//
		//
		//
		fDownloadView->RemoveItem(selectedItem);
			
		//
		//
		//
		selectedItem->GetTorrentObject()->Remove(removeData);
	}
}

void MainWindow::Update()
{
//	if( fListView->LockLooper() ) 
//	{
//		fListView->Invalidate();
//		fListView->UnlockLooper();
//	}
}


void MainWindow::AddTorrent(TorrentObject* torrentObject)
{
	torrentObject->Update();
	
	//
	// @TODO: fDownloadView->AddItem(torrentObject);
	//
	fDownloadView->AddItem(torrentObject);
}
